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

#include <ogdf/planarity/PlanarizationGridLayout.h>

#include <talipot/OGDFLayoutPluginBase.h>

static constexpr std::string_view paramHelp[] = {
    // page ratio
    "Sets the option pageRatio."};

class OGDFPlanarizationGrid : public tlp::OGDFLayoutPluginBase {

  public:
    PLUGININFORMATION(
        "Planarization Grid (OGDF)", "Carsten Gutwenger", "12/11/2007",
        "The planarization grid layout algorithm applies the planarization approach "
        "for crossing minimization, combined with the topology-shape-metrics approach "
        "for orthogonal planar graph drawing. It produces drawings with few crossings "
        "and is suited for small to medium sized sparse graphs. It uses a planar grid "
        "layout algorithm to produce a drawing on a grid.",
        "1.0", "Planar")
    OGDFPlanarizationGrid(const tlp::PluginContext *context)
        : OGDFLayoutPluginBase(context,
                               tlp::getOGDFLayoutModule<ogdf::PlanarizationGridLayout>(context)) {
        addInParameter<double>("page ratio", paramHelp[0].data(), "1.1");
    }

    void beforeCall() override {
        auto *pgl = static_cast<ogdf::PlanarizationGridLayout *>(ogdfLayoutAlgo);

        if (dataSet != nullptr) {
            double dval = 0;

            if (dataSet->get("page ratio", dval)) {
                pgl->pageRatio(dval);
            }
        }
    }
};

PLUGIN(OGDFPlanarizationGrid)
