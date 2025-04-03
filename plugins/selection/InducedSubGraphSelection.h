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

#ifndef INDUCED_SUB_GRAPH_SELECTION_H
#define INDUCED_SUB_GRAPH_SELECTION_H

#include <talipot/BooleanProperty.h>
#include <talipot/PropertyAlgorithm.h>

#include "../utils/PluginNames.h"

/// This selection plugins enables to find a subgraph induced by a set of nodes.
/**
 * Let V' a subset of nodes, G' is an induced subgraph of G if all edges
 * (and only these ones) element of G that link
 * two nodes (only one in case of loop) of V' are in G'.
 *
 *  \author David Auber, LaBRI University Bordeaux I France:
 *   auber@labri.fr
 */
class InducedSubGraphSelection : public tlp::BooleanAlgorithm {
  public:
    PLUGININFORMATION(
        tlp::SelectionAlgorithm::InducedSubGraphSelection, "David Auber", "08/08/2001",
        "Selects all the nodes/edges of the subgraph induced by a set of selected nodes.", "1.1",
        "Selection")
    InducedSubGraphSelection(const tlp::PluginContext *context);
    bool run() override;
};

#endif // INDUCED_SUB_GRAPH_SELECTION_H
