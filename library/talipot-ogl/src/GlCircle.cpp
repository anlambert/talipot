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

#include <talipot/GlCircle.h>
#include <talipot/GlXMLTools.h>

using namespace std;

namespace tlp {

//===================================================
GlCircle::GlCircle(const Coord &center, float radius, const Color &outlineColor,
                   const Color &fillColor, bool filled, bool outlined, float startAngle,
                   uint segments)
    : GlRegularPolygon(center, Size(radius, radius, 0), segments, fillColor, outlineColor, filled,
                       outlined) {
    assert(segments <= 256);
    setStartAngle(startAngle);
}
//===========================================================
void GlCircle::set(const Coord &center, float radius, float startAngle) {
    this->position = center;
    this->size = Size(radius, radius, 0);
    this->startAngle = startAngle;
    computePolygon();
}
//===========================================================
void GlCircle::getXML(string &outString) {

    GlXMLTools::createProperty(outString, "type", "GlCircle", "GlEntity");

    GlPolygon::getXMLOnlyData(outString);
}
}
