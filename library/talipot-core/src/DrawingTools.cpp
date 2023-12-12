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

#include <talipot/DrawingTools.h>
#include <talipot/LayoutProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/BooleanProperty.h>
#include <talipot/ConvexHull.h>

using namespace std;
using namespace tlp;

static void rotate(Coord &vec, double alpha) {
  Coord backupVec = vec;
  double zRot = -2.0 * M_PI * alpha / 360.0;
  float cosz = cos(zRot);
  float sinz = sin(zRot);
  vec[0] = backupVec[0] * cosz - backupVec[1] * sinz;
  vec[1] = backupVec[0] * sinz + backupVec[1] * cosz;
}

/**
 * Compute all points of a Tulip layout (with size, rotation, edge bends, and node position)
 * @todo take edge size into account
 * @todo create unit test to automatically check that function
 */
static std::vector<Coord> computeGraphPoints(const Graph *graph, const std::vector<node> &nodes,
                                             const std::vector<edge> &edges,
                                             const LayoutProperty *layout, const SizeProperty *size,
                                             const DoubleProperty *rotation,
                                             const BooleanProperty *selection) {
  std::set<node> processedNodes;
  std::vector<Coord> gPoints;
  for (auto n : nodes) {
    if (!selection || selection->getNodeValue(n)) {
      const Size &nSize = size->getNodeValue(n);
      const Coord &point = layout->getNodeValue(n);
      double rot = rotation->getNodeValue(n);
      vector<Coord> points = {{nSize[0] / 2, nSize[1] / 2, nSize[2] / 2},
                              {-nSize[0] / 2, -nSize[1] / 2, -nSize[2] / 2},
                              {nSize[0] / 2, -nSize[1] / 2, -nSize[2] / 2},
                              {-nSize[0] / 2, nSize[1] / 2, nSize[2] / 2}};

      for (uint i = 0; i < 4; ++i) {
        if (rot) {
          rotate(points[i], rot);
        }
        gPoints.push_back(points[i] + point);
      }

      processedNodes.insert(n);
    }
  }

  for (auto e : edges) {
    if (!selection || selection->getEdgeValue(e)) {
      const auto &[src, tgt] = graph->ends(e);
      if (!processedNodes.contains(src)) {
        gPoints.push_back(layout->getNodeValue(src));
        processedNodes.insert(src);
      }
      for (const Coord &coord : layout->getEdgeValue(e)) {
        gPoints.push_back(coord);
      }
      if (!processedNodes.contains(tgt)) {
        gPoints.push_back(layout->getNodeValue(tgt));
        processedNodes.insert(tgt);
      }
    }
  }
  return gPoints;
}

//===========================================================================
BoundingBox tlp::computeBoundingBox(const Graph *graph, const LayoutProperty *layout,
                                    const SizeProperty *size, const DoubleProperty *rotation,
                                    const BooleanProperty *selection) {
  return computeBoundingBox(graph, graph->nodes(), graph->edges(), layout, size, rotation,
                            selection);
}
//===========================================================================
BoundingBox tlp::computeBoundingBox(const Graph *graph, const std::vector<node> &nodes,
                                    const std::vector<edge> &edges, const LayoutProperty *layout,
                                    const SizeProperty *size, const DoubleProperty *rotation,
                                    const BooleanProperty *selection) {
  std::vector<Coord> gPoints =
      computeGraphPoints(graph, nodes, edges, layout, size, rotation, selection);
  BoundingBox bbox;
  for (const Coord &point : gPoints) {
    bbox.expand(point);
  }
  return bbox;
}
//===========================================================================
pair<Coord, Coord> tlp::computeBoundingRadius(const Graph *graph, const LayoutProperty *layout,
                                              const SizeProperty *size,
                                              const DoubleProperty *rotation,
                                              const BooleanProperty *selection) {
  pair<Coord, Coord> result;

  if (graph->isEmpty()) {
    return result;
  }

  BoundingBox boundingBox = tlp::computeBoundingBox(graph, layout, size, rotation, selection);
  Coord center = boundingBox.center();
  result = {center, center};

  double maxRad = 0;
  for (auto n : graph->nodes()) {
    const Coord &curCoord = layout->getNodeValue(n);
    Size curSize = size->getNodeValue(n) / 2.0f;

    if (selection == nullptr || selection->getNodeValue(n)) {
      double nodeRad = sqrt(curSize.getW() * curSize.getW() + curSize.getH() * curSize.getH());
      Coord radDir = curCoord - center;
      double curRad = nodeRad + radDir.norm();

      if (radDir.norm() < 1e-6) {
        curRad = nodeRad;
        radDir = {1, 0, 0};
      }

      if (curRad > maxRad) {
        maxRad = curRad;
        radDir /= radDir.norm();
        radDir *= curRad;
        result.second = radDir + center;
      }
    }
  }

  if (layout->hasNonDefaultValuatedEdges()) {
    for (auto e : graph->edges()) {
      if (selection == nullptr || selection->getEdgeValue(e)) {
        for (const auto &coord : layout->getEdgeValue(e)) {
          double curRad = (coord - center).norm();

          if (curRad > maxRad) {
            maxRad = curRad;
            result.second = coord;
          }
        }
      }
    }
  }

  return result;
}
//======================================================================================
vector<Coord> tlp::computeConvexHull(const std::vector<Coord> &allPoints) {
  vector<uint> hullIndices;
  convexHull(allPoints, hullIndices); // compute the convex hull
  vector<Coord> finalResult(hullIndices.size());

  uint i = 0;
  for (auto idx : hullIndices) {
    finalResult[i] = allPoints[idx];
    finalResult[i++][2] = 0;
  }

  return finalResult;
}
//======================================================================================

