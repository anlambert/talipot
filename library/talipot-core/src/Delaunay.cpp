/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/Delaunay.h>
#include <talipot/Graph.h>
#include <talipot/DrawingTools.h>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

extern "C" {
#ifdef HAVE_REENTRANT_QHULL
#include <qhull_ra.h>
#else
#include <qhull_a.h>
#endif
}

using namespace std;
using namespace tlp;

typedef Matrix<long double, 3> Mat3ld;
typedef Vector<long double, 3> Vec3ld;

static bool runQHull(int dim, vector<double> &points, vector<pair<uint, uint>> &edges,
                     vector<vector<uint>> &simplices) {

  // Set default options for qhull delaunay triangulation
  // - Qt : triangulated output
  // - Qbb : scale the last coordinate to [0,m] for Delaunay
  // - Qc : keep coplanar points with nearest facet
  // - Qz - add a point-at-infinity for Delaunay triangulations
  string qhullOptions = "Qt Qbb Qc Qz";
  // build qhull command
  string qhullCommand = string("qhull d ") + qhullOptions;

// initialize qhull
#ifdef HAVE_REENTRANT_QHULL
  qhT qh_qh;
  qhT *qh = &qh_qh;
  QHULL_LIB_CHECK
  qh_zero(qh, stderr);
  int qhullKo = qh_new_qhull(qh, dim, points.size() / dim, &points[0], false,
                             const_cast<char *>(qhullCommand.c_str()), nullptr, stderr);
#else
  int qhullKo = qh_new_qhull(dim, points.size() / dim, &points[0], false,
                             const_cast<char *>(qhullCommand.c_str()), nullptr, stderr);
#endif

  if (!qhullKo) {

    set<pair<uint, uint>> placedEdges;

// call qhull delaunay triangulation
#ifdef HAVE_REENTRANT_QHULL
    qh_triangulate(qh);
#else
    qh_triangulate();
#endif

    facetT *facet = nullptr;
    vertexT *vertex = nullptr, **vertexp;

    // iterate over generated simplices (triangles or tetrahedra) and get corresponding delaunay
    // edges
    FORALLfacets {

      if (!facet->upperdelaunay) {
        int pointId0 = 0, pointId1 = 0, pointId2 = 0, pointId3 = -1;
        int i = 0;
        FOREACHvertex_(facet->vertices) {
#ifdef HAVE_REENTRANT_QHULL

          if (i == 0) {
            pointId0 = qh_pointid(qh, vertex->point);
          } else if (i == 1) {
            pointId1 = qh_pointid(qh, vertex->point);
          } else if (i == 2) {
            pointId2 = qh_pointid(qh, vertex->point);
          } else {
            pointId3 = qh_pointid(qh, vertex->point);
          }

#else

          if (i == 0) {
            pointId0 = qh_pointid(vertex->point);
          } else if (i == 1) {
            pointId1 = qh_pointid(vertex->point);
          } else if (i == 2) {
            pointId2 = qh_pointid(vertex->point);
          } else {
            pointId3 = qh_pointid(vertex->point);
          }

#endif
          ++i;
        }

        vector<uint> simplex;
        pair edge1 = {min(pointId0, pointId1), max(pointId0, pointId1)};
        pair edge2 = {min(pointId1, pointId2), max(pointId1, pointId2)};
        pair edge3 = {min(pointId2, pointId0), max(pointId2, pointId0)};

        if (!placedEdges.contains(edge1)) {
          edges.push_back(edge1);
          placedEdges.insert(edge1);
        }

        if (!placedEdges.contains(edge2)) {
          edges.push_back(edge2);
          placedEdges.insert(edge2);
        }

        if (!placedEdges.contains(edge3)) {
          edges.push_back(edge3);
          placedEdges.insert(edge3);
        }

        simplex.reserve(3);
        simplex.push_back(pointId0);
        simplex.push_back(pointId1);
        simplex.push_back(pointId2);

        if (pointId3 != -1) {
          simplex.push_back(pointId3);
          pair edge4 = {min(pointId0, pointId3), max(pointId0, pointId3)};
          pair edge5 = {min(pointId1, pointId3), max(pointId1, pointId3)};
          pair edge6 = {min(pointId2, pointId3), max(pointId2, pointId3)};

          if (!placedEdges.contains(edge4)) {
            edges.push_back(edge4);
            placedEdges.insert(edge4);
          }

          if (!placedEdges.contains(edge5)) {
            edges.push_back(edge5);
            placedEdges.insert(edge5);
          }

          if (!placedEdges.contains(edge6)) {
            edges.push_back(edge6);
            placedEdges.insert(edge6);
          }
        }

        simplices.push_back(simplex);
      }
    }
  }

  // free memory allocated by qhull
  int curlong, totlong;
#ifdef HAVE_REENTRANT_QHULL
  qh_freeqhull(qh, !qh_ALL);
  qh_memfreeshort(qh, &curlong, &totlong);
#else
  qh_freeqhull(!qh_ALL);
  qh_memfreeshort(&curlong, &totlong);
#endif

  return !qhullKo;
}

