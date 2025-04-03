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

#include <talipot/SortIterator.h>

using namespace tlp;

Iterator<node> *NumericProperty::getSortedNodes(const Graph *sg, bool ascendingOrder) {
    if (!sg) {
        sg = getGraph();
    }

    return new SortNodeIterator(sg->getNodes(), this, ascendingOrder);
}

Iterator<edge> *NumericProperty::getSortedEdges(const Graph *sg, bool ascendingOrder) {
    if (!sg) {
        sg = getGraph();
    }

    return new SortEdgeIterator(sg->getEdges(), this, ascendingOrder);
}

Iterator<edge> *NumericProperty::getSortedEdgesBySourceValue(const Graph *sg, bool ascendingOrder) {
    if (!sg) {
        sg = getGraph();
    }

    return new SortSourceEdgeIterator(sg->getEdges(), sg, this, ascendingOrder);
}

Iterator<edge> *NumericProperty::getSortedEdgesByTargetValue(const Graph *sg, bool ascendingOrder) {
    if (!sg) {
        sg = getGraph();
    }

    return new SortTargetEdgeIterator(sg->getEdges(), sg, this, ascendingOrder);
}

Iterator<edge> *NumericProperty::getSortedEdgesByExtremitiesValues(const Graph *sg,
                                                                   bool ascendingOrder) {
    if (!sg) {
        sg = getGraph();
    }

    return new SortExtremitiesEdgeIterator(sg->getEdges(), sg, this, ascendingOrder);
}
