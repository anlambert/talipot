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

#include <QMouseEvent>

#include <talipot/GlWidget.h>
#include <talipot/MouseSelector.h>

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
              node n(selectedEntity.getGraphElementId());
              result = (*selection)[n];

              if (revertSelection || boolVal != result) {
                if (needPush) {
                  graph->push();
                  needPush = false;
                }

                (*selection)[n] = !result;
              }
            }

            break;

          case SelectedEntity::EDGE_SELECTED:

            if (_mode == EdgesAndNodes || _mode == EdgesOnly) {
              edge e(selectedEntity.getGraphElementId());
              result = (*selection)[e];

              if (revertSelection || boolVal != result) {
                if (needPush) {
                  graph->push();
                  needPush = false;
                }

                (*selection)[e] = !result;
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
            node n(entity.getGraphElementId());
            (*selection)[n] = revertSelection ? !(*selection)[n] : boolVal;
          }
        }

        if (_mode == EdgesAndNodes || _mode == EdgesOnly) {
          for (const auto &entity : tmpSetEdge) {
            edge e(entity.getGraphElementId());
            (*selection)[e] = revertSelection ? !(*selection)[e] : boolVal;
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

  float yy = glWidget->height() - y;
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, glWidget->width(), 0, glWidget->height(), -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);
  float col[4] = {0, 0, 0, 0.2f};

  if (mousePressModifier ==
#if defined(__APPLE__)
      Qt::AltModifier
#else
      Qt::ControlModifier
#endif
  ) {
    col[0] = 1.;
    col[1] = 0.8f;
    col[2] = 1.;
  } else if (mousePressModifier == Qt::ShiftModifier) {
    col[0] = 1.;
    col[1] = .7f;
    col[2] = .7f;
  } else {
    col[0] = 0.8f;
    col[1] = 0.8f;
    col[2] = 0.7f;
  }

  setColor(col);
  glBegin(GL_QUADS);
  glVertex2f(x, yy);
  glVertex2f(x + w, yy);
  glVertex2f(x + w, yy - h);
  glVertex2f(x, yy - h);
  glEnd();
  glDisable(GL_BLEND);
  glLineWidth(2);
  glLineStipple(2, 0xAAAA);
  glEnable(GL_LINE_STIPPLE);
  glBegin(GL_LINE_LOOP);
  glVertex2f(x, yy);
  glVertex2f(x + w, yy);
  glVertex2f(x + w, yy - h);
  glVertex2f(x, yy - h);
  glEnd();
  glLineWidth(1);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopAttrib();
  return true;
}