//================================================================================================

static void normalize(Vec3f &v) {
  if (v.norm() != 0) {
    v /= v.norm();
  }
}

//================================================================================================

// B -- C
// |    |
// A -- D
static vector<Coord> buildGrid(const Coord &A, const Coord &B, const Coord &C, const Coord &D,
                               const float nbSubDiv = 10) {
  vector<Coord> ret;

  for (float i = 1; i < nbSubDiv - 1; ++i) {
    Coord start = A + (i / (nbSubDiv - 1) * (B - A));
    Coord end = D + (i / (nbSubDiv - 1) * (C - D));

    for (float j = 0; j < nbSubDiv; ++j) {
      ret.push_back(start + (j / (nbSubDiv - 1)) * (end - start));
    }
  }

  return ret;
}

//================================================================================================

static tlp::Coord computeTriangleCircumscribedCenter(const tlp::Coord &A, const tlp::Coord &B,
                                                     const tlp::Coord &C) {
  Vec3ld Ad, Bd, Cd;

  for (size_t i = 0; i < 3; ++i) {
    Ad[i] = A[i];
    Bd[i] = B[i];
    Cd[i] = C[i];
  }

  Vec3ld a = Ad - Cd;
  Vec3ld b = Bd - Cd;
  Vec3ld axb = a ^ b;
  long double anorm = a.norm();
  long double bnorm = b.norm();
  long double axbnorm = axb.norm();

  if (axbnorm != 0) {
    Vec3ld c = Cd + (((anorm * anorm * b) - (bnorm * bnorm * a)) ^ axb) / (2.0 * axbnorm * axbnorm);
    return tlp::Coord(c[0], c[1], c[2]);
  } else {
    return (A + B + C) / 3.0f;
  }
}

//================================================================================================

static tlp::Coord computeTetrahedronCircumscribedCenter(const tlp::Coord &A, const tlp::Coord &B,
                                                        const tlp::Coord &C, const tlp::Coord &D) {

  Vec3ld Ad, Bd, Cd, Dd;

  for (size_t i = 0; i < 3; ++i) {
    Ad[i] = A[i];
    Bd[i] = B[i];
    Cd[i] = C[i];
    Dd[i] = D[i];
  }

  Vec3ld da = Dd - Ad;
  Vec3ld ba = Bd - Ad;
  Vec3ld ca = Cd - Ad;

  long double danorm = da.norm();
  long double banorm = ba.norm();
  long double canorm = ca.norm();

  Mat3ld m;

  for (int i = 0; i < 3; ++i) {
    m[0][i] = ba[i];
    m[1][i] = ca[i];
    m[2][i] = da[i];
  }

  long double det = m.determinant();

  if (det != 0) {
    Vec3ld c = Ad + ((danorm * danorm * (ba ^ ca)) + (canorm * canorm * (da ^ ba)) +
                     (banorm * banorm * (ca ^ da))) /
                        (2.0 * det);
    return tlp::Coord(c[0], c[1], c[2]);
  } else {
    return (A + B + C + D) / 4.0f;
  }
}

//================================================================================================

struct Face {
  Face(uint n1 = UINT_MAX, uint n2 = UINT_MAX, uint n3 = UINT_MAX) {
    sortedIndexes.reserve(3);
    sortedIndexes.push_back(n1);
    sortedIndexes.push_back(n2);
    sortedIndexes.push_back(n3);
    sort(sortedIndexes.begin(), sortedIndexes.end());
  }

