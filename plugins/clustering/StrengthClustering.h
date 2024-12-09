/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef STRENGTH_CLUSTERING_H
#define STRENGTH_CLUSTERING_H

#include <string>
#include <unordered_set>
#include <vector>
#include <talipot/hash.h>
#include <talipot/PluginHeaders.h>

/**
 * This plugin performs a single-linkage clustering.
 * The similarity measure used here is the Strength Metric computed on edges.
 * The best threshold is found using MQ Quality Measure.
 * See :
 *
 * Y. Chiricota, F. Jourdan and G. Melancon, G. \n
 * "Software components capture using graph clustering", \n
 * IEEE Computer Society, \n
 * 2003.
 *
 */
class StrengthClustering : public tlp::DoubleAlgorithm {
public:
  PLUGININFORMATION("Strength Clustering", "David Auber", "27/01/2003",
                    "Implements a single-linkage clustering. The similarity measure used here is "
                    "the Strength Metric computed on edges. The best threshold is found using MQ "
                    "Quality Measure. See :<br/>"
                    "<b>Software component capture using graph clustering</b>, Y. Chiricota. "
                    "F.Jourdan, an G.Melancon, IWPC (2002).",
                    "2.0", "Clustering")
  StrengthClustering(tlp::PluginContext *context);
  ~StrengthClustering() override;
  bool run() override;
  bool check(std::string &) override;

private:
  void computeNodePartition(double threshold, std::vector<std::unordered_set<tlp::node>> &);
  double computeMQValue(const std::vector<std::unordered_set<tlp::node>> &partition, tlp::Graph *);
  double findBestThreshold(int numberOfSteps, bool &stopped);
  tlp::DoubleProperty *values;
};

#endif // STRENGTH_CLUSTERING_H
