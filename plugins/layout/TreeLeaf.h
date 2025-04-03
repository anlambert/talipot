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

#ifndef TREE_LEAF_H
#define TREE_LEAF_H

#include <talipot/PluginHeaders.h>

/**
 * \file
 * \brief An implementation of a simple tree layout.
 *
 * This plugin is an implementation of a simple tree layout.
 * All leaves are placed at a distance one (x-coordinates) and the order
 * is the one of a suffix ordering. The y coordinates is the depth in the
 * tree. The other nodes are placed at the center of their children
 * (x-coordinates), and the y-coordinate is their depth in the tree.
 *
 * <b>HISTORY</b>
 *
 * - 01/12/99 Version 0.0.1: Initial release
 *
 *
 * \note Let n be the number of nodes, the algorithm complexity is in O(n).
 *
 *
 * \author David Auber University Bordeaux I France: Email:auber@labri.fr
 */
class OrientableLayout;
class OrientableSizeProxy;

class TreeLeaf : public tlp::LayoutAlgorithm {
  public:
    PLUGININFORMATION("Tree Leaf", "David Auber", "01/12/1999",
                      "Implements a simple level-based tree layout.", "1.1", "Tree")
    TreeLeaf(const tlp::PluginContext *context);
    ~TreeLeaf() override;
    bool run() override;

  private:
    float minLayerSpacing;
    float nodeSpacing;
    bool uniformLayerDistance;
    std::vector<float> levelHeights;
    float dfsPlacement(tlp::Graph *tree, tlp::node n, float x, float y, uint depth,
                       OrientableLayout *oriLayout, OrientableSizeProxy *oriSize);
    void computeLevelHeights(tlp::Graph *tree, tlp::node n, uint depth,
                             OrientableSizeProxy *oriSize);
};

#endif // TREE_LEAF_H
