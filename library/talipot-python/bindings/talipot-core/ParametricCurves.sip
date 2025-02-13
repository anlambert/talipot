/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

%ModuleHeaderCode
#include <talipot/ParametricCurves.h>
%End

namespace tlp {

tlp::Coord computeBezierPoint(const std::vector<tlp::Coord> &controlPoints, const float t);
%Docstring
tlp.computeBezierPoint(controlPoints, t)

Computes the position of the point at t (0 <= t <= 1) along a Bezier curve defined by a set of
control points.

:param controlPoints:
   the control points of the Bezier curve

:type controlPoints:
   list of :class:`tlp.Coord`

:param t:
   the curve parameter

:type t:
   float

:rtype:
   :class:`tlp.Coord`
%End

// =================================================================================================

void computeBezierPoints(const std::vector<tlp::Coord> &controlPoints, std::vector<tlp::Coord> &curvePoints /Out/, const uint nbCurvePoints = 100);
%Docstring
tlp.computeBezierPoints(controlPoints, nbCurvePoints=100)

Computes a set of points approximating a Bezier curve.

:param controlPoints:
   the control points of the Bezier curve

:type controlPoints:
   list of :class:`tlp.Coord`

:param nbCurvePoints:
   the number of curve points to compute

:type nbCurvePoints:
   integer

:rtype:
   list of :class:`tlp.Coord`
%End


// =================================================================================================

tlp::Coord computeCatmullRomPoint(const std::vector<tlp::Coord> &controlPoints, const float t, const bool closedCurve = false, const float alpha = 0.5);
%Docstring
tlp.computeCatmullRomPoint(controlPoints, t, closedCurve=False, alpha=0.5)

Computes the position of the point at t (0 <= t <= 1) along a Catmull-Rom curve defined by a set of
control points. The features of this type of spline are the following :

   -  the spline passes through all of the control points
   -  the spline is C1 continuous, meaning that there are no discontinuities in the tangent
      direction and magnitude
   -  the spline is not C2 continuous. The second derivative is linearly interpolated within each
      segment, causing the curvature to vary linearly over the length of the segment

:param controlPoints:
   the control points of the Catmull Rom curve

:type controlPoints:
   list of :class:`tlp.Coord`

:param t:
   the curve parameter

:type t:
   float

:param closedCurve:
   if :const:`True`, the curve will be closed, meaning a Bezier segment will connect the last and
   first control point

:type closedCurve:
   boolean

:param alpha:
   curve parameterization parameter (0 <= alpha <= 1), alpha = 0 -> uniform parameterization, alpha
   = 0.5 -> centripetal parameterization, alpha = 1.0 -> chord-length parameterization

:type alpha:
   float

:rtype:
   :class:`tlp.Coord`
%End

// =================================================================================================

void computeCatmullRomPoints(const std::vector<tlp::Coord> &controlPoints, std::vector<tlp::Coord> &curvePoints /Out/, const bool closedCurve = false, const uint nbCurvePoints = 100, const float alpha = 0.5);
%Docstring
tlp.computeCatmullRomPoints(controlPoints, closedCurve=False, nbCurvePoints=100, alpha=0.5)

Computes a set of points approximating a Catmull-Rom curve.

:param controlPoints:
   the control points of the Catmull Rom curve

:type controlPoints:
   list of :class:`tlp.Coord`

:param closedCurve:
   if :const:`True`, the curve will be closed, meaning a Bezier segment will connect the last and
   first control point

:type closedCurve:
   boolean

:param nbCurvePoints:
   the number of curve points to compute

:type nbCurvePoints:
   integer

:param alpha:
   curve parameterization parameter (0 <= alpha <= 1), alpha = 0 -> uniform parameterization, alpha
   = 0.5 -> centripetal parameterization, alpha = 1.0 -> chord-length parameterization

:type alpha:
   floatts to compute

:rtype:
   list of :class:`tlp.Coord`
%End

// =================================================================================================

tlp::Coord computeOpenUniformBsplinePoint(const std::vector<tlp::Coord> &controlPoints, const float t, const uint curveDegree = 3);
%Docstring
tlp.computeOpenUniformBsplinePoint(controlPoints, t, curveDegree=3)

Computes the position of the point at t (0 <= t <= 1) along an open uniform B-spline curve defined
by a set of control points. An uniform B-spline is a piecewise collection of Bezier curves of the
same degree, connected end to end. The features of this type of spline are the following :

   -  the spline is C^2 continuous, meaning there is no discontinuities in curvature
   -  the spline has local control : its parameters only affect a small part of the entire spline

A B-spline is qualified as open when it passes through its first and last control points.

:param controlPoints:
   the control points of the B-spline curve

:type controlPoints:
   list of :class:`tlp.Coord`

:param t:
   the curve parameter

:type t:
   float

:param curveDegree:
   the B-spline degree

:type curveDegree:
   integer

:rtype:
   :class:`tlp.Coord`
%End

// =================================================================================================

void computeOpenUniformBsplinePoints(const std::vector<tlp::Coord> &controlPoints, std::vector<tlp::Coord> &curvePoints /Out/, const uint curveDegree = 3, const uint nbCurvePoints = 100);
%Docstring
tlp.computeOpenUniformBsplinePoints(controlPoints, curveDegree=3, nbCurvePoints=100)

Computes a set of points approximating an open uniform B-spline curve.

:param controlPoints:
   the control points of the Catmull Rom curve

:type controlPoints:
   list of :class:`tlp.Coord`

:param curveDegree:
   the B-spline degree

:type curveDegree:
   integer

:param nbCurvePoints:
   the number of curve points to compute

:type nbCurvePoints:
   integer

:rtype:
   list of :class:`tlp.Coord`
%End

};
