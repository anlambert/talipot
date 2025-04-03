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

#include <talipot/PluginHeaders.h>

using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // input
    "Property to stringify values on labels.",

    // selection
    "Set of elements for which to set the labels.",

    // nodes
    "Sets labels on nodes.",

    // edges
    "Set labels on edges."};

class ToLabels : public tlp::StringAlgorithm {
  public:
    PLUGININFORMATION("To labels", "Ludwig Fiolka", "2012/03/16",
                      "Maps the labels of the graph elements onto the values of a given property.",
                      "1.0", "")
    ToLabels(const tlp::PluginContext *context) : StringAlgorithm(context) {
        addInParameter<PropertyInterface *>("input", paramHelp[0].data(), "viewMetric", true);
        addInParameter<BooleanProperty>("selection", paramHelp[1].data(), "", false);
        addInParameter<bool>("nodes", paramHelp[2].data(), "true");
        addInParameter<bool>("edges", paramHelp[3].data(), "true");
    }

    bool run() override {
        PropertyInterface *input = nullptr;
        BooleanProperty *selection = nullptr;
        bool onNodes = true;
        bool onEdges = true;

        if (dataSet != nullptr) {
            dataSet->get("input", input);
            dataSet->get("selection", selection);
            dataSet->get("nodes", onNodes);
            dataSet->get("edges", onEdges);
        }

        pluginProgress->showPreview(false);

        if (onNodes) {
            pluginProgress->setComment("Copying nodes values");
            int step = 0, max_step = graph->numberOfNodes();
            for (auto n : selection ? selection->getNonDefaultValuatedNodes() : graph->getNodes()) {
                if ((++step % 100) == 0) {
                    pluginProgress->progress(step, max_step);
                }

                result->setNodeValue(n, input->getNodeStringValue(n));
            }
        }

        if (onEdges) {
            pluginProgress->setComment("Copying edges values");
            int step = 0, max_step = graph->numberOfEdges();
            for (auto e : selection ? selection->getNonDefaultValuatedEdges() : graph->getEdges()) {
                if ((++step % 100) == 0) {
                    pluginProgress->progress(step, max_step);
                }

                result->setEdgeValue(e, input->getEdgeStringValue(e));
            }
        }

        return true;
    }
};

PLUGIN(ToLabels)
