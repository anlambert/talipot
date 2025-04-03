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

#include "ClusterMetric.h"

#include <talipot/GraphMeasure.h>

PLUGIN(ClusterMetric)

using namespace std;
using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // depth
    "Maximal depth of a computed cluster."};
//=================================================
ClusterMetric::ClusterMetric(const tlp::PluginContext *context) : DoubleAlgorithm(context) {
    addInParameter<uint>("depth", paramHelp[0].data(), "1");
}
//=================================================
static double clusterGetEdgeValue(Graph *graph, tlp::NodeVectorProperty<double> &clusters,
                                  const edge e) {
    const auto &[src, tgt] = graph->ends(e);
    const double v1 = clusters.getNodeValue(src);
    const double v2 = clusters.getNodeValue(tgt);

    double sum = v1 * v1 + v2 * v2;

    if (sum) {
        return 1. - fabs(v1 - v2) / sqrt(sum);
    }

    return 0.;
}
//=================================================
bool ClusterMetric::run() {
    uint maxDepth = 1;

    if (dataSet != nullptr) {
        dataSet->get("depth", maxDepth);
    }

    tlp::NodeVectorProperty<double> clusters(graph);
    clusteringCoefficient(graph, clusters, maxDepth);
    clusters.copyToProperty(result);

    const std::vector<edge> &edges = graph->edges();
    uint nbEdges = edges.size();

    for (uint i = 0; i < nbEdges; ++i) {
        edge e = edges[i];
        result->setEdgeValue(e, clusterGetEdgeValue(graph, clusters, e));
    }

    return true;
}
