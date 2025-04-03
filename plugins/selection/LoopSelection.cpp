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

#include "LoopSelection.h"

PLUGIN(LoopSelection)

using namespace std;
using namespace tlp;

LoopSelection::LoopSelection(const tlp::PluginContext *context) : BooleanAlgorithm(context) {
    addOutParameter<uint>("#edges selected", "The number of loops selected");
}
//============================================
bool LoopSelection::run() {
    result->setAllNodeValue(false);
    unsigned cpt = 0;
    for (auto e : graph->edges()) {
        const auto &[src, tgt] = graph->ends(e);

        if (src == tgt) {
            result->setEdgeValue(e, true);
            ++cpt;
        }
    }

    // output some useful information
    if (dataSet != nullptr) {
        dataSet->set("#edges selected", cpt);
    }

    return true;
}
//============================================
