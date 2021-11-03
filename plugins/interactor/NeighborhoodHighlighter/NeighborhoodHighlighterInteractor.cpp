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

#include <QApplication>
#include <QTimeLine>
#include <QWheelEvent>

#include <talipot/MouseInteractors.h>
#include <talipot/GlCircle.h>
#include <talipot/QtGlSceneZoomAndPanAnimator.h>
#include <talipot/NodeLinkDiagramView.h>

#include "NeighborhoodHighlighterInteractor.h"
#include "../../utils/PluginNames.h"
#include "../../utils/InteractorIcons.h"

using namespace std;

namespace tlp {

class GraphLayoutMorphing : public AdditionalGlSceneAnimation {

public:
  ~GraphLayoutMorphing() override = default;
  GraphLayoutMorphing(Graph *graph, LayoutProperty *srcLayout, LayoutProperty *destLayout,
                      LayoutProperty *viewLayout)
      : graph(graph), srcLayout(srcLayout), destLayout(destLayout), viewLayout(viewLayout) {}

  void animationStep(int animationStep) override {
    for (auto n : graph->nodes()) {
      const Coord &startPos = srcLayout->getNodeValue(n);
      const Coord &endPos = destLayout->getNodeValue(n);
      viewLayout->setNodeValue(
          n, startPos + (animationStep / float(nbAnimationSteps) * (endPos - startPos)));
    }

    for (auto e : graph->edges()) {
      const vector<Coord> &srcBends = srcLayout->getEdgeValue(e);
      const vector<Coord> &destBends = destLayout->getEdgeValue(e);
      vector<Coord> newBends;

      for (size_t i = 0; i < destBends.size(); ++i) {
        newBends.push_back(
            srcBends[i] + (animationStep / float(nbAnimationSteps) * (destBends[i] - srcBends[i])));
      }

      viewLayout->setEdgeValue(e, newBends);
    }
  }

private:
  Graph *graph;
  LayoutProperty *srcLayout, *destLayout, *viewLayout;
};

class NeighborNodesEdgeLengthOrdering {

public:
  NeighborNodesEdgeLengthOrdering(node centralNode, LayoutProperty *layout)
      : centralNode(centralNode), layout(layout) {}