std::vector<Coord> tlp::computeConvexHull(const Graph *graph, const LayoutProperty *layout,
                                          const SizeProperty *size, const DoubleProperty *rotation,
                                          const BooleanProperty *selection) {

  std::vector<Coord> gPoints =
      computeGraphPoints(graph, graph->nodes(), graph->edges(), layout, size, rotation, selection);
  return computeConvexHull(gPoints);
}

//======================================================================================

// implementation based on http://mathworld.wolfram.com/Line-LineIntersection.html
// reference : Hill, F. S. Jr. "The Pleasures of 'Perp Dot' Products." Ch. II.5 in Graphics Gems IV
// (Ed. P. S. Heckbert). San Diego: Academic Press, pp. 138-148, 1994.
bool tlp::computeLinesIntersection(const std::pair<tlp::Coord, tlp::Coord> &line1,
                                   const std::pair<tlp::Coord, tlp::Coord> &line2,
                                   tlp::Coord &intersectionPoint) {

  const auto &[line1P1, line1P2] = line1;
  const auto &[line2P1, line2P2] = line2;

  Coord a = line1P2 - line1P1;
  Coord b = line2P2 - line2P1;
  Coord axb = a ^ b;
  float axbnorm = axb.norm();

  // lines are parallel, no intersection
  if (axbnorm == 0) {
    return false;
  }

  Coord c = line2P1 - line1P1;
  // skew lines, no intersection
  if (c.dotProduct(axb) != 0) {
    return false;
  }

  // lines intersects, compute the point
  float s = (c ^ b).dotProduct(axb) / (axbnorm * axbnorm);
  intersectionPoint = line1P1 + a * s;

  return true;
}

//======================================================================================================

Coord tlp::computePolygonCentroid(const vector<Coord> &points) {
  vector<Vec3d> pointsCp;
  pointsCp.reserve(points.size() + 1);

  for (const auto &point : points) {
    pointsCp.push_back(Vec3d(point[0], point[1], 0.0));
  }

  pointsCp.push_back(Vec3d(points[0][0], points[0][1], 0.0));
  double A = 0.0;
  double Cx = 0.0;
  double Cy = 0.0;

  for (size_t i = 0; i < pointsCp.size() - 1; ++i) {
    A += (pointsCp[i][0] * pointsCp[i + 1][1] - pointsCp[i + 1][0] * pointsCp[i][1]);
    Cx += (pointsCp[i][0] + pointsCp[i + 1][0]) *
          (pointsCp[i][0] * pointsCp[i + 1][1] - pointsCp[i + 1][0] * pointsCp[i][1]);
    Cy += (pointsCp[i][1] + pointsCp[i + 1][1]) *
          (pointsCp[i][0] * pointsCp[i + 1][1] - pointsCp[i + 1][0] * pointsCp[i][1]);
  }

  A *= 0.5;
  Cx *= 1.0 / (6.0 * A);
  Cy *= 1.0 / (6.0 * A);
  return Coord(float(Cx), float(Cy));
}

//======================================================================================================

static inline void normalize(Vec3f &v) {
  if (v.norm() != 0) {
    v /= v.norm();
  }
}

//======================================================================================================

bool tlp::isLayoutCoPlanar(const vector<Coord> &points, Mat3f &invTransformMatrix) {
  Coord A = points[0], B, C;
  bool BSet = false;

  // pick three points to define a plane
  for (size_t i = 1; i < points.size(); ++i) {
    if (!BSet && points[i] != A) {
      B = points[i];
      BSet = true;
    } else if (BSet) {
      // pick a third point non aligned with the two others
      C = points[i];

      if (((C - A) ^ (B - A)).norm() > 1e-3) {
        break;
      }
    }
  }

  Coord a = B - A;
  Coord b = C - A;
  normalize(a);
  normalize(b);
  Coord c = a ^ b;
  normalize(c);
  b = c ^ a;
  normalize(b);

  // compute the distance of each point to the plane
  for (const Coord &D : points) {
    // if the point is too far from the plane, the layout is not coplanar
    if (abs(c.dotProduct(D - A)) > 1e-3) {
      return false;
    }
  }

  // compute the inverse transform matrix for projecting the points in the z = 0 plane
  invTransformMatrix[0][0] = a[0];
  invTransformMatrix[1][0] = a[1];
  invTransformMatrix[2][0] = a[2];
  invTransformMatrix[0][1] = b[0];
  invTransformMatrix[1][1] = b[1];
  invTransformMatrix[2][1] = b[2];
  invTransformMatrix[0][2] = c[0];
  invTransformMatrix[1][2] = c[1];
  invTransformMatrix[2][2] = c[2];
  invTransformMatrix.inverse();

  return true;
}

//======================================================================================================

std::vector<tlp::Coord> tlp::computeRegularPolygon(uint numberOfSides, const tlp::Coord &center,
                                                   const tlp::Size &size, float startAngle) {

  assert(numberOfSides > 2);

  BoundingBox box;
  vector<Coord> points;
  float delta = (2.0f * M_PI) / float(numberOfSides);

  for (uint i = 0; i < numberOfSides; ++i) {
    float deltaX = cos(i * delta + startAngle);
    float deltaY = sin(i * delta + startAngle);
    points.push_back(Coord(deltaX, deltaY, center[2]));
    box.expand(points.back());
  }

  for (auto &point : points) {
    point.set(
        center[0] + ((point[0] - ((box[1][0] + box[0][0]) / 2.)) / ((box[1][0] - box[0][0]) / 2.)) *
                        size[0],
        center[1] + ((point[1] - ((box[1][1] + box[0][1]) / 2.)) / ((box[1][1] - box[0][1]) / 2.)) *
                        size[1]);
  }

  return points;
}
