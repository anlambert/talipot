/**
 *
 * Copyright (C) 2019-2020  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef CONNECTED_COMPONENT_PACKING_H
#define CONNECTED_COMPONENT_PACKING_H

#include <talipot/PropertyAlgorithm.h>

class ConnectedComponentPacking : public tlp::LayoutAlgorithm {
public:
  PLUGININFORMATION("Connected Component Packing", "David Auber", "26/05/05",
                    "Implements a layout packing of the connected components of a graph. It builds "
                    "a layout of the graph connected components so that they do not overlap and "
                    "minimizes the lost space (packing).",
                    "1.0", "Misc")
  ConnectedComponentPacking(const tlp::PluginContext *context);
  bool run() override;
};

#endif // CONNECTED_COMPONENT_PACKING_H
