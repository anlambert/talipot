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

#include <QEvent>
#include <QWheelEvent>
#include <QPinchGesture>
#include <QPanGesture>

#include <talipot/GlNode.h>
#include <talipot/GlWidget.h>
#include <talipot/View.h>
#include <talipot/Observable.h>
#include <talipot/GlGraph.h>
#include <talipot/GlView.h>
#include <talipot/GlBoundingBoxSceneVisitor.h>
#include <talipot/DrawingTools.h>
#include <talipot/QtGlSceneZoomAndPanAnimator.h>
#include <talipot/NodeLinkDiagramView.h>
#include <talipot/MouseInteractors.h>
#include <talipot/MaterialDesignIcons.h>
#include <talipot/FontIconManager.h>

#include <iostream>

using namespace tlp;
using namespace std;

//===============================================================
bool MousePanNZoomNavigator::eventFilter(QObject *widget, QEvent *e) {
  // according to Qt's doc, this constant has been defined by wheel mouse vendors
  // we need it to interpret the value of QWheelEvent->angleDelta().y()
  auto *g = static_cast<GlWidget *>(widget);

  if (e->type() == QEvent::Wheel) {
    auto *we = static_cast<QWheelEvent *>(e);

    auto delta = we->angleDelta().y();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    auto pos = we->position();
#else
    auto pos = we->pos();
#endif

    if (delta && we->modifiers() == Qt::NoModifier) {
      g->getScene()->zoomXY(g->screenToViewport(delta) / 120, g->screenToViewport(pos.x()),
                            g->screenToViewport(pos.y()));
      g->draw(false);
      return true;
    }
  }

  if (e->type() == QEvent::Gesture) {
    auto *gesture = static_cast<QGestureEvent *>(e);
    QPointF center;

    if (gesture->gesture(Qt::PinchGesture)) {
      auto *pinch = static_cast<QPinchGesture *>(gesture->gesture(Qt::PinchGesture));
      Camera &camera = g->getScene()->getGraphCamera();

      // store the camera scale factor when starting the gesture
      if (pinch->state() == Qt::GestureStarted) {
        cameraScaleFactor = camera.getZoomFactor();
        isGesturing = true;
      }

      if (pinch->changeFlags() & QPinchGesture::ScaleFactorChanged) {
        // only set the zoom factor if two events in a row were in the same direction (zoom in or
        // out) to smooth a bit the effect.
        if ((pinch->lastScaleFactor() > 1 && pinch->scaleFactor() > 1) ||
            (pinch->lastScaleFactor() <= 1 && pinch->scaleFactor() <= 1)) {
          camera.setZoomFactor(cameraScaleFactor * pinch->totalScaleFactor());
        }
      }

      if (pinch->changeFlags() & QPinchGesture::RotationAngleChanged) {
        // rotates the camera
        camera.rotate((pinch->rotationAngle() - pinch->lastRotationAngle()) / 180 * M_PI, 0, 0, 1);
      }

      if (pinch->state() == Qt::GestureFinished) {
        isGesturing = false;
      }

      if (gesture->gesture(Qt::PanGesture)) {
        auto *pan = static_cast<QPanGesture *>(gesture->gesture(Qt::PanGesture));

        if (pan->state() == Qt::GestureStarted) {
          isGesturing = true;
        }

        if (pan->state() == Qt::GestureFinished) {
          isGesturing = false;
        }

        center = pan->delta();
        g->getScene()->translateCamera(g->screenToViewport(pan->delta().x()),
                                       -g->screenToViewport(pan->delta().y()), 0);
      }
    }

    g->draw(false);
    return true;
  }

  return false;
}

//===============================================================
void MouseElementDeleter::delElement(Graph *graph, SelectedEntity &selectedEntity) {
  switch (selectedEntity.getEntityType()) {
  case SelectedEntity::NODE_SELECTED:
    graph->delNode(node(selectedEntity.getComplexEntityId()));
    break;

  case SelectedEntity::EDGE_SELECTED:
    graph->delEdge(edge(selectedEntity.getComplexEntityId()));
    break;

  default:
    break;
  }
}

