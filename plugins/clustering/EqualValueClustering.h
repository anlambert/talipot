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

#ifndef EQUAL_VALUE_CLUSTERING_H
#define EQUAL_VALUE_CLUSTERING_H

#include <talipot/PluginHeaders.h>
#include <talipot/NumericProperty.h>

class EqualValueClustering : public tlp::Algorithm {
  public:
    PLUGININFORMATION("Equal Value", "Patrick Mary", "16/04/2014",
                      "Performs a graph clusterization grouping in the same cluster the nodes or "
                      "edges having the same value for a given property.",
                      "1.1", "Clustering")
    EqualValueClustering(tlp::PluginContext *context);
    bool run() override;
    bool computeClusters(tlp::NumericProperty *prop, bool onNodes, bool connected);
    bool computeClusters(tlp::PropertyInterface *prop, bool onNodes, bool connected);
};

#endif // EQUAL_VALUE_CLUSTERING_H
