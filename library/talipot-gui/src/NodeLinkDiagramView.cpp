/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "ui_GridOptionsWidget.h"

#include <QGraphicsView>
#include <QMenu>

#include <talipot/GlMetaNodeRenderer.h>
#include <talipot/GlGrid.h>
#include <talipot/DrawingTools.h>
#include <talipot/ItemDelegate.h>
#include <talipot/ParameterListModel.h>
#include <talipot/GlGraph.h>
#include <talipot/GlVertexArrayManager.h>
#include <talipot/GlOverviewGraphicsItem.h>
#include <talipot/QtGlSceneZoomAndPanAnimator.h>
#include <talipot/GlCompositeHierarchyManager.h>
#include <talipot/NodeLinkDiagramView.h>
#include <talipot/GraphModel.h>
#include <talipot/FontIconManager.h>
#include <talipot/MaterialDesignIcons.h>

using namespace tlp;
using namespace std;

const string NodeLinkDiagramView::viewName("Node Link Diagram");

NodeLinkDiagramView::NodeLinkDiagramView(const tlp::PluginContext *)
    : GlView(true), _grid(nullptr), _gridOptions(nullptr), manager(nullptr), _hasHulls(false),
      grid_ui(nullptr) {}

NodeLinkDiagramView::~NodeLinkDiagramView() {
  if (grid_ui) {
    delete grid_ui->tableView->itemDelegate();
  }

  delete grid_ui;
  delete manager;
}

void NodeLinkDiagramView::updateGrid() {
  delete _grid;
  _grid = nullptr;

  if (_gridOptions == nullptr) {
    return;
  }

  DataSet gridData =
      static_cast<ParameterListModel *>(_gridOptions->findChild<QTableView *>()->model())
          ->parametersValues();
  StringCollection gridMode;
  gridData.get<StringCollection>("Grid mode", gridMode);
  int mode = gridMode.getCurrent();

  if (mode == 0) {
    return;
  }

  Coord margins;
  Size gridSize;
  Color gridColor;
  bool onX = true, only = true, onZ = true;
  gridData.get<Coord>("Margins", margins);
  gridData.get<Size>("Grid size", gridSize);
  gridData.get<Color>("Grid color", gridColor);
  gridData.get<bool>("X grid", onX);
  gridData.get<bool>("Y grid", only);
  gridData.get<bool>("Z grid", onZ);

  GlGraphInputData *inputData = glWidget()->inputData();
  BoundingBox graphBB =
      computeBoundingBox(graph(), inputData->layout(), inputData->sizes(), inputData->rotations());
  Coord bottomLeft = Coord(graphBB[0] - margins);
  Coord topRight = Coord(graphBB[1] + margins);

  if (mode == 1) {
    for (int i = 0; i < 3; ++i) {
      gridSize[i] = abs(topRight[i] - bottomLeft[i]) / gridSize[i];
    }
  }

  bool displays[3];
  displays[0] = onX;
  displays[1] = only;
  displays[2] = onZ;

  _grid = new GlGrid(bottomLeft, topRight, gridSize, gridColor, displays);
  glWidget()->scene()->getLayer("Main")->addGlEntity(_grid, "Node Link Diagram Component grid");
}

void NodeLinkDiagramView::draw() {
  updateGrid();
  GlView::draw();
}

void NodeLinkDiagramView::setState(const tlp::DataSet &data) {
  ParameterDescriptionList gridParameters;
  gridParameters.add<StringCollection>("Grid mode", "", "No grid;Space divisions;Fixed size", true);
  gridParameters.add<Size>("Grid size", "", "(1,1,1)", false);
  gridParameters.add<Size>("Margins", "", "(0.5,0.5,0.5)", false);
  gridParameters.add<Color>("Grid color", "", "(0,0,0,255)", false);
  gridParameters.add<bool>("X grid", "", "true", false);
  gridParameters.add<bool>("Y grid", "", "true", false);
  gridParameters.add<bool>("Z grid", "", "true", false);
  auto *model = new ParameterListModel(gridParameters, nullptr, this);

  grid_ui = new Ui::GridOptionsWidget;
  _gridOptions = new QDialog(graphicsView());
  grid_ui->setupUi(_gridOptions);
  grid_ui->tableView->setModel(model);
  grid_ui->tableView->setItemDelegate(new ItemDelegate(grid_ui->tableView));

  setOverviewVisible(true);
  setQuickAccessBarVisible(true);
  GlView::setState(data);

  bool keepSPOV = false;
  data.get<bool>("keepScenePointOfViewOnSubgraphChanging", keepSPOV);
  glWidget()->setKeepScenePointOfViewOnSubgraphChanging(keepSPOV);

  if (!data.empty()) {
    createScene(graph(), data);
  }
  registerTriggers();

  if (overviewItem()) {
    overviewItem()->setLayerVisible("Foreground", false);
  }
}

