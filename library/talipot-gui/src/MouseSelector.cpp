/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <QMouseEvent>

#include <talipot/GlWidget.h>
#include <talipot/MouseSelector.h>
#include <talipot/GlComplexPolygon.h>

using namespace std;
using namespace tlp;

//==================================================================
MouseSelector::MouseSelector(Qt::MouseButton button, Qt::KeyboardModifier modifier,
                             SelectionMode mode)
    : mButton(button), kModifier(modifier), x(0), y(0), w(0), h(0), started(false), graph(nullptr),
      _mode(mode) {}
//==================================================================
bool MouseSelector::eventFilter(QObject *widget, QEvent *e) {
  auto *qMouseEv = static_cast<QMouseEvent *>(e);
  auto *glWidget = static_cast<GlWidget *>(widget);
  Graph *g = glWidget->inputData()->graph();

  if (e->type() == QEvent::MouseButtonPress) {

    if (qMouseEv->buttons() == mButton &&
        (kModifier == Qt::NoModifier || qMouseEv->modifiers() & kModifier)) {
      if (!started) {
        x = qMouseEv->pos().x();
        y = qMouseEv->pos().y();
        w = 0;
        h = 0;
        started = true;
        graph = g;
        mousePressModifier = qMouseEv->modifiers();
      } else {
        if (g != graph) {
          graph = nullptr;
          started = false;
          return false;
        }
      }

      return true;
    }

    if (qMouseEv->buttons() == Qt::MiddleButton) {
      started = false;
      glWidget->redraw();
      return true;
    }
  }

  if (e->type() == QEvent::MouseMove &&
      ((qMouseEv->buttons() & mButton) &&
       (kModifier == Qt::NoModifier || qMouseEv->modifiers() & kModifier))) {

    if (g != graph) {
      graph = nullptr;
      started = false;
    }

    if (started) {
      int clampedX = qMouseEv->pos().x();
      int clampedY = qMouseEv->pos().y();

      if (clampedX < 0) {
        clampedX = 0;
      }

      if (clampedY < 0) {
        clampedY = 0;
      }

      if (clampedX > glWidget->width()) {
        clampedX = glWidget->width();
      }

      if (clampedY > glWidget->height()) {
        clampedY = glWidget->height();
      }

      w = clampedX - x;
      h = clampedY - y;
      glWidget->redraw();
      return true;
    }

    return false;
  }

  if (e->type() == QEvent::MouseButtonRelease) {

    if (g != graph) {
      graph = nullptr;
      started = false;
      return false;
    }

    if (started) {
      Observable::holdObservers();
      BooleanProperty *selection = glWidget->inputData()->selection();
      bool revertSelection = false; // add to selection
      bool boolVal = true;
      bool needPush = true; // undo management

      if (mousePressModifier !=
#if defined(__APPLE__)
          Qt::AltModifier
#else
          Qt::ControlModifier
#endif
      ) {
        if (mousePressModifier == Qt::ShiftModifier && kModifier != Qt::ShiftModifier) {
          boolVal = false;
        } else {
          if (selection->getNodeDefaultValue() || selection->getEdgeDefaultValue()) {
            graph->push();
            selection->setAllNodeValue(false);
            selection->setAllEdgeValue(false);
            needPush = false;
          }

          if (selection->hasNonDefaultValuatedNodes()) {
            if (needPush) {
              graph->push();
              needPush = false;
            }
            selection->setAllNodeValue(false);
          }

          if (selection->hasNonDefaultValuatedEdges()) {
            if (needPush) {
              graph->push();
              needPush = false;
            }
            selection->setAllEdgeValue(false);
          }
        }
      } else {
        boolVal = true;
      }

      if ((w == 0) && (h == 0)) {
        SelectedEntity selectedEntity;
        bool result = glWidget->pickNodesEdges(x, y, selectedEntity);

        if (result) {
          switch (selectedEntity.getEntityType()) {
          case SelectedEntity::NODE_SELECTED:

            if (_mode == EdgesAndNodes || _mode == NodesOnly) {
              result = selection->getNodeValue(node(selectedEntity.getGraphElementId()));

              if (revertSelection || boolVal != result) {
                if (needPush) {
                  graph->push();
                  needPush = false;
                }

                selection->setNodeValue(node(selectedEntity.getGraphElementId()), !result);
              }
            }

            break;

          case SelectedEntity::EDGE_SELECTED:

            if (_mode == EdgesAndNodes || _mode == EdgesOnly) {
              result = selection->getEdgeValue(edge(selectedEntity.getGraphElementId()));

              if (revertSelection || boolVal != result) {
                if (needPush) {
                  graph->push();
                  needPush = false;
                }

                selection->setEdgeValue(edge(selectedEntity.getGraphElementId()), !result);
              }
            }

            break;

          default:
            break;
          }
        }
      } else {
        vector<SelectedEntity> tmpSetNode;
        vector<SelectedEntity> tmpSetEdge;

        if (w < 0) {
          w *= -1;
          x -= w;
        }

        if (h < 0) {
          h *= -1;
          y -= h;
        }

        glWidget->pickNodesEdges(x, y, w, h, tmpSetNode, tmpSetEdge);

        if (needPush) {
          graph->push();
        }

        if (_mode == EdgesAndNodes || _mode == NodesOnly) {
          for (const auto &entity : tmpSetNode) {
            selection->setNodeValue(node(entity.getGraphElementId()),
                                    revertSelection
                                        ? !selection->getNodeValue(node(entity.getGraphElementId()))
                                        : boolVal);
          }
        }

        if (_mode == EdgesAndNodes || _mode == EdgesOnly) {
          for (const auto &entity : tmpSetEdge) {
            selection->setEdgeValue(edge(entity.getGraphElementId()),
                                    revertSelection
                                        ? !selection->getEdgeValue(edge(entity.getGraphElementId()))
                                        : boolVal);
          }
        }
      }

      started = false;
      graph->popIfNoUpdates();
      Observable::unholdObservers();
      glWidget->redraw();
      return true;
    }
  }

  return false;
}
//==================================================================
bool MouseSelector::draw(GlWidget *glWidget) {
  if (!started) {
    return false;
  }

  if (glWidget->inputData()->graph() != graph) {
    graph = nullptr;
    started = false;
  }

  Camera *camera = &glWidget->scene()->getLayer("Main")->getCamera();
  Camera camera2D(camera->getScene(), false);

  float yy = glWidget->height() - y;
  Color color = {204, 204, 178};

  if (mousePressModifier ==
#if defined(__APPLE__)
      Qt::AltModifier
#else
      Qt::ControlModifier
#endif
  ) {
    color = {255, 204, 255};
  } else if (mousePressModifier == Qt::ShiftModifier) {
    color = {255, 178, 178};
  }

  float xf = float(x);
  vector<Coord> rectPoints = {
      {glWidget->screenToViewport(xf), glWidget->screenToViewport(yy)},
      {glWidget->screenToViewport(xf + w), glWidget->screenToViewport(yy)},
      {glWidget->screenToViewport(xf + w), glWidget->screenToViewport(yy - h)},
      {glWidget->screenToViewport(xf), glWidget->screenToViewport(yy - h)}};

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  camera2D.initGl();
  GlComplexPolygon complexPolygon(rectPoints, Color(color[0], color[1], color[2], 100), color);
  complexPolygon.setOutlineSize(2);
  complexPolygon.setOutlineStippled(true);
  complexPolygon.draw(0, nullptr);

  return true;
}