  bool operator()(tlp::node n1, tlp::node n2) const {
    Coord centralNodeCoord = layout->getNodeValue(centralNode);
    Coord n1Coord = layout->getNodeValue(n1);
    Coord n2Coord = layout->getNodeValue(n2);
    float centralToN1Dist = centralNodeCoord.dist(n1Coord);
    float centralToN2Dist = centralNodeCoord.dist(n2Coord);
    return centralToN1Dist < centralToN2Dist;
  }

private:
  node centralNode;
  LayoutProperty *layout;
};

class MouseEventDiscardFilter : public QObject {

public:
  bool eventFilter(QObject *, QEvent *event) override {
    return event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonDblClick ||
           event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease;
  }
};

const unsigned char maxCircleAlphaValue = 200;

PLUGIN(NeighborhoodHighlighterInteractor)

NeighborhoodHighlighterInteractor::NeighborhoodHighlighterInteractor(const PluginContext *)
    : GLInteractorComposite(interactorIcon(InteractorType::NeighborhoodHighlighting),
                            "Highlight node neighborhood"),
      configWidget(nullptr) {}

NeighborhoodHighlighterInteractor::~NeighborhoodHighlighterInteractor() {
  delete configWidget;
}

bool NeighborhoodHighlighterInteractor::isCompatible(const std::string &viewName) const {
  return ((viewName == NodeLinkDiagramView::viewName) ||
          (viewName == ViewName::HistogramViewName)
          // Interactor does not work
          //||(viewName==ViewName::MatrixViewName)
          || (viewName == ViewName::ScatterPlot2DViewName));
}

void NeighborhoodHighlighterInteractor::construct() {
  configWidget = new NeighborhoodHighlighterConfigWidget();

  push_back(new MousePanNZoomNavigator());
  auto *neighborhoodHighlighter = new NeighborhoodHighlighter();
  neighborhoodHighlighter->setConfigWidget(configWidget);
  push_back(neighborhoodHighlighter);
}

NeighborhoodHighlighter::NeighborhoodHighlighter()
    : originalGraph(nullptr), originalGlGraph(nullptr), neighborhoodGraph(nullptr),
      glNeighborhoodGraph(nullptr), neighborhoodGraphLayout(nullptr),
      neighborhoodGraphCircleLayout(nullptr), neighborhoodGraphOriginalLayout(nullptr),
      neighborhoodGraphColors(nullptr), neighborhoodGraphBackupColors(nullptr),
      centralNodeLocked(false), circleLayoutSet(false), glWidget(nullptr), neighborhoodDist(1),
      configWidget(nullptr), circleAlphaValue(maxCircleAlphaValue), startAlpha(0), endAlpha(255),
      nbAnimSteps(0) {}

NeighborhoodHighlighter::NeighborhoodHighlighter(
    const NeighborhoodHighlighter &neighborhoodHighlighter)
    : neighborhoodGraph(nullptr), glNeighborhoodGraph(nullptr), neighborhoodGraphLayout(nullptr),
      neighborhoodGraphCircleLayout(nullptr), neighborhoodGraphOriginalLayout(nullptr),
      neighborhoodGraphColors(nullptr), neighborhoodGraphBackupColors(nullptr),
      centralNodeLocked(false), circleLayoutSet(false), neighborhoodDist(1),
      circleAlphaValue(maxCircleAlphaValue) {
  configWidget = neighborhoodHighlighter.configWidget;
  connect(configWidget, &NeighborhoodHighlighterConfigWidget::updateNeighborhoodGraph, this,
          &NeighborhoodHighlighter::updateNeighborhoodGraph);
}

NeighborhoodHighlighter::~NeighborhoodHighlighter() {
  cleanupNeighborhoodGraph();
}

void NeighborhoodHighlighter::viewChanged(View *view) {
  originalGraph = nullptr;

  if (view == nullptr) {
    glWidget = nullptr;
    return;
  }

  auto *glView = static_cast<GlView *>(view);
  glWidget = glView->glWidget();
}

bool NeighborhoodHighlighter::eventFilter(QObject *, QEvent *e) {

  checkIfGraphHasChanged();

  if (originalGraph == nullptr) {
    originalGraph = glWidget->scene()->getGlGraph()->getGraph();
    originalGlGraph = glWidget->scene()->getGlGraph();

    if (!glWidget->hasMouseTracking()) {
      glWidget->setMouseTracking(true);
    }

    for (PropertyInterface *prop : originalGraph->getObjectProperties()) {
      if (prop->getTypename() == "double") {
        configWidget->setPropertyToUse(prop->getName());
      }
    }
  }

  SelectedEntity selectedEntity;

  if (e->type() == QEvent::Wheel && centralNodeLocked && !circleLayoutSet) {
    auto *we = static_cast<QWheelEvent *>(e);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    auto pos = we->position();
#else
    auto pos = we->pos();
#endif
    if (selectInAugmentedDisplayGraph(pos.x(), pos.y(), selectedEntity) &&
        selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED) {
      if (selectedEntity.getComplexEntityId() == neighborhoodGraphCentralNode.id) {
        int numSteps = we->angleDelta().y() / 120;
        neighborhoodDist += numSteps;

        if (neighborhoodDist < 1) {
          neighborhoodDist = 1;
        }

        configWidget->setCurrentMaxDistanceForReachableNodes(neighborhoodDist);
        neighborhoodGraph->updateWithDistance(neighborhoodDist);
        updateNeighborhoodGraphLayoutAndColors();
        delete glNeighborhoodGraph;
        updateGlNeighborhoodGraph();
        glWidget->redraw();
        return true;
      }
    }

  } else if (e->type() == QEvent::MouseMove) {
    auto *qMouseEv = static_cast<QMouseEvent *>(e);

    if (!centralNodeLocked) {
      node tmpNode = selectNodeInOriginalGraph(glWidget, qMouseEv->pos().x(), qMouseEv->pos().y());

      if (tmpNode != selectedNode) {
        buildNeighborhoodGraph(tmpNode, originalGraph);
        selectedNode = tmpNode;
      }
      if (tmpNode.isValid()) {
        glWidget->setCursor(Qt::PointingHandCursor);
      } else {
        glWidget->setCursor(QCursor());
      }
    } else {
      *neighborhoodGraphColors = *neighborhoodGraphBackupColors;

      if (selectInAugmentedDisplayGraph(qMouseEv->pos().x(), qMouseEv->pos().y(), selectedEntity) &&
          selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED) {
        if (selectedEntity.getComplexEntityId() != neighborhoodGraphCentralNode.id) {
          neighborhoodGraphColors->setNodeValue(node(selectedEntity.getComplexEntityId()),
                                                Color(0, 255, 0));
        } else {
          neighborhoodGraphColors->setNodeValue(node(selectedEntity.getComplexEntityId()),
                                                Color(0, 0, 255));
        }
      }

      if (selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED) {
        selectedNeighborNode = node(selectedEntity.getComplexEntityId());
        glWidget->setCursor(Qt::PointingHandCursor);
      } else {
        selectedNeighborNode = node();
        glWidget->setCursor(QCursor());
      }
    }

    glWidget->redraw();
    return true;
  } else if (e->type() == QEvent::MouseButtonPress &&
             static_cast<QMouseEvent *>(e)->buttons() == Qt::LeftButton) {
    if (neighborhoodGraphCentralNode.isValid() && !centralNodeLocked) {
      centralNodeLocked = true;
    } else if (neighborhoodGraphCentralNode.isValid() && centralNodeLocked && !circleLayoutSet &&
               selectedNeighborNode.isValid() &&
               selectedNeighborNode == neighborhoodGraphCentralNode && neighborhoodDist == 1) {
      computeNeighborhoodGraphCircleLayout();
      computeNeighborhoodGraphBoundingBoxes();
      auto *layoutMorphing =
          new GraphLayoutMorphing(neighborhoodGraph, neighborhoodGraphOriginalLayout,
                                  neighborhoodGraphCircleLayout, neighborhoodGraphLayout);
      performZoomAndPan(neighborhoodGraphCircleLayoutBB, layoutMorphing);
      delete layoutMorphing;
      circleLayoutSet = true;
    } else if (neighborhoodGraphCentralNode.isValid() && circleLayoutSet &&
               !selectedNeighborNode.isValid()) {
      circleLayoutSet = false;
      centralNodeLocked = false;
      neighborhoodGraphCentralNode = node();
      neighborhoodDist = 1;
      configWidget->setCurrentMaxDistanceForReachableNodes(neighborhoodDist);
      glWidget->redraw();
    } else if (selectedNeighborNode.isValid()) {
      if (circleLayoutSet) {
        if (selectedNeighborNode == neighborhoodGraphCentralNode) {
          auto *layoutMorphing =
              new GraphLayoutMorphing(neighborhoodGraph, neighborhoodGraphCircleLayout,
                                      neighborhoodGraphOriginalLayout, neighborhoodGraphLayout);
          performZoomAndPan(neighborhoodGraphOriginalLayoutBB, layoutMorphing);
          delete layoutMorphing;
          circleLayoutSet = false;
        } else {
          node currentCentralNode = neighborhoodGraphCentralNode;
          buildNeighborhoodGraph(selectedNeighborNode, originalGraph);
          computeNeighborhoodGraphCircleLayout();
          computeNeighborhoodGraphBoundingBoxes();
          BoundingBox destBB;

          if (configWidget->bringAndGoAnimation1()) {
            destBB = neighborhoodGraphOriginalLayoutBB;
          } else {
            destBB = neighborhoodGraphCircleLayoutBB;
          }

          buildNeighborhoodGraph(currentCentralNode, originalGraph);
          computeNeighborhoodGraphCircleLayout();
          computeNeighborhoodGraphBoundingBoxes();
          *neighborhoodGraphLayout = *neighborhoodGraphCircleLayout;
          auto *layoutMorphing =
              new GraphLayoutMorphing(neighborhoodGraph, neighborhoodGraphCircleLayout,
                                      neighborhoodGraphOriginalLayout, neighborhoodGraphLayout);
          performZoomAndPan(destBB, layoutMorphing);
          delete layoutMorphing;

          if (configWidget->bringAndGoAnimation1()) {
            morphCircleAlpha(maxCircleAlphaValue, 0);
          }

          buildNeighborhoodGraph(selectedNeighborNode, originalGraph);
          computeNeighborhoodGraphCircleLayout();
          computeNeighborhoodGraphBoundingBoxes();

          if (configWidget->bringAndGoAnimation1()) {
            morphCircleAlpha(0, maxCircleAlphaValue);
          }

          layoutMorphing =
              new GraphLayoutMorphing(neighborhoodGraph, neighborhoodGraphOriginalLayout,
                                      neighborhoodGraphCircleLayout, neighborhoodGraphLayout);
          performZoomAndPan(neighborhoodGraphCircleLayoutBB, layoutMorphing);
          delete layoutMorphing;
        }
      } else if (selectedNeighborNode != neighborhoodGraphCentralNode) {
        neighborhoodDist = 1;
        configWidget->setCurrentMaxDistanceForReachableNodes(neighborhoodDist);
        buildNeighborhoodGraph(selectedNeighborNode, originalGraph);
        glWidget->redraw();
      }
    } else {
      centralNodeLocked = false;
      circleLayoutSet = false;
      neighborhoodDist = 1;
      configWidget->setCurrentMaxDistanceForReachableNodes(neighborhoodDist);
      neighborhoodGraphCentralNode = node();
      glWidget->setCursor(QCursor());
      glWidget->redraw();
    }

    return true;
  }

  return false;
}
node NeighborhoodHighlighter::selectNodeInOriginalGraph(GlWidget *glWidget, int x, int y) {
  node n;
  glWidget->makeCurrent();
  vector<SelectedEntity> selectedElements;
  glWidget->scene()->selectEntities(
      static_cast<RenderingEntitiesFlag>(RenderingNodes | RenderingWithoutRemove),
      glWidget->screenToViewport(x - 1), glWidget->screenToViewport(y - 1),
      glWidget->screenToViewport(3), glWidget->screenToViewport(3), nullptr, selectedElements);

  if (!selectedElements.empty()) {
    n = node(selectedElements[0].getComplexEntityId());
  }

  return n;
}

bool NeighborhoodHighlighter::selectInAugmentedDisplayGraph(const int x, const int y,
                                                            SelectedEntity &selectedEntity) {
  GlLayer *l = glWidget->scene()->getLayer("Main");
  GlGraph *glGraph = static_cast<GlGraph *>(l->findGlEntity("graph"));
  l->deleteGlEntity("graph");
  l->addGlEntity(glNeighborhoodGraph, "graph");
  bool ret = glWidget->pickNodesEdges(x, y, selectedEntity);
  l->deleteGlEntity("graph");
  l->addGlEntity(glGraph, "graph");
  return ret;
}

void NeighborhoodHighlighter::performZoomAndPan(const BoundingBox &destBB,
                                                AdditionalGlSceneAnimation *additionalAnimation) {
  MouseEventDiscardFilter medf;
  glWidget->installEventFilter(&medf);
  QtGlSceneZoomAndPanAnimator sceneZoomAndPan(glWidget, destBB);

  if (additionalAnimation != nullptr) {
    sceneZoomAndPan.setAdditionalGlSceneAnimation(additionalAnimation);
  }

  sceneZoomAndPan.animateZoomAndPan();
  glWidget->removeEventFilter(&medf);
}

void NeighborhoodHighlighter::cleanupNeighborhoodGraph() {
  delete glNeighborhoodGraph;
  glNeighborhoodGraph = nullptr;
  delete neighborhoodGraph;
  neighborhoodGraph = nullptr;
  delete neighborhoodGraphLayout;
  neighborhoodGraphLayout = nullptr;
  delete neighborhoodGraphCircleLayout;
  neighborhoodGraphCircleLayout = nullptr;
  delete neighborhoodGraphOriginalLayout;
  neighborhoodGraphOriginalLayout = nullptr;
  delete neighborhoodGraphColors;
  neighborhoodGraphColors = nullptr;
  delete neighborhoodGraphBackupColors;
  neighborhoodGraphBackupColors = nullptr;
}

void NeighborhoodHighlighter::buildNeighborhoodGraph(node n, Graph *g) {

  neighborhoodGraphCentralNode = node();

  cleanupNeighborhoodGraph();

  if (n.isValid()) {

    neighborhoodGraphCentralNode = n;

    neighborhoodGraph = new NodeNeighborhoodView(
        g, n, configWidget->getNeighborsType(), neighborhoodDist,
        configWidget->computeReachableSubGraph(), configWidget->propertyToUse(),
        configWidget->numberOfNodesToBring());

    neighborhoodGraphLayout = new LayoutProperty(neighborhoodGraph);
    neighborhoodGraphCircleLayout = new LayoutProperty(neighborhoodGraph);
    neighborhoodGraphOriginalLayout = new LayoutProperty(neighborhoodGraph);
    neighborhoodGraphColors = new ColorProperty(neighborhoodGraph);
    neighborhoodGraphBackupColors = new ColorProperty(neighborhoodGraph);

    updateNeighborhoodGraphLayoutAndColors();
    updateGlNeighborhoodGraph();

    circleCenter = neighborhoodGraphLayout->getNodeValue(n);
  }
}

void NeighborhoodHighlighter::updateNeighborhoodGraphLayoutAndColors() {
  if (originalGlGraph != nullptr) {
    GlGraphInputData *originalInputData = originalGlGraph->getInputData();
    LayoutProperty *origGraphLayout = originalInputData->layout();
    ColorProperty *origGraphColors = originalInputData->colors();

    for (auto n2 : neighborhoodGraph->nodes()) {
      neighborhoodGraphOriginalLayout->setNodeValue(n2, origGraphLayout->getNodeValue(n2));
      neighborhoodGraphBackupColors->setNodeValue(n2, origGraphColors->getNodeValue(n2));
    }

    for (auto e : neighborhoodGraph->edges()) {
      neighborhoodGraphOriginalLayout->setEdgeValue(e, origGraphLayout->getEdgeValue(e));
      neighborhoodGraphBackupColors->setEdgeValue(e, origGraphColors->getEdgeValue(e));
    }

    *neighborhoodGraphLayout = *neighborhoodGraphOriginalLayout;
    *neighborhoodGraphColors = *neighborhoodGraphBackupColors;
  }
}

void NeighborhoodHighlighter::updateGlNeighborhoodGraph() {
  GlGraphInputData *originalInputData = originalGlGraph->getInputData();
  glNeighborhoodGraph = new GlGraph(neighborhoodGraph);
  GlGraphInputData *glNeighborhoodGraphInputData = glNeighborhoodGraph->getInputData();
  glNeighborhoodGraphInputData->setBorderColors(originalInputData->borderColors());
  glNeighborhoodGraphInputData->setBorderWidths(originalInputData->borderWidths());
  glNeighborhoodGraphInputData->setColors(neighborhoodGraphColors);
  glNeighborhoodGraphInputData->setLabels(originalInputData->labels());
  glNeighborhoodGraphInputData->setLabelColors(originalInputData->labelColors());
  glNeighborhoodGraphInputData->setLabelBorderColors(originalInputData->labelBorderColors());
  glNeighborhoodGraphInputData->setLabelPositions(originalInputData->labelPositions());
  glNeighborhoodGraphInputData->setLayout(neighborhoodGraphLayout);
  glNeighborhoodGraphInputData->setRotations(originalInputData->rotations());
  glNeighborhoodGraphInputData->setSelection(originalInputData->selection());
  glNeighborhoodGraphInputData->setShapes(originalInputData->shapes());
  glNeighborhoodGraphInputData->setSizes(originalInputData->sizes());
  glNeighborhoodGraphInputData->setTextures(originalInputData->textures());
  glNeighborhoodGraphInputData->setFonts(originalInputData->fonts());
  glNeighborhoodGraphInputData->setFontSizes(originalInputData->fontSizes());
  glNeighborhoodGraphInputData->setSrcAnchorShapes(originalInputData->srcAnchorShapes());
  glNeighborhoodGraphInputData->setSrcAnchorSizes(originalInputData->srcAnchorSizes());
  glNeighborhoodGraphInputData->setTgtAnchorShapes(originalInputData->tgtAnchorShapes());
  glNeighborhoodGraphInputData->setTgtAnchorSizes(originalInputData->tgtAnchorSizes());
}

void NeighborhoodHighlighter::updateNeighborhoodGraph() {
  if (neighborhoodGraphCentralNode.isValid()) {
    buildNeighborhoodGraph(neighborhoodGraphCentralNode, originalGraph);
    glWidget->redraw();
  }
}

void NeighborhoodHighlighter::computeNeighborhoodGraphBoundingBoxes() {
  Coord centralNodeCoord =
      neighborhoodGraphOriginalLayout->getNodeValue(neighborhoodGraphCentralNode);
  float originalLayoutRadius = computeNeighborhoodGraphRadius(neighborhoodGraphOriginalLayout);
  float circleLayoutRadius = computeNeighborhoodGraphRadius(neighborhoodGraphCircleLayout);
  neighborhoodGraphOriginalLayoutBB[0] =
      centralNodeCoord - Coord(originalLayoutRadius, originalLayoutRadius);
  neighborhoodGraphOriginalLayoutBB[1] =
      centralNodeCoord + Coord(originalLayoutRadius, originalLayoutRadius);
  neighborhoodGraphCircleLayoutBB[0] =
      centralNodeCoord - Coord(circleLayoutRadius, circleLayoutRadius);
  neighborhoodGraphCircleLayoutBB[1] =
      centralNodeCoord + Coord(circleLayoutRadius, circleLayoutRadius);
}

void NeighborhoodHighlighter::computeNeighborhoodGraphCircleLayout() {

  Size centralNodeSize =
      originalGlGraph->getInputData()->sizes()->getNodeValue(neighborhoodGraphCentralNode);
  Coord centralNodeCoord = neighborhoodGraphLayout->getNodeValue(neighborhoodGraphCentralNode);
  neighborhoodGraphCircleLayout->setNodeValue(neighborhoodGraphCentralNode, centralNodeCoord);

  vector<node> neighborsNodes;
  for (auto n : neighborhoodGraph->nodes()) {
    if (n != neighborhoodGraphCentralNode) {
      neighborsNodes.push_back(n);
    }
  }

  sort(neighborsNodes.begin(), neighborsNodes.end(),
       NeighborNodesEdgeLengthOrdering(neighborhoodGraphCentralNode, neighborhoodGraphLayout));

  BoundingBox centralNodeBB(Coord(centralNodeCoord.getX() - centralNodeSize.getW() / 2,
                                  centralNodeCoord.getY() - centralNodeSize.getH() / 2, 0),
                            Coord(centralNodeCoord.getX() + centralNodeSize.getW() / 2,
                                  centralNodeCoord.getY() + centralNodeSize.getH() / 2, 0));
  vector<BoundingBox> neighborhoodNodesNewLayoutBB;
  neighborhoodNodesNewLayoutBB.resize(neighborsNodes.size());

  for (size_t i = 0; i < neighborsNodes.size(); ++i) {

    Size neighborNodeSize =
        originalGlGraph->getInputData()->sizes()->getNodeValue(neighborsNodes[i]);
    Coord neighborNodeCoord = neighborhoodGraphLayout->getNodeValue(neighborsNodes[i]);
    float edgeLength = centralNodeCoord.dist(neighborNodeCoord);

    bool nodePosOk = false;
    uint j = 1;

    while (!nodePosOk) {
      float ratio = ((centralNodeSize.getW() / 2) + j++ * neighborNodeSize.getW()) / edgeLength;
      Coord newNodeCoord = centralNodeCoord + ratio * (neighborNodeCoord - centralNodeCoord);
      neighborhoodNodesNewLayoutBB[i] =
          BoundingBox(Coord(newNodeCoord.getX() - neighborNodeSize.getW() / 2,
                            newNodeCoord.getY() - neighborNodeSize.getH() / 2, 0),
                      Coord(newNodeCoord.getX() + neighborNodeSize.getW() / 2,
                            newNodeCoord.getY() + neighborNodeSize.getH() / 2, 0));
      nodePosOk = true;

      if (i > 0) {
        for (uint k = 0; k < i; ++k) {
          nodePosOk = nodePosOk &&
                      !neighborhoodNodesNewLayoutBB[i].intersect(neighborhoodNodesNewLayoutBB[k]) &&
                      !neighborhoodNodesNewLayoutBB[i].intersect(centralNodeBB);
        }
      }

      if (nodePosOk) {
        neighborhoodGraphCircleLayout->setNodeValue(neighborsNodes[i], newNodeCoord);
      }
    }
  }

  for (auto e : neighborhoodGraph->edges()) {
    const auto &[src, tgt] = neighborhoodGraph->ends(e);

    Coord finalBendsCoord;
    if (src != neighborhoodGraphCentralNode) {
      Coord srcNodeCoord = neighborhoodGraphCircleLayout->getNodeValue(src);
      finalBendsCoord = circleCenter + (srcNodeCoord - circleCenter) / 2.f;
    } else {
      Coord tgtNodeCoord = neighborhoodGraphCircleLayout->getNodeValue(tgt);
      finalBendsCoord = circleCenter + (tgtNodeCoord - circleCenter) / 2.f;
    }

    vector<Coord> edgesBends = neighborhoodGraphLayout->getEdgeValue(e);

    for (auto &edgesBend : edgesBends) {
      edgesBend = finalBendsCoord;
    }

    neighborhoodGraphCircleLayout->setEdgeValue(e, edgesBends);
  }
}

void NeighborhoodHighlighter::morphCircleAlpha(unsigned char startA, unsigned endA,
                                               int nbAnimationSteps) {
  startAlpha = startA;
  endAlpha = endA;
  nbAnimSteps = nbAnimationSteps;
  QTimeLine timeLine(500);
  timeLine.setFrameRange(0, nbAnimSteps);
  connect(&timeLine, &QTimeLine::frameChanged, this,
          &NeighborhoodHighlighter::morphCircleAlphaAnimStep);
  timeLine.start();

  while (timeLine.state() != QTimeLine::NotRunning) {
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
  }
}

void NeighborhoodHighlighter::morphCircleAlphaAnimStep(int animStep) {
  circleAlphaValue = uchar(startAlpha + animStep / float(nbAnimSteps) * (endAlpha - startAlpha));
  glWidget->redraw();
}

float NeighborhoodHighlighter::computeNeighborhoodGraphRadius(
    LayoutProperty *neighborhoodGraphLayoutProp) {
  float radius = 0;
  node n;
  Coord centralNodeCoord = neighborhoodGraphLayoutProp->getNodeValue(neighborhoodGraphCentralNode);
  for (auto n : neighborhoodGraph->nodes()) {
    Coord nodeCoord = neighborhoodGraphLayoutProp->getNodeValue(n);
    Size nodeSize = originalGlGraph->getInputData()->sizes()->getNodeValue(n);
    float dist = centralNodeCoord.dist(nodeCoord) + nodeSize.getW();

    if (dist > radius) {
      radius = dist;
    }
  }
  return radius;
}

void NeighborhoodHighlighter::checkIfGraphHasChanged() {
  if (glWidget->scene()->getGlGraph()->getGraph() != originalGraph) {
    neighborhoodGraphCentralNode = node();
    centralNodeLocked = false;
    circleLayoutSet = false;
    cleanupNeighborhoodGraph();
    originalGraph = glWidget->scene()->getGlGraph()->getGraph();
    originalGlGraph = glWidget->scene()->getGlGraph();
  }
}

bool NeighborhoodHighlighter::draw(GlWidget *glWidget) {

  checkIfGraphHasChanged();

  if (neighborhoodGraphCentralNode.isValid() && glNeighborhoodGraph != nullptr) {

    Camera *camera = &(glWidget->scene()->getLayer("Main")->getCamera());
    camera->initGl();

    glLineWidth(1.0);
    glPointSize(1.0);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    glDepthFunc(GL_LEQUAL);
    glPolygonMode(GL_FRONT, GL_FILL);
    glColorMask(1, 1, 1, 1);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glIndexMask(UINT_MAX);
    glClearStencil(0xFFFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

    GlCircle circle(circleCenter, computeNeighborhoodGraphRadius(neighborhoodGraphLayout),
                    Color(0, 0, 0, circleAlphaValue), Color(255, 255, 255, circleAlphaValue), true,
                    true, 0.0, 60);
    circle.draw(0, nullptr);

    GlGraphRenderingParameters renderingParameters = glWidget->renderingParameters();
    renderingParameters.setNodesStencil(1);
    renderingParameters.setNodesLabelStencil(1);
    renderingParameters.setDisplayEdges(configWidget->isdisplayEdgesCBChecked());
    glNeighborhoodGraph->setRenderingParameters(renderingParameters);
    glNeighborhoodGraph->draw(10, camera);
  }

  return true;
}
}
