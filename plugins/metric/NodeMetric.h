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

#ifndef NODE_METRIC_H
#define NODE_METRIC_H

#include <talipot/PluginHeaders.h>

/** This plugin computes the number of nodes in the subtree induced by each node.
 *
 *  \note This algorithm assigns to each node a value defined as following : If two nodes are in the
 * same
 *  connected component they have the same value else they have a
 *  different value.
 *
 */
class NodeMetric : public tlp::DoubleAlgorithm {
  public:
    PLUGININFORMATION("Node", "David Auber", "20/12/1999",
                      "Computes the number of nodes in the subtree induced by each node.<br/>"
                      "<b>The graph must be acyclic</b>.",
                      "1.0", "Hierarchical")
    NodeMetric(const tlp::PluginContext *context);
    bool run() override;
    bool check(std::string &) override;

  private:
    double getNodeValue(const tlp::node n);
};

#endif // NODE_METRIC_H