void NodeLinkDiagramView::graphChanged(tlp::Graph *graph) {
  GlGraph *glGraph = glWidget()->scene()->glGraph();
  Graph *oldGraph = glGraph ? glGraph->graph() : nullptr;
  loadGraphOnScene(graph);
  registerTriggers();

  if (oldGraph == nullptr || graph == nullptr || (oldGraph->getRoot() != graph->getRoot()) ||
      !glWidget()->keepScenePointOfViewOnSubgraphChanging()) {
    centerView();
  }

  emit drawNeeded();
  drawOverview();
}

tlp::DataSet NodeLinkDiagramView::state() const {
  DataSet data = sceneData();
  data.set("keepScenePointOfViewOnSubgraphChanging",
           glWidget()->keepScenePointOfViewOnSubgraphChanging());

  return data;
}

void NodeLinkDiagramView::initRenderingParameters(GlGraphRenderingParameters &rp) {
  rp.setViewNodeLabel(true);
  rp.setEdgeColorInterpolate(false);
  rp.setNodesStencil(0x0002);
  rp.setNodesLabelStencil(0x0001);
}

//==================================================
void NodeLinkDiagramView::createScene(Graph *graph, DataSet dataSet) {
  if (manager) {
    delete manager;
    manager = nullptr;
  }

  GlScene *scene = glWidget()->scene();
  scene->clearLayersList();

  std::string sceneInput;

  if (dataSet.exists("scene")) {
    dataSet.get("scene", sceneInput);
  }

  if (sceneInput.empty()) {
    // Default scene
    auto *layer = new GlLayer("Main");
    auto *backgroundLayer = new GlLayer("Background");
    backgroundLayer->setVisible(false);
    auto *foregroundLayer = new GlLayer("Foreground");
    foregroundLayer->setVisible(true);

    backgroundLayer->set2DMode();
    foregroundLayer->set2DMode();

    scene->addExistingLayer(backgroundLayer);
    scene->addExistingLayer(layer);
    scene->addExistingLayer(foregroundLayer);
    auto *glGraph = new GlGraph(graph, scene);
    scene->getLayer("Main")->addGlEntity(glGraph, "graph");
    initRenderingParameters(glGraph->renderingParameters());
    scene->centerScene();
  } else {
    size_t pos = sceneInput.find("TalipotBitmapDir/");

    while (pos != std::string::npos) {
      sceneInput.replace(pos, 15, TalipotBitmapDir);
      pos = sceneInput.find("TalipotBitmapDir/");
    }

    pos = sceneInput.find("TalipotLibDir/");

    while (pos != std::string::npos) {
      sceneInput.replace(pos, 12, TalipotLibDir);
      pos = sceneInput.find("TalipotLibDir/");
    }

    scene->setWithXML(sceneInput, graph);
  }

  if (dataSet.exists("Display")) {
    DataSet renderingParameters;
    dataSet.get("Display", renderingParameters);
    GlGraphRenderingParameters &rp = scene->glGraph()->renderingParameters();
    rp.setParameters(renderingParameters);

    string s;

    if (renderingParameters.get("elementsOrderingPropertyName", s) && !s.empty()) {
      rp.setElementOrderingProperty(dynamic_cast<tlp::NumericProperty *>(graph->getProperty(s)));
    }
  }

  useHulls(true);

  if (dataSet.exists("Hulls")) {
    DataSet hullsSet;
    dataSet.get<DataSet>("Hulls", hullsSet);
    manager->setVisible(true);
    manager->setData(hullsSet);
  }

  // glWidget()->emitGraphChanged();
}
//==================================================
DataSet NodeLinkDiagramView::sceneData() const {
  GlScene *scene = glWidget()->scene();
  DataSet outDataSet = GlView::state();
  outDataSet.set("Display", scene->glGraph()->renderingParameters().getParameters());
  std::string out;
  scene->getXML(out);
  size_t pos = out.find(TalipotBitmapDir);

  while (pos != std::string::npos) {
    out.replace(pos, TalipotBitmapDir.size(), "TalipotBitmapDir/");
    pos = out.find(TalipotBitmapDir);
  }

  outDataSet.set("scene", out);

  if (_hasHulls && manager->isVisible()) {
    outDataSet.set("Hulls", manager->getData());
  }

  return outDataSet;
}
//==================================================
void NodeLinkDiagramView::loadGraphOnScene(Graph *graph) {
  GlScene *scene = glWidget()->scene();

  if (!scene->getLayer("Main")) {
    createScene(graph, DataSet());
    return;
  }

  if (_hasHulls) {
    manager->setGraph(graph);
  }

  GlGraph *oldGlGraph = static_cast<GlGraph *>(scene->getLayer("Main")->findGlEntity("graph"));

  if (!oldGlGraph) {
    createScene(graph, DataSet());

    return;
  }

  GlMetaNodeRenderer *metaNodeRenderer = oldGlGraph->inputData()->metaNodeRenderer();
  // prevent deletion of MetaNodeRenderer when deleting oldGlGraph
  oldGlGraph->inputData()->setMetaNodeRenderer(nullptr, false);
  auto *glGraph = new GlGraph(graph);
  glGraph->setRenderingParameters(oldGlGraph->renderingParameters());

  metaNodeRenderer->setInputData(glGraph->inputData());

  glGraph->inputData()->setMetaNodeRenderer(metaNodeRenderer);

  if (oldGlGraph->inputData()->graph() == graph) {
    delete glGraph->inputData()->glVertexArrayManager();
    glGraph->inputData()->setGlVertexArrayManager(oldGlGraph->inputData()->glVertexArrayManager());
    // prevent deletion of GlVertexArrayManager when deleting oldGlGraph
    oldGlGraph->inputData()->setGlVertexArrayManager(nullptr);
    glGraph->inputData()->glVertexArrayManager()->setInputData(glGraph->inputData());
  }

  scene->getLayer("Main")->addGlEntity(glGraph, "graph");

  delete oldGlGraph;
  glWidget()->emitGraphChanged();
}

