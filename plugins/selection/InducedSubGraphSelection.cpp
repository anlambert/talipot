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

#include <talipot/StableIterator.h>

#include "InducedSubGraphSelection.h"

using namespace tlp;

PLUGIN(InducedSubGraphSelection)

//=================================================================================
static constexpr std::string_view paramHelp[] = {
    // Nodes
    "Set of nodes from which the induced subgraph is computed.",
    // Use edges
    "If true, source and target nodes of selected edges will also be added in the input set of "
    "nodes."};
//=================================================================================
InducedSubGraphSelection::InducedSubGraphSelection(const tlp::PluginContext *context)
    : BooleanAlgorithm(context) {
    addInParameter<BooleanProperty>("Nodes", paramHelp[0].data(), "viewSelection");
    addInParameter<bool>("Use edges", paramHelp[1].data(), "false");
    addOutParameter<uint>("#edges selected", "The number of newly selected edges");
    // old name
    declareDeprecatedName("Induced Sub-Graph");
}
//=================================================================================
bool InducedSubGraphSelection::run() {
    BooleanProperty *entrySelection = nullptr;
    bool useEdges = false;

    if (dataSet != nullptr) {
        dataSet->get("Nodes", entrySelection);
        dataSet->get("Use edges", useEdges);
    }

    if (entrySelection == nullptr) {
        entrySelection = graph->getBooleanProperty("viewSelection");
    }

    // as the input selection property and the result property can be the same one,
    // if needed, use a stable iterator to keep a copy of the input selected nodes as all values
    // of the result property are reset to false below
    // delete done by the for loop
    Iterator<node> *itN = (result == entrySelection)
                              ? stableIterator(entrySelection->getNodesEqualTo(true))
                              : entrySelection->getNodesEqualTo(true);

    Iterator<edge> *itE = (result == entrySelection)
                              ? stableIterator(entrySelection->getEdgesEqualTo(true))
                              : entrySelection->getEdgesEqualTo(true);

    result->setAllNodeValue(false);
    result->setAllEdgeValue(false);

    // add input selected nodes to result selection
    for (auto current : itN) {
        result->setNodeValue(current, true);
    }

    if (useEdges) {
        for (auto e : itE) {
            result->setNodeValue(graph->source(e), true);
            result->setNodeValue(graph->target(e), true);
        }
    } else {
        delete itE;
    }

    // now add edges whose extremities are selected to result selection
    unsigned sel = 0;
    for (auto current : result->getNodesEqualTo(true)) {
        for (auto e : graph->getOutEdges(current)) {
            if (result->getNodeValue(graph->target(e))) {
                result->setEdgeValue(e, true);
                ++sel;
            }
        }
    }

    // output some useful information
    if (dataSet != nullptr) {
        dataSet->set("#edges selected", sel);
    }

    return true;
}
//=================================================================================