  bool operator==(const Face &f) const {
    return sortedIndexes[0] == f.sortedIndexes[0] && sortedIndexes[1] == f.sortedIndexes[1] &&
           sortedIndexes[2] == f.sortedIndexes[2];
  }

  bool isValid() const {
    return sortedIndexes[0] != UINT_MAX && sortedIndexes[1] != UINT_MAX;
  }

  uint size() const {
    uint ret = 0;

    for (uint i = 0; i < 3; ++i) {
      if (sortedIndexes[i] != UINT_MAX) {
        ++ret;
      }
    }

    return ret;
  }

  vector<uint> sortedIndexes;
};

namespace std {
template <>
struct hash<Face> {
  inline std::size_t operator()(const Face &f) const {
    size_t seed = 0;
    tlp_hash_combine(seed, f.sortedIndexes[0]);
    tlp_hash_combine(seed, f.sortedIndexes[1]);
    tlp_hash_combine(seed, f.sortedIndexes[2]);
    return seed;
  }
};
} // namespace std

//================================================================================================

bool tlp::delaunayTriangulation(vector<Coord> &points, vector<pair<uint, uint>> &edges,
                                vector<vector<uint>> &simplices, bool voronoiMode) {

  vector<double> pointsQHull;
  BoundingBox bb;
  // check if the layout is coplanar
  Mat3f invTransformMatrix;
  bool coPlanarLayout = isLayoutCoPlanar(points, invTransformMatrix);
  Mat3f transformMatrix = invTransformMatrix;

  if (coPlanarLayout) {
    transformMatrix.inverse();
  }

  int dim = 3;

  // if the layout is not coplanar, use a 3d delaunay triangulation
  if (!coPlanarLayout) {
    pointsQHull.reserve(points.size() * 3);

    // copy coordinates in the pointsQHull vector
    for (const auto &point : points) {
      bb.expand(point);
      pointsQHull.push_back(point.getX());
      pointsQHull.push_back(point.getY());
      pointsQHull.push_back(point.getZ());
    }

    // If we compute a delaunay triangulation for then computing the dual voronoi diagram,
    // enclose the layout in a cube whose each face is a grid.
    // It ensures that each site will have an associated connected voronoi cell
    // meaning we don't have to deal with voronoi rays.
    if (voronoiMode) {

      float newWidth = bb.width() * 1.2f;
      float newHeight = bb.height() * 1.2f;
      float newDepth = bb.depth() * 1.2f;

      Coord p1 = Coord(bb.center()) + Coord(-newWidth / 2, -newHeight / 2, -newDepth / 2);
      Coord p2 = Coord(bb.center()) + Coord(-newWidth / 2, newHeight / 2, -newDepth / 2);
      Coord p3 = Coord(bb.center()) + Coord(newWidth / 2, newHeight / 2, -newDepth / 2);
      Coord p4 = Coord(bb.center()) + Coord(newWidth / 2, -newHeight / 2, -newDepth / 2);

      Coord p5 = Coord(bb.center()) + Coord(-newWidth / 2, -newHeight / 2, newDepth / 2);
      Coord p6 = Coord(bb.center()) + Coord(-newWidth / 2, newHeight / 2, newDepth / 2);
      Coord p7 = Coord(bb.center()) + Coord(newWidth / 2, newHeight / 2, newDepth / 2);
      Coord p8 = Coord(bb.center()) + Coord(newWidth / 2, -newHeight / 2, newDepth / 2);

      vector<Coord> mesh;

      vector<Coord> grid = buildGrid(p1, p2, p3, p4);
      mesh.insert(mesh.end(), grid.begin(), grid.end());
      grid = buildGrid(p5, p6, p7, p8);
      mesh.insert(mesh.end(), grid.begin(), grid.end());
      grid = buildGrid(p1, p5, p8, p4);
      mesh.insert(mesh.end(), grid.begin(), grid.end());
      grid = buildGrid(p2, p6, p7, p3);
      mesh.insert(mesh.end(), grid.begin(), grid.end());
      grid = buildGrid(p1, p5, p6, p2);
      mesh.insert(mesh.end(), grid.begin(), grid.end());
      grid = buildGrid(p4, p8, p7, p3);
      mesh.insert(mesh.end(), grid.begin(), grid.end());

      for (const auto &point : mesh) {
        points.push_back(point);
        pointsQHull.push_back(point.getX());
        pointsQHull.push_back(point.getY());
        pointsQHull.push_back(point.getZ());
      }
    }

  } else {
    dim = 2;
    pointsQHull.reserve(points.size() * 2);

    // copy coordinates in the pointsQHull vector
    for (const auto &point : points) {
      Coord p = Coord(invTransformMatrix * point);
      pointsQHull.push_back(p.getX());
      pointsQHull.push_back(p.getY());
    }

    // If we compute a delaunay triangulation for then computing the dual voronoi diagram,
    // enclose the layout in its convex hull.
    // It ensures that each site will have an associated connected voronoi cell
    // meaning we don't have to deal with voronoi rays.
    if (voronoiMode) {

      vector<Coord> inputHullPoints;
      inputHullPoints.reserve(points.size());

      for (const auto &point : points) {
        Coord p = Coord(invTransformMatrix * point);
        inputHullPoints.push_back(p);
      }

      vector<Coord> hullPoints = computeConvexHull(inputHullPoints);
      vector<Coord> newHullPoints;
      float nbSubdiv = 4;

      for (size_t i = 0; i < hullPoints.size() - 1; ++i) {
        for (float j = 0; j < nbSubdiv - 1; ++j) {
          newHullPoints.push_back(hullPoints[i] +
                                  (j / (nbSubdiv - 1)) * (hullPoints[i + 1] - hullPoints[i]));
        }
      }

      for (float j = 0; j < nbSubdiv - 1; ++j) {
        newHullPoints.push_back(hullPoints.back() +
                                (j / (nbSubdiv - 1)) * (hullPoints[0] - hullPoints.back()));
      }

      hullPoints = newHullPoints;
      Coord centroid = computePolygonCentroid(hullPoints);

      for (auto &hullPoint : hullPoints) {
        float dist = centroid.dist(hullPoint);
        Coord dir = hullPoint - centroid;
        normalize(dir);
        hullPoint = centroid + dir * 1.2f * dist;
        points.push_back(Coord(transformMatrix * hullPoint));
        pointsQHull.push_back(hullPoint.getX());
        pointsQHull.push_back(hullPoint.getY());
      }
    }
  }

  return runQHull(dim, pointsQHull, edges, simplices);
}

