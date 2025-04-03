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

namespace tlp {

/**
 * A class to draw a curve interpolating a set of points with C^2 continuity
 *
 * This class allows to draw a cubic B-spline interpolating a set of points. The resulting curve
 * is C^2 continuous, so there is no discontinuities in curvature.
 */
class TLP_GL_SCOPE GlCubicBSplineInterpolation : public GlOpenUniformCubicBSpline {

  public:
    /**
     * GlCubicBSplineInterpolation constructor
     *
     * \param pointsToInterpolate the set of points to interpolate
     * \param startColor the color at the start of the curve
     * \param endColor the color at the end of the curve
     * \param startSize the width at the start of the curve
     * \param endSize the width at the end of the curve
     * \param nbCurvePoints the number of curve points to generate
     */
    GlCubicBSplineInterpolation(const std::vector<Coord> &pointsToInterpolate,
                                const Color &startColor, const Color &endColor,
                                const float startSize, const float endSize,
                                const uint nbCurvePoints = 100);

  private:
    std::vector<Coord>
    constructInterpolatingCubicBSpline(const std::vector<Coord> &pointsToInterpolate);
};
}
