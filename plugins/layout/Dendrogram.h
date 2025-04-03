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

#ifndef DENDROGRAM_H
#define DENDROGRAM_H

#include <map>
#include <vector>
#include <talipot/PluginHeaders.h>

class OrientableLayout;
class OrientableCoord;
class OrientableSizeProxy;

/** This plugin is an implementation of a dendrogram, an extended implementation
 *  of a "Bio representation" which includes variable orientation
 *  and variable node sizelayout.
 *
 *  \note This works on tree.
 *  Let n be the number of nodes, the algorithm complexity is in O(n).
 *
 *  \author Julien Testut, Antony Durand, Pascal Ollier, Yashvin Nababsing, \n
 *  Sebastien Leclerc, Thibault Ruchon, Eric Dauchier \n
 *  University Bordeaux I France
 **/
class Dendrogram : public tlp::LayoutAlgorithm {
  public:
    PLUGININFORMATION("Dendrogram",
                      "Julien Testut, Antony Durand, Pascal Ollier, Yashvin Nababsing, "
                      "Sebastien Leclerc, Thibault Ruchon, Eric Dauchier",
                      "03/12/04",
                      "This is an implementation of a dendrogram, an extended "
                      "implementation of a <b>Bio representation</b> which includes "
                      "variable orientation and variable node sizelayout.",
                      "1.0", "Tree")
    Dendrogram(const tlp::PluginContext *context);
    ~Dendrogram() override;

    bool run() override;

  private:
    float spacing;
    float nodeSpacing;

    std::map<tlp::node, float> leftshift;
    tlp::node root;
    tlp::Graph *tree;
    std::vector<float> levelHeights;

    float setAllNodesCoordX(tlp::node n, float rightMargin, OrientableLayout *oriLayout,
                            OrientableSizeProxy *oriSize);
    void setAllNodesCoordY(OrientableLayout *oriLayout, OrientableSizeProxy *oriSize);
    float computeFatherXPosition(tlp::node father, OrientableLayout *oriLayout);
    void shiftAllNodes(tlp::node n, float shift, OrientableLayout *oriLayout);
    void setNodePosition(tlp::node n, float x, float y, float z, OrientableLayout *oriLayout);
    void setCoordY(tlp::node n, float &maxYLeaf, OrientableLayout *oriLayout,
                   OrientableSizeProxy *oriSize);
    void computeLevelHeights(tlp::Graph *tree, tlp::node n, uint depth,
                             OrientableSizeProxy *oriSize);
};

#endif // DENDROGRAM_H
