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

#include <QApplication>
#include <QTimeLine>

#include <talipot/QtGlSceneZoomAndPanAnimator.h>
#include <talipot/GlWidget.h>

namespace tlp {

QtGlSceneZoomAndPanAnimator::QtGlSceneZoomAndPanAnimator(
    GlWidget *glWidget, const BoundingBox &boundingBox, const double duration,
    const std::string &layerName, const bool optimalPath, const double velocity, const double p)
    : GlSceneZoomAndPan(glWidget->scene(), boundingBox, layerName, 0, optimalPath, p),
      glWidget(glWidget), animationDurationMsec(duration) {
  if (doZoomAndPan) {
    animationDurationMsec *= S / velocity;
  }

  nbAnimationSteps = animationDurationMsec / 40 + 1;
}

void QtGlSceneZoomAndPanAnimator::animateZoomAndPan() {
  QTimeLine timeLine(animationDurationMsec);
  timeLine.setFrameRange(0, nbAnimationSteps);
  connect(&timeLine, &QTimeLine::frameChanged, this,
          &QtGlSceneZoomAndPanAnimator::zoomAndPanAnimStepSlot);

  if (doZoomAndPan || (additionalAnimation != nullptr)) {
    timeLine.start();

    while (timeLine.state() != QTimeLine::NotRunning) {
      QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
  }
}

void QtGlSceneZoomAndPanAnimator::zoomAndPanAnimStepSlot(int animationStep) {
  zoomAndPanAnimationStep(animationStep);
  glWidget->draw(false);
}
}