void NodeLinkDiagramView::registerTriggers() {
  clearRedrawTriggers();

  if (graph() == nullptr) {
    return;
  }

  addRedrawTrigger(glWidget()->scene()->glGraph()->graph());
  std::set<tlp::PropertyInterface *> properties = glWidget()->inputData()->properties();

  for (auto *p : properties) {
    addRedrawTrigger(p);
  }
}

void NodeLinkDiagramView::setZOrdering(bool f) {
  glWidget()->renderingParameters().setElementZOrdered(f);
  centerView();
}

void NodeLinkDiagramView::showGridControl() {
  if (_gridOptions->exec() == QDialog::Rejected) {
    return;
  }

  updateGrid();
  emit drawNeeded();
}

void NodeLinkDiagramView::requestChangeGraph(Graph *graph) {
  this->loadGraphOnScene(graph);
  registerTriggers();
  emit graphSet(graph);
  centerView();
  draw();
}

void NodeLinkDiagramView::fillContextMenu(QMenu *menu, const QPointF &point) {

  // Check if a node/edge is under the mouse pointer
  bool result;
  SelectedEntity entity;
  result = glWidget()->pickNodesEdges(point.x(), point.y(), entity);

  if (result) {
    menu->addSeparator();
    isNode = entity.getEntityType() == SelectedEntity::NODE_SELECTED;
    itemId = entity.getGraphElementId();
    QString sId = QString::number(itemId);

    menu->addAction((isNode ? "Node #" : "Edge #") + sId)->setEnabled(false);

    menu->addSeparator();

    QMenu *selectMenu = menu->addMenu("Select");
    QAction *action;

    if (isNode) {
      action = selectMenu->addAction("node", [this] { selectItem(); });
      action->setToolTip("Select the node #" + sId);
      action = selectMenu->addAction("predecessor nodes", [this] { selectInNodes(); });
      action->setToolTip("Select the predecessors of node #" + sId);
      action = selectMenu->addAction("successor nodes", [this] { selectOutNodes(); });
      action->setToolTip("Select the successors of node #" + sId);
      action = selectMenu->addAction("input edges", [this] { selectInEdges(); });
      action->setToolTip("Select the input edges of node #" + sId);
      action = selectMenu->addAction("output edges", [this] { selectOutEdges(); });
      action->setToolTip("Select the output edges of node #" + sId);
      action = selectMenu->addAction("node and all its neighbour nodes (including edges)",
                                     [this] { selectNodeAndAllNeighbourNodesAndEdges(); });
      action->setToolTip("Select the node #" + sId + " and its neighbours and adjacent edges");
    } else {
      action = selectMenu->addAction("edge", [this] { selectItem(); });
      action->setToolTip("Select the edge #" + sId);
      action = selectMenu->addAction("edge extremities", [this] { selectExtremities(); });
      action->setToolTip("Select the source and target nodes of edge #" + sId);
      action =
          selectMenu->addAction("edge and its extremities", [this] { selectEdgeAndExtremities(); });
      action->setToolTip("Select the edge #" + sId + " and its source and target nodes");
    }

    QMenu *addToSelectionMenu = menu->addMenu("Add to selection");

    if (isNode) {
      action = addToSelectionMenu->addAction("node", [this] { addItemToSelection(); });
      action->setToolTip("Add the node #" + sId + " to the current selection");
      action =
          addToSelectionMenu->addAction("predecessor nodes", [this] { addInNodesToSelection(); });
      action->setToolTip("Add the predecessors of node #" + sId + " to the current selection");
      action =
          addToSelectionMenu->addAction("successor nodes", [this] { addOutNodesToSelection(); });
      action->setToolTip("Add the successors of node #" + sId + " to the current selection");
      action = addToSelectionMenu->addAction("input edges", [this] { addInEdgesToSelection(); });
      action->setToolTip("Add the input edges of node #" + sId + " to the current selection");
      action = addToSelectionMenu->addAction("output edges", [this] { addOutEdgesToSelection(); });
      action->setToolTip("Add the output edges of node #" + sId + " to the current selection");
      action = addToSelectionMenu->addAction(
          tr("node and all its neighbour nodes (including edges)"),
          [this] { addNodeAndAllNeighbourNodesAndEdgesToSelection(); });
      action->setToolTip("Add the node #" + sId +
                         "and its neighbours and adjacent edges to the current selection");
    } else {
      action = addToSelectionMenu->addAction("edge", [this] { addItemToSelection(); });
      action->setToolTip("Add the edge #" + sId + " to the current selection");
      action = addToSelectionMenu->addAction("edge extremities",
                                             [this] { addExtremitiesToSelection(); });
      action->setToolTip("Add the source and target nodes of edge #" + sId +
                         " to the current selection");
      action = addToSelectionMenu->addAction("edge and its extremities",
                                             [this] { addEdgeAndExtremitiesToSelection(); });
      action->setToolTip("Add the edge #" + sId +
                         "and its  source and target nodes to the current selection");
    }

    QMenu *removeFromSelectionMenu = menu->addMenu("Remove from selection");

    if (isNode) {
      action = removeFromSelectionMenu->addAction("node", [this] { removeItemFromSelection(); });
      action->setToolTip("Remove the node #" + sId + " from the current selection");
      action = removeFromSelectionMenu->addAction("predecessor nodes",
                                                  [this] { removeInNodesFromSelection(); });
      action->setToolTip("Remove the predecessors of node #" + sId + " from the current selection");
      action = removeFromSelectionMenu->addAction("successor nodes",
                                                  [this] { removeOutNodesFromSelection(); });
      action->setToolTip("Remove the successors of node #" + sId + " from the current selection");
      action = removeFromSelectionMenu->addAction("input edges",
                                                  [this] { removeInEdgesFromSelection(); });
      action->setToolTip("Remove the input edges of node #" + sId + " from the current selection");
      action = removeFromSelectionMenu->addAction("output edges",
                                                  [this] { removeOutEdgesFromSelection(); });
      action->setToolTip("Remove the output edges of node #" + sId + " from the current selection");
      action = removeFromSelectionMenu->addAction(
          tr("node and all its neighbour nodes (including edges)"),
          [this] { removeNodeAndAllNeighbourNodesAndEdgesFromSelection(); });
    } else {
      action = removeFromSelectionMenu->addAction("edge", [this] { removeItemFromSelection(); });
      action->setToolTip("Remove the edge #" + sId + " from the current selection");
      action = removeFromSelectionMenu->addAction("edge extremities",
                                                  [this] { removeExtremitiesFromSelection(); });
      action->setToolTip("Remove the extremities of edge #" + sId + " from the current selection");
      action = removeFromSelectionMenu->addAction(
          "edge and its extremities", [this] { removeEdgeAndExtremitiesFromSelection(); });
      action->setToolTip("Remove the edge #" + sId +
                         " and its extremities from the current selection");
    }

    QMenu *toggleMenu = menu->addMenu("Toggle selection of");

    if (isNode) {
      action = toggleMenu->addAction("node", [this] { addRemoveItemToSelection(); });
      action->setToolTip("Invert the selection of the node #" + sId);
      action =
          toggleMenu->addAction("predecessor nodes", [this] { addRemoveInNodesToSelection(); });
      action->setToolTip("Invert the selection of the predecessors of the node #" + sId);
      action = toggleMenu->addAction("successor nodes", [this] { addRemoveOutNodesToSelection(); });
      action->setToolTip("Invert the selection of the successors of the node #" + sId);
      action = toggleMenu->addAction("input edges", [this] { addRemoveInEdgesToSelection(); });
      action->setToolTip("Invert the selection of the input edges of the node #" + sId);
      action = toggleMenu->addAction("output edges", [this] { addRemoveOutEdgesToSelection(); });
      action->setToolTip("Invert the selection of the output edges of the node #" + sId);
      action = toggleMenu->addAction("node and all its neighbour nodes (including edges)",
                                     [this] { addRemoveNodeAndAllNeighbourNodesAndEdges(); });
      action->setToolTip("Invert the selection of the node #" + sId +
                         "and its neighbours and adjacent edges");
    } else {
      action = toggleMenu->addAction("edge", [this] { addRemoveItemToSelection(); });
      action->setToolTip("Invert the selection of the edge #" + sId);
      action =
          toggleMenu->addAction("edge extremities", [this] { addRemoveExtremitiesToSelection(); });
      action->setToolTip("Invert the selection of the source and target nodes of the edge #" + sId);
      action = toggleMenu->addAction("edge and its extremities",
                                     [this] { addRemoveEdgeAndExtremitiesToSelection(); });
      action->setToolTip("Invert the selection of the edge #" + sId +
                         " and its source and target nodes");
    }

    auto genEltToolTip = [this, sId](const QString &s) {
      return s + (isNode ? " node #" : " edge #") + sId;
    };

    action = menu->addAction("Delete", [this] { deleteItem(); });
    action->setToolTip(genEltToolTip("Delete the"));

    QMenu *updateMenu = menu->addMenu("Edit");
    action = updateMenu->addAction("Color", [this] { editColor(); });
    action->setToolTip(genEltToolTip("Display a dialog to update the color of the"));
    action = updateMenu->addAction("Label", [this] { editLabel(); });
    action->setToolTip(genEltToolTip("Display a dialog to update the label of the"));
    action = updateMenu->addAction("Shape", [this] { editShape(); });
    action->setToolTip(genEltToolTip("Display a dialog to update the shape of the"));
    action = updateMenu->addAction("Size", [this] { editSize(); });
    action->setToolTip(genEltToolTip("Display a dialog to update the size of the"));

    if (isNode) {
      Graph *metaGraph = graph()->getNodeMetaInfo(node(entity.getGraphElementId()));

      if (metaGraph) {
        action = menu->addAction("Go inside", [this] { goInsideItem(); });
        action->setToolTip("Display the subgraph represented by the meta-node #" + sId);
        menu->addAction("Ungroup", [this] { ungroupItem(); });
        action->setToolTip("Replace the meta-node #" + sId + " by the subgraph it represents");
      }

      View::fillContextMenu(menu, node(entity.getGraphElementId()));
    } else {
      View::fillContextMenu(menu, edge(entity.getGraphElementId()));
    }
  } else {
    GlView::fillContextMenu(menu, point);

    QAction *action = menu->addAction(
        FontIconManager::icon(MaterialDesignIcons::LayersTripleOutline), "Use Z ordering");
    action->setToolTip(
        "The graph elements are displayed according the ordering of their z coordinate");
    action->setCheckable(true);
    action->setChecked(glWidget()->renderingParameters().isElementZOrdered());
    connect(action, &QAction::triggered, this, &NodeLinkDiagramView::setZOrdering);
    action = menu->addAction(FontIconManager::icon(MaterialDesignIcons::Grid),
                             "Grid display parameters", [this] { showGridControl(); });
    action->setToolTip("Display the grid setup wizard");
  }
}

