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

#ifndef OCT_TREE_H
#define OCT_TREE_H

#include <talipot/Node.h>
#include <talipot/Coord.h>
#include <talipot/DoubleProperty.h>
#include <vector>

using namespace tlp;

/**
 * Octtree for graph nodes with positions in 3D space.
 * Contains all graph nodes that are located in a given cuboid in 3D space.
 * From Andreas Noack's java implementation.
 */

class OctTree {
    friend class LinLogLayout;

  protected:
    bool isLeaf;
    bool firstNode;

    // Maximum depth of tree nodes.
    uint MAX_DEPTH; // = 18;
    uint MAX_CHILDREN;

    // For leafs, the corresponding graph node; for non-leafs <code>null</code>.
    tlp::node node;

    // Children of this tree node.
    OctTree **_children;

    // Number of children of this tree node.
    uint childCount; // = 0;

    // Barycenter of the contained graph nodes.
    Coord position;

    // Total weight of the contained graph nodes.
    double weight;

    // Minimum coordinates of the cuboid in each of the 3 dimensions.
    Coord minPos;

    // Maximum coordinates of the cuboid in each of the 3 dimensions.
    Coord maxPos;

    // The weight metric
    tlp::DoubleProperty *linLogWeight;

  public:
    // constructor
    OctTree(tlp::node node, Coord position, Coord minPos, Coord maxPos,
            tlp::DoubleProperty *_linLogWeight, bool _fistNode);
    // destructor
    ~OctTree();

    // Adds a graph node to the octtree
    void addNode(tlp::node newNode, Coord newPos, uint depth);

    // Adds a graph node to the OctTree, without changing the position and weight of the root
    void addNode2(tlp::node newNode, Coord newPos, uint depth);

    // Removes a graph node from the octtree
    void removeNode(tlp::node oldNode, Coord oldPos, uint depth);
    // Returns the maximum extension of the octtree
    double width();

    // Returns the height of the octtree
    int getHeight();

    // Returns the current node
    tlp::node getNode();

    // Sets the maximum number of children for a branch of the octtree
    void setMaxChildren(uint);

    // Prints the tree on a console output
    void printTree(uint);
};

#endif // OCT_TREE_H