//================================================================================================

static void addVoronoiEdge(VoronoiDiagram &voronoiDiagram, const Face &face,
                           const VoronoiDiagram::Edge &edge) {
  bool addEdge = false;

  if (face.sortedIndexes[0] < voronoiDiagram.nbSites()) {
    voronoiDiagram.siteToCellEdges[face.sortedIndexes[0]].push_back(voronoiDiagram.edges.size());
    addEdge = true;
  }

  if (face.sortedIndexes[1] < voronoiDiagram.nbSites()) {
    voronoiDiagram.siteToCellEdges[face.sortedIndexes[1]].push_back(voronoiDiagram.edges.size());
    addEdge = true;
  }

  if (face.size() > 2 && face.sortedIndexes[2] < voronoiDiagram.nbSites()) {
    voronoiDiagram.siteToCellEdges[face.sortedIndexes[2]].push_back(voronoiDiagram.edges.size());
    addEdge = true;
  }

  if (addEdge) {
    voronoiDiagram.edges.push_back(edge);
    voronoiDiagram.verticesDegree[edge.first] += 1;
    voronoiDiagram.verticesDegree[edge.second] += 1;
  }
}

//================================================================================================

bool tlp::voronoiDiagram(vector<Coord> &sites, VoronoiDiagram &voronoiDiagram) {

  vector<pair<uint, uint>> edges;
  vector<vector<uint>> simplices;
  voronoiDiagram.sites = sites;
  uint nbSites = sites.size();

  // compute the Delaunay triangulation (some dummy sites will be added in the sites vector
  // in order to have a connected voronoi cell for each input sites)
  bool ret = delaunayTriangulation(sites, edges, simplices, true);

  // now compute the dual voronoi diagram
  if (ret) {
    // Iterate over each delaunay simplex
    node_hash_map<Face, uint> faceToCircumCenter;
    map<Coord, uint> circumCenterToIdx;
    tlp::Coord A, B, C, D;

    for (const auto &simplex : simplices) {
      vector<uint> sitesIdx;

      uint j = 0;
      for (auto idx : simplex) {
        if (j == 0) {
          A = sites[idx];
        } else if (j == 1) {
          B = sites[idx];
        } else if (j == 2) {
          C = sites[idx];
        } else {
          D = sites[idx];
        }
        sitesIdx.push_back(idx);
        ++j;
      }

      bool treatSimplex = false;

      for (uint j : sitesIdx) {
        if (j < nbSites) {
          treatSimplex = true;
          break;
        }
      }

      // only treats simplices which have at least one original site in its vertices
      if (!treatSimplex) {
        continue;
      }

      // compute the circumscribed center of the simplex (triangle in 2d, tetrahedron in 3d)
      tlp::Coord circumCenterPos;

      if (simplex.size() == 3) {
        circumCenterPos = computeTriangleCircumscribedCenter(A, B, C);
      } else {
        circumCenterPos = computeTetrahedronCircumscribedCenter(A, B, C, D);
      }

      uint circumCenterIdx = 0;

      if (circumCenterToIdx.contains(circumCenterPos)) {
        circumCenterIdx = circumCenterToIdx[circumCenterPos];
      } else {
        // add the voronoi vertex in the voronoi diagram
        circumCenterIdx = voronoiDiagram.vertices.size();
        circumCenterToIdx[circumCenterPos] = circumCenterIdx;
        voronoiDiagram.verticesDegree[circumCenterIdx] = 0;
        voronoiDiagram.vertices.push_back(circumCenterPos);
      }

      Face face1, face2, face3, face4;

      // try to connect two voronoi vertices of adjacent facets

      // 2d voronoi
      if (simplex.size() == 3) {
        face1 = Face(simplex[0], simplex[1]);
        face2 = Face(simplex[1], simplex[2]);
        face3 = Face(simplex[0], simplex[2]);
      }

      // 3d voronoi
      else {
        face1 = Face(simplex[0], simplex[1], simplex[2]);
        face2 = Face(simplex[1], simplex[2], simplex[3]);
        face3 = Face(simplex[0], simplex[2], simplex[3]);
        face4 = Face(simplex[0], simplex[1], simplex[3]);
      }

      if (const auto it = faceToCircumCenter.find(face1); it != faceToCircumCenter.end()) {
        addVoronoiEdge(voronoiDiagram, face1, {circumCenterIdx, faceToCircumCenter[face1]});
      } else {
        faceToCircumCenter[face1] = circumCenterIdx;
      }

      if (const auto it = faceToCircumCenter.find(face2); it != faceToCircumCenter.end()) {
        addVoronoiEdge(voronoiDiagram, face2, {circumCenterIdx, faceToCircumCenter[face2]});
      } else {
        faceToCircumCenter[face2] = circumCenterIdx;
      }

      if (const auto it = faceToCircumCenter.find(face3); it != faceToCircumCenter.end()) {
        addVoronoiEdge(voronoiDiagram, face3, {circumCenterIdx, faceToCircumCenter[face3]});
      } else {
        faceToCircumCenter[face3] = circumCenterIdx;
      }

      // 3d voronoi case
      if (face4.isValid()) {

        if (const auto it = faceToCircumCenter.find(face4); it != faceToCircumCenter.end()) {
          addVoronoiEdge(voronoiDiagram, face4, {circumCenterIdx, faceToCircumCenter[face4]});
        } else {
          faceToCircumCenter[face4] = circumCenterIdx;
        }
      }
    }

    // compute the voronoi cells associated to sites
    voronoiDiagram.cells.reserve(nbSites);

    for (uint i = 0; i < nbSites; ++i) {
      VoronoiDiagram::Cell cell;
      const vector<VoronoiDiagram::Edge> &voronoiEdges = voronoiDiagram.voronoiEdgesForSite(i);

      for (const auto &[srcIdx, tgtIdx] : voronoiEdges) {
        cell.insert(srcIdx);
        cell.insert(tgtIdx);
      }

      voronoiDiagram.siteToCell[i] = voronoiDiagram.cells.size();
      voronoiDiagram.cells.push_back(cell);
    }
  }

  // restore the original sites vector (as the delaunay triangulation adds dummy sites)
  sites.resize(nbSites);
  return ret;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
