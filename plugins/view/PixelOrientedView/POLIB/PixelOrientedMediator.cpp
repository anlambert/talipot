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

#include "PixelOrientedMediator.h"

using namespace tlp;

PixelOrientedMediator::PixelOrientedMediator(LayoutFunction *layout, ColorFunction *color)
    : layout(layout), color(color), trans1(new FishEyesScreen()),
      trans2(new UniformDeformationScreen()), zoomBak(1), translationXBak(0), translationYBak(0),
      fishEyeRadiusBak(0), centerItem(uint(-1)) {
    zoom = 1.0;
    totalMove[0] = 0;
    totalMove[1] = 0;
    totalFishMove[0] = 0;
    totalFishMove[1] = 0;
    fishTranslation[0] = 0;
    fishTranslation[1] = 0;
}

PixelOrientedMediator::~PixelOrientedMediator() = default;

Vec2f PixelOrientedMediator::screenToScene(const Vec2i &p) {
    Vec2f q;
    q[0] = double(p[0]) - double(imageSize[0] / 2.);
    q[1] = double(p[1]) - double(imageSize[1] / 2.);
    return trans2->unproject(trans1->unproject(q));
}

Vec2i PixelOrientedMediator::sceneToScreen(const Vec2i &p) {
    Vec2f q;
    q[0] = double(p[0]);
    q[1] = double(p[1]);
    Vec2f res1 = trans1->project(trans2->project(q));
    res1[0] += imageSize[0] / 2.0;
    res1[1] += imageSize[1] / 2.0;
    Vec2i result;
    result[0] = int(rint(res1[0]));
    result[1] = int(rint(res1[1]));
    return result;
}

void PixelOrientedMediator::changeZoom(int i) {
    if (i < 50) {
        zoom = double(i) / 50.;
        trans2->setZoom(double(i) / 50.);
    } else {
        zoom = double(i) - 49.;
        trans2->setZoom(double(i) - 49.);
    }
}

void PixelOrientedMediator::backupScreenFunctionsParameters() {
    zoomBak = trans2->getZoom();
    trans2->getTranslation(translationXBak, translationYBak);
    fishEyeRadiusBak = trans1->getRadius();
}

void PixelOrientedMediator::setScreenFunctionsParameters(double zoom, double translationX,
                                                         double translationY,
                                                         double fishEyeRadius) {
    trans2->setZoom(zoom);
    trans2->setTranslation(translationX, translationY);
    trans1->setRadius(fishEyeRadius);
}

void PixelOrientedMediator::restoreScreenFunctionsParameters() {
    trans2->setZoom(zoomBak);
    trans2->setTranslation(translationXBak, translationYBak);
    trans1->setRadius(fishEyeRadiusBak);
}

void PixelOrientedMediator::setLastMousePosition(const int x, const int y) {
    int yScreen = imageSize[1] - y;
    lastMousePosition[0] = x;
    lastMousePosition[1] = yScreen;
}

void PixelOrientedMediator::updateFishEyePosition(const int x, const int y, DimensionBase *data) {
    int yScreen = imageSize[1] - y;

    if (lastMousePosition[0] == x && lastMousePosition[1] == yScreen) {
        totalFishMove[0] = 0;
        totalFishMove[1] = 0;
        fishTranslation[0] = 0;
        fishTranslation[1] = 0;
        Vec2i point;
        point[0] = x;
        point[1] = y;
        Vec2f pos = screenToScene(point);
        point[0] = int(rint(pos[0]));
        point[1] = int(rint(pos[1]));
        centerItem = data->getItemIdAtRank(layout->unproject(point));
        fishCenter = trans2->project(pos);
        trans1->setCenter(fishCenter[0], fishCenter[1]);
    } else {
        totalMove[0] += int((lastMousePosition[0] - x) / zoom);
        totalMove[1] -= int((lastMousePosition[1] - yScreen) / zoom);
        totalFishMove[0] += lastMousePosition[0] - x;
        totalFishMove[1] += lastMousePosition[1] - yScreen;
    }
}

void PixelOrientedMediator::translateFishEye(const int x, const int y) {
    int yScreen = imageSize[1] - y;
    //  trans2->setTranslation((totalMove[0] + (lastMousePosition[0] - x) / zoom),
    //              (totalMove[1] - (lastMousePosition[1] - yScreen) / zoom));
    fishTranslation[0] = -((totalFishMove[0] + lastMousePosition[0] - x));
    fishTranslation[1] = ((totalFishMove[1] + lastMousePosition[1] - yScreen));
    trans1->setCenter(fishCenter[0] - ((totalFishMove[0] + lastMousePosition[0] - x)),
                      fishCenter[1] + ((totalFishMove[1] + lastMousePosition[1] - yScreen)));
}

uint PixelOrientedMediator::getRankForPixelPos(Vec2i pos) {
    Vec2f fPos = screenToScene(pos);
    Vec2i fPosI;
    fPosI[0] = int(rint(fPos[0]));
    fPosI[1] = int(rint(fPos[1]));
    return layout->unproject(fPosI);
}

Color PixelOrientedMediator::getColorForPixelAtPos(Vec2i pos, DimensionBase *data,
                                                   bool withFishEye) {
    Color backgroundColor;
    backgroundColor.fill(255);

    Vec2f fPos = screenToScene(pos);
    Vec2i fPosI;
    fPosI[0] = int(rint(fPos[0]));
    fPosI[1] = int(rint(fPos[1]));

    uint rank = layout->unproject(fPosI);

    if (rank < data->numberOfItems()) {
        Color curColor =
            color->getColor(data->getItemValueAtRank(rank), data->getItemIdAtRank(rank));

        if (withFishEye) {
            Vec2f fPosIf;
            fPosIf[0] = fPosI[0];
            fPosIf[1] = fPosI[1];
            Vec2f delta = fPos - fPosIf;
            double dist = delta.norm();
            Vec2f tmp;
            tmp[0] = pos[0] - double(imageSize[0]) / 2. - fishTranslation[0];
            tmp[1] = pos[1] - double(imageSize[0]) / 2. - fishTranslation[1];

            if ((fishCenter - tmp).norm() < 70.) {
                for (uint i = 0; i < 3; ++i) {
                    double a = -double(curColor[i]);
                    double b = curColor[i];
                    curColor[i] = uchar(a * dist * dist + b);
                }
            }
        }

        return curColor;

    } else {
        return backgroundColor;
    }
}

Vec2i PixelOrientedMediator::getPixelPosForRank(const uint rank) {
    return sceneToScreen(layout->project(rank));
}
