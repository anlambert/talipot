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

#include "ParallelEdgesSelection.h"

#include <talipot/ConcatIterator.h>
#include <talipot/SimpleTest.h>

PLUGIN(ParallelEdgesSelection)

using namespace std;
using namespace tlp;

ParallelEdgesSelection::ParallelEdgesSelection(const tlp::PluginContext *context)
    : BooleanAlgorithm(context) {
    addInParameter<bool>(
        "directed", "Indicates if the graph should be considered as directed or not.", "false");
    addOutParameter<uint>("#edges selected", "The number of parallel edges selected");
}

bool ParallelEdgesSelection::run() {
    bool directed = false;
    if (dataSet) {
        dataSet->get("directed", directed);
    }

    auto [loops, parallelEdges] = SimpleTest::getLoopsAndParallelEdges(graph, directed);

    result->setAllNodeValue(false);
    result->setAllEdgeValue(false);

    for (auto e : parallelEdges) {
        result->setEdgeValue(e, true);
    }

    // output some useful information
    if (dataSet != nullptr) {
        dataSet->set("#edges selected", uint(parallelEdges.size()));
    }

    return true;
}
