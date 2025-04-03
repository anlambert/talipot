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

#include "SpanningTreeSelection.h"

#include <talipot/GraphTools.h>

PLUGIN(SpanningTreeSelection)

using namespace std;
using namespace tlp;

SpanningTreeSelection::SpanningTreeSelection(const tlp::PluginContext *context)
    : BooleanAlgorithm(context) {
    addOutParameter<uint>("#edges selected", "The number of 'tree' selected edges");
}

SpanningTreeSelection::~SpanningTreeSelection() = default;

///===========================================================
bool SpanningTreeSelection::run() {
    // first initialize result
    result->setAllNodeValue(false);
    result->setAllEdgeValue(false);

    if (graph->existProperty("viewSelection")) {
        BooleanProperty *viewSelection = graph->getBooleanProperty("viewSelection");

        for (auto n : graph->nodes()) {
            if (viewSelection->getNodeValue(n)) {
                result->setNodeValue(n, true);
            }
        }
    }

    selectSpanningForest(graph, result, pluginProgress);

    // output some useful information
    if (dataSet != nullptr) {
        dataSet->set("#edges selected", result->numberOfNonDefaultValuatedEdges());
    }

    return true;
}
