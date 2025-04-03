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

#ifndef DEGREE_METRIC_H
#define DEGREE_METRIC_H

#include <talipot/DoubleProperty.h>
#include <talipot/PropertyAlgorithm.h>

/**
 * This plugin compute the degree of each node
 *
 */
class DegreeMetric : public tlp::DoubleAlgorithm {
  public:
    PLUGININFORMATION("Degree", "David Auber", "04/10/2001", "Assigns its degree to each node.",
                      "1.0", "Graph")
    DegreeMetric(const tlp::PluginContext *context);
    bool run() override;
    bool check(std::string &errorMsg) override;
};

#endif // DEGREE_METRIC_H