void NodeLinkDiagramView::addRemoveItemToSelection(bool pushGraph, bool toggleSelection,
                                                   bool selectValue, bool resetSelection) {
  BooleanProperty *elementSelected = graph()->getBooleanProperty("viewSelection");

  if (pushGraph) {
    graph()->push();
  }

  if (resetSelection) {
    elementSelected->setAllNodeValue(false);
    elementSelected->setAllEdgeValue(false);
  }

  // selection add/remove graph item
  if (isNode) {
    elementSelected->setNodeValue(
        node(itemId), toggleSelection ? !elementSelected->getNodeValue(node(itemId)) : selectValue);
  } else {
    elementSelected->setEdgeValue(
        edge(itemId), toggleSelection ? !elementSelected->getEdgeValue(edge(itemId)) : selectValue);
  }
}

void NodeLinkDiagramView::addRemoveInNodesToSelection(bool pushGraph, bool toggleSelection,
                                                      bool selectValue, bool resetSelection) {
  BooleanProperty *elementSelected = graph()->getBooleanProperty("viewSelection");

  if (pushGraph) {
    graph()->push();
  }

  if (resetSelection) {
    elementSelected->setAllNodeValue(false);
    elementSelected->setAllEdgeValue(false);
  }

  MutableContainer<bool> inNodes;
  for (auto neigh : graph()->getInNodes(node(itemId))) {
    if (!inNodes.get(neigh.id)) {
      elementSelected->setNodeValue(neigh, toggleSelection ? !elementSelected->getNodeValue(neigh)
                                                           : selectValue);
      inNodes.set(neigh.id, true);
    }
  }
}