bool MouseElementDeleter::eventFilter(QObject *widget, QEvent *e) {
  auto *qMouseEv = dynamic_cast<QMouseEvent *>(e);

  if (qMouseEv != nullptr) {
    SelectedEntity selectedEntity;

    if (glWidget == nullptr) {
      glWidget = static_cast<GlWidget *>(widget);
    }

    if (e->type() == QEvent::MouseMove) {
      if (glWidget->pickNodesEdges(qMouseEv->x(), qMouseEv->y(), selectedEntity)) {
        QIcon icon = FontIconManager::icon(MaterialDesignIcons::DeleteOutline);
        glWidget->setCursor(icon.pixmap(32, 32));
      } else {
        glWidget->setCursor(Qt::ArrowCursor);
      }

      return false;
    } else if (e->type() == QEvent::MouseButtonPress && qMouseEv->button() == Qt::LeftButton) {
      if (glWidget->pickNodesEdges(qMouseEv->x(), qMouseEv->y(), selectedEntity)) {
        Observable::holdObservers();
        Graph *graph = glWidget->getGlGraphInputData()->getGraph();
        // allow to undo
        graph->push();
        delElement(graph, selectedEntity);
        graph->popIfNoUpdates();
        glWidget->redraw();
        Observable::unholdObservers();
        return true;
      }
    }
  }

  return false;
}

void MouseElementDeleter::clear() {
  if (glWidget) {
    glWidget->setCursor(QCursor());
  }
}
//===============================================================
class MouseRotXRotY : public InteractorComponent {
public:
  MouseRotXRotY() : x(INT_MAX), y(INT_MAX) {}
  ~MouseRotXRotY() override = default;
  int x, y;
  bool eventFilter(QObject *, QEvent *) override;
};

bool MouseRotXRotY::eventFilter(QObject *widget, QEvent *e) {
  if (e->type() == QEvent::MouseButtonPress) {
    auto *qMouseEv = static_cast<QMouseEvent *>(e);
    x = qMouseEv->x();
    y = qMouseEv->y();
    return true;
  }

  if (e->type() == QEvent::MouseMove) {
    auto *qMouseEv = static_cast<QMouseEvent *>(e);
    auto *glWidget = static_cast<GlWidget *>(widget);
    int deltaX, deltaY;
    deltaX = qMouseEv->x() - x;
    deltaY = qMouseEv->y() - y;

    if (abs(deltaX) > abs(deltaY)) {
      deltaY = 0;
    } else {
      deltaX = 0;
    }

    if (deltaY != 0) {
      glWidget->getScene()->rotateCamera(glWidget->screenToViewport(deltaY), 0, 0);
    }

    if (deltaX != 0) {
      glWidget->getScene()->rotateCamera(0, glWidget->screenToViewport(deltaX), 0);
    }

    x = qMouseEv->x();
    y = qMouseEv->y();
    glWidget->draw(false);
    return true;
  }

  return false;
}
//===============================================================
class MouseZoomRotZ : public InteractorComponent {
public:
  MouseZoomRotZ() : x(INT_MAX), y(INT_MAX), inRotation(false), inZoom(false) {}
  ~MouseZoomRotZ() override = default;
  int x, y;
  bool inRotation, inZoom;
  bool eventFilter(QObject *, QEvent *) override;
};

bool MouseZoomRotZ::eventFilter(QObject *widget, QEvent *e) {
  if (e->type() == QEvent::MouseButtonPress) {
    auto *qMouseEv = static_cast<QMouseEvent *>(e);
    x = qMouseEv->x();
    y = qMouseEv->y();
    inRotation = false;
    inZoom = false;
    return true;
  }

  if (e->type() == QEvent::MouseMove) {
    auto *qMouseEv = static_cast<QMouseEvent *>(e);
    auto *glWidget = static_cast<GlWidget *>(widget);
    int deltaX, deltaY;

    if (!inRotation && !inZoom) {
      deltaX = qMouseEv->x() - x;
      deltaY = qMouseEv->y() - y;

      if (deltaY && abs(deltaX) >= 3 * abs(deltaY)) {
        inRotation = true;
        inZoom = false;
      } else if (deltaX && abs(deltaY) >= 3 * abs(deltaX)) {
        inZoom = true;
        inRotation = false;
      } else {
      }

      x = qMouseEv->x();
      y = qMouseEv->y();
    }

    if (inZoom) {
      // Zoom
      deltaY = qMouseEv->y() - y;
      glWidget->getScene()->zoom(-glWidget->screenToViewport(deltaY / 2));
      y = qMouseEv->y();
    }

    if (inRotation) {
      // Rotation
      deltaX = qMouseEv->x() - x;
      glWidget->getScene()->rotateCamera(0, 0, glWidget->screenToViewport(deltaX));
      x = qMouseEv->x();
    }

    glWidget->draw(false);
    return true;
  }

  return false;
}
//===============================================================
class MouseMove : public InteractorComponent {
public:
  int x, y;
  MouseMove() : x(INT_MAX), y(INT_MAX) {}
  ~MouseMove() override = default;
  bool eventFilter(QObject *, QEvent *) override;
};

