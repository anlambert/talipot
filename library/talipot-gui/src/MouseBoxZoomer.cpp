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
#include <talipot/QtGlSceneZoomAndPanAnimator.h>
#include <talipot/GlBoundingBoxSceneVisitor.h>
#include <talipot/MouseBoxZoomer.h>

using namespace std;
using namespace tlp;

MouseBoxZoomer::MouseBoxZoomer(Qt::MouseButton button, Qt::KeyboardModifier modifier)
    : mButton(button), kModifier(modifier), x(0), y(0), w(0), h(0), started(false), graph(nullptr) {
}
MouseBoxZoomer::~MouseBoxZoomer() = default;
//=====================================================================
bool MouseBoxZoomer::eventFilter(QObject *widget, QEvent *e) {
  auto *glw = static_cast<GlWidget *>(widget);
  GlGraphInputData *inputData = glw->getGlGraphInputData();

  if (e->type() == QEvent::MouseButtonPress) {
    auto *qMouseEv = static_cast<QMouseEvent *>(e);

    if (qMouseEv->buttons() == mButton &&
        (kModifier == Qt::NoModifier || qMouseEv->modifiers() & kModifier)) {
      if (!started) {
        x = qMouseEv->x();
        y = glw->height() - qMouseEv->y();
        w = 0;
        h = 0;
        started = true;
        graph = inputData->getGraph();
      } else {
        if (inputData->getGraph() != graph) {
          graph = nullptr;
          started = false;
        }
      }

      return true;
    }

    if (qMouseEv->buttons() == Qt::MiddleButton) {
      started = false;
      glw->redraw();
      return true;
    }

    return false;
  }

  if (e->type() == QEvent::MouseMove) {
    auto *qMouseEv = static_cast<QMouseEvent *>(e);

    if ((qMouseEv->buttons() & mButton) &&
        (kModifier == Qt::NoModifier || qMouseEv->modifiers() & kModifier)) {
      if (inputData->getGraph() != graph) {
        graph = nullptr;
        started = false;
      }

      if (started) {
        if ((qMouseEv->x() > 0) && (qMouseEv->x() < glw->width())) {
          w = qMouseEv->x() - x;
        }

        if ((qMouseEv->y() > 0) && (qMouseEv->y() < glw->height())) {
          h = y - (glw->height() - qMouseEv->y());
        }

        glw->redraw();
        return true;
      }
    }
  }

  if (e->type() == QEvent::MouseButtonDblClick) {
    GlBoundingBoxSceneVisitor bbVisitor(inputData);
    glw->getScene()->getLayer("Main")->acceptVisitor(&bbVisitor);
    QtGlSceneZoomAndPanAnimator zoomAnPan(glw, bbVisitor.getBoundingBox());
    zoomAnPan.animateZoomAndPan();
    return true;
  }

  if (e->type() == QEvent::MouseButtonRelease) {

    auto *qMouseEv = static_cast<QMouseEvent *>(e);

    if ((qMouseEv->button() == mButton &&
         (kModifier == Qt::NoModifier || qMouseEv->modifiers() & kModifier))) {
      if (inputData->getGraph() != graph) {
        graph = nullptr;
        started = false;
      }

      if (started) {
        started = false;

        if (!(w == 0 && h == 0)) {
          float width = glw->width();
          float height = glw->height();

          Coord bbMin = Coord(width - x, height - y + h);
          Coord bbMax = Coord(width - (x + w), height - y);

          if (abs(bbMax[0] - bbMin[0]) > 1 && abs(bbMax[1] - bbMin[1]) > 1) {

            BoundingBox sceneBB;
            sceneBB.expand(
                glw->getScene()->getGraphCamera().viewportTo3DWorld(glw->screenToViewport(bbMin)));
            sceneBB.expand(
                glw->getScene()->getGraphCamera().viewportTo3DWorld(glw->screenToViewport(bbMax)));

            QtGlSceneZoomAndPanAnimator zoomAnPan(glw, sceneBB);
            zoomAnPan.animateZoomAndPan();
          }
        }
      }

      return true;
    }
  }

  return false;
}
//=====================================================================
bool MouseBoxZoomer::draw(GlWidget *glw) {
  if (!started) {
    return false;
  }

  if (glw->getGlGraphInputData()->getGraph() != graph) {
    graph = nullptr;
    started = false;
  }

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, glw->width(), 0, glw->height(), -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);

  float col[4] = {0.8f, 0.4f, 0.4f, 0.2f};
  setColor(col);
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + w, y);
  glVertex2f(x + w, y - h);
  glVertex2f(x, y - h);
  glEnd();
  glDisable(GL_BLEND);

  glLineWidth(2);
  glLineStipple(2, 0xAAAA);
  glEnable(GL_LINE_STIPPLE);
  glBegin(GL_LINE_LOOP);
  glVertex2f(x, y);
  glVertex2f(x + w, y);
  glVertex2f(x + w, y - h);
  glVertex2f(x, y - h);
  glEnd();

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopAttrib();
  return true;
}

//=====================================================================
