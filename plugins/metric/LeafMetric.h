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

#ifndef LEAF_METRIC_H
#define LEAF_METRIC_H

#include <talipot/DoubleProperty.h>
#include <talipot/PropertyAlgorithm.h>

/** This plugin computes the number of leaves in the subtree induced by each node.
 *
 *
 */
class LeafMetric : public tlp::DoubleAlgorithm {
  public:
    PLUGININFORMATION("Leaf", "David Auber", "20/12/1999",
                      "Computes the number of leaves in the subtree induced by each node.<br/>"
                      "<b>The graph must be acyclic</b>.",
                      "1.0", "Hierarchical")
    LeafMetric(const tlp::PluginContext *context);
    bool check(std::string &) override;
    bool run() override;

  private:
    double getNodeValue(const tlp::node n);
};

#endif // LEAF_METRIC_H
