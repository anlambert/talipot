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

#ifndef TREE_TOOLS_H
#define TREE_TOOLS_H

#include <talipot/Node.h>
#include <talipot/LayoutProperty.h>
#include <talipot/SizeProperty.h>

namespace tlp {
class Graph;
}

//====================================================================
inline bool isLeaf(const tlp::Graph *tree, tlp::node n) {
    return tree->isElement(n) && tree->outdeg(n) == 0;
}

//====================================================================
inline float getNodeX(tlp::LayoutProperty *pLayout, tlp::node current) {
    return pLayout->getNodeValue(current).getX();
}

//====================================================================
inline float getNodeY(tlp::LayoutProperty *pLayout, tlp::node current) {
    return pLayout->getNodeValue(current).getY();
}

//====================================================================
inline float getNodeHeight(tlp::SizeProperty *size, tlp::node current) {
    return size->getNodeValue(current).getH();
}

//====================================================================
inline float getNodeWidth(tlp::SizeProperty *size, tlp::node current) {
    return size->getNodeValue(current).getW();
}
#endif // TREE_TOOLS_H
