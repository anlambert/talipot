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

#include "MouseLassoNodesSelector.h"
#include "../../utils/StandardInteractorPriority.h"
#include "../../utils/PluginNames.h"
#include "../../utils/InteractorIcons.h"

#include <talipot/MouseInteractors.h>
#include <talipot/GlComplexPolygon.h>
#include <talipot/NodeLinkDiagramView.h>

#include <QMouseEvent>

using namespace std;
using namespace tlp;

MouseLassoNodesSelectorInteractor::MouseLassoNodesSelectorInteractor(const tlp::PluginContext *)
    : NodeLinkDiagramViewInteractor(interactorIcon(InteractorType::Lasso),
                                    "Select nodes in a freehand drawn region",
                                    StandardInteractorPriority::FreeHandSelection) {}

bool MouseLassoNodesSelectorInteractor::isCompatible(const std::string &viewName) const {
  return ((viewName == NodeLinkDiagramView::viewName) ||
          (viewName == ViewName::ScatterPlot2DViewName) ||
          (viewName == ViewName::HistogramViewName) ||
          (viewName == ViewName::PixelOrientedViewName) || (viewName == ViewName::MatrixViewName) ||
          (viewName == ViewName::ParallelCoordinatesViewName));
}

void MouseLassoNodesSelectorInteractor::construct() {
  setConfigurationWidgetText(QString("<h3>Select nodes in a freehand drawn region</h3>") +
                             "<b>Mouse left</b> down begins the freehand drawing of the selection "
                             "region,<br/><b>Mouse left</b> up ends the drawing of the "
                             "region.<br/>All the nodes enclosed in the region are selected and "
                             "the edges linking them too.");
  push_back(new MouseLassoNodesSelectorInteractorComponent());
  push_back(new MousePanNZoomNavigator());
}

PLUGIN(MouseLassoNodesSelectorInteractor)

MouseLassoNodesSelectorInteractorComponent::MouseLassoNodesSelectorInteractorComponent()
    : drawInteractor(false), camera(nullptr), graph(nullptr), viewSelection(nullptr),
      dragStarted(false) {}

MouseLassoNodesSelectorInteractorComponent::~MouseLassoNodesSelectorInteractorComponent() = default;

// found on http://local.wasp.uwa.edu.au/~pbourke/geometry/insidepoly/
bool pointInsidePolygon(const vector<Coord> &polygon, const Coord &point) {
  uint i, j;
  bool ret = false;

  for (i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
    if ((((polygon[i].getY() <= point.getY()) && (point.getY() < polygon[j].getY())) ||
         ((polygon[j].getY() <= point.getY()) && (point.getY() < polygon[i].getY()))) &&
        (point.getX() < (polygon[j].getX() - polygon[i].getX()) *
                                (point.getY() - polygon[i].getY()) /
                                (polygon[j].getY() - polygon[i].getY()) +
                            polygon[i].getX())) {
      ret = !ret;
    }
  }

  return ret;
}

bool isPolygonAincludesInB(const vector<Coord> &A, const vector<Coord> &B) {
  bool ret = true;

  for (const auto &p : A) {
    ret = ret && pointInsidePolygon(B, p);

    if (!ret) {
      break;
    }
  }

  return ret;
}

