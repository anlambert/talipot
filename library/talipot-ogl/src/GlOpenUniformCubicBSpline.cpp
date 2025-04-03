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

#include <talipot/GlOpenUniformCubicBSpline.h>
#include <talipot/GlBezierCurve.h>
#include <talipot/ParametricCurves.h>
#include <talipot/GlShaderProgram.h>

using namespace std;

namespace tlp {

static string bSplineSpecificShaderCode =
    "uniform float stepKnots;"

    "const int curveDegree = 3;"
    "float coeffs[curveDegree + 1];"

    "vec3 computeCurvePoint(float t) {"
    "	if (t == 0.0) {"
    "		return getControlPoint(0);"
    "	} else if (t >= 1.0) {"
    "		return getControlPoint(nbControlPoints - 1);"
    "	} else {"
    "		int k = curveDegree;"
    "		float cpt = 0.0;"
    "		while (t > (cpt * stepKnots) && t >= ((cpt+1.0) * stepKnots)) {"
    "			++k;"
    "			++cpt;"
    "		}"
    "		float knotVal = cpt * stepKnots;"
    "		for (int i = 0 ; i < (curveDegree + 1) ; ++i) {"
    "			coeffs[i] = 0.0;"
    "		}"
    "		coeffs[curveDegree] = 1.0;"
    "		for (int i = 1 ; i <= curveDegree ; ++i) {"
    "			coeffs[curveDegree-i] = (clamp(knotVal + stepKnots, 0.0, 1.0) - t) "
    "/ (clamp(knotVal + stepKnots, 0.0, 1.0) - clamp(knotVal + (-i+1) * stepKnots, 0.0, 1.0)) "
    "* coeffs[curveDegree-i+1];"
    "			int tabIdx = curveDegree-i+1;"
    "			for (int j = -i+1 ; j <= -1 ; ++j) {"
    "				coeffs[tabIdx] = ((t - clamp(knotVal + j * stepKnots, 0.0, "
    "1.0)) / (clamp(knotVal + (j+i) * stepKnots, 0.0, 1.0) - clamp(knotVal + j * stepKnots, "
    "0.0, 1.0))) * coeffs[tabIdx] + ((clamp(knotVal + (j+i+1) * stepKnots, 0.0, 1.0) - t) / "
    "(clamp(knotVal + (j+i+1) * stepKnots, 0.0, 1.0) - clamp(knotVal + (j+1) * stepKnots, 0.0, "
    "1.0))) * coeffs[tabIdx+1];"
    "				++tabIdx;"
    "			}"
    "			coeffs[curveDegree] = ((t - knotVal) / (clamp(knotVal + i * "
    "stepKnots, 0.0, 1.0) - knotVal)) * coeffs[curveDegree];"
    "		}"
    "		int startIdx = k - curveDegree;"
    "		vec3 curvePoint = vec3(0.0);"
    "		for (int i = 0 ; i <= curveDegree ; ++i) {"
    "			curvePoint += coeffs[i] * getControlPoint(startIdx + i);"
    "		}"
    "		return curvePoint;"
    "	}"
    "}";

const uint curveDegree = 3;

GlOpenUniformCubicBSpline::GlOpenUniformCubicBSpline()
    : AbstractGlCurve("open uniform cubic bspline vertex shader", bSplineSpecificShaderCode) {}

GlOpenUniformCubicBSpline::GlOpenUniformCubicBSpline(const vector<Coord> &controlPoints,
                                                     const Color &startColor, const Color &endColor,
                                                     const float startSize, const float endSize,
                                                     const uint nbCurvePoints)
    : AbstractGlCurve("open uniform cubic bspline vertex shader", bSplineSpecificShaderCode,
                      controlPoints, startColor, endColor, startSize, endSize, nbCurvePoints) {}

GlOpenUniformCubicBSpline::~GlOpenUniformCubicBSpline() = default;

void GlOpenUniformCubicBSpline::setCurveVertexShaderRenderingSpecificParameters() {
    curveShaderProgram->setUniformFloat("stepKnots", stepKnots);
}

void GlOpenUniformCubicBSpline::drawCurve(std::vector<Coord> &controlPoints,
                                          const Color &startColor, const Color &endColor,
                                          const float startSize, const float endSize,
                                          const uint nbCurvePoints) {

    nbKnots = controlPoints.size() + curveDegree + 1;
    stepKnots = 1.0f / ((float(nbKnots) - 2.0f * (float(curveDegree) + 1.0f)) + 2.0f - 1.0f);

    if (controlPoints.size() < (curveDegree + 1)) {
        static GlBezierCurve curve;
        curve.setOutlined(outlined);
        curve.setOutlineColor(outlineColor);
        curve.setLineCurve(lineCurve);
        curve.setCurveLineWidth(curveLineWidth);
        curve.setCurveQuadBordersWidth(curveQuadBordersWidth);
        curve.setOutlineColorInterpolation(outlineColorInterpolation);
        curve.setTexture(texture);
        curve.setBillboardCurve(billboardCurve);
        curve.setLookDir(lookDir);
        curve.drawCurve(controlPoints, startColor, endColor, startSize, endSize, nbCurvePoints);
    } else {
        AbstractGlCurve::drawCurve(controlPoints, startColor, endColor, startSize, endSize,
                                   nbCurvePoints);
    }
}

Coord GlOpenUniformCubicBSpline::computeCurvePointOnCPU(const std::vector<Coord> &controlPoints,
                                                        float t) {
    return computeOpenUniformBsplinePoint(controlPoints, t, curveDegree);
}

void GlOpenUniformCubicBSpline::computeCurvePointsOnCPU(const std::vector<Coord> &controlPoints,
                                                        std::vector<Coord> &curvePoints,
                                                        uint nbCurvePoints) {
    computeOpenUniformBsplinePoints(controlPoints, curvePoints, curveDegree, nbCurvePoints);
}
}
