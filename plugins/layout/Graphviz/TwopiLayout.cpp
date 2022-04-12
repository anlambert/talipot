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

class TwopiLayout : public GraphvizLayoutBase {
public:
  PLUGININFORMATION(
      "twopi (Graphviz)", "Antoine Lambert", "04/2022",
      "Nodes are placed on concentric circles depending their distance from a given root node.",
      "1.0", "Hierarchical")
  TwopiLayout(const PluginContext *context) : GraphvizLayoutBase(context, "twopi") {}
};

PLUGIN(TwopiLayout)
