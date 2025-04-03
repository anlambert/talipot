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

#include <talipot/GlPolygon.h>

using namespace std;

namespace tlp {
GlPolygon::GlPolygon(const bool filled, const bool outlined, const string &textureName,
                     const float outlineSize) {
    setFillMode(filled);
    setOutlineMode(outlined);
    setTextureName(textureName);
    setOutlineSize(outlineSize);
}
//=====================================================
GlPolygon::GlPolygon(const vector<Coord> &points, const vector<Color> &fcolors,
                     const vector<Color> &ocolors, const bool filled, const bool outlined,
                     const string &textureName, const float outlineSize) {
    setPoints(points);
    setFillColors(fcolors);
    setOutlineColors(ocolors);
    setFillMode(filled);
    setOutlineMode(outlined);
    setTextureName(textureName);
    setOutlineSize(outlineSize);
}
//=====================================================
GlPolygon::GlPolygon(const uint nbPoints, const uint nbFillColors, const uint nbOutlineColors,
                     const bool filled, const bool outlined, const string &textureName,
                     const float outlineSize) {
    vector<Coord> points;
    points.resize(nbPoints);
    setPoints(points);
    vector<Color> fillColors;
    fillColors.resize(nbFillColors, Color(0, 0, 0, 255));
    setFillColors(fillColors);
    vector<Color> outlineColors;
    outlineColors.resize(nbOutlineColors);
    setOutlineColors(outlineColors);
    setFillMode(filled);
    setOutlineMode(outlined);
    setTextureName(textureName);
    setOutlineSize(outlineSize);
}
//=====================================================
GlPolygon::~GlPolygon() = default;
//=====================================================
void GlPolygon::resizePoints(const uint nbPoints) {
    assert(nbPoints >= 3);
    points.resize(nbPoints);
    clearGenerated();
}
//=====================================================
void GlPolygon::resizeColors(const uint nbColors) {
    assert(nbColors >= 1);
    points.resize(nbColors);
    clearGenerated();
}
//=====================================================
const tlp::Coord &GlPolygon::point(const uint i) const {
    return points[i];
}
//=====================================================
tlp::Coord &GlPolygon::point(const uint i) {
    return points[i];
}
}
