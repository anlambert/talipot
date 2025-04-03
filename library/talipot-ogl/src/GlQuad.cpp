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

#include <talipot/GlQuad.h>

#include <talipot/GlXMLTools.h>

using namespace std;

namespace tlp {

GlQuad::GlQuad() : GlPolygon(4, 4, 4, true, false) {}

GlQuad::GlQuad(const Coord &p1, const Coord &p2, const Coord &p3, const Coord &p4,
               const Color &color)
    : GlPolygon(4, 4, 4, true, false) {
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
    points[3] = p4;
    setFillColor(color);
    recomputeBoundingBox();
}

GlQuad::GlQuad(const Coord &p1, const Coord &p2, const Coord &p3, const Coord &p4, const Color &c1,
               const Color &c2, const Color &c3, const Color &c4) {
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
    points[3] = p4;
    fillColors[0] = c1;
    fillColors[1] = c2;
    fillColors[2] = c3;
    fillColors[3] = c4;
    recomputeBoundingBox();
}

GlQuad::~GlQuad() = default;

void GlQuad::setColor(const Color &color) {
    setFillColor(color);
}

void GlQuad::setPosition(const int idPosition, const Coord &position) {
    setPoint(idPosition, position);
}

void GlQuad::setColor(const int idColor, const Color &color) {
    setFillColor(idColor, color);
}

const Coord &GlQuad::getPosition(const int idPosition) const {
    return (*(const_cast<vector<Coord> *>(&points)))[idPosition];
}

const Color &GlQuad::getColor(const int idColor) const {
    return (*(const_cast<vector<Color> *>(&fillColors)))[idColor];
}
//===========================================================
void GlQuad::getXML(string &outString) {

    GlXMLTools::createProperty(outString, "type", "GlQuad", "GlEntity");

    GlPolygon::getXMLOnlyData(outString);
}
//============================================================
void GlQuad::setWithXML(const string &inString, uint &currentPosition) {

    GlPolygon::setWithXML(inString, currentPosition);
}
}
