/**
 *
 * Copyright (C) 2024-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <ogdf/basic/graph_generators/deterministic.h>

#include "OGDFImportBase.h"

using namespace std;

static constexpr string_view paramHelp[] = {
    // meridians
    "the number of meridians",

    // latitudes
    "the number of latitudes",
};

//=================================================================================

class OGDFGlobeGraph : public OGDFImportBase {
public:
  PLUGININFORMATION("Globe Graph (OGDF)", "Antoine Lambert", "03/2024",
                    "Creates a globe graph with a given number of meridians and latitudes. "
                    "The graph will contain a node at each crossing of a meridian and a latitude, "
                    "and a node at each pole.",
                    "1.0", "OGDF")

  OGDFGlobeGraph(tlp::PluginContext *context) : OGDFImportBase(context) {
    addInParameter<int>("meridians", paramHelp[0].data(), "30");
    addInParameter<int>("latitudes", paramHelp[1].data(), "30");
  }

  bool importOGDFGraph() override {
    int meridians = 30;
    int latitudes = 30;
    if (dataSet != nullptr) {
      dataSet->get("meridians", meridians);
      dataSet->get("latitudes", latitudes);
    }
    ogdf::globeGraph(*G, meridians, latitudes);
    return true;
  }
};

PLUGIN(OGDFGlobeGraph)
