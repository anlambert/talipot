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

#include <talipot/PropertyAlgorithm.h>
#include <talipot/LayoutProperty.h>

static constexpr std::string_view paramHelp[] = {
    // layout
    "The layout property from which a perfect aspect ratio has to be computed.",

    // subgraph only
    "When applied on a subgraph, scales only the layout of this subgraph"};

class PerfectLayout : public tlp::LayoutAlgorithm {
  public:
    PLUGININFORMATION("Perfect aspect ratio", "Tulip team", "09/19/2010",
                      "Scales the graph layout to get an aspect ratio of 1.", "1.1", "")

    PerfectLayout(const tlp::PluginContext *context) : LayoutAlgorithm(context) {
        addInParameter<tlp::LayoutProperty>("layout", paramHelp[0].data(), "viewLayout", false);
        addInParameter<bool>("Subgraph only", paramHelp[1].data(), "false");
    }
    bool run() override {
        tlp::LayoutProperty *layout = nullptr;
        bool subgraphOnly(false);

        if (dataSet != nullptr) {
            dataSet->get("layout", layout);
            dataSet->get("Subgraph only", subgraphOnly);
        }

        if (!layout) {
            layout = graph->getLayoutProperty("viewLayout");
        }

        result->copy(layout);
        result->perfectAspectRatio(subgraphOnly ? graph : nullptr);
        return true;
    }
};

PLUGIN(PerfectLayout)
