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

#ifndef REACHABLE_SUB_GRAPH_SELECTION_H
#define REACHABLE_SUB_GRAPH_SELECTION_H

#include <talipot/BooleanProperty.h>
#include <talipot/PropertyAlgorithm.h>

#include "../utils/PluginNames.h"

/**
 * This selection plugin enables to find all nodes and edges at a given distance of one of the node
 * of a given set of nodes.
 * It takes three parameters :
 * - <b>int</b> distance
 * - <b>StringCollection</b> edge direction : output edges , input edges , all edges
 * - <b>BooleanProperty *</b> startingnodes : the selected nodes of this BooleanProperty
 * (those whose associated value is true) will be used as starting nodes.
 *
 *  \author David Auber, LaBRI University Bordeaux I France:
 *   auber@labri.fr
 */
class ReachableSubGraphSelection : public tlp::BooleanAlgorithm {
  public:
    PLUGININFORMATION(tlp::SelectionAlgorithm::ReachableSubGraphSelection, "David Auber",
                      "01/12/1999",
                      "Selects all nodes and edges at a given distance of a set of selected nodes.",
                      "1.1", "Selection")

    ReachableSubGraphSelection(const tlp::PluginContext *context);
    bool run() override;
};

#endif // REACHABLE_SUB_GRAPH_SELECTION_H
