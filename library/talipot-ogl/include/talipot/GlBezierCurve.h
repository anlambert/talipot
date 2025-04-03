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

#ifndef TALIPOT_GL_BEZIER_CURVE_H
#define TALIPOT_GL_BEZIER_CURVE_H

#include <talipot/AbstractGlCurve.h>

namespace tlp {

/**
 * @ingroup OpenGL
 * @brief A class to draw Bezier curves
 *
 * This class allows to draw Bezier curves defined by an arbitrary number of control points.
 * Bezier curves are named after their inventor, Dr. Pierre Bezier. He was an engineer with the
 * Renault car company and set out in the early 1960's to develop a curve formulation which would
 * lend itself to shape design.
 * Bezier curves are widely used in computer graphics to model smooth curves. A Bezier curve is
 * completely contained in the convex hull of its control points and passes through its first and
 * last control points. The curve is also always tangent to the first and last convex hull polygon
 * segments.
 * In addition, the curve shape tends to follow the polygon shape.
 *
 */
class TLP_GL_SCOPE GlBezierCurve : public AbstractGlCurve {

  public:
    GlBezierCurve();

    /**
     * @brief GlBezierCurve constructor
     *
     * @param controlPoints a vector of control points (size must be greater or equal to 2)
     * @param startColor the color at the start of the curve
     * @param endColor the color at the end of the curve
     * @param startSize the width at the start of the curve
     * @param endSize the width at the end of the curve
     * @param nbCurvePoints the number of curve points to generate
     */
    GlBezierCurve(const std::vector<Coord> &controlPoints, const Color &startColor,
                  const Color &endColor, const float &startSize, const float &endSize,
                  const uint nbCurvePoints = 200);

    ~GlBezierCurve() override;

    void drawCurve(std::vector<Coord> &controlPoints, const Color &startColor,
                   const Color &endColor, const float startSize, const float endSize,
                   const uint nbCurvePoints = 200) override;

  protected:
    Coord computeCurvePointOnCPU(const std::vector<Coord> &controlPoints, float t) override;

    void computeCurvePointsOnCPU(const std::vector<Coord> &controlPoints,
                                 std::vector<Coord> &curvePoints, uint nbCurvePoints) override;

    std::string genCurveVertexShaderSpecificCode();
};
}
#endif // TALIPOT_GL_BEZIER_CURVE_H
