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

#ifndef MAKE_SELECTION_GRAPH_H
#define MAKE_SELECTION_GRAPH_H

#include <talipot/BooleanProperty.h>
#include <talipot/PropertyAlgorithm.h>
#include <talipot/GraphTest.h>

#include "../utils/PluginNames.h"

/**
 * This selection plugin extends the current selection to a graph.
 * All selected edges will also have their extremities selected (no dangling edges).
 *
 *  \author Bruno Pinaud, LaBRI University of Bordeaux France:
 */

class MakeSelectionGraph : public tlp::BooleanAlgorithm {
  public:
    MakeSelectionGraph(const tlp::PluginContext *context);
    PLUGININFORMATION(
        tlp::SelectionAlgorithm::MakeSelectionGraph, "Bruno Pinaud", "28/11/2016",
        "Extends the selection to have a graph.<br/>"
        "All selected edges of the current graph will have their extremities selected "
        "(no dangling edges).",
        "1.0", "Selection")
    bool run() override;
};

class IsGraphTest : public tlp::GraphTest {
    PLUGININFORMATION("Graph", "Bruno Pinaud", "29/11/2016",
                      "Tests whether the set of the selected "
                      "elements of the current graph is a "
                      "graph or not (no dangling edges).",
                      "1.0", "Topological Test")
  public:
    IsGraphTest(const tlp::PluginContext *context);
    bool test() override;
};

#endif // MAKE_SELECTION_GRAPH_H
