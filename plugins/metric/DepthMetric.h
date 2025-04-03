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

#ifndef DEPTH_METRIC_H
#define DEPTH_METRIC_H

#include <talipot/DoubleProperty.h>
#include <talipot/PropertyAlgorithm.h>

/** This plugins compute for each node n, the maximum path-length between n and the other nodes.
 *  The graph must be acyclic.
 *
 *  \note This algorithm works on general graphs.
 *
 */
class DepthMetric : public tlp::DoubleAlgorithm {
  public:
    PLUGININFORMATION("Depth", "David Auber", "15/02/2001",
                      "For each node n on an acyclic graph,"
                      "it computes the maximum path length between n and the other node."
                      "<br><b>The graph must be acyclic</b>.",
                      "1.1", "Hierarchical")
    DepthMetric(const tlp::PluginContext *context);
    bool run() override;
    bool check(std::string &) override;

  private:
    double getNodeValue(const tlp::node n);
    tlp::NumericProperty *edgeWeight;
};

#endif // DEPTH_METRIC_H
