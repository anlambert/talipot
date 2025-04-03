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

#include "IdMetric.h"

#include <talipot/StringCollection.h>

PLUGIN(IdMetric)

using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // target
    "Whether the id is copied only for nodes, only for edges, or for both."};

#define TARGET_TYPE "target"
#define TARGET_TYPES "both;nodes;edges"
#define NODES_TARGET 1
#define EDGES_TARGET 2
#define BOTH_TARGET 0

//==================================================================
IdMetric::IdMetric(const tlp::PluginContext *context) : DoubleAlgorithm(context) {
    addInParameter<StringCollection>(TARGET_TYPE, paramHelp[0].data(), TARGET_TYPES, true,
                                     "<b>both</b> <br> </b>node</b>s <br> <b>edges</b>");

    // result needs to be an inout parameter
    // in order to preserve the original values of non targeted elements
    // i.e if "target" = "nodes", the values of edges must be preserved
    // and if "target" = "edges", the values of nodes must be preserved
    parameters.setDirection("result", INOUT_PARAM);
}
//==================================================================
bool IdMetric::run() {
    bool nodes(true), edges(true);

    if (dataSet != nullptr) {
        StringCollection targetType;
        dataSet->get(TARGET_TYPE, targetType);

        if (targetType.getCurrent() == NODES_TARGET) {
            edges = false;
            nodes = true;
        } else if (targetType.getCurrent() == EDGES_TARGET) {
            edges = true;
            nodes = false;
        } else {
            edges = true;
            nodes = true;
        }
    }

    if (nodes) {
        for (auto n : graph->nodes()) {
            result->setNodeValue(n, n.id);
        }
    }

    if (edges) {
        for (auto e : graph->edges()) {
            result->setEdgeValue(e, e.id);
        }
    }

    return true;
}
//=================================================================
