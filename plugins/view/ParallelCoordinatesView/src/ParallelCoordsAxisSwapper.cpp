/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <tulip/GlLayer.h>
#include <tulip/GlBoundingBoxSceneVisitor.h>
#include <tulip/GlRect.h>
#include <tulip/GlCircle.h>
#include <tulip/GlQuad.h>
#include <tulip/GlMainWidget.h>
#include <tulip/Camera.h>

#include "ParallelTools.h"
#include "ParallelCoordinatesView.h"
#include "ParallelCoordsAxisSwapper.h"
#include "ParallelAxis.h"

#include <QMouseEvent>

using namespace std;

namespace tlp {

static Color axisHighlight(14, 241, 212, 127);

static Color axisToSwapHighlight(0, 255, 0, 127);

ParallelCoordsAxisSwapper::ParallelCoordsAxisSwapper()
    : parallelView(nullptr), selectedAxis(nullptr), otherAxisToSwap(nullptr),
      initialSelectedAxisRotAngle(0), dragStarted(false), x(0), y(0), mouseMove(false),
      axisSwapStarted(false) {}

ParallelCoordsAxisSwapper::~ParallelCoordsAxisSwapper() {}

void ParallelCoordsAxisSwapper::viewChanged(View *view) {
  parallelView = static_cast<ParallelCoordinatesView *>(view);
}

bool ParallelCoordsAxisSwapper::eventFilter(QObject *widget, QEvent *e) {

  GlMainWidget *glWidget = static_cast<GlMainWidget *>(widget);
  QMouseEvent *me = static_cast<QMouseEvent *>(e);

  mouseMove = false;

  if (e->type() == QEvent::MouseMove && !axisSwapStarted) {

    mouseMove = true;

    if (!dragStarted) {
      selectedAxis = parallelView->getAxisUnderPointer(me->x(), me->y());
    } else {
      x = glWidget->width() - me->x();
      y = me->y();
      Coord screenCoords(x, y, 0);
      Coord sceneCoords(glWidget->getScene()->getLayer("Main")->getCamera().viewportTo3DWorld(
          glWidget->screenToViewport(screenCoords)));

      if (parallelView->getLayoutType() == ParallelCoordinatesDrawing::CIRCULAR) {
        float rotAngle = computeABACAngleWithAlKashi(Coord(0.0f, 0.0f, 0.0f),
                                                     Coord(0.0f, 50.0f, 0.0f), sceneCoords);

        if (sceneCoords.getX() < 0.0f) {
          selectedAxis->setRotationAngle(rotAngle);
        } else {
          selectedAxis->setRotationAngle(-rotAngle);
        }

      } else {
        Coord translationVector(sceneCoords - selectedAxis->getBaseCoord());
        selectedAxis->translate(Coord(translationVector.getX(), 0.0f, 0.0f));
      }

      otherAxisToSwap = parallelView->getAxisUnderPointer(me->x(), me->y());
    }

    parallelView->refresh();
    return true;

  } else if (e->type() == QEvent::MouseButtonPress && me->button() == Qt::LeftButton) {
    if (selectedAxis != nullptr && !dragStarted) {
      dragStarted = true;
      parallelView->removeAxis(selectedAxis);
      initialSelectedAxisRotAngle = selectedAxis->getRotationAngle();
      selectedAxis->setRotationAngle(0.0f);
      initialSelectedAxisCoord = selectedAxis->getBaseCoord();
      parallelView->getGlMainWidget()->draw();
    }

    return true;

  } else if (e->type() == QEvent::MouseButtonRelease && me->button() == Qt::LeftButton) {

    if (selectedAxis != nullptr && dragStarted) {
      selectedAxis->setRotationAngle(0.0f);
      Coord translationVector(initialSelectedAxisCoord - selectedAxis->getBaseCoord());
      selectedAxis->translate(Coord(translationVector.getX(), translationVector.getY(), 0.0f));
      selectedAxis->setRotationAngle(initialSelectedAxisRotAngle);
      parallelView->addAxis(selectedAxis);

      if (otherAxisToSwap != nullptr && otherAxisToSwap != selectedAxis) {
        axisSwapStarted = true;
        parallelView->swapAxis(selectedAxis, otherAxisToSwap);
        axisSwapStarted = false;
        otherAxisToSwap = nullptr;
      }

      selectedAxis = nullptr;
      dragStarted = false;
      parallelView->draw();
    }

    return true;
  }

  selectedAxis = nullptr;
  return false;
}

bool ParallelCoordsAxisSwapper::draw(GlMainWidget *glMainWidget) {

  if (selectedAxis != nullptr) {

    glMainWidget->getScene()->getLayer("Main")->getCamera().initGl();

    GlQuad *axisHighlightRect = nullptr;
    BoundingBox axisBB;

    if (!dragStarted) {
      Array<Coord, 4> axisBP(selectedAxis->getBoundingPolygonCoords());
      axisHighlightRect = new GlQuad(axisBP[0], axisBP[1], axisBP[2], axisBP[3], axisHighlight);

    } else {
      if (otherAxisToSwap != nullptr && otherAxisToSwap != selectedAxis) {
        Array<Coord, 4> axisBP(otherAxisToSwap->getBoundingPolygonCoords());
        axisHighlightRect =
            new GlQuad(axisBP[0], axisBP[1], axisBP[2], axisBP[3], axisToSwapHighlight);
      }
    }

    if (axisHighlightRect != nullptr) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);
      glEnable(GL_LIGHTING);
      axisHighlightRect->draw(0, nullptr);
      glDisable(GL_LIGHTING);
      glDisable(GL_BLEND);
      delete axisHighlightRect;
    }

    if (dragStarted && mouseMove) {
      selectedAxis->disableTrickForSelection();
      selectedAxis->draw(0, &glMainWidget->getScene()->getLayer("Main")->getCamera());
      selectedAxis->enableTrickForSelection();
    }

    return true;
  }

  return false;
}
} // namespace tlp
