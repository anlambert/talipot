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

#ifndef BUBBLE_TREE_H
#define BUBBLE_TREE_H

#include <talipot/PluginHeaders.h>

/** This plug-in implement the bubble tree drawing algorithm
 *  first published as:
 *
 *  Bubble Tree Drawing Algorithm : \n
 *  D. Auber and S. Grivet and J-P Domenger and Guy Melancon, \n
 *  In International Conference on Computer Vision and Graphics, pages 633-641, september 2004.
 *
 *
 *  \note  This algorithm only works on trees.
 *  Let n be the number of nodes, the algorithm complexity is in O(n) or O(nlog(n)),
 *  By default O(nlog(n)) algorithm is used, but one can choose the complexity by using
 *  the argument (bool)"complexity" (true means  O(nlog(n), false  O(n)).
 *  The algorithm can manage nodes of different size. The SizeProperty "viewSize"
 *  is used by default if no parameters are given to the plug-in.
 *  The parameter is (SizeProperty*) "node size".
 *
 *
 *
 *  \author David Auber, S. Grivet  University Bordeaux I France:
 *   auber@labri.fr, grivet@labri.fr
 */
class BubbleTree : public tlp::LayoutAlgorithm {
  public:
    PLUGININFORMATION(
        "Bubble Tree", "D.Auber/S.Grivet", "16/05/2003",
        "Implement the bubble tree drawing algorithm first published as:<br/>"
        "<b>Bubble Tree Drawing Algorithm</b>, D. Auber and S. Grivet and J-P Domenger "
        "and Guy Melancon, ICCVG, pages 633-641 (2004).",
        "1.1", "Tree")
    BubbleTree(const tlp::PluginContext *context);
    ~BubbleTree() override;
    bool run() override;

  private:
    double computeRelativePosition(tlp::node n,
                                   tlp::NodeVectorProperty<tlp::Vec5d> &relativePosition);
    void calcLayout(tlp::node n, tlp::NodeVectorProperty<tlp::Vec5d> &relativePosition);
    void calcLayout2(tlp::node n, tlp::Vec5d &nrPos,
                     tlp::NodeVectorProperty<tlp::Vec5d> &relativePosition, const tlp::Vec3d &,
                     const tlp::Vec3d &);
    tlp::Graph *tree;
    tlp::SizeProperty *nodeSize;
    bool nAlgo;
};

#endif // BUBBLE_TREE_H
