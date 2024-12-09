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

#ifndef IMPROVED_WALKER_H
#define IMPROVED_WALKER_H

#include <talipot/hash.h>
#include <vector>
#include <talipot/PluginHeaders.h>
#include "TreeTools.h"

class OrientableLayout;
class OrientableCoord;
class OrientableSizeProxy;
class ImprovedWalkerIterator;

/** This plugin is an implementation of a linear Walker's algorithm:
 *
 *  Christoph Buchheim and Michael Junger and Sebastian Leipert,
 *  Improving Walker's Algorithm to Run in Linear Time
 *  citeseer.ist.psu.edu/buchheim02improving.html
 *
 *  \note This algorithm works on tree.
 *  Let n be the number of nodes, the algorithm complexity is in O(n).
 *
 *  \author Julien Testut, Antony Durand, Pascal Ollier, Yashvin Nababsing, \n
 *  Sebastien Leclerc, Thibault Ruchon, Eric Dauchier \n
 *  University Bordeaux I France
 **/

class ImprovedWalker : public tlp::LayoutAlgorithm {
  friend class ImprovedWalkerUnitTests;

public:
  PLUGININFORMATION("Improved Walker",
                    "Julien Testut, Antony Durand, Pascal Ollier, "
                    "Yashvin Nababsing, Sebastien Leclerc, "
                    "Thibault Ruchon, Eric Dauchier",
                    "11/11/04",
                    "It is a linear implementation of the Walker's tree layout improved algorithm "
                    "described in<br/>"
                    "<b>Improving Walker's Algorithm to Run in Linear Time</b>, Christoph Buchheim "
                    "and Michael Junger and Sebastian Leipert (2002).",
                    "1.0", "Tree")
  ImprovedWalker(const tlp::PluginContext *context);
  ~ImprovedWalker() override;

  bool run() override;

private:
  typedef std::vector<float> levelToFloatType;
  typedef flat_hash_map<tlp::node, float> nodeToFloatType;
  typedef flat_hash_map<tlp::node, int> nodeToIntegerPropertyType;
  typedef flat_hash_map<tlp::node, tlp::node> nodeToNodeType;

  tlp::Graph *tree;

  float spacing;
  float nodeSpacing;

  OrientableLayout *oriLayout;
  OrientableSizeProxy *oriSize;

  int depthMax;
  nodeToIntegerPropertyType order;
  levelToFloatType maxYbyLevel;
  levelToFloatType posYbyLevel;
  nodeToFloatType prelimX;
  nodeToFloatType modChildX;
  nodeToNodeType thread;
  nodeToFloatType shiftNode;
  nodeToFloatType shiftDelta;
  nodeToNodeType ancestor;

  int initializeAllNodes(tlp::node root);
  int initializeNode(tlp::node root, uint depth);
  int countSibling(tlp::node from, tlp::node to);
  ImprovedWalkerIterator *iterateSibling(tlp::node from, tlp::node to);
  tlp::Iterator<tlp::node> *getChildren(tlp::node n);
  ImprovedWalkerIterator *getReversedChildren(tlp::node n);

  void firstWalk(tlp::node v);
  void secondWalk(tlp::node v, float modifierX, int depth);
  void combineSubtree(tlp::node v, tlp::node *defaultAncestor);
  void moveSubtree(tlp::node fromNode, tlp::node toNode, float rightShift);
  void executeShifts(tlp::node v);

  tlp::node getFather(tlp::node n);

  tlp::node leftmostChild(tlp::node n);
  tlp::node rightmostChild(tlp::node n);

  tlp::node leftSibling(tlp::node n);
  tlp::node rightSibling(tlp::node n);
  tlp::node leftMostSibling(tlp::node n);

  tlp::node nextRightContour(tlp::node v);
  tlp::node nextLeftContour(tlp::node v);
  tlp::node findCommonAncestor(tlp::node left, tlp::node right, tlp::node defaultAncestor);
};

//====================================================================
tlp::node ImprovedWalker::getFather(tlp::node n) {
  if (!n.isValid() || tree->indeg(n) < 1) {
    return tlp::node();
  }

  return tree->getInNode(n, 1);
}

//====================================================================
tlp::node ImprovedWalker::leftmostChild(tlp::node n) {
  if (!n.isValid() || tree->outdeg(n) < 1) {
    return tlp::node();
  }

  return tree->getOutNode(n, 1);
}

//====================================================================
tlp::node ImprovedWalker::rightmostChild(tlp::node n) {
  int pos;

  if (!n.isValid() || (pos = tree->outdeg(n)) < 1) {
    return tlp::node();
  }

  return tree->getOutNode(n, pos);
}

//====================================================================
tlp::node ImprovedWalker::leftSibling(tlp::node n) {
  tlp::node father = getFather(n);
  if (!father.isValid() || order[n] <= 1 || uint(order[n] - 1) > tree->outdeg(father)) {
    return tlp::node();
  } else {
    return tree->getOutNode(father, order[n] - 1);
  }
}

//====================================================================
tlp::node ImprovedWalker::rightSibling(tlp::node n) {
  tlp::node father = getFather(n);

  if (!father.isValid() || order[n] >= int(tree->outdeg(father))) {
    return tlp::node();
  }

  return tree->getOutNode(father, order[n] + 1);
}

//====================================================================
tlp::node ImprovedWalker::leftMostSibling(tlp::node n) {
  return leftmostChild(getFather(n));
}

//====================================================================
tlp::node ImprovedWalker::nextRightContour(tlp::node n) {
  if (isLeaf(tree, n)) {
    return thread[n];
  } else {
    return rightmostChild(n);
  }
}

//====================================================================
tlp::node ImprovedWalker::nextLeftContour(tlp::node n) {
  if (isLeaf(tree, n)) {
    return thread[n];
  } else {
    return leftmostChild(n);
  }
}

//====================================================================
tlp::node ImprovedWalker::findCommonAncestor(tlp::node left, tlp::node right,
                                             tlp::node defaultAncestor) {
  if (getFather(ancestor[left]) == getFather(right)) {
    return ancestor[left];
  } else {
    return defaultAncestor;
  }
}

#endif // IMPROVED_WALKER_H
