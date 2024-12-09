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

#ifndef OCTREE_BUNDLE_H
#define OCTREE_BUNDLE_H

#include <talipot/Vector.h>
#include <talipot/DrawingTools.h>

class OctreeBundle {
public:
  static void compute(tlp::Graph *graph, double splitRatio, tlp::LayoutProperty *layout = nullptr,
                      tlp::SizeProperty *size = nullptr);
  //=====================================
  static bool isIn(const tlp::Coord &p, const tlp::Coord &a, const tlp::Coord &b,
                   const tlp::Coord &c, const tlp::Coord &d);

protected:
  void createOctree(tlp::Graph *graph, tlp::LayoutProperty *layout = nullptr,
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
  typedef flat_hash_map<tlp::Coord, tlp::node> MapVecNode;
  MapVecNode mapN;
  //=====================================
  tlp::node splitEdge(tlp::node a, tlp::node b);
  //=====================================
  void elmentSplitting(const tlp::Coord &a, const tlp::Coord &b, const tlp::Coord &c,
                       const tlp::Coord &d, const std::vector<tlp::node> &input,
                       std::vector<tlp::node> &in, std::vector<tlp::node> &out);
  //=====================================
  void recQuad(const tlp::Coord fr[4], const tlp::Coord ba[4], const std::vector<tlp::node> &input);
  tlp::node addNode(const tlp::Coord &pos);
  std::vector<std::pair<tlp::node, tlp::node>> unvalidEdges;
};

#endif // OCTREE_BUNDLE_H