void MouseLassoNodesSelectorInteractorComponent::selectGraphElementsUnderPolygon(
    GlWidget *glWidget) {
  vector<Coord> polygonVprt;
  BoundingBox polygonVprtBB;

  for (uint i = 0; i < polygon.size(); ++i) {
    polygonVprt.push_back(polygon[i]);
    polygonVprtBB.expand(polygonVprt[i]);
  }

  polygonVprt.push_back(polygon[0]);

  int xStart = int(polygonVprtBB[0][0]);
  int yStart = int(polygonVprtBB[0][1]);
  int xEnd = int(polygonVprtBB[1][0]);
  int yEnd = int(polygonVprtBB[1][1]);

  vector<SelectedEntity> tmpNodes;
  vector<SelectedEntity> tmpEdges;
  glWidget->pickNodesEdges(glWidget->viewportToScreen(xStart),
                           glWidget->height() - glWidget->viewportToScreen(yEnd),
                           glWidget->viewportToScreen(xEnd - xStart),
                           glWidget->viewportToScreen(yEnd - yStart), tmpNodes, tmpEdges);

  if (!tmpNodes.empty()) {
    vector<node> selectedNodes;
    GlNode glNode;

    bool needPush = true;

    for (const auto &tmpNode : tmpNodes) {
      glNode.n = node(tmpNode.getComplexEntityId());
      BoundingBox nodeBB(glNode.getBoundingBox(glWidget->inputData()));
      float dx = nodeBB[1][0] - nodeBB[0][0];
      float dy = nodeBB[1][1] - nodeBB[0][1];
      float dz = nodeBB[1][2] - nodeBB[0][2];
      const float f = 0.2f;
      nodeBB[0][0] = nodeBB[0][0] + f * dx;
      nodeBB[0][1] = nodeBB[0][1] + f * dy;
      nodeBB[0][2] = nodeBB[0][2] + f * dz;
      nodeBB[1][0] = nodeBB[1][0] - f * dx;
      nodeBB[1][1] = nodeBB[1][1] - f * dy;
      nodeBB[1][2] = nodeBB[1][2] - f * dz;
      vector<float> xVec;
      vector<float> yVec;
      Coord nodeBBBLBScr = camera->worldTo2DViewport(nodeBB[0]);
      xVec.push_back(nodeBBBLBScr.getX());
      yVec.push_back(nodeBBBLBScr.getY());
      Coord nodeBBTLBScr =
          camera->worldTo2DViewport(Coord(nodeBB[0][0], nodeBB[1][1], nodeBB[0][2]));
      xVec.push_back(nodeBBTLBScr[0]);
      yVec.push_back(nodeBBTLBScr[1]);
      Coord nodeBBTRBScr =
          camera->worldTo2DViewport(Coord(nodeBB[1][0], nodeBB[1][1], nodeBB[0][2]));
      xVec.push_back(nodeBBTRBScr[0]);
      yVec.push_back(nodeBBTRBScr[1]);
      Coord nodeBBBRBScr =
          camera->worldTo2DViewport(Coord(nodeBB[1][0], nodeBB[0][1], nodeBB[0][2]));
      xVec.push_back(nodeBBBRBScr[0]);
      yVec.push_back(nodeBBBRBScr[1]);
      Coord nodeBBBLFScr =
          camera->worldTo2DViewport(Coord(nodeBB[0][0], nodeBB[0][1], nodeBB[1][2]));
      xVec.push_back(nodeBBBLFScr[0]);
      yVec.push_back(nodeBBBLFScr[1]);
      Coord nodeBBTLFScr =
          camera->worldTo2DViewport(Coord(nodeBB[0][0], nodeBB[1][1], nodeBB[1][2]));
      xVec.push_back(nodeBBTLFScr[0]);
      yVec.push_back(nodeBBTLFScr[1]);
      Coord nodeBBTRFScr = camera->worldTo2DViewport(nodeBB[1]);
      xVec.push_back(nodeBBTRFScr[0]);
      yVec.push_back(nodeBBTRFScr[1]);
      Coord nodeBBBRFScr =
          camera->worldTo2DViewport(Coord(nodeBB[1][0], nodeBB[0][1], nodeBB[1][2]));
      xVec.push_back(nodeBBBRFScr[0]);
      yVec.push_back(nodeBBBRFScr[1]);
      vector<Coord> quad;
      quad.push_back(
          Coord(*min_element(xVec.begin(), xVec.end()), *min_element(yVec.begin(), yVec.end())));
      quad.push_back(
          Coord(*min_element(xVec.begin(), xVec.end()), *max_element(yVec.begin(), yVec.end())));
      quad.push_back(
          Coord(*max_element(xVec.begin(), xVec.end()), *max_element(yVec.begin(), yVec.end())));
      quad.push_back(
          Coord(*max_element(xVec.begin(), xVec.end()), *min_element(yVec.begin(), yVec.end())));
      quad.push_back(quad[0]);

      if (isPolygonAincludesInB(quad, polygonVprt)) {
        if (needPush) {
          viewSelection->getGraph()->push();
          needPush = false;
        }

        viewSelection->setNodeValue(node(tmpNode.getComplexEntityId()), true);
        selectedNodes.push_back(node(tmpNode.getComplexEntityId()));
      }
    }

    for (uint i = 0; i < selectedNodes.size(); ++i) {
      for (uint j = i + 1; j < selectedNodes.size(); ++j) {
        vector<edge> edges = graph->getEdges(selectedNodes[i], selectedNodes[j], false);

        for (auto edge : edges) {
          viewSelection->setEdgeValue(edge, true);
        }
      }
    }
  }
}

