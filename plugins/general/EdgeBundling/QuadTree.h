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

#ifndef QUAD_TREE_H
#define QUAD_TREE_H

#include <talipot/hash.h>
#include <vector>
#include <talipot/Graph.h>
#include <talipot/Vector.h>
#include <talipot/DrawingTools.h>

class QuadTreeBundle {
public:
  static void compute(tlp::Graph *graph, double splitRatio, tlp::LayoutProperty *layout = nullptr,
                      tlp::SizeProperty *size = nullptr);
  //=====================================
  static bool isIn(const tlp::Coord &p, const tlp::Coord &a, const tlp::Coord &b);

protected:
  void createQuadTree(tlp::Graph *graph, tlp::LayoutProperty *layout = nullptr,
                      tlp::SizeProperty *size = nullptr);

private:
  double minSize;
  double nbNodesInOriginalGraph;
  std::vector<tlp::node> resultNode;
  tlp::LayoutProperty *layout;
  tlp::SizeProperty *size;
  tlp::DoubleProperty *rot;
  tlp::Graph *graph;
  double splitRatio;
  typedef tlp::Vec2d Vec2D;
  typedef flat_hash_map<Vec2D, tlp::node> MapVecNode;
  MapVecNode mapN;
  //=====================================
  tlp::node splitEdge(tlp::node a, tlp::node b);
  //=====================================
  void elmentSplitting(const tlp::Coord &a, const tlp::Coord &b,
                       const std::vector<tlp::node> &input, std::vector<tlp::node> &in,
                       std::vector<tlp::node> &out);
  //=====================================
  void recQuad(const tlp::node a, const tlp::node b, const tlp::node c, const tlp::node d,
               const std::vector<tlp::node> &input);
};
#endif // QUAD_TREE_H
