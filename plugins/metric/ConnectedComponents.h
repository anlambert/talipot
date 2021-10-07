/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef CONNECTED_COMPONENTS_H
#define CONNECTED_COMPONENTS_H

#include <talipot/DoubleProperty.h>
#include <talipot/PropertyAlgorithm.h>

/** This plugin is an implementation of the connected component decomposition algorithm. each
 *  node and edge that belongs to the same component receives the same value.
 *
 *  \note This algorithm assigns to each node a value defined as following : If two nodes are in the
 * same
 *  connected component they have the same value else they have a different value.
 *
 */
class ConnectedComponents : public tlp::DoubleAlgorithm {
public:
  PLUGININFORMATION("Connected Components", "David Auber", "01/07/2002",
                    "Implements a decomposition in connected components. \
                    This algorithm assigns to each node a value defined as following: if two nodes are in the same \
                    connected component they have the same value else they have a different value. Edges get the value of their source node.",
                    "1.0", "Component")
  ConnectedComponents(const tlp::PluginContext *context);
  bool run() override;
};

#endif // CONNECTED_COMPONENTS_H