void NodeLinkDiagramView::addRemoveOutNodesToSelection(bool pushGraph, bool toggleSelection,
                                                       bool selectValue, bool resetSelection) {
  BooleanProperty *elementSelected = graph()->getBooleanProperty("viewSelection");

  if (pushGraph) {
    graph()->push();
  }

  if (resetSelection) {
    elementSelected->setAllNodeValue(false);
    elementSelected->setAllEdgeValue(false);
  }

  MutableContainer<bool> outNodes;
  for (auto neigh : graph()->getOutNodes(node(itemId))) {
    if (!outNodes.get(neigh.id)) {
      elementSelected->setNodeValue(neigh, toggleSelection ? !elementSelected->getNodeValue(neigh)
                                                           : selectValue);
      outNodes.set(neigh.id, true);
    }
  }
}

void NodeLinkDiagramView::addRemoveInEdgesToSelection(bool pushGraph, bool toggleSelection,
                                                      bool selectValue, bool resetSelection) {
  BooleanProperty *elementSelected = graph()->getBooleanProperty("viewSelection");

  if (pushGraph) {
    graph()->push();
  }

  if (resetSelection) {
    elementSelected->setAllNodeValue(false);
    elementSelected->setAllEdgeValue(false);
  }

  for (auto e : graph()->getInEdges(node(itemId))) {
    elementSelected->setEdgeValue(e, toggleSelection ? !elementSelected->getEdgeValue(e)
                                                     : selectValue);
  }
}

