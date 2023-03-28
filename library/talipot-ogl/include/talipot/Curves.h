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

#ifndef TALIPOT_CURVES_H
#define TALIPOT_CURVES_H

#include <talipot/OpenGlIncludes.h>

#include <talipot/Coord.h>
#include <talipot/Color.h>
#include <talipot/config.h>

#include <vector>

namespace tlp {

template <typename T, size_t N>
class Matrix;
//====================================================================
// return vertices, update startN and endN to prevent bad edge drawing
TLP_GL_SCOPE void computeCleanVertices(const std::vector<Coord> &bends, const Coord &startPoint,
                                       const Coord &endPoint, Coord &startN, Coord &endN,
                                       std::vector<Coord> &vertices, bool adjustTangent = true);
TLP_GL_SCOPE void polyLine(const std::vector<Coord> &, /* polyline vertices */
                           const Color &,              /* start color */
                           const Color &);             /* end color */
TLP_GL_SCOPE void
polyQuad(const std::vector<Coord> &,                   /* polyline vertces */
         const Color &,                                /* start color */
         const Color &,                                /* end color */
         const float,                                  /* start size */
         const float,                                  /* end size */
         const Coord &,                                /* normal to the begin of the curve */
         const Coord &,                                /* normal to the end curve */
         bool, /* if true : use start and end color to border lines, if false : use borderColor*/
         const Color &,                       /* border color */
         const std::string &textureName = "", /* textureName */
         const float outlineWidth = 0);
TLP_GL_SCOPE void
simpleQuad(const std::vector<Coord> &, /* quad vertces */
           const Color &,              /* start color */
           const Color &,              /* end color */
           const float,                /* start size */
           const float,                /* end size */
           const Coord &,              /* normal to the begin of the curve */
           const Coord &,              /* normal to the end curve */
           const Coord &,              /* direction of the camera*/
           bool, /* if true : use start and end color to border lines, if false : use borderColor*/
           const Color &,                                                      /* border color */
           const std::string &textureName = "", const float outlineWidth = 0); /* textureName */
TLP_GL_SCOPE void splineQuad(const std::vector<Coord> &, /* polyline vertces */
                             const Color &,              /* start color */
                             const Color &,              /* end color */
                             const float,                /* start size */
                             const float,                /* end size */
                             const Coord &,              /* nomal to the begin of the curve */
                             const Coord &);             /* nomal to the end curve */
TLP_GL_SCOPE void splineLine(const std::vector<Coord> &, /* polyline vertces */
                             const Color &,              /* start color */
                             const Color &);             /* end color */
//====================================================================
TLP_GL_SCOPE void getColors(const Coord *line, uint lineSize, const Color &c1, const Color &c2,
                            std::vector<Color> &);
TLP_GL_SCOPE void getSizes(const std::vector<Coord> &line, float s1, float s2,
                           std::vector<float> &);
TLP_GL_SCOPE GLfloat *buildCurvePoints(const std::vector<Coord> &vertices,
                                       const std::vector<float> &sizes, const Coord &startN,
                                       const Coord &endN, uint &resultSize,
                                       std::vector<uint> *dec = nullptr);
TLP_GL_SCOPE void buildCurvePoints(const std::vector<Coord> &vertices,
                                   const std::vector<float> &sizes, const Coord &startN,
                                   const Coord &endN, std::vector<Coord> &result);
}
#endif // TALIPOT_CURVES_H
