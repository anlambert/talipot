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

#ifndef PARALLEL_EDGE_SELECTION_H
#define PARALLEL_EDGE_SELECTION_H

#include <talipot/BooleanProperty.h>
#include <talipot/PropertyAlgorithm.h>

#include "../utils/PluginNames.h"

/**
 * This selection plugins enables to find the parallel edges in a graph.
 * Let e1, e2 two edges, e1 and e2 are parallel edges if source(e1) = source(e2) and
 * target(e1) = target(e2).
 * If it exists n edges between two nodes, only n-1 edges will be selected.
 *
 *  \author David Auber, LaBRI University Bordeaux I France:
 *   auber@labri.fr
 */
class ParallelEdgesSelection : public tlp::BooleanAlgorithm {
  public:
    PLUGININFORMATION(tlp::SelectionAlgorithm::ParallelEdgesSelection, "David Auber", "20/01/2003",
                      "Selects the parallel edges of a graph.<br/>Two edges are "
                      "considered as parallel if they have the same source/origin and the same "
                      "target/destination."
                      "If it exists n edges between two nodes, only n-1 edges will be selected.",
                      "1.1", "Selection")
    ParallelEdgesSelection(const tlp::PluginContext *context);
    bool run() override;
};

#endif // PARALLEL_EDGE_SELECTION_H
