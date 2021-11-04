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

#include <talipot/QtGlSceneZoomAndPanAnimator.h>
#include <talipot/GlWidget.h>

using namespace std;

namespace tlp {
void zoomOnScreenRegion(GlWidget *glWidget, const BoundingBox &boundingBox, const bool optimalPath,
                        const double velocity, const double p) {
  QtGlSceneZoomAndPanAnimator animator(glWidget, boundingBox, 1000, "Main", optimalPath, velocity,
                                       p);
  animator.animateZoomAndPan();
}

void zoomOnScreenRegionWithoutAnimation(GlWidget *glWidget, const BoundingBox &boundingBox) {
  Camera &camera = glWidget->scene()->graphCamera();
  Coord bbScreenFirst = camera.worldTo2DViewport(Coord(boundingBox[0]));
  Coord bbScreenSecond = camera.worldTo2DViewport(Coord(boundingBox[1]));
  float bbWidthScreen = bbScreenSecond.getX() - bbScreenFirst.getX();
  float bbHeightScreen = bbScreenSecond.getY() - bbScreenFirst.getY();

  bbWidthScreen += bbWidthScreen * .1;
  bbHeightScreen += bbHeightScreen * .1;

  float startSize, endSize;

  if (bbHeightScreen < bbWidthScreen) {
    startSize = glWidget->width();
    endSize = bbWidthScreen;
  } else {
    startSize = glWidget->height();
    endSize = bbHeightScreen;
  }

  float zoomFactor = startSize / endSize;

  double zoomStart = camera.getZoomFactor();
  double zoomEnd = zoomStart * zoomFactor;
  bool withZoom = zoomFactor < 0.99 || zoomFactor > 1.01;

  Coord newCamCenter = Coord(boundingBox[0]) + Coord(boundingBox[1]) / 2.0f;
  newCamCenter.setZ(boundingBox[0][2]);

  camera.setCenter(newCamCenter);
  camera.setEyes(Coord(0, 0, camera.getSceneRadius()));
  camera.setEyes(camera.getEyes() + camera.getCenter());
  camera.setUp(Coord(0, 1., 0));

  if (withZoom) {
    camera.setZoomFactor(zoomEnd);
  }
}
}
