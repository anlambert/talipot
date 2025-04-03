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

#ifndef STRENGTH_METRIC_H
#define STRENGTH_METRIC_H

#include <unordered_set>
#include <talipot/PluginHeaders.h>

/** This plugin computes the Strength metric described in :
 *
 *  Y. Chiricota. F.Jourdan, and G.Melancon\n
 *  "Software component capture using graph clustering", \n
 *  "IWPC", \n
 *   2002.
 *
 */
class StrengthMetric : public tlp::DoubleAlgorithm {
  public:
    PLUGININFORMATION("Strength", "David Auber", "26/02/2003",
                      "Computes the Strength metric described in<br/><b>Software component capture "
                      "using graph clustering</b>, Y. Chiricota. F.Jourdan, an G.Melancon, IWPC "
                      "(2002).",
                      "1.0", "Graph")
    StrengthMetric(const tlp::PluginContext *context);
    ~StrengthMetric() override;
    double getNodeValue(const tlp::node n);
    double getEdgeValue(const tlp::edge n);
    bool run() override;

  private:
    double e(const std::unordered_set<tlp::node> &U);
    double e(std::unordered_set<tlp::node> &U, std::unordered_set<tlp::node> &V);
    double s(std::unordered_set<tlp::node> &U, std::unordered_set<tlp::node> &V);
    double s(const std::unordered_set<tlp::node> &U);
};

#endif // STRENGTH_METRIC_H