bool MouseMove::eventFilter(QObject *widget, QEvent *e) {
  if (e->type() == QEvent::MouseButtonPress) {
    auto *qMouseEv = static_cast<QMouseEvent *>(e);
    x = qMouseEv->x();
    y = qMouseEv->y();
    return true;
  }

  if (e->type() == QEvent::MouseMove) {
    auto *qMouseEv = static_cast<QMouseEvent *>(e);
    auto *glWidget = static_cast<GlWidget *>(widget);

    if (qMouseEv->x() != x) {
      glWidget->getScene()->translateCamera(glWidget->screenToViewport(qMouseEv->x() - x), 0, 0);
    }

    if (qMouseEv->y() != y) {
      glWidget->getScene()->translateCamera(0, glWidget->screenToViewport(y - qMouseEv->y()), 0);
    }

    x = qMouseEv->x();
    y = qMouseEv->y();
    glWidget->draw(false);
    return true;
  }

  return false;
}

// animation during meta node interaction
class MyQtGlSceneZoomAndPanAnimator : public tlp::QtGlSceneZoomAndPanAnimator {
public:
  MyQtGlSceneZoomAndPanAnimator(tlp::GlWidget *glWidget, tlp::View *view,
                                const tlp::BoundingBox &boundingBox, tlp::Graph *graph, tlp::node n,
                                const float &color)
      : tlp::QtGlSceneZoomAndPanAnimator(glWidget, boundingBox), view(view), graph(graph), n(n),
        alphaEnd(color) {
    tlp::ColorProperty *colorProp = graph->getColorProperty("viewColor");
    alphaBegin = colorProp->getNodeValue(n)[3];
  }

protected:
  void zoomAndPanAnimStepSlot(int animationStep) override;

protected:
  tlp::View *view;
  tlp::Graph *graph;
  tlp::node n;
  float alphaEnd;
  float alphaBegin;
};

