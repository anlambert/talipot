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
#include <talipot/MouseBoxZoomer.h>
#include <talipot/GlComplexPolygon.h>

using namespace std;
using namespace tlp;

MouseBoxZoomer::MouseBoxZoomer(Qt::MouseButton button, Qt::KeyboardModifier modifier)
    : mButton(button), kModifier(modifier), x(0), y(0), w(0), h(0), started(false), graph(nullptr) {
}
MouseBoxZoomer::~MouseBoxZoomer() = default;
//=====================================================================
bool MouseBoxZoomer::eventFilter(QObject *widget, QEvent *e) {
  auto *glw = static_cast<GlWidget *>(widget);
  GlGraphInputData *inputData = glw->inputData();

  if (e->type() == QEvent::MouseButtonPress) {
    auto *qMouseEv = static_cast<QMouseEvent *>(e);

    if (qMouseEv->buttons() == mButton &&
        (kModifier == Qt::NoModifier || qMouseEv->modifiers() & kModifier)) {
      if (!started) {
        x = qMouseEv->pos().x();
        y = glw->height() - qMouseEv->pos().y();
        w = 0;
        h = 0;
        started = true;
        graph = inputData->graph();
      } else {
        if (inputData->graph() != graph) {
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
      if (inputData->graph() != graph) {
        graph = nullptr;
        started = false;
      }

      if (started) {
        if ((qMouseEv->pos().x() > 0) && (qMouseEv->pos().x() < glw->width())) {
          w = qMouseEv->pos().x() - x;
        }

        if ((qMouseEv->pos().y() > 0) && (qMouseEv->pos().y() < glw->height())) {
          h = y - (glw->height() - qMouseEv->pos().y());
        }

        glw->redraw();
        return true;
      }
    }
  }

  if (e->type() == QEvent::MouseButtonDblClick) {
    glw->zoomAndPanAnimation(BoundingBox());
    return true;
  }

  if (e->type() == QEvent::MouseButtonRelease) {

    auto *qMouseEv = static_cast<QMouseEvent *>(e);

    if ((qMouseEv->button() == mButton &&
         (kModifier == Qt::NoModifier || qMouseEv->modifiers() & kModifier))) {
      if (inputData->graph() != graph) {
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
                glw->scene()->graphCamera().viewportTo3DWorld(glw->screenToViewport(bbMin)));
            sceneBB.expand(
                glw->scene()->graphCamera().viewportTo3DWorld(glw->screenToViewport(bbMax)));
            glw->zoomAndPanAnimation(sceneBB);
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

  if (glw->inputData()->graph() != graph) {
    graph = nullptr;
    started = false;
  }

  Camera *camera = &glw->scene()->getLayer("Main")->getCamera();
  Camera camera2D(camera->getScene(), false);

  float xf = float(x);
  float yf = float(y);
  float wf = float(w);
  float hf = float(h);

  vector<Coord> rectPoints = {{xf, yf}, {xf + wf, yf}, {xf + wf, yf - hf}, {xf, yf - hf}};
  Color color = {200, 0, 0};

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  camera2D.initGl();
  GlComplexPolygon complexPolygon(rectPoints, Color(color[0], color[1], color[2], 100), color);
  complexPolygon.setOutlineSize(2);
  complexPolygon.setOutlineStippled(true);
  complexPolygon.draw(0, nullptr);

  return true;
}

//=====================================================================
