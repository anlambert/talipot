/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef ECCENTRICITY_H
#define ECCENTRICITY_H

#include <talipot/DoubleProperty.h>
#include <talipot/NumericProperty.h>

/** This plugin compute the eccentricity/closeness centrality of each node
 *
 * Eccentricity is the maximum distance to go from a node to all others.
 * In this version the Eccentricity value can be normalized
 * (1 means that a node is one of the most eccentric in the network,
 * 0 means that a node is on the centers of the network).
 *
 * More information  about the use of eccentricity metric can be found in :
 *
 * Visone: Analysis and visualization of social networks. \n
 * "Book. Graph Drawing Software. (Ed. Michael Junger & Petra Mutzel", \n
 * Authors : Ulrik Brandes and Dorothea Wagner. \n
 * "2004", \n
 * pages 321-340.
 *
 *
 * Closeness Centrality is the mean of shortest-paths lengths from a node to others.
 * The normalized values are computed using the reciprocal of the sum of these distances
 * (see "http://en.wikipedia.org/wiki/Closeness_(graph_theory)#Closeness_centrality" for more
 * details).
 *
 *  \note The complexity of the algorithm is O(|V| * |E|) time and O(1) space
 *        for unweighted graphs and O(|V| * |E| \log |V|) time for weighted graphs.
 *
 *   <b>HISTORY</b>
 *
 *   - 18/06/2004 Version 2.0: Normalisation and Closeness Centrality
 *   - 27/04/2019 Version 2.1: Weighted version
 */
class EccentricityMetric : public tlp::DoubleAlgorithm {
public:
  PLUGININFORMATION("Eccentricity", "Auber/Munzner", "18/06/2004",
                    "Computes the eccentricity/closeness centrality of each node.<br>"
                    "<b>Eccentricity</b> is the maximum distance to go from a node to all others. "
                    "In this version the Eccentricity value can be normalized (1 means that a node "
                    "is one of the most eccentric in the network, 0 means that a node is on the "
                    "centers of the network).<br>"
                    "<b>Closeness Centrality</b> is the mean of shortest-paths lengths from a node "
                    "to others. The normalized values are computed using the reciprocal of the sum "
                    "of these distances.",
                    "2.2", "Graph")
  EccentricityMetric(const tlp::PluginContext *context);
  ~EccentricityMetric() override;
  bool run() override;
  double compute(tlp::node n, tlp::NodeVectorProperty<double> &maxDistance);

private:
  bool allPaths;
  bool norm;
  bool directed;
  tlp::NumericProperty *weight;
};

#endif // ECCENTRICITY_H