void MyQtGlSceneZoomAndPanAnimator::zoomAndPanAnimStepSlot(int animationStep) {
  int nbAnimationSteps = animationDurationMsec / 40 + 1;
  float decAlpha = (alphaEnd - alphaBegin) / nbAnimationSteps;
  ColorProperty *colorProp = graph->getColorProperty("viewColor");
  Color color = colorProp->getNodeValue(n);
  color[3] = alphaBegin + decAlpha * animationStep;
  colorProp->setNodeValue(n, color);
  QtGlSceneZoomAndPanAnimator::zoomAndPanAnimationStep(animationStep);
  view->draw();
}
//===============================================================
bool MouseNKeysNavigator::eventFilter(QObject *widget, QEvent *e) {
  if (isGesturing) {
    return MousePanNZoomNavigator::eventFilter(widget, e);
  }

  if (currentSpecInteractorComponent) {
    if (currentSpecInteractorComponent->eventFilter(widget, e)) {
      return true;
    }
  }

  auto *glWidget = static_cast<GlWidget *>(widget);
  auto *qMouseEv = static_cast<QMouseEvent *>(e);

  if (e->type() == QEvent::MouseButtonDblClick && qMouseEv->button() == Qt::LeftButton) {
    Graph *graph = glWidget->getGlGraphInputData()->getGraph();

    if (qMouseEv->modifiers() != Qt::ControlModifier) {
      vector<SelectedEntity> tmpNodes;
      vector<SelectedEntity> tmpEdges;
      glWidget->pickNodesEdges(qMouseEv->x() - 1, qMouseEv->y() - 1, 3, 3, tmpNodes, tmpEdges);
      node metaNode;
      bool find = false;

      for (auto tmpNode : tmpNodes) {
        if (graph->isMetaNode(node(tmpNode.getComplexEntityId()))) {
          metaNode = node(tmpNode.getComplexEntityId());
          find = true;
          break;
        }
      }

      if (find) {
        Graph *metaGraph = graph->getNodeMetaInfo(metaNode);

        if (metaGraph && nldv) {
          graphHierarchy.push_back(graph);
          nodeHierarchy.push_back(metaNode);
          cameraHierarchy.push_back(nldv->goInsideItem(metaNode));
        }
      } else { // no double click on a metanode. Do not block event.
        return false;
      }

      return true;
    } else {
      if (!graphHierarchy.empty() && nldv) {
        Graph *oldGraph = graphHierarchy.back();
        graphHierarchy.pop_back();
        Camera camera = cameraHierarchy.back();
        cameraHierarchy.pop_back();
        node n = nodeHierarchy.back();
        nodeHierarchy.pop_back();

        Observable::holdObservers();

        ColorProperty *colorProp = oldGraph->getColorProperty("viewColor");
        float alphaOrigin = colorProp->getNodeValue(n)[3];
        Color color = colorProp->getNodeValue(n);
        color[3] = 0;
        colorProp->setNodeValue(n, color);

        Observable::unholdObservers();

        nldv->requestChangeGraph(oldGraph);
        glWidget->getScene()->getLayer("Main")->getCamera().setCenter(camera.getCenter());
        glWidget->getScene()->getLayer("Main")->getCamera().setEyes(camera.getEyes());
        glWidget->getScene()->getLayer("Main")->getCamera().setSceneRadius(camera.getSceneRadius());
        glWidget->getScene()->getLayer("Main")->getCamera().setUp(camera.getUp());
        glWidget->getScene()->getLayer("Main")->getCamera().setZoomFactor(camera.getZoomFactor());
        glWidget->draw(false);

        GlBoundingBoxSceneVisitor visitor(glWidget->getGlGraphInputData());
        glWidget->getScene()->getLayer("Main")->acceptVisitor(&visitor);
        BoundingBox boundingBox(visitor.getBoundingBox());

        MyQtGlSceneZoomAndPanAnimator navigator(glWidget, nldv, boundingBox, oldGraph, n,
                                                alphaOrigin);
        navigator.animateZoomAndPan();

        return true;
      }
    }
  }

  if (e->type() == QEvent::MouseButtonPress) {
    auto *qMouseEv = static_cast<QMouseEvent *>(e);

    if (qMouseEv->buttons() == Qt::LeftButton) {
      oldCursor = glWidget->cursor();
      InteractorComponent *currentMouse;
      // give focus so we can catch key events
      glWidget->setFocus();

      if (qMouseEv->modifiers() &
#if defined(__APPLE__)
          Qt::AltModifier
#else
          Qt::ControlModifier
#endif
      )
        currentMouse = new MouseZoomRotZ();
      else if (qMouseEv->modifiers() & Qt::ShiftModifier) {
        currentMouse = new MouseRotXRotY();
      } else {
        currentMouse = new MouseMove();
        glWidget->setCursor(QCursor(Qt::ClosedHandCursor));
      }

      bool result = currentMouse->eventFilter(widget, e);

      currentSpecInteractorComponent = currentMouse;

      // currentMouseID = abstractView->pushInteractor(currentMouse);
      return result;
    }

    return false;
  }

  if (e->type() == QEvent::MouseButtonRelease) {
    glWidget->setCursor(oldCursor);
    delete currentSpecInteractorComponent;
    currentSpecInteractorComponent = nullptr;
    return true;
  }

  if (e->type() == QEvent::KeyPress) {
    auto *ke = static_cast<QKeyEvent *>(e);
    int delta = (ke->isAutoRepeat() ? 3 : 1);

    switch (ke->key()) {
    case Qt::Key_Left:
      glWidget->getScene()->translateCamera(glWidget->screenToViewport(delta * 2), 0, 0);
      break;

    case Qt::Key_Right:
      glWidget->getScene()->translateCamera(glWidget->screenToViewport(-1 * delta * 2), 0, 0);
      break;

    case Qt::Key_Up:
      glWidget->getScene()->translateCamera(0, glWidget->screenToViewport(-1 * delta * 2), 0);
      break;

    case Qt::Key_Down:
      glWidget->getScene()->translateCamera(0, glWidget->screenToViewport(delta * 2), 0);
      break;

    case Qt::Key_PageUp:
      glWidget->getScene()->zoom(glWidget->screenToViewport(delta));
      break;

    case Qt::Key_PageDown:
      glWidget->getScene()->zoom(glWidget->screenToViewport(-1 * delta));
      break;

    case Qt::Key_Home:
      glWidget->getScene()->translateCamera(0, 0, glWidget->screenToViewport(-1 * delta * 2));
      break;

    case Qt::Key_End:
      glWidget->getScene()->translateCamera(0, 0, glWidget->screenToViewport(delta * 2));
      break;

    case Qt::Key_Insert:
      glWidget->getScene()->rotateCamera(0, 0, glWidget->screenToViewport(-1 * delta * 2));
      break;

    case Qt::Key_Delete:
      glWidget->getScene()->rotateCamera(0, 0, glWidget->screenToViewport(delta * 2));
      break;

    default:
      return false;
    }

    glWidget->draw();
    return true;
  }

  if (e->type() == QEvent::KeyRelease) {
    auto *ke = static_cast<QKeyEvent *>(e);

    switch (ke->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_PageUp:
    case Qt::Key_PageDown:
    case Qt::Key_Home:
    case Qt::Key_End:
    case Qt::Key_Insert:
    case Qt::Key_Delete:
      break;

    default:
      return false;
    }

    return true;
  }

  return MousePanNZoomNavigator::eventFilter(widget, e);
}

void MouseNKeysNavigator::viewChanged(View *view) {
  nldv = static_cast<NodeLinkDiagramView *>(view);
}

void MouseNKeysNavigator::clear() {}
