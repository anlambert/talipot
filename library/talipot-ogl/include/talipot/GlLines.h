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

#ifndef TALIPOT_GL_LINES_H
#define TALIPOT_GL_LINES_H

#include <vector>

#include <talipot/OpenGlIncludes.h>

#include <talipot/Coord.h>
#include <talipot/Color.h>

#include <talipot/config.h>

namespace tlp {

struct TLP_GL_SCOPE GlLines {
    // Curves types: linear, bezier, spline order 3, spline order 4
    enum InterpolationMethod { LINEAR = 0, BEZIER, SPLINE3, SPLINE4 };
    enum StippleType { TLP_PLAIN = 0, TLP_DOT, TLP_DASHED, TLP_ALTERNATE };

    static void glDrawPoint(const Coord &p);

    static void glDrawLine(const Coord &startPoint, const Coord &endPoint, const double width,
                           const uint stippleType, const Color &startColor, const Color &endColor,
                           const bool arrow = false, const double arrowWidth = 1,
                           const double arrowHeight = 1);
    static void glDrawCurve(const Coord &startPoint, const std::vector<Coord> &bends,
                            const Coord &endPoint, const double width, const uint stippleType,
                            const Color &startColor, const Color &endColor,
                            const bool arrow = false, const double arrowWidth = 1,
                            const double arrowHeight = 1);
    static void glDrawBezierCurve(const Coord &startPoint, const std::vector<Coord> &bends,
                                  const Coord &endPoint, uint steps, const double width,
                                  const uint stippleType, const Color &startColor,
                                  const Color &endColor, const bool arrow = false,
                                  const double arrowWidth = 1, const double arrowHeight = 1);
    static void glDrawSplineCurve(const Coord &startPoint, const std::vector<Coord> &bends,
                                  const Coord &endPoint, uint steps, const double width,
                                  const uint stippleType, const Color &startColor,
                                  const Color &endColor, const bool arrow = false,
                                  const double arrowWidth = 1, const double arrowHeight = 1);
    static void glDrawSpline2Curve(const Coord &startPoint, const std::vector<Coord> &bends,
                                   const Coord &endPoint, uint steps, const double width,
                                   const uint stippleType, const Color &startColor,
                                   const Color &endColor, const bool arrow = false,
                                   const double arrowWidth = 1, const double arrowHeight = 1);

  private:
    static void glDisableLineStipple(uint stippleType);
    static void glEnableLineStipple(uint stippleType);
    static GLfloat *buildCurvePoints(const Coord &startPoint, const std::vector<Coord> &bends,
                                     const Coord &endPoint);
    static GLfloat *buildCurvePoints(const Coord &p0, const Coord &p1, const Coord &p2,
                                     const Coord &p3);
};
}
#endif // TALIPOT_GL_LINES_H
