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

#ifndef MIXED_MODEL_H
#define MIXED_MODEL_H

#include <talipot/hash.h>

/** This plugin is an implementation of the planar polyline graph
 *  drawing algorithm, the mixed model algorithm, first published as:
 *
 *  C. Gutwenger and P. Mutzel, \n
 *  "Planar Polyline Drawings with Good Angular Resolution", \n
 *  "Lecture Notes In Computer Science, Vol. 1547" \n
 *  "Proceedings of the 6th International Symposium on Graph Drawing," \n
 *  pages "167--182" \n
 *  1998 \n
 *
 *  Let n be the number of nodes, the original algorithm complexity is in O(n).\n
 *  But the implementation of the canonical ordering has not been made in O(n).\n
 *  This version of the algorithm considers each connected component of the graph,
 *  tests if it is planar or not. If not, it computes a planar subgraphs, which is
 *  a maximal planar "sub-map". Then an area aware version of Gutwenger and Mutzel 's
 *  algorithm is used, and if the connected component was not planar, it adds the
 *  "unplanar" edges in 3D. Finally, it uses the Connected Components Packing plugin
 *  of Tulip Software to pack the connected components.\n
 *
 */
class MixedModel : public tlp::LayoutAlgorithm {
public:
  PLUGININFORMATION("Mixed Model", "Romain BOURQUI ", "09/11/2005",
                    "Implements the planar polyline graph drawing algorithm, the mixed model "
                    "algorithm, first published as:<br/>"
                    "<b>Planar Polyline Drawings with Good Angular Resolution</b>, C. Gutwenger "
                    "and P. Mutzel, LNCS, Vol. 1547 pages 167--182 (1998).",
                    "1.0", "Planar")
  MixedModel(const tlp::PluginContext *context);
  ~MixedModel() override;
  bool run() override;
  bool check(std::string &) override;

private:
  std::vector<tlp::edge> getPlanarSubGraph(tlp::PlanarConMap *graph,
                                           const std::vector<tlp::edge> &unplanar_edges);
  void initPartition();
  void assignInOutPoints();
  void computeCoords();
  void placeNodesEdges();

  tlp::edge existEdge(tlp::node n, tlp::node v) {
    return carte->existEdge(n, v, false);
  }

  tlp::node rightV(uint k);
  tlp::node leftV(uint k);
  int next_right(uint k, const tlp::node v);
  int next_left(uint k, const tlp::node v);

  tlp::PlanarConMap *carte;
  std::vector<std::vector<tlp::node>> V;
  flat_hash_map<tlp::node, tlp::Coord> NodeCoords;

  flat_hash_map<tlp::node, int> outl;
  flat_hash_map<tlp::node, int> outr;
  flat_hash_map<tlp::node, int> inl;
  flat_hash_map<tlp::node, int> inr;

  flat_hash_map<tlp::node, uint> rank;
  flat_hash_map<tlp::node, std::vector<tlp::edge>> EdgesIN;
  flat_hash_map<tlp::node, std::vector<tlp::edge>> EdgesOUT;

  flat_hash_map<tlp::edge, std::vector<tlp::Coord>> InPoints;
  flat_hash_map<tlp::edge, tlp::Coord> OutPoints;

  tlp::Graph *Pere;
  tlp::PlanarConMap *graphMap;
  tlp::Graph *currentGraph;
  std::vector<tlp::edge> dummy;
  flat_hash_map<tlp::node, std::vector<tlp::Coord>> out_points;
  tlp::MutableContainer<tlp::Coord> nodeSize;
  std::vector<tlp::edge> unplanar_edges;
  bool planar;
  tlp::SizeProperty *sizeResult;
  tlp::IntegerProperty *shapeResult;
};

#endif // MIXED_MODEL_H
