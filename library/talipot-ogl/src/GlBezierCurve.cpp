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

#include <talipot/GlBezierCurve.h>
#include <talipot/GlCatmullRomCurve.h>
#include <talipot/ParametricCurves.h>

using namespace std;

static const uint CONTROL_POINTS_LIMIT = 120;

static string bezierSpecificVertexShaderSrc =

    "vec3 computeCurvePoint(float t) {"
    "	if (t == 0.0) {"
    "		return getControlPoint(0);"
    "	} else if (t == 1.0) {"
    "		return getControlPoint(nbControlPoints - 1);"
    "	} else {"
    "		float s = (1.0 - t);"
    "     float r = float(nbControlPoints);"
    "     float curCoeff = 1.0;"
    "     float t2 = 1.0;"
    "		vec3 bezierPoint = vec3(0.0);"
    "		for (int i = 0 ; i < nbControlPoints ; ++i) { "
    "			bezierPoint += getControlPoint(i).xyz * curCoeff * t2 * pow(s, "
    "float(nbControlPoints - 1 - i));"
    "         float c = float(i+1);"
    "         curCoeff *= (r-c)/c;"
    "         t2 *= t;"
    "		}"
    "		return bezierPoint;"
    "	}"
    "}"

    ;

namespace tlp {

GlBezierCurve::GlBezierCurve()
    : AbstractGlCurve("bezier vertex shader", bezierSpecificVertexShaderSrc) {}

GlBezierCurve::GlBezierCurve(const vector<Coord> &controlPoints, const Color &startColor,
                             const Color &endColor, const float &startSize, const float &endSize,
                             const uint nbCurvePoints)
    : AbstractGlCurve("bezier vertex shader", bezierSpecificVertexShaderSrc, controlPoints,
                      startColor, endColor, startSize, endSize, nbCurvePoints) {}

GlBezierCurve::~GlBezierCurve() = default;

void GlBezierCurve::computeCurvePointsOnCPU(const std::vector<Coord> &controlPoints,
                                            std::vector<Coord> &curvePoints, uint nbCurvePoints) {
    computeBezierPoints(controlPoints, curvePoints, nbCurvePoints);
}

Coord GlBezierCurve::computeCurvePointOnCPU(const std::vector<Coord> &controlPoints, float t) {
    return computeBezierPoint(controlPoints, t);
}

void GlBezierCurve::drawCurve(std::vector<Coord> &controlPoints, const Color &startColor,
                              const Color &endColor, const float startSize, const float endSize,
                              const uint nbCurvePoints) {

    if (controlPoints.size() <= CONTROL_POINTS_LIMIT) {
        AbstractGlCurve::drawCurve(controlPoints, startColor, endColor, startSize, endSize,
                                   nbCurvePoints);
    } else {

        static GlCatmullRomCurve curve;

        const uint nbApproximationPoints = 40;
        vector<Coord> curvePoints;
        computeBezierPoints(controlPoints, curvePoints, nbApproximationPoints);
        curve.setClosedCurve(false);
        curve.setOutlined(outlined);
        curve.setOutlineColor(outlineColor);
        curve.setTexture(texture);
        curve.setBillboardCurve(billboardCurve);
        curve.setLookDir(lookDir);
        curve.drawCurve(curvePoints, startColor, endColor, startSize, endSize, nbCurvePoints);
    }
}
}
