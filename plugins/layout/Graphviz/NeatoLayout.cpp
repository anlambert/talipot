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

class NeatoLayout : public GraphvizLayoutBase {
public:
  PLUGININFORMATION("neato (Graphviz)", "Antoine Lambert", "04/2022",
                    "neato is a reasonable default tool to use for undirected graphs that aren't "
                    "too large (about 100 nodes), when you don't know anything else about the "
                    "graph.\n\nneato attempts to minimize a global energy function, which is "
                    "equivalent to statistical multi-dimensional scaling. The solution is achieved "
                    "using stress majorization1, though the older Kamada-Kawai algorithm",
                    "1.0", "Force Directed")
  NeatoLayout(const PluginContext *context) : GraphvizLayoutBase(context, "neato") {}
  bool check(std::string &err) override {
    if (graph->numberOfNodes() > 100) {
      err = "The graph is too large (more than 100 nodes) for that layout algorithm.";
      return false;
    }
    return true;
  }
};

PLUGIN(NeatoLayout)