void NodeLinkDiagramView::addRemoveOutEdgesToSelection(bool pushGraph, bool toggleSelection,
                                                       bool selectValue, bool resetSelection) {
  BooleanProperty *elementSelected = graph()->getBooleanProperty("viewSelection");

  if (pushGraph) {
    graph()->push();
  }

  if (resetSelection) {
    elementSelected->setAllNodeValue(false);
    elementSelected->setAllEdgeValue(false);
  }

  for (auto e : graph()->getOutEdges(node(itemId))) {
    elementSelected->setEdgeValue(e, toggleSelection ? !elementSelected->getEdgeValue(e)
                                                     : selectValue);
  }
}

void NodeLinkDiagramView::addRemoveNodeAndAllNeighbourNodesAndEdges(bool toggleSelection,
                                                                    bool selectValue,
                                                                    bool resetSelection) {
  graph()->push();
  addRemoveItemToSelection(false, toggleSelection, selectValue, resetSelection);
  addRemoveInEdgesToSelection(false, toggleSelection, selectValue);
  addRemoveOutEdgesToSelection(false, toggleSelection, selectValue);
  addRemoveInNodesToSelection(false, toggleSelection, selectValue);
  addRemoveOutNodesToSelection(false, toggleSelection, selectValue);
}

void NodeLinkDiagramView::addRemoveExtremitiesToSelection(bool pushGraph, bool toggleSelection,
                                                          bool selectValue, bool resetSelection) {
  BooleanProperty *elementSelected = graph()->getBooleanProperty("viewSelection");

  if (pushGraph) {
    graph()->push();
  }

  if (resetSelection) {
    elementSelected->setAllNodeValue(false);
    elementSelected->setAllEdgeValue(false);
  }

  node src = graph()->source(edge(itemId));
  node tgt = graph()->target(edge(itemId));
  elementSelected->setNodeValue(src, toggleSelection ? !elementSelected->getNodeValue(src)
                                                     : selectValue);

  if (src != tgt) {
    elementSelected->setNodeValue(tgt, toggleSelection ? !elementSelected->getNodeValue(tgt)
                                                       : selectValue);
  }
}

