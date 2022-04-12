/**
 *
 * Copyright (C) 2022-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "GraphvizLayoutBase.h"

using namespace std;
using namespace tlp;

class DotLayout : public GraphvizLayoutBase {
public:
  PLUGININFORMATION("dot (Graphviz)", "Antoine Lambert", "04/2022",
                    "dot is a layout to use if edges have directionality.\n\nThe layout algorithm "
                    "aims edges in the same direction (top to bottom, or left to right) and then "
                    "attempts to avoid edge crossings and reduce edge length.",
                    "1.0", "Hierarchical")
  DotLayout(const PluginContext *context) : GraphvizLayoutBase(context, "dot") {}
};

PLUGIN(DotLayout)
