/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_PARAMETRIC_CURVES_H
#define TALIPOT_PARAMETRIC_CURVES_H

#include <vector>

#include <talipot/config.h>
#include <talipot/Coord.h>

namespace tlp {

/**
 * Compute the position of a point 'p' at t (0 <= t <= 1)
 * along Bezier curve defined by a set of control points
 *
 * \param controlPoints a vector of control points
 * \param t curve parameter value (0 <= t <= 1)
 */
TLP_SCOPE Coord computeBezierPoint(const std::vector<Coord> &controlPoints, const float t);

/** Compute a set of points approximating a Bézier curve
 *
 *  \param controlPoints a vector of control points
 *  \param curvePoints an empty vector to store the computed points
 *  \param nbCurvePoints number of points to generate
 */
TLP_SCOPE void computeBezierPoints(const std::vector<Coord> &controlPoints,
                                   std::vector<Coord> &curvePoints, const uint nbCurvePoints = 100);

/**
 * Compute the position of a point 'p' at t (0 <= t <= 1)
 * along Catmull-Rom curve defined by a set of control points.
 * The features of this type of spline are the following :
 *    -> the spline passes through all of the control points
 *    -> the spline is C1 continuous, meaning that there are no discontinuities in the tangent
 * direction and magnitude
 *      -> the spline is not C2 continuous.  The second derivative is linearly interpolated within
 * each segment, causing the curvature to vary linearly over the length of the segment
 *
 * \param controlPoints a vector of control points
 * \param t curve parameter value (0 <= t <= 1)
 * \param closedCurve if true, the curve will be closed, meaning a Bézier segment will connect the
 * last and first control point
 * \param alpha curve parameterization parameter (0 <= alpha <= 1), alpha = 0 -> uniform
 * parameterization, alpha = 0.5 -> centripetal parameterization, alpha = 1.0 -> chord-length
 * parameterization
 */
TLP_SCOPE Coord computeCatmullRomPoint(const std::vector<Coord> &controlPoints, const float t,
                                       const bool closedCurve = false, const float alpha = 0.5);

/** Compute a set of points approximating a Catmull-Rom curve
 *
 *  \param controlPoints a vector of control points
 *  \param curvePoints an empty vector to store the computed points
 *  \param closedCurve if true, the curve will be closed, meaning a Bézier segment will connect the
 * last and first control point
 *  \param alpha curve parameterization parameter (0 <= alpha <= 1), alpha = 0 -> uniform
 * parameterization, alpha = 0.5 -> centripetal parameterization, alpha = 1.0 -> chord-length
 * parameterization
 *  \param nbCurvePoints number of points to generate
 */
TLP_SCOPE void computeCatmullRomPoints(const std::vector<Coord> &controlPoints,
                                       std::vector<Coord> &curvePoints,
                                       const bool closedCurve = false,
                                       const uint nbCurvePoints = 100, const float alpha = 0.5);

/**
 * Compute the position of a point 'p' at t (0 <= t <= 1)
 * along open uniform B-spline curve defined by a set of control points.
 * An uniform B-spline is a piecewise collection of Bézier curves of the same degree, connected end
 * to end.
 * The features of this type of spline are the following :
 *   -> the spline is C^2 continuous, meaning there is no discontinuities in curvature
 *     -> the spline has local control : its parameters only affect a small part of the entire
 * spline
 * A B-spline is qualified as open when it passes through its first and last control points.
 * \param controlPoints a vector of control points
 * \param t curve parameter value (0 <= t <= 1)
 * \param curveDegree the B-spline degree
 */

TLP_SCOPE Coord computeOpenUniformBsplinePoint(const std::vector<Coord> &controlPoints,
                                               const float t, const uint curveDegree = 3);

/** Compute a set of points approximating an open uniform B-spline curve
 *
 *  \param controlPoints a vector of control points
 *  \param curvePoints an empty vector to store the computed points
 *  \param curveDegree the B-spline degree
 *  \param nbCurvePoints number of points to generate
 */
TLP_SCOPE void computeOpenUniformBsplinePoints(const std::vector<Coord> &controlPoints,
                                               std::vector<Coord> &curvePoints,
                                               const uint curveDegree = 3,
                                               const uint nbCurvePoints = 100);
}

#endif // TALIPOT_PARAMETRIC_CURVES_H
