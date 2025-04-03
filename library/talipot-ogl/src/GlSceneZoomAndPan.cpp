/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/GlSceneZoomAndPan.h>
#include <talipot/GlScene.h>

using namespace std;

namespace tlp {

GlSceneZoomAndPan::GlSceneZoomAndPan(GlScene *glScene, const BoundingBox &boundingBox,
                                     const std::string &layerName, const int nbAnimationSteps,
                                     const bool optimalPath, const double p)
    : camera(glScene->getLayer(layerName)->getCamera()), viewport(glScene->getViewport()),
      nbAnimationSteps(nbAnimationSteps), optimalPath(optimalPath), p(p),
      camCenterStart(camera.getCenter()), camCenterEnd(boundingBox.center()),
      additionalAnimation(nullptr) {

    camCenterEnd[2] = camCenterStart[2];

    Coord blScene = camera.viewportTo3DWorld(Coord(0, 0, 0));
    Coord trScene = camera.viewportTo3DWorld(Coord(viewport[2], viewport[3], 0));

    BoundingBox sceneBB;
    sceneBB.expand(blScene);
    sceneBB.expand(trScene);

    zoomAreaWidth = boundingBox[1][0] - boundingBox[0][0];
    zoomAreaHeight = boundingBox[1][1] - boundingBox[0][1];

    float aspectRatio = viewport[2] / float(viewport[3]);

    if (zoomAreaWidth > (aspectRatio * zoomAreaHeight)) {
        w0 = sceneBB[1][0] - sceneBB[0][0];
        w1 = zoomAreaWidth;
    } else {
        w0 = sceneBB[1][1] - sceneBB[0][1];
        w1 = zoomAreaHeight;
    }

    u0 = 0;
    u1 = camCenterStart.dist(camCenterEnd);

    if (u1 < 1e-3) {
        u1 = 0;
    }

    if (optimalPath) {
        if (u0 != u1) {
            b0 = (w1 * w1 - w0 * w0 + p * p * p * p * u1 * u1) / (2 * w0 * p * p * u1);
            b1 = (w1 * w1 - w0 * w0 - p * p * p * p * u1 * u1) / (2 * w1 * p * p * u1);
            r0 = log(-b0 + sqrt(b0 * b0 + 1));
            r1 = log(-b1 + sqrt(b1 * b1 + 1));

            S = (r1 - r0) / p;
        } else {
            S = abs(log(w1 / w0)) / p;
        }
    } else {
        wm = max(w0, max(w1, p * p * (u1 - u0) / 2));
        sA = log(wm / w0) / p;
        sB = sA + p * (u1 - u0) / wm;
        S = sB + log(wm / w1) / p;
    }

    doZoomAndPan = abs(w0 - w1) > 1e-3 || u1 > 0;
}

void GlSceneZoomAndPan::setAdditionalGlSceneAnimation(
    AdditionalGlSceneAnimation *additionalAnimation) {
    this->additionalAnimation = additionalAnimation;

    if (additionalAnimation != nullptr) {
        additionalAnimation->setNbAnimationSteps(nbAnimationSteps);
    }
}

void GlSceneZoomAndPan::zoomAndPanAnimationStep(int animationStep) {
    if (doZoomAndPan) {
        double t = animationStep / double(nbAnimationSteps);
        double s = t * S;
        double u = 0, w = 0, f = 0;

        if (optimalPath) {
            if (u0 != u1) {
                u = w0 / (p * p) * cosh(r0) * tanh(p * s + r0) - w0 / (p * p) * sinh(r0) + u0;
                w = w0 * cosh(r0) / cosh(p * s + r0);
                f = u / u1;
            } else {
                double k = (w1 < w0) ? -1 : 1;
                w = w0 * exp(k * p * s);
                f = 0;
            }
        } else {
            if (s >= 0 && s < sA) {
                u = u0;
                w = w0 * exp(p * s);
            } else if (s >= sA && s < sB) {
                u = wm * (s - sA) / p + u0;
                w = wm;
            } else {
                u = u1;
                w = wm * exp(p * (sB - s));
            }

            if (u0 != u1) {
                f = u / u1;
            } else {
                f = 0;
            }
        }

        camera.setCenter(camCenterStart + (camCenterEnd - camCenterStart) * float(f));
        camera.setEyes(Coord(0, 0, camera.getSceneRadius()));
        camera.setEyes(camera.getEyes() + camera.getCenter());
        camera.setUp(Coord(0, 1., 0));

        Coord bbViewportFirst =
            camera.worldTo2DViewport(camera.getCenter() - Coord(w / 2, w / 2, 0));
        Coord bbViewportSecond =
            camera.worldTo2DViewport(camera.getCenter() + Coord(w / 2, w / 2, 0));
        float bbWidthViewport = abs(bbViewportSecond.getX() - bbViewportFirst.getX());
        float bbHeightViewport = abs(bbViewportSecond.getY() - bbViewportFirst.getY());
        double newZoomFactor = 0.0;

        float aspectRatio = viewport[2] / float(viewport[3]);

        if (zoomAreaWidth > (zoomAreaHeight * aspectRatio)) {
            newZoomFactor = viewport[2] / bbWidthViewport;
        } else {
            newZoomFactor = viewport[3] / bbHeightViewport;
        }

        camera.setZoomFactor(camera.getZoomFactor() * newZoomFactor);
    }

    if (additionalAnimation != nullptr) {
        additionalAnimation->animationStep(animationStep);
    }
}
}
