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

#ifndef CLUSTER_METRIC_H
#define CLUSTER_METRIC_H

#include <talipot/DoubleProperty.h>
#include <talipot/PropertyAlgorithm.h>

/** This plugin computes the Cluster metric described in :
 *
 *  Y. Chiricota. F.Jourdan, an G.Melancon \n
 *  "Software component capture using graph clustering",\n
 *  "IWPC", \n
 *   2002.
 *
 *  Extended to unbounded depth (for the neighbors) :
 *
 *  \note This algorithm works on general graphs. The algorithm use the parameter
 *  depth in order to determine the depth of the neighbors.
 *
 */
class ClusterMetric : public tlp::DoubleAlgorithm {
  public:
    PLUGININFORMATION("Cluster", "David Auber", "26/02/2003",
                      "Computes the Cluster metric as described in<br/>"
                      "<b>Software component capture using graph clustering</b>, Y. Chiricota. "
                      "F.Jourdan, an G.Melancon, IWPC (2002).",
                      "1.0", "Graph")
    ClusterMetric(const tlp::PluginContext *context);
    bool run() override;
};

#endif // CLUSTER_METRIC_H
