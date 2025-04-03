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

#include "Kruskal.h"

#include <talipot/GraphTools.h>
#include <talipot/ConnectedTest.h>
#include <talipot/DoubleProperty.h>

PLUGIN(Kruskal)

using namespace std;
using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // edge weight
    "Metric containing the edge weights."};
//======================================================
Kruskal::Kruskal(const tlp::PluginContext *context) : BooleanAlgorithm(context) {
    addInParameter<NumericProperty *>("edge weight", paramHelp[0].data(), "viewMetric");
    addOutParameter<uint>("#edges selected", "The number of newly selected edges");
}

//======================================================

bool Kruskal::check(std::string &erreurMsg) {
    if (!ConnectedTest::isConnected(graph)) {
        erreurMsg = "The graph must be connected.";
        return false;
    }

    return true;
}
//======================================================
/// Compute the Minimum Spanning Tree
bool Kruskal::run() {
    /* Initialisation */
    NumericProperty *edgeWeight = nullptr;

    if (dataSet != nullptr) {
        dataSet->get("edge weight", edgeWeight);
    }

    if (edgeWeight == nullptr) {
        edgeWeight = graph->getDoubleProperty("viewMetric");
    }

    selectMinimumSpanningTree(graph, result, edgeWeight, pluginProgress);

    // output some useful information
    if (dataSet != nullptr) {
        dataSet->set("#edges selected", result->numberOfNonDefaultValuatedEdges());
    }

    return true;
}
//=======================================================================
