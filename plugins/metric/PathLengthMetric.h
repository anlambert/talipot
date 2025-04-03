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

#ifndef PATH_LENGTH_METRIC_H
#define PATH_LENGTH_METRIC_H

#include <talipot/PluginHeaders.h>

class PathLengthMetric : public tlp::DoubleAlgorithm {
  public:
    PLUGININFORMATION("Path Length", "David Auber", "15/02/2001",
                      "Assigns to each node the number of paths that goes through it.<br/><b>The "
                      "graph must be acyclic</b>.",
                      "1.0", "Hierarchical")
    PathLengthMetric(const tlp::PluginContext *context);
    bool run() override;
    bool check(std::string &) override;

  private:
    double getNodeValue(const tlp::node n, tlp::DoubleProperty *leafMetric);
};

#endif // PATH_LENGTH_METRIC_H