void NodeLinkDiagramView::addRemoveEdgeAndExtremitiesToSelection(bool toggleSelection,
                                                                 bool selectValue,
                                                                 bool resetSelection) {
  graph()->push();
  addRemoveItemToSelection(false, toggleSelection, selectValue, resetSelection);
  addRemoveExtremitiesToSelection(false, toggleSelection, selectValue);
}

void NodeLinkDiagramView::selectItem() {
  addRemoveItemToSelection(true, false, true, true);
}

void NodeLinkDiagramView::selectInNodes(bool pushGraph) {
  addRemoveInNodesToSelection(pushGraph, false, true, true);
}

void NodeLinkDiagramView::selectOutNodes(bool pushGraph) {
  addRemoveOutNodesToSelection(pushGraph, false, true, true);
}

void NodeLinkDiagramView::selectInEdges(bool pushGraph) {
  addRemoveInEdgesToSelection(pushGraph, false, true, true);
}

void NodeLinkDiagramView::selectOutEdges(bool pushGraph) {
  addRemoveOutEdgesToSelection(pushGraph, false, true, true);
}

void NodeLinkDiagramView::selectNodeAndAllNeighbourNodesAndEdges() {
  addRemoveNodeAndAllNeighbourNodesAndEdges(false, true, true);
}

void NodeLinkDiagramView::selectExtremities(bool pushGraph) {
  addRemoveExtremitiesToSelection(pushGraph, false, true, true);
}

void NodeLinkDiagramView::selectEdgeAndExtremities() {
  addRemoveEdgeAndExtremitiesToSelection(false, true, true);
}

void NodeLinkDiagramView::addItemToSelection() {
  addRemoveItemToSelection(true, false, true);
}

void NodeLinkDiagramView::addInNodesToSelection(bool pushGraph) {
  addRemoveInNodesToSelection(pushGraph, false, true);
}

void NodeLinkDiagramView::addOutNodesToSelection(bool pushGraph) {
  addRemoveOutNodesToSelection(pushGraph, false, true);
}

void NodeLinkDiagramView::addInEdgesToSelection(bool pushGraph) {
  addRemoveInEdgesToSelection(pushGraph, false, true);
}

void NodeLinkDiagramView::addOutEdgesToSelection(bool pushGraph) {
  addRemoveOutEdgesToSelection(pushGraph, false, true);
}

void NodeLinkDiagramView::addNodeAndAllNeighbourNodesAndEdgesToSelection() {
  addRemoveNodeAndAllNeighbourNodesAndEdges(false, true);
}

void NodeLinkDiagramView::addExtremitiesToSelection(bool pushGraph) {
  addRemoveExtremitiesToSelection(pushGraph, false, true);
}

void NodeLinkDiagramView::addEdgeAndExtremitiesToSelection() {
  addRemoveEdgeAndExtremitiesToSelection(false, true);
}

void NodeLinkDiagramView::removeItemFromSelection() {
  addRemoveItemToSelection(true, false, false);
}

void NodeLinkDiagramView::removeInNodesFromSelection(bool pushGraph) {
  addRemoveInNodesToSelection(pushGraph, false, false);
}

