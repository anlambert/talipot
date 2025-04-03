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

#ifndef SPANNING_TREE_SELECTION_H
#define SPANNING_TREE_SELECTION_H

#include <talipot/BooleanProperty.h>
#include <talipot/PropertyAlgorithm.h>

/**
 * This selection plugins enables to find a subgraph of G that is a forest (a set of trees).
 *
 *  \author David Auber, LaBRI University Bordeaux I France:
 *   auber@labri.fr
 */
class SpanningTreeSelection : public tlp::BooleanAlgorithm {
  public:
    PLUGININFORMATION("Spanning Forest", "David Auber", "01/12/1999",
                      "Selects a subgraph of a graph that is a forest (a set of trees).", "1.0",
                      "Selection")
    SpanningTreeSelection(const tlp::PluginContext *context);
    ~SpanningTreeSelection() override;
    bool run() override;
};

#endif // SPANNING_TREE_SELECTION_H
