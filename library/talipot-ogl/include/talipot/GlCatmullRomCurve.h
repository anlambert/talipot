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

#ifndef TALIPOT_GL_CATMULL_ROM_CURVE_H
#define TALIPOT_GL_CATMULL_ROM_CURVE_H

#include <vector>

#include <talipot/AbstractGlCurve.h>

namespace tlp {

/**
 * @ingroup OpenGL
 * @brief A class to draw a Catmull-Rom curve
 *
 * This class allow to draw a Catmull-Rom curve, a smooth curve which passes through all its control
 * points.
 * Catmull-Rom splines are a family of cubic interpolating splines formulated such that the tangent
 * at each
 * control point is calculated using the previous and next control point point of the spline.
 * Catmull-Rom splines have C^1 continuity, local control, and interpolation, but do not lie within
 * the convex
 * hull of their control points.
 */
class TLP_GL_SCOPE GlCatmullRomCurve : public AbstractGlCurve {

    enum ParameterizationType { UNIFORM, CHORD_LENGTH, CENTRIPETAL };

  public:
    GlCatmullRomCurve();

    /**
     * @brief GlCatmullRomCurve constructor
     *
     * @param controlPoints a vector of control points (size must be greater or equal to 4)
     * @param startColor the color at the start of the curve
     * @param endColor the color at the end of the curve
     * @param startSize the width at the start of the curve
     * @param endSize the width at the end of the curve
     * @param closedCurve if true, the curve will be closed and a bezier segment will be drawn
     * between the last and first control point
     * @param paramType curve parameterization type (GlCatmullRomCurve::UNIFORM |
     * GlCatmullRomCurve::CENTRIPETAL | GlCatmullRomCurve::CHORD_LENGTH (default))
     * @param nbCurvePoints the number of curve points to generate
     */
    GlCatmullRomCurve(const std::vector<Coord> &controlPoints, const Color &startColor,
                      const Color &endColor, const float startSize, const float endSize,
                      const bool closedCurve = false, const uint nbCurvePoints = 200,
                      const ParameterizationType paramType = CENTRIPETAL);

    ~GlCatmullRomCurve() override;

    void setParameterizationType(const ParameterizationType paramType) {
        this->paramType = paramType;
    }

    void drawCurve(std::vector<Coord> &controlPoints, const Color &startColor,
                   const Color &endColor, const float startSize, const float endSize,
                   const uint nbCurvePoints = 200) override;

    void setClosedCurve(const bool closedCurve) {
        this->closedCurve = closedCurve;
    }

  protected:
    void setCurveVertexShaderRenderingSpecificParameters() override;

    Coord computeCurvePointOnCPU(const std::vector<Coord> &controlPoints, float t) override;

    void computeCurvePointsOnCPU(const std::vector<Coord> &controlPoints,
                                 std::vector<Coord> &curvePoints, uint nbCurvePoints) override;

  private:
    bool closedCurve;
    float totalLength;
    float alpha;
    ParameterizationType paramType;
};
}

#endif // TALIPOT_GL_CATMULL_ROM_CURVE_H
