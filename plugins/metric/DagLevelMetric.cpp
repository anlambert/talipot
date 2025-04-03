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

#include "DagLevelMetric.h"

PLUGIN(DagLevelMetric)

using namespace std;
using namespace tlp;

DagLevelMetric::DagLevelMetric(const tlp::PluginContext *context) : DoubleAlgorithm(context) {}

DagLevelMetric::~DagLevelMetric() = default;
//======================================================
bool DagLevelMetric::run() {
    NodeVectorProperty<uint> level(graph);
    dagLevel(graph, level);
    level.copyToProperty(result);

    return true;
}
//======================================================
bool DagLevelMetric::check(std::string &erreurMsg) {
    if (AcyclicTest::isAcyclic(graph)) {
        erreurMsg = "";
        return true;
    } else {
        erreurMsg = "The graph must be a acylic.";
        return false;
    }
}