bool MouseLassoNodesSelectorInteractorComponent::eventFilter(QObject *obj, QEvent *e) {
  auto *glWidget = static_cast<GlWidget *>(obj);

  if (!glWidget->hasMouseTracking()) {
    glWidget->setMouseTracking(true);
  }

  auto *me = dynamic_cast<QMouseEvent *>(e);

  if (!me) {
    return false;
  }

  camera = &glWidget->scene()->getLayer("Main")->getCamera();
  graph = glWidget->inputData()->graph();
  viewSelection = graph->getBooleanProperty("viewSelection");

  currentPointerScreenCoord = Coord(me->pos().x(), glWidget->height() - me->pos().y());

  if (me->type() == QEvent::MouseMove) {
    if (dragStarted) {
      polygon.push_back(glWidget->screenToViewport(currentPointerScreenCoord));
      drawInteractor = true;
      glWidget->redraw();
    }

    return true;
  } else if (me->type() == QEvent::MouseButtonPress) {
    if (me->button() == Qt::LeftButton) {
      dragStarted = true;
      polygon.push_back(glWidget->screenToViewport(currentPointerScreenCoord));
    } else if (me->button() == Qt::RightButton) {
      dragStarted = false;

      if (!polygon.empty()) {
        polygon.clear();
        drawInteractor = true;
        glWidget->redraw();
      } else {
        Observable::holdObservers();
        SelectedEntity selectedEntity;
        bool result = glWidget->pickNodesEdges(me->pos().x(), me->pos().y(), selectedEntity);

        if (result && selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED) {
          bool sel = viewSelection->getNodeValue(node(selectedEntity.getComplexEntityId()));
          viewSelection->setNodeValue(node(selectedEntity.getComplexEntityId()), !sel);
        }

        Observable::unholdObservers();
      }
    }

    return true;
  } else if (me->type() == QEvent::MouseButtonRelease) {
    dragStarted = false;

    if (me->button() == Qt::LeftButton && polygon.size() > 10) {
      Observable::holdObservers();

      if (me->modifiers() != Qt::ControlModifier) {
        viewSelection->setAllNodeValue(false);
        viewSelection->setAllEdgeValue(false);
      }

      selectGraphElementsUnderPolygon(glWidget);
      Observable::unholdObservers();
    }

    polygon.clear();
  }

  return false;
}

bool MouseLassoNodesSelectorInteractorComponent::draw(GlWidget *glWidget) {

  if (!drawInteractor) {
    return false;
  }

  if (!polygon.empty()) {

    Camera camera2D(camera->getScene(), false);

    Color backgroundColor = glWidget->scene()->getBackgroundColor();
    Color foregroundColor;
    int bgV = backgroundColor.getV();

    if (bgV < 128) {
      foregroundColor = Color(255, 255, 255);
    } else {
      foregroundColor = Color(0, 0, 0);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    camera2D.initGl();
    GlComplexPolygon complexPolygon(polygon, Color(0, 255, 0, 100), Color(0, 255, 0));
    complexPolygon.draw(0, nullptr);
  }

  drawInteractor = false;

  return true;
}
