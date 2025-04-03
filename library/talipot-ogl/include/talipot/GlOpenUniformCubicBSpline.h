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

#ifndef TALIPOT_GL_OPEN_UNIFORM_CUBIC_B_SPLINE_H
#define TALIPOT_GL_OPEN_UNIFORM_CUBIC_B_SPLINE_H

#include <talipot/AbstractGlCurve.h>

namespace tlp {

/**
 * @ingroup OpenGL
 * @brief A class to draw open uniform cubic B-splines
 *
 * A B-spline is a convenient form for representing complicated, smooth curves. A cubic uniform
 * B-spline is a
 * piecewise collection of cubic Bezier curves, connected end to end. A cubic B-spline is C^2
 * continuous, meaning there is no discontinuities in curvature.
 * B-splines have local control : parameters of a B-spline only affect a small part of the entire
 * spline.
 * A B-spline is qualified as open when it passes through its first and last control points.
 *
 */
class TLP_GL_SCOPE GlOpenUniformCubicBSpline : public AbstractGlCurve {

  public:
    /**
     * @brief Constructor
     * @warning Don't use it, see other construstor
     */
    GlOpenUniformCubicBSpline();

    /**
     * @brief GlOpenUniformCubicBSpline constructor
     *
     * @param controlPoints a vector of control points (size must be greater or equal to 4)
     * @param startColor the color at the start of the curve
     * @param endColor the color at the end of the curve
     * @param startSize the width at the start of the curve
     * @param endSize the width at the end of the curve
     * @param nbCurvePoints the number of curve points to generate
     */
    GlOpenUniformCubicBSpline(const std::vector<Coord> &controlPoints, const Color &startColor,
                              const Color &endColor, const float startSize, const float endSize,
                              const uint nbCurvePoints = 200);

    ~GlOpenUniformCubicBSpline() override;

    void drawCurve(std::vector<Coord> &controlPoints, const Color &startColor,
                   const Color &endColor, const float startSize, const float endSize,
                   const uint nbCurvePoints = 200) override;

  protected:
    void setCurveVertexShaderRenderingSpecificParameters() override;

    Coord computeCurvePointOnCPU(const std::vector<Coord> &controlPoints, float t) override;

    void computeCurvePointsOnCPU(const std::vector<Coord> &controlPoints,
                                 std::vector<Coord> &curvePoints, uint nbCurvePoints) override;

  private:
    unsigned nbKnots;
    float stepKnots;
};
}

#endif // TALIPOT_GL_OPEN_UNIFORM_CUBIC_B_SPLINE_H
