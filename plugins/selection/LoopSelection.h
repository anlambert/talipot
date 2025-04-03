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

#ifndef LOOP_SELECTION_H
#define LOOP_SELECTION_H

#include <talipot/BooleanProperty.h>
#include <talipot/PropertyAlgorithm.h>

#include "../utils/PluginNames.h"

/**
 * This selection plugins enables to find loops in a graph
 * A loop is an edge that has the same source and target.
 *
 *  \author David Auber, LaBRI University Bordeaux I France:
 *   auber@labri.fr
 */
class LoopSelection : public tlp::BooleanAlgorithm {
  public:
    PLUGININFORMATION(
        tlp::SelectionAlgorithm::LoopSelection, "David Auber", "20/01/2003",
        "Selects loops in a graph.<br/>A loop is an edge that has the same source and target.",
        "1.0", "Selection")
    LoopSelection(const tlp::PluginContext *context);
    bool run() override;
};

#endif // LOOP_SELECTION_H
