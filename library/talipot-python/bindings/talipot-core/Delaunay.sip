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
#include <talipot/Delaunay.h>
typedef std::pair<uint, uint> pairUintUint;
typedef std::vector<uint> vectorUint;
typedef std::vector<pairUintUint> vector_pairUintUint;
typedef std::vector<vectorUint> vector_vectorUint;
%End

typedef std::pair<uint, uint> pairUintUint /TypeHint="Tuple[int, int]"/;
typedef std::vector<uint> vectorUint /TypeHint="List[int]"/;
typedef std::vector<pairUintUint> vector_pairUintUint /TypeHint="List[Tuple[int, int]]"/;
typedef std::vector<vectorUint> vector_vectorUint /TypeHint="List[List[int]]"/;

namespace tlp {

std::pair<vector_pairUintUint, vector_vectorUint> delaunayTriangulation(std::vector<tlp::Coord> &points) /TypeHint="Tuple[List[Tuple[int, int]], List[List[int]]]"/;
%Docstring
tlp.delaunayTriangulation(points)

Computes the delaunay triangulation of a set of points and returns the set of delaunay edges and
simplices (triangles in 2d, tetrahedra in 3d). Returns a tuple whose first member is the list of
edges and second member is the list of simplices, or :const:`None` if the triangulation failed.
Edges and simplices are defined using indexes into the original list of points.

:param points:
   a list of points on which to compute a delaunay triangulation

:type points:
   list of :class:`tlp.Coord`

:rtype:
   (list of (integer, integer), list of integer) or :const:`None`
%End

%MethodCode
std::vector<std::pair<uint, uint>> edges;
std::vector<std::vector<uint>> simplices;
if (tlp::delaunayTriangulation(*a0, edges, simplices)) {
  sipRes = new std::pair<std::vector<std::pair<uint, uint>>, std::vector<std::vector<uint>>>(
      edges, simplices);
} else {
  sipRes = NULL;
}
%End

class VoronoiDiagram /Abstract/ {
%Docstring
That class defines a voronoi diagram. Use the :func:`tlp.voronoiDiagram` function to get an instance
of it.
%End

%TypeHeaderCode
#include <talipot/Delaunay.h>
%End

public:

  uint nbSites() const;
%Docstring
tlp.VoronoiDiagram.nbSites()

Returns the number of voronoi sites.

:rtype:
   integer
%End

  uint nbVertices() const;
%Docstring
tlp.VoronoiDiagram.nbVertices()

Returns the number of voronoi vertices.

:rtype:
   integer
%End

  uint nbEdges() const;
%Docstring
tlp.VoronoiDiagram.nbEdges()

Returns the number of voronoi edges.

:rtype:
   integer
%End

  const tlp::Coord &site(const uint siteIdx);
%Docstring
tlp.VoronoiDiagram.site(siteIdx)

Returns the ith voronoi site.

:param siteIdx:
   the index of the site to return

:type siteIdx:
   integer

:rtype:
   :class:`tlp.Coord`
%End

  const tlp::Coord &vertex(const uint vertexIdx);
%Docstring
tlp.VoronoiDiagram.vertex(vertexIdx)

Returns the ith voronoi vertex.

:param vertexIdx:
   the index of the vertex to return

:type vertexIdx:
   integer

:rtype:
   :class:`tlp.Coord`
%End

  const pairUintUint &edge(const uint edgeIdx) /TypeHint="Tuple[int, int]"/;
%Docstring
tlp.VoronoiDiagram.edge(edgeIdx)

Returns the ith voronoi edge. An edge is defined by two indexes in the voronoi vertices.

:param edgeIdx:
   the index of the edge to return

:type edgeIdx:
   integer

:rtype:
   (integer, integer)
%End

  const std::set<uint> &cell(const uint cellIdx);
%Docstring
tlp.VoronoiDiagram.cell(cellIdx)

Returns the ith voronoi cell. A cell is defined by a list of indexes in the voronoi vertices.

:param cellIdx:
   the index of the cell to return

:type cellIdx:
   integer

:rtype:
   set of integer
%End

  uint degreeOfVertex(const uint vertexIdx);
%Docstring
tlp.VoronoiDiagram.degreeOfVertex(vertexIdx)

Returns the degree (number of incident edges) of the ith voronoi vertex

:param vertexIdx:
   the index of the vertex on which to return the degree

:type vertexIdx:
   integer

:rtype:
   integer
%End

  std::vector<pairUintUint> voronoiEdgesForSite(const uint siteIdx) /TypeHint="List[Tuple[int, int]]"/;
%Docstring
tlp.VoronoiDiagram.voronoiEdgesForSite(siteIdx)

Returns the voronoi edges surrounding the ith site.

:param siteIdx:
   the index of the site on which to return edges

:type siteIdx:
   integer

:rtype:
   list of (integer, integer)
%End

  const std::set<uint> &voronoiCellForSite(const uint siteIdx);
%Docstring
tlp.VoronoiDiagram.voronoiCellForSite(siteIdx)

Returns the cell associated to the ith site.

:param siteIdx:
   the index of the site on which to return the cell

:type siteIdx:
   integer

:rtype:
   set of integer
%End

};

tlp::VoronoiDiagram voronoiDiagram(std::vector<tlp::Coord> &sites);
%Docstring
tlp.voronoiDiagram(sites)

Computes the voronoi diagram of a set of sites (for 2d and 3d layouts). It automatically computes
the set of all voronoi vertices, edges and cells. In order to not have to deal with infinite voronoi
rays, the input layout is enclosed in its convex hull in 2d or in its bounding box in 3d. It enables
to have a connected voronoi cell for each input site.

:param sites:
   the sites from which to compute the voronoi diagram

:type sites:
   list of :class:`tlp.Coord`

:rtype:
   :class:`tlp.VoronoiDiagram`
%End

%MethodCode
tlp::VoronoiDiagram voronoiDiagram;
if (tlp::voronoiDiagram(*a0, voronoiDiagram)) {
  sipRes = new tlp::VoronoiDiagram(voronoiDiagram);
} else {
  sipRes = NULL;
}
%End

};
