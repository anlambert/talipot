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

class FdpLayout : public GraphvizLayoutBase {
public:
  PLUGININFORMATION(
      "fdp (Graphviz)", "Antoine Lambert", "04/2022",
      "spring model layouts similar to those of neato, but does this by reducing forces rather "
      "than working with energy.\n\nfdp implements the Fruchterman-Reingold heuristic1 including a "
      "multigrid solver that handles larger graphs and clustered undirected graphs.",
      "1.0", "Force Directed")
  FdpLayout(const PluginContext *context) : GraphvizLayoutBase(context, "fdp") {}
};

PLUGIN(FdpLayout)
