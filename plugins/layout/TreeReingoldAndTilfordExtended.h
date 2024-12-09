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

#ifndef TREE_REINGOLD_AND_TILFORD_EXTENDED_H
#define TREE_REINGOLD_AND_TILFORD_EXTENDED_H

#include <string>
#include <cmath>
#include <talipot/hash.h>
#include <talipot/PluginHeaders.h>

struct LR {
  double L;
  double R;
  int size;
};

/** This plugin is an implementation of the hierarchical tree layout
 *  algorithm first published as:
 *
 *  E.M. Reingold and J.S. Tilford, \n
 *  "Tidier Drawings of Trees", \n
 *  "IEEE Transactions on Software Engineering" \n
 *  pages "223--228" \n
 *  1981 \n
 *
 *  \note This algorithm only works on trees; so if the graph is not a tree
 *  it will be applied on a spanning tree.\n
 *  Let n be the number of nodes, the algorithm complexity is in O(n).\n
 *  It extends the original Reingold and Tilford by providing management
 *  of different node size and different edge length. \n \n
 *  The algorithm use the (Size)"viewSize" property for element size and the
 *  (int)"treeEdgeLength" for the edge length.
 *
 */
class TreeReingoldAndTilfordExtended : public tlp::LayoutAlgorithm {
public:
  PLUGININFORMATION("Hierarchical Tree (R-T Extended)", "David Auber and Romain Bourqui",
                    "24/09/2010",
                    "Implements the hierarchical tree layout algorithm first published as:<br/>"
                    "<b>Tidier Drawings of Trees</b>, E.M. Reingold and J.S. Tilford, IEEE "
                    "Transactions on Software Engineering pages 223--228 (1981).",
                    "1.1", "Tree")
  TreeReingoldAndTilfordExtended(const tlp::PluginContext *context);
  ~TreeReingoldAndTilfordExtended() override;
  bool run() override;

private:
  void calcLayout(tlp::node, flat_hash_map<tlp::node, double> *, double, double, int,
                  flat_hash_map<int, double> &);
  double calcDecal(const std::list<LR> &, const std::list<LR> &) const;
  std::list<LR> *mergeLRList(std::list<LR> *, std::list<LR> *, double decal);
  std::list<LR> *TreePlace(tlp::node, flat_hash_map<tlp::node, double> *);
  void TreeLevelSizing(tlp::node, flat_hash_map<int, double> &, int,
                       flat_hash_map<tlp::node, int> &levels);

  tlp::Graph *tree;
  tlp::SizeProperty *sizes;
  tlp::IntegerProperty *lengthMetric;
  bool ortho;
  bool useLength;
  std::string orientation;
  float spacing;
  float nodeSpacing;
  bool compactLayout;
};

#endif // TREE_REINGOLD_AND_TILFORD_EXTENDED_H
