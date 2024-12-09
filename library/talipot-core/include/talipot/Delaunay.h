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

#ifndef TALIPOT_DELAUNAY_H
#define TALIPOT_DELAUNAY_H

#include <vector>
#include <set>
#include <talipot/hash.h>

#include <talipot/Coord.h>

namespace tlp {

/**
 * @ingroup Graph
 * \brief functions for Delaunay Triangulations
 *
 * \author : David Auber/Daniel Archambault/Antoine Lambert : auber@labri.fr
 *
 * Computes the delaunay triangulation and returns the set of delaunay edges in the
 * vector edges and delaunay simplices (triangles in 2d, tetrahedra in 3d) of the triangulation in
 * the vector simplices.
 * Edges and simplices are defined using a indexes into the original
 * set of points.
 */
TLP_SCOPE bool delaunayTriangulation(std::vector<Coord> &points,
                                     std::vector<std::pair<uint, uint>> &edges,
                                     std::vector<std::vector<uint>> &simplices,
                                     bool voronoiMode = false);

/**
 * @ingroup Graph
 * @brief The VoronoiDiagram class
 */
class TLP_SCOPE VoronoiDiagram {
public:
  // A voronoi site.
  typedef Coord Site;

  // A voronoi vertex.
  typedef Coord Vertex;

  // A voronoi edge defined by the indexes of its extremities in the vertices vector
  typedef std::pair<uint, uint> Edge;

  // A voronoi Cell defined by the indexes of its vertices in the vertices vector
  typedef std::set<uint> Cell;

  // Returns the number of voronoi sites
  uint nbSites() const {
    return sites.size();
  }

  // Returns the number of voronoi vertices
  uint nbVertices() const {
    return vertices.size();
  }

  // Returns the number of voronoi edges
  uint nbEdges() const {
    return edges.size();
  }

  // Returns the ith site
  const Site &site(const uint siteIdx) {
    return sites[siteIdx];
  }

  // Returns the ith voronoi vertex
  const Vertex &vertex(const uint vertexIdx) {
    return vertices[vertexIdx];
  }

  // Returns the ith voronoi edge
  const Edge &edge(const uint edgeIdx) {
    return edges[edgeIdx];
  }

  // Returns the ith voronoi cell
  const Cell &cell(const uint cellIdx) {
    return cells[cellIdx];
  }

  // Returns the degree of the ith voronoi vertex
  uint degreeOfVertex(const uint vertexIdx) {
    return verticesDegree[vertexIdx];
  }

  // Returns the edges of the voronoi cell for the ith site
  std::vector<Edge> voronoiEdgesForSite(const uint siteIdx) {
    std::vector<Edge> ret;

    for (uint i : siteToCellEdges[siteIdx]) {
      ret.push_back(edges[i]);
    }

    return ret;
  }

  // Returns the cell for the ith site
  const Cell &voronoiCellForSite(const uint siteIdx) {
    return cells[siteToCell[siteIdx]];
  }

  // Stores lists of each of these types defining the voronoi diagram
  std::vector<Site> sites;
  std::vector<Vertex> vertices;
  std::vector<Edge> edges;
  std::vector<Cell> cells;
  node_hash_map<uint, std::vector<uint>> siteToCellEdges;
  node_hash_map<uint, uint> siteToCell;
  node_hash_map<uint, uint> verticesDegree;
};

/**
 * Computes the voronoi diagram of a set of points (for 2d and 3d layouts).
 * The set of input points are given in sites.  The resultant voronoi diagram is returned
 * in voronoiDiagram.  It automatically computes the set of all voronoi
 * vertices, edges and cells. In order to not have to deal with infinite
 * voronoi rays, the input layout is enclosed in its convex hull in 2d or
 * in its bounding box in 3d. It enables to have a connected voronoi cell
 * for each input site.
 */
TLP_SCOPE bool voronoiDiagram(std::vector<Coord> &sites, VoronoiDiagram &voronoiDiagram);
}
#endif // TALIPOT_DELAUNAY_H
