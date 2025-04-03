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

#include <talipot/GlStar.h>

const float startAngle = float(M_PI) / 2.0f;

using namespace std;

namespace tlp {

GlStar::GlStar(const Coord &position, const Size &size, uint numberOfStarPoints,
               const Color &fillColor, const Color &outlineColor, bool outlined,
               const string &textureName, float outlineSize)
    : GlComplexPolygon(vector<Coord>(), fillColor, outlineColor, 0, textureName),
      position(position), size(size), numberOfStarPoints(numberOfStarPoints) {
    setFillColor(fillColor);
    setOutlineColor(outlineColor);
    setOutlineMode(outlined);
    setTextureName(textureName);
    setOutlineSize(outlineSize);
    computeStar();
}
//=====================================================
GlStar::~GlStar() = default;
//=====================================================
uint GlStar::getNumberOfStarPoints() const {
    return numberOfStarPoints;
}
//=====================================================
void GlStar::computeStar() {
    boundingBox = BoundingBox();

    BoundingBox box;
    vector<Coord> points;
    auto delta = float(2.0 * M_PI / numberOfStarPoints);

    for (uint i = 0; i < numberOfStarPoints; ++i) {
        float deltaX = cos(i * delta + startAngle);
        float deltaY = sin(i * delta + startAngle);
        points.push_back(Coord(deltaX, deltaY, 0));
        box.expand(points.back());
        deltaX = 0.5f * cos(i * delta + delta / 2.0f + startAngle);
        deltaY = 0.5f * sin(i * delta + delta / 2.0f + startAngle);
        points.push_back(Coord(deltaX, deltaY, 0));
        box.expand(points.back());
    }

    for (auto &p : points) {
        p[0] = position[0] +
               ((p[0] - ((box[1][0] + box[0][0]) / 2.)) / ((box[1][0] - box[0][0]) / 2.)) * size[0];
        p[1] = position[1] +
               ((p[1] - ((box[1][1] + box[0][1]) / 2.)) / ((box[1][1] - box[0][1]) / 2.)) * size[1];
    }

    boundingBox.expand(position + size / 2.f);
    boundingBox.expand(position - size / 2.f);

    createPolygon(points, 0);
    runTessellation();
}
}
