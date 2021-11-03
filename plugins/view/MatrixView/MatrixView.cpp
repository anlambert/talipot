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

#include <QMenu>

#include "MatrixView.h"
#include "PropertyValuesDispatcher.h"
#include "GlMatrixBackgroundGrid.h"
#include "MatrixViewQuickAccessBar.h"

#include <talipot/TlpQtTools.h>
#include <talipot/GlGraph.h>
#include <talipot/GlyphManager.h>
#include <talipot/ParametricCurves.h>
#include <talipot/ViewSettings.h>

using namespace std;

namespace tlp {

MatrixView::MatrixView(const PluginContext *)
    : NodeLinkDiagramView(), _bar(nullptr), _matrixGraph(nullptr),
      _graphEntitiesToDisplayedNodes(nullptr), _displayedNodesToGraphEntities(nullptr),
      _displayedEdgesToGraphEdges(nullptr), _displayedNodesAreNodes(nullptr), _dispatcher(nullptr),
      _configurationWidget(nullptr), _mustUpdateSizes(false), _mustUpdateLayout(false),
      _isOriented(false) {}

MatrixView::~MatrixView() {
  deleteDisplayedGraph();
}

QuickAccessBar *MatrixView::getQuickAccessBarImpl() {
  _bar = new MatrixViewQuickAccessBar(_configurationWidget);
  connect(_bar, &MatrixViewQuickAccessBar::settingsChanged, this, &MatrixView::applySettings);
  return _bar;
}

void MatrixView::setState(const DataSet &ds) {

  clearRedrawTriggers();

  setOverviewVisible(true);

  if (!_configurationWidget) {
    _configurationWidget = new MatrixViewConfigurationWidget();
    connect(_configurationWidget, &MatrixViewConfigurationWidget::changeBackgroundColor, this,
            &MatrixView::setBackgroundColor);
    connect(_configurationWidget, &MatrixViewConfigurationWidget::metricSelected, this,
            &MatrixView::setOrderingMetric);
    connect(_configurationWidget, &MatrixViewConfigurationWidget::setGridDisplayMode, this,
            &MatrixView::setGridDisplayMode);
    connect(_configurationWidget, &MatrixViewConfigurationWidget::showEdges, this,
            &MatrixView::showEdges);
    connect(_configurationWidget, &MatrixViewConfigurationWidget::enableEdgeColorInterpolation,
            this, &MatrixView::enableEdgeColorInterpolation);
    connect(_configurationWidget, &MatrixViewConfigurationWidget::updateOriented, this,
            &MatrixView::setOriented);
  }

  _configurationWidget->setGraph(graph());

  initDisplayedGraph();
  registerTriggers();

  bool status = true;
  ds.get("show Edges", status);
  showEdges(status);
  _configurationWidget->setDisplayEdges(status);

  ds.get("ascending order", status);
  _configurationWidget->setAscendingOrder(status);

  Color c = getGlWidget()->getScene()->getBackgroundColor();
  ds.get("Background Color", c);
  _configurationWidget->setBackgroundColor(tlp::colorToQColor(c));

  unsigned grid = 0;
  ds.get("Grid mode", grid);
  _configurationWidget->setgridmode(grid);

  int orderingindex = 0;
  ds.get("ordering", orderingindex);
  _configurationWidget->setOrderingProperty(orderingindex);

  status = false;
  ds.get("oriented", status);
  _isOriented = status;
  _configurationWidget->setOriented(status);
  status = false;

  ds.get("edge color interpolation", status);
  enableEdgeColorInterpolation(status);
  _configurationWidget->setEdgeColorInterpolation(status);

  bool quickAccessBarVisible = false;

  if (ds.get<bool>("quickAccessBarVisible", quickAccessBarVisible)) {
    needQuickAccessBar = true;
    setQuickAccessBarVisible(quickAccessBarVisible);
  } else { // display quickaccessbar
    setQuickAccessBarVisible(true);
  }
}

void MatrixView::showEdges(bool show) {
  getGlWidget()->getGlGraphRenderingParameters().setDisplayEdges(show);
  emit drawNeeded();
}

void MatrixView::enableEdgeColorInterpolation(bool flag) {
  getGlWidget()->getGlGraphRenderingParameters().setEdgeColorInterpolate(flag);
  emit drawNeeded();
}

void MatrixView::setOriented(bool flag) {
  if (flag != _isOriented) {
    _isOriented = flag;
    Observable::holdObservers();

    if (_isOriented) {
      for (auto e : graph()->edges()) {
        // delete the second node mapping the current edge
        vector<int> edgeNodes = _graphEntitiesToDisplayedNodes->getEdgeValue(e);
        _matrixGraph->delNode(node(edgeNodes[1]));
        edgeNodes.resize(1);
        _graphEntitiesToDisplayedNodes->setEdgeValue(e, edgeNodes);
      }
    } else {
      for (auto e : graph()->edges()) {
        // must add the symmetric node
        vector<int> edgeNodes = _graphEntitiesToDisplayedNodes->getEdgeValue(e);
        edgeNodes.push_back(_matrixGraph->addNode().id);
        _graphEntitiesToDisplayedNodes->setEdgeValue(e, edgeNodes);

        // layout and shape will be updated in updateLayout method
        // but other view properties must be set now
        for (const string &strProp : _sourceToTargetProperties) {
          PropertyInterface *prop = _matrixGraph->getProperty(strProp);
          prop->setNodeStringValue(node(edgeNodes[1]),
                                   prop->getNodeStringValue(node(edgeNodes[0])));
        }
      }
    }

    Observable::unholdObservers();
    emit drawNeeded();
  }
}

void MatrixView::graphChanged(Graph *) {
  setState(DataSet());
}

DataSet MatrixView::state() const {
  DataSet ds;
  ds.set("show Edges", getGlWidget()->getGlGraphRenderingParameters().isDisplayEdges());
  ds.set("edge color interpolation",
         getGlWidget()->getGlGraphRenderingParameters().isEdgeColorInterpolate());
  ds.set("ascending order", _configurationWidget->ascendingOrder());
  ds.set("Grid mode", _configurationWidget->gridDisplayMode());
  ds.set("Background Color", getGlWidget()->getScene()->getBackgroundColor());
  ds.set("ordering", _configurationWidget->orderingProperty());
  ds.set("oriented", _isOriented);

  if (needQuickAccessBar) {
    ds.set("quickAccessBarVisible", quickAccessBarVisible());
  }

  return ds;
}

QList<QWidget *> MatrixView::configurationWidgets() const {
  return QList<QWidget *>() << _configurationWidget;
}

void MatrixView::fillContextMenu(QMenu *menu, const QPointF &point) {
  GlView::fillContextMenu(menu, point);
  // Check if a node/edge is under the mouse pointer
  SelectedEntity entity;

  if (getGlWidget()->pickNodesEdges(point.x(), point.y(), entity)) {
    menu->addSeparator();
    isNode = entity.getEntityType() == SelectedEntity::NODE_SELECTED;
    itemId = entity.getComplexEntityId();
    QString sId = QString::number(itemId);

    if (isNode) {
      if (!_displayedNodesAreNodes->getNodeValue(node(itemId))) {
        isNode = false;
      }

      itemId = _displayedNodesToGraphEntities->getNodeValue(node(itemId));
    } else {
      itemId = _displayedEdgesToGraphEdges->getEdgeValue(edge(itemId));
    }

    menu->addAction((isNode ? "Node #" : "Edge #") + sId)->setEnabled(false);

    menu->addSeparator();

    auto genEltToolTip = [this, sId](const QString &s) {
      return s + (isNode ? " node #" : " edge #") + sId;
    };

    QAction *action = menu->addAction("Toggle selection", [this] { addRemoveItemToSelection(); });
    action->setToolTip(genEltToolTip("Invert the selection of the"));
    action = menu->addAction("Select", this, &MatrixView::selectItem);
    action->setToolTip(genEltToolTip("Select the"));
    action = menu->addAction("Delete", this, &MatrixView::deleteItem);
    action->setToolTip(genEltToolTip("Delete the"));
  }
}

void MatrixView::draw() {
  if (_mustUpdateSizes) {
    normalizeSizes();
    _mustUpdateSizes = false;
  }

  if (_mustUpdateLayout) {
    updateLayout();
    _mustUpdateLayout = false;
  }

  getGlWidget()->draw();
}

void MatrixView::refresh() {
  getGlWidget()->redraw();
}

void MatrixView::deleteDisplayedGraph() {
  for (auto *obs : triggers()) {
    removeRedrawTrigger(obs);
  }

  delete _matrixGraph;
  _matrixGraph = nullptr;
  delete _graphEntitiesToDisplayedNodes;
  _graphEntitiesToDisplayedNodes = nullptr;
  delete _displayedNodesToGraphEntities;
  _displayedNodesToGraphEntities = nullptr;
  delete _displayedEdgesToGraphEdges;
  _displayedEdgesToGraphEdges = nullptr;
  delete _displayedNodesAreNodes;
  _displayedNodesAreNodes = nullptr;
  delete _dispatcher;
  _dispatcher = nullptr;
}

void MatrixView::initDisplayedGraph() {
  _mustUpdateLayout = true;
  _mustUpdateSizes = true;

  deleteDisplayedGraph();

  if (graph() == nullptr) {
    return;
  }

  _matrixGraph = newGraph();
  _matrixGraph->reserveNodes(2 * (graph()->numberOfNodes() + graph()->numberOfEdges()));
  _matrixGraph->reserveEdges(graph()->numberOfEdges());

  _graphEntitiesToDisplayedNodes = new IntegerVectorProperty(graph());
  _displayedNodesAreNodes = new BooleanProperty(_matrixGraph);
  _displayedNodesToGraphEntities = new IntegerProperty(_matrixGraph);
  _displayedEdgesToGraphEdges = new IntegerProperty(_matrixGraph);
  createScene(_matrixGraph, DataSet());

  Observable::holdObservers();
  for (auto n : graph()->nodes()) {
    addNode(graph(), n);
  }

  for (auto e : graph()->edges()) {
    addEdge(graph(), e);
  }
  Observable::unholdObservers();

  GlGraphInputData *inputData = getGlWidget()->getGlGraphInputData();
  _sourceToTargetProperties.clear();
  _sourceToTargetProperties.insert(inputData->colors()->getName());
  _sourceToTargetProperties.insert(inputData->shapes()->getName());
  _sourceToTargetProperties.insert(inputData->labels()->getName());
  _sourceToTargetProperties.insert(inputData->fonts()->getName());
  _sourceToTargetProperties.insert(inputData->fontSizes()->getName());
  _sourceToTargetProperties.insert(inputData->borderWidths()->getName());
  _sourceToTargetProperties.insert(inputData->borderColors()->getName());
  _sourceToTargetProperties.insert(inputData->labelColors()->getName());
  _sourceToTargetProperties.insert(inputData->selection()->getName());
  _sourceToTargetProperties.insert(inputData->textures()->getName());
  set<string> targetToSourceProperties;
  targetToSourceProperties.insert(inputData->selection()->getName());
  _dispatcher = new PropertyValuesDispatcher(
      graph(), _matrixGraph, _sourceToTargetProperties, targetToSourceProperties,
      _graphEntitiesToDisplayedNodes, _displayedNodesAreNodes, _displayedNodesToGraphEntities,
      _displayedEdgesToGraphEdges, _edgesMap);

  GlGraphRenderingParameters &renderingParameters = getGlWidget()->getGlGraphRenderingParameters();
  renderingParameters.setLabelScaled(true);
  renderingParameters.setLabelsDensity(100);

  _configurationWidget->setBackgroundColor(
      colorToQColor(getGlWidget()->getScene()->getBackgroundColor()));
  addGridBackground();

  if (_mustUpdateSizes) {
    normalizeSizes();
    _mustUpdateSizes = false;
  }

  if (_mustUpdateLayout) {
    updateLayout();
    _mustUpdateLayout = false;
  }

  centerView();
}

void MatrixView::normalizeSizes(double maxVal) {
  if (graph() == nullptr) {
    return;
  }

  float maxWidth = FLT_MIN, maxHeight = FLT_MIN;
  SizeProperty *originalSizes = getGlWidget()->getGlGraphInputData()->sizes();
  SizeProperty *matrixSizes = getGlWidget()->getGlGraphInputData()->sizes();

  for (auto n : graph()->nodes()) {
    const Size &s = originalSizes->getNodeValue(n);
    maxWidth = max<float>(maxWidth, s[0]);
    maxHeight = max<float>(maxHeight, s[1]);
  }

  Observable::holdObservers();
  for (auto n : _matrixGraph->nodes()) {
    if (!_displayedNodesAreNodes->getNodeValue(n)) {
      continue;
    }

    const Size &s =
        originalSizes->getNodeValue(node(_displayedNodesToGraphEntities->getNodeValue(n)));
    matrixSizes->setNodeValue(n, Size(s[0] * maxVal / maxWidth, s[1] * maxVal / maxHeight, 1));
  }
  Observable::unholdObservers();
}

void MatrixView::addNode(tlp::Graph *, const tlp::node n) {
  _mustUpdateLayout = true;
  _mustUpdateSizes = true;

  vector<int> nodeToDisplayedNodes;
  nodeToDisplayedNodes.reserve(2);

  for (int i = 0; i < 2; ++i) {
    node dispNode = _matrixGraph->addNode();
    nodeToDisplayedNodes.push_back(dispNode);
    _displayedNodesToGraphEntities->setNodeValue(dispNode, n.id);
    _displayedNodesAreNodes->setNodeValue(dispNode, true);
  }

  _graphEntitiesToDisplayedNodes->setNodeValue(n, nodeToDisplayedNodes);
}

void MatrixView::addEdge(tlp::Graph *g, const tlp::edge e) {
  _mustUpdateLayout = true;
  _mustUpdateSizes = true;

  vector<int> edgeToDisplayedNodes;
  edgeToDisplayedNodes.reserve(2);

  for (int i = 0; i < 2; ++i) {
    node dispEdge = _matrixGraph->addNode();
    edgeToDisplayedNodes.push_back(dispEdge);
    _displayedNodesToGraphEntities->setNodeValue(dispEdge, e.id);
    _displayedNodesAreNodes->setNodeValue(dispEdge, false);
  }

  _graphEntitiesToDisplayedNodes->setEdgeValue(e, edgeToDisplayedNodes);

  const auto &[src, tgt] = g->ends(e);

  node dispSrc = node(_graphEntitiesToDisplayedNodes->getNodeValue(src)[0]);

  node dispTgt = node(_graphEntitiesToDisplayedNodes->getNodeValue(tgt)[0]);

  edge dispEdge = _matrixGraph->addEdge(dispSrc, dispTgt);

  _edgesMap[e] = dispEdge;

  _displayedEdgesToGraphEdges->setEdgeValue(dispEdge, e.id);

  ColorProperty *originalColors = graph()->getColorProperty("viewColor");
  ColorProperty *colors = getGlWidget()->getGlGraphInputData()->colors();

  colors->setEdgeValue(dispEdge, originalColors->getEdgeValue(e));
}

void MatrixView::treatEvent(const Event &message) {
  const auto *graphEvent = dynamic_cast<const GraphEvent *>(&message);

  if (graphEvent) {
    if (graphEvent->getType() == GraphEvent::TLP_ADD_EDGE) {
      addEdge(graphEvent->getGraph(), graphEvent->getEdge());
    }

    if (graphEvent->getType() == GraphEvent::TLP_DEL_NODE) {
      delNode(graphEvent->getGraph(), graphEvent->getNode());
    }

    if (graphEvent->getType() == GraphEvent::TLP_DEL_EDGE) {
      delEdge(graphEvent->getGraph(), graphEvent->getEdge());
    }
  }
}

void MatrixView::delNode(tlp::Graph *, const tlp::node n) {
  _mustUpdateLayout = true;
  _mustUpdateSizes = true;

  const vector<int> &vect = _graphEntitiesToDisplayedNodes->getNodeValue(n);

  for (auto id : vect) {
    _matrixGraph->delNode(node(id));
  }
}

void MatrixView::delEdge(tlp::Graph *, const tlp::edge e) {
  _mustUpdateLayout = true;
  _mustUpdateSizes = true;

  const vector<int> &vect = _graphEntitiesToDisplayedNodes->getEdgeValue(e);

  for (auto id : vect) {
    _matrixGraph->delNode(node(id));
  }

  _matrixGraph->delEdge(_edgesMap[e]);
  _edgesMap.remove(e);
}

template <typename PROPTYPE>
struct AscendingPropertySorter {
  PROPTYPE *prop;
  AscendingPropertySorter(PropertyInterface *_prop) : prop(static_cast<PROPTYPE *>(_prop)) {}
  bool operator()(node a, node b) {
    return prop->getNodeValue(a) < prop->getNodeValue(b);
  }
};

template <typename PROPTYPE>
struct DescendingPropertySorter {
  PROPTYPE *prop;
  DescendingPropertySorter(PropertyInterface *_prop) : prop(static_cast<PROPTYPE *>(_prop)) {}
  bool operator()(node a, node b) {
    return prop->getNodeValue(a) > prop->getNodeValue(b);
  }
};

struct DescendingIdSorter {
  bool operator()(node a, node b) {
    return a.id > b.id;
  }
};

void MatrixView::updateNodesOrder() {
  _orderedNodes.clear();
  _orderedNodes.resize(graph()->numberOfNodes());
  int i = 0;
  for (auto n : graph()->nodes()) {
    _orderedNodes[i++] = n;
  }

  if (graph()->existProperty(_orderingMetricName)) {
    PropertyInterface *pi = graph()->getProperty(_orderingMetricName);

    if (pi->getTypename() == "double") {
      if (_configurationWidget->ascendingOrder()) {
        sort(_orderedNodes.begin(), _orderedNodes.end(),
             AscendingPropertySorter<DoubleProperty>(pi));
      } else {
        sort(_orderedNodes.begin(), _orderedNodes.end(),
             DescendingPropertySorter<DoubleProperty>(pi));
      }

    } else if (pi->getTypename() == "int") {
      if (_configurationWidget->ascendingOrder()) {
        sort(_orderedNodes.begin(), _orderedNodes.end(),
             AscendingPropertySorter<IntegerProperty>(pi));
      } else {
        sort(_orderedNodes.begin(), _orderedNodes.end(),
             DescendingPropertySorter<IntegerProperty>(pi));
      }
    } else if (pi->getTypename() == "string") {
      if (_configurationWidget->ascendingOrder()) {
        sort(_orderedNodes.begin(), _orderedNodes.end(),
             AscendingPropertySorter<StringProperty>(pi));
      } else {
        sort(_orderedNodes.begin(), _orderedNodes.end(),
             DescendingPropertySorter<StringProperty>(pi));
      }
    }
  } else if (!_configurationWidget->ascendingOrder()) {
    sort(_orderedNodes.begin(), _orderedNodes.end(), DescendingIdSorter());
  }
}

void MatrixView::updateLayout() {
  if (graph() == nullptr) {
    return;
  }

  holdObservers();
  updateNodesOrder();

  LayoutProperty *layout = getGlWidget()->getGlGraphInputData()->layout();
  Coord horiz = {1, 0, 0}, vert = {0, -1, 0};
  IntegerProperty *position = getGlWidget()->getGlGraphInputData()->labelPositions();

  for (auto on : _orderedNodes) {
    const vector<int> &dispNodes = _graphEntitiesToDisplayedNodes->getNodeValue(node(on));
    layout->setNodeValue(node(dispNodes[0]), horiz);
    position->setNodeValue(node(dispNodes[0]), LabelPosition::Top);
    layout->setNodeValue(node(dispNodes[1]), vert);
    position->setNodeValue(node(dispNodes[1]), LabelPosition::Left);
    horiz[0] += 1;
    vert[1] -= 1;
  }

  IntegerProperty *shapes = getGlWidget()->getGlGraphInputData()->shapes();
  int shape = GlyphManager::glyphId("2D - Square");
  for (auto e : graph()->edges()) {
    const auto &[src, tgt] = graph()->ends(e);
    const vector<int> &srcNodes = _graphEntitiesToDisplayedNodes->getNodeValue(src),
                      &tgtNodes = _graphEntitiesToDisplayedNodes->getNodeValue(tgt),
                      &edgeNodes = _graphEntitiesToDisplayedNodes->getEdgeValue(e);

    // 0 => horizontal line, 1 => vertical line
    Coord src0 = layout->getNodeValue(node(srcNodes[0])),
          tgt0 = layout->getNodeValue(node(tgtNodes[0])),
          src1 = layout->getNodeValue(node(srcNodes[1])),
          tgt1 = layout->getNodeValue(node(tgtNodes[1]));

    layout->setNodeValue(node(edgeNodes[0]), Coord(tgt0[0], src1[1], 0));
    shapes->setNodeValue(node(edgeNodes[0]), shape);

    if (!_isOriented) {
      layout->setNodeValue(node(edgeNodes[1]), Coord(src0[0], tgt1[1], 0));
      shapes->setNodeValue(node(edgeNodes[1]), shape);
    }
  }

  for (auto e : _matrixGraph->edges()) {
    const auto &[src, tgt] = _matrixGraph->ends(e);

    auto srcPos = layout->getNodeValue(src);
    auto tgtPos = layout->getNodeValue(tgt);
    float xMax = max(srcPos[0], tgtPos[0]);
    float xMin = min(srcPos[0], tgtPos[0]);
    float dist = (xMax - xMin);
    std::vector<Coord> bends(4);
    bends[0] = srcPos;
    bends[1] = srcPos;
    bends[1][1] += dist / 3. + 1.;
    bends[2] = tgtPos;
    bends[2][1] += dist / 3. + 1.;
    bends[3] = tgtPos;
    vector<Coord> curvePoints;
    computeBezierPoints(bends, curvePoints, 20);
    layout->setEdgeValue(e, curvePoints);
  }

  unholdObservers();
}

void MatrixView::setBackgroundColor(QColor c) {
  getGlWidget()->getScene()->setBackgroundColor(QColorToColor(c));
  emit drawNeeded();
}

void MatrixView::setOrderingMetric(const std::string &name) {
  if (!name.empty() && !graph()->existProperty(name)) {
    return;
  }

  if (graph()->existProperty(_orderingMetricName)) {
    graph()->getProperty(_orderingMetricName)->removeObserver(this);
  }

  _orderingMetricName = name;

  if (graph()->existProperty(name)) {
    graph()->getProperty(name)->addObserver(this);
  }

  _mustUpdateLayout = true;
  emit drawNeeded();
}

void MatrixView::setGridDisplayMode() {
  emit drawNeeded();
}

void MatrixView::registerTriggers() {
  for (auto *obs : triggers()) {
    removeRedrawTrigger(obs);
  }

  if (graph()) {
    addRedrawTrigger(graph());

    for (PropertyInterface *prop : _matrixGraph->getObjectProperties()) {
      addRedrawTrigger(prop);
    }
  }
}

void MatrixView::addGridBackground() {
  removeGridBackground();
  GlLayer *backgroundLayer = getGlWidget()->getScene()->getLayer("MatrixView_Background");
  backgroundLayer->addGlEntity(new GlMatrixBackgroundGrid(this), "MatrixView_backgroundGrid");
}

void MatrixView::removeGridBackground() {
  GlLayer *backgroundLayer = getGlWidget()->getScene()->getLayer("MatrixView_Background");

  if (!backgroundLayer) {
    backgroundLayer = new GlLayer(
        "MatrixView_Background", &(getGlWidget()->getScene()->getLayer("Main")->getCamera()), true);
    backgroundLayer->clear();
    getGlWidget()->getScene()->addExistingLayerBefore(backgroundLayer, "Main");
  } else {
    GlEntity *entity = backgroundLayer->findGlEntity("MatrixView_backgroundGrid");
    delete entity;
  }
}

void MatrixView::applySettings() {
  _mustUpdateLayout = true;
  emit drawNeeded();
}

PLUGIN(MatrixView)
}
