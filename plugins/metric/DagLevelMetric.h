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

#ifndef DAG_LEVEL_METRIC_H
#define DAG_LEVEL_METRIC_H

#include <talipot/PluginHeaders.h>

/** This plugin is an implementation of a DAG layer decomposition
 *
 *  \note This algorithm works on general DAG, the complexity is in O(|E|+|V|);
 *
 */
class DagLevelMetric : public tlp::DoubleAlgorithm {
  public:
    PLUGININFORMATION("Dag Level", "David Auber", "10/03/2000",
                      "Implements a DAG layer decomposition.", "1.0", "Hierarchical")
    DagLevelMetric(const tlp::PluginContext *context);
    ~DagLevelMetric() override;
    bool run() override;
    bool check(std::string &erreurMsg) override;
};

#endif // DAG_LEVEL_METRIC_H
