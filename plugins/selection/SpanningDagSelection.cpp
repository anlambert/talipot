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

#include <talipot/AcyclicTest.h>
#include <talipot/VectorProperty.h>

#include "SpanningDagSelection.h"

PLUGIN(SpanningDagSelection)

using namespace std;
using namespace tlp;

//=================================================================
SpanningDagSelection::SpanningDagSelection(const tlp::PluginContext *context)
    : BooleanAlgorithm(context) {
    addOutParameter<uint>("#edges selected", "The number of 'dag' selected edges");
}
//=================================================================
bool SpanningDagSelection::run() {
    for (auto n : graph->nodes()) {
        result->setNodeValue(n, true);
    }

    EdgeVectorProperty<bool> edgeprop(graph);
    edgeprop.setAll(true);

    vector<edge> obstructions;
    AcyclicTest::acyclicTest(graph, &obstructions);

    for (auto e : obstructions) {
        edgeprop[e] = false;
    }

    edgeprop.copyToProperty(result);

    // output some useful information
    if (dataSet != nullptr) {
        dataSet->set("#edges selected", uint(graph->numberOfEdges() - obstructions.size()));
    }

    return true;
}
//=================================================================
