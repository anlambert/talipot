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

#include "OctTree.h"

using namespace tlp;

/**
 * Creates an OctTree containing exactly one graph node.
 *
 * @param node     graph node
 * @param position position of the graph node
 * @param minPos   minimum coordinates of the cuboid
 * @param maxPos   maximum coordinates of the cuboid
 */
OctTree::OctTree(tlp::node _node, Coord _position, Coord _minPos, Coord _maxPos,
                 tlp::DoubleProperty *_linLogWeight, bool _firstNode)
    : position(_position[0], _position[1], _position[2]),
      minPos(_minPos[0], _minPos[1], _minPos[2]), maxPos(_maxPos[0], _maxPos[1], _maxPos[2]) {

    firstNode = _firstNode;
    linLogWeight = _linLogWeight;
    MAX_DEPTH = 8;
    MAX_CHILDREN = 8;

    node = _node;
    isLeaf = true;

    if (firstNode) {
        isLeaf = false;
    }

    childCount = 0;
    _children = nullptr;
    this->weight = 0.0;

    if (!firstNode) {
        this->weight = linLogWeight->getNodeValue(_node);
    }
}

/**
 * Deletes the an OctTree.
 */
OctTree::~OctTree() {
    if (_children != nullptr) {

        for (uint i = 0; i < MAX_CHILDREN; ++i) {
            if (_children[i] != nullptr) {
                delete _children[i];
            }

            _children[i] = nullptr;
        }

        delete[] _children;
        _children = nullptr;
    }
}

/**
 *  Sets the maximum number of children in the OctTree.
 *
 *  @param max maximum nimber of children
 */

void OctTree::setMaxChildren(uint max) {
    MAX_CHILDREN = max;
}

/**
 * Adds a graph node to the octtree.
 *
 * @param newNode  graph node
 * @param newPos   position of the graph node
 * @param depth    depth of this tree node in the octtree
 */
void OctTree::addNode(tlp::node newNode, Coord newPos, uint depth) {
    if (depth > MAX_DEPTH - 1) {
        std::cerr << "assert: adding a node at a depth deeper than the max depth (add1)\n";
        return;
    }

    double nnWeight = linLogWeight->getNodeValue(newNode);

    if (nnWeight == 0.0) {
        return;
    }

    if (isLeaf) {
        addNode2(node, position, depth);
        isLeaf = false;
    }

    for (int d = 0; d < 3; ++d) {
        position[d] = (weight * position[d] + nnWeight * newPos[d]) / (weight + nnWeight);
    }

    weight += nnWeight;

    addNode2(newNode, newPos, depth);
}

/**
 * Returns the current node of this OctTree
 */

tlp::node OctTree::getNode() {
    return node;
}

/**
 * Adds a graph node to the OctTree,
 * without changing the position and weight of the root.
 *
 * @param newNode  graph node
 * @param newPos   position of the graph node
 * @param depth    depth of this tree node in the octtree
 */
void OctTree::addNode2(tlp::node newNode, Coord newPos, uint depth) {
    if (depth > MAX_DEPTH - 1) {
        std::cerr << "assert: adding a node at a depth deeper than the max depth! (add2)\n";
        return;
    }

    if (depth == MAX_DEPTH - 1) {
        if (childCount == MAX_CHILDREN) {
            OctTree **_oldChildren = _children;
            _children = new OctTree *[2 * MAX_CHILDREN];

            for (uint i = 0; i < MAX_CHILDREN; ++i) {
                _children[i] = _oldChildren[i];
            }

            for (uint i = MAX_CHILDREN; i < MAX_CHILDREN * 2; ++i) {
                _children[i] = nullptr;
            }

            MAX_CHILDREN *= 2;
        }

        if (childCount == 0 || _children == nullptr) {
            _children = new OctTree *[MAX_CHILDREN];

            for (uint i = 0; i < MAX_CHILDREN; ++i) {
                _children[i] = nullptr;
            }
        }

        _children[childCount++] = new OctTree(newNode, newPos, newPos, newPos, linLogWeight, false);

        return;
    }

    // on localise le noeud
    int childIndex = 0;

    for (int d = 0; d < 3; ++d) {
        if (newPos[d] > (minPos[d] + maxPos[d]) / 2) {
            childIndex += 1 << d;
        }
    }

    if (childCount == 0 || _children == nullptr) {
        _children = new OctTree *[MAX_CHILDREN];

        for (uint i = 0; i < MAX_CHILDREN; ++i) {
            _children[i] = nullptr;
        }
    }

    // si la place est vide
    if (_children[childIndex] == nullptr) {

        Coord newMinPos;
        Coord newMaxPos;

        for (int d = 0; d < 3; ++d) {
            if ((childIndex & 1 << d) == 0) {
                newMinPos[d] = minPos[d];
                newMaxPos[d] = (minPos[d] + maxPos[d]) / 2;
            } else {
                newMinPos[d] = (minPos[d] + maxPos[d]) / 2;
                newMaxPos[d] = maxPos[d];
            }
        }

        childCount++;
        _children[childIndex] =
            new OctTree(newNode, newPos, newMinPos, newMaxPos, linLogWeight, false);
    } else {
        _children[childIndex]->addNode(newNode, newPos, depth + 1);
    }
}