void NodeLinkDiagramView::removeOutNodesFromSelection(bool pushGraph) {
  addRemoveOutNodesToSelection(pushGraph, false, false);
}

void NodeLinkDiagramView::removeInEdgesFromSelection(bool pushGraph) {
  addRemoveInEdgesToSelection(pushGraph, false, false);
}

void NodeLinkDiagramView::removeOutEdgesFromSelection(bool pushGraph) {
  addRemoveOutEdgesToSelection(pushGraph, false, false);
}

void NodeLinkDiagramView::removeNodeAndAllNeighbourNodesAndEdgesFromSelection() {
  addRemoveNodeAndAllNeighbourNodesAndEdges(false, false);
}

void NodeLinkDiagramView::removeExtremitiesFromSelection(bool pushGraph) {
  addRemoveExtremitiesToSelection(pushGraph, false, false);
}

void NodeLinkDiagramView::removeEdgeAndExtremitiesFromSelection() {
  addRemoveEdgeAndExtremitiesToSelection(false, false);
}

void NodeLinkDiagramView::deleteItem() {
  graph()->push();

  if (isNode) {
    graph()->delNode(node(itemId));
  } else {
    graph()->delEdge(edge(itemId));
  }
}

void NodeLinkDiagramView::editValue(PropertyInterface *pi) {
  ItemDelegate tid(glWidget());
  QVariant val =
      ItemDelegate::showEditorDialog(isNode ? NODE : EDGE, pi, graph(), &tid, glWidget(), itemId);

  // Check if edition has been cancelled
  if (!val.isValid()) {
    return;
  }

  graph()->push();

  if (isNode) {
    GraphModel::setNodeValue(itemId, pi, val);
  } else {
    GraphModel::setEdgeValue(itemId, pi, val);
  }

  graph()->popIfNoUpdates();
}

void NodeLinkDiagramView::editColor() {
  editValue(glWidget()->inputData()->colors());
}

void NodeLinkDiagramView::editLabel() {
  editValue(glWidget()->inputData()->labels());
}

void NodeLinkDiagramView::editShape() {
  editValue(glWidget()->inputData()->shapes());
}

void NodeLinkDiagramView::editSize() {
  editValue(glWidget()->inputData()->sizes());
}

const Camera &NodeLinkDiagramView::goInsideItem(node meta) {
  Graph *metaGraph = graph()->getNodeMetaInfo(meta);
  Size size = glWidget()->inputData()->sizes()->getNodeValue(meta);
  Coord coord = glWidget()->inputData()->layout()->getNodeValue(meta);
  BoundingBox bb;
  bb.expand(coord - size / 2.f);
  bb.expand(coord + size / 2.f);
  QtGlSceneZoomAndPanAnimator zoomAnPan(glWidget(), bb);
  zoomAnPan.animateZoomAndPan();
  loadGraphOnScene(metaGraph);
  registerTriggers();
  emit graphSet(metaGraph);
  centerView();
  draw();
  return glWidget()->scene()->getLayer("Main")->getCamera();
}

void NodeLinkDiagramView::goInsideItem() {
  goInsideItem(node(itemId));
}

void NodeLinkDiagramView::ungroupItem() {
  graph()->push();
  graph()->openMetaNode(node(itemId));
}

void NodeLinkDiagramView::useHulls(bool hasHulls) {
  if (manager && (hasHulls == _hasHulls)) {
    return;
  }

  _hasHulls = hasHulls;

  if (_hasHulls) {
    GlScene *scene = glWidget()->scene();

    manager = new GlCompositeHierarchyManager(
        scene->glGraph()->inputData()->graph(), scene->getLayer("Main"), "Hulls",
        scene->glGraph()->inputData()->layout(), scene->glGraph()->inputData()->sizes(),
        scene->glGraph()->inputData()->rotations());
    // Now we remove and add GlGraph to be sure of the order (first Hulls and after
    // GlGraph)
    // This code doesn't affect the behavior of talipot but the tlp file is modified
    scene->getLayer("Main")->deleteGlEntity(scene->glGraph());
    scene->getLayer("Main")->addGlEntity(scene->glGraph(), "graph");
  }
}

bool NodeLinkDiagramView::hasHulls() const {
  return _hasHulls;
}

PLUGIN(NodeLinkDiagramView)
