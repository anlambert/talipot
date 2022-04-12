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

class SfdpLayout : public GraphvizLayoutBase {
public:
  PLUGININFORMATION(
      "sfdp (Graphviz)", "Antoine Lambert", "04/2022",
      "sfdp is a fast, multilevel, force-directed algorithm that efficiently layouts large graphs, "
      "outlined in \"Efficient and High Quality Force-Dircted Graph Drawing\".\n\n Multiscale "
      "version of the fdp layout, for the layout of large graphs.",
      "1.0", "Multilevel")
  SfdpLayout(const PluginContext *context) : GraphvizLayoutBase(context, "sfdp") {}
};

PLUGIN(SfdpLayout)