/**
 * Prints the OctTree on a console output at the desired depth
 *
 * @param depth the desired depth
 */

void OctTree::printTree(uint depth) {
    std::cerr << "\n";

    for (uint i = 0; i < depth; ++i) {
        std::cerr << "\t";
    }

    std::cerr << "[d(" << depth << "),w(" << weight << "),n(" << node.id << "),l(" << isLeaf
              << "),p(" << position[0] << "," << position[1] << "," << position[2] << "),";

    if (_children != nullptr) {
        for (uint i = 0; i < MAX_CHILDREN; ++i) {
            if (_children[i] == nullptr) {
                std::cerr << "X,";
            } else {
                std::cerr << "O,";
            }
        }
    }

    for (uint i = 0; i < MAX_CHILDREN; ++i) {
        if (_children[i] != nullptr) {
            if (depth < MAX_DEPTH) {
                _children[i]->printTree(depth + 1);
            }
        }

        std::cerr << "]\n";
    }
}

/**
 * Removes a graph node from the octtree.
 *
 * @param oldNode  graph node
 * @param oldPos   position of the graph node
 * @param depth    current depth
 */
void OctTree::removeNode(tlp::node oldNode, Coord oldPos, uint depth) {
    if (depth > MAX_DEPTH - 1) {
        std::cerr << "assert: remove a node at a depth deeper than the max depth: " << depth
                  << " / " << MAX_DEPTH - 1 << "\n";
        return;
    }

    double onWeight = linLogWeight->getNodeValue(oldNode);

    if (onWeight == 0.0) {
        return;
    }

    if (weight <= onWeight) {
        weight = 0.0;
        for (uint i = 0; i < childCount; ++i) {
            delete _children[i];
            _children[i] = nullptr;
        }

        delete[] _children;
        _children = nullptr;

        childCount = 0;
        return;
    }

    for (int d = 0; d < 3; ++d) {
        position[d] = (weight * position[d] - onWeight * oldPos[d]) / (weight - onWeight);
    }

    weight -= onWeight;

    if (depth == MAX_DEPTH - 1) {
        if (childCount > 0) {
            uint childIndex = 0;

            bool endwhile = false;

            while (!endwhile) {
                if (childIndex < MAX_CHILDREN) {
                    if (_children[childIndex] == nullptr) {
                        std::cerr << "this part of the tree is null\n";
                        childIndex++;
                    } else if ((_children[childIndex])->node.id != oldNode.id) {
                        childIndex++;
                    } else {
                        endwhile = true;
                    }

                } else {
                    std::cerr << "we're stopping at the end of the table: " << childIndex << "\n";
                    endwhile = true;
                }
            }

            if (childIndex == MAX_CHILDREN) {
                std::cerr << "assert: removing a non existent node in the tree\n";
                return;
            }

            delete _children[childIndex];
            _children[childIndex] = nullptr;

            for (uint i = childIndex; i < childCount - 1; ++i) {
                _children[i] = _children[i + 1];
            }

            _children[childCount - 1] = nullptr; // delete

            --childCount;

        } else {
            std::cerr << "assert ChildCount <= 0: " << childCount << "\n";
        }

    } else {
        int childIndex = 0;

        // on localise le noeud
        for (int d = 0; d < 3; ++d) {
            if (oldPos[d] > (minPos[d] + maxPos[d]) / 2) {
                childIndex += 1 << d;
            }
        }

        if (_children[childIndex] != nullptr) {

            _children[childIndex]->removeNode(oldNode, oldPos, depth + 1);

            if (_children[childIndex]->weight == 0.0) {
                delete _children[childIndex];
                _children[childIndex] = nullptr;
                --childCount;
            }
        } else {
            std::cerr << "assert: the selected child it is not supposed to be nullptr!\n";
            return;
        }
    }
}

/**
 * Returns the maximum extension of the octtree.
 *
 * @return maximum over all dimensions of the extension of the octtree
 */
double OctTree::width() {
    double width = 0.0;

    for (int d = 0; d < 3; ++d) {
        if (maxPos[d] - minPos[d] > width) {
            width = maxPos[d] - minPos[d];
        }
    }

    return width;
}

/**
 * Returns the height of the octtree.
 *
 * @return height of the octtree
 */
int OctTree::getHeight() {
    int height = -1;

    for (uint i = 0; i < childCount; ++i) {
        OctTree *aChild = _children[i];

        if (aChild != nullptr) {
            height = std::max(height, aChild->getHeight());
        }
    }

    return height + 1;
}
