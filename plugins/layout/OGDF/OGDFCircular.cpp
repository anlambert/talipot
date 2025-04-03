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

#include <ogdf/misclayout/CircularLayout.h>

#include <talipot/OGDFLayoutPluginBase.h>

static constexpr std::string_view paramHelp[] = {
    // minDistCircle
    "The minimal distance between nodes on a circle.",

    // minDistLevel
    "The minimal distance between father and child circle.",

    // minDistSibling
    "The minimal distance between circles on same level.",

    // minDistCC
    "The minimal distance between connected components.",

    // pageRatio
    "The page ratio used for packing connected components."};

class OGDFCircular : public tlp::OGDFLayoutPluginBase {

  public:
    PLUGININFORMATION("Circular (OGDF)", "Carsten Gutwenger", "13/11/2007",
                      "Implements a circular layout.", "1.4", "Basic")
    OGDFCircular(const tlp::PluginContext *context)
        : OGDFLayoutPluginBase(context, tlp::getOGDFLayoutModule<ogdf::CircularLayout>(context)) {
        addInParameter<double>("minDistCircle", paramHelp[0].data(), "20.0", false);
        addInParameter<double>("minDistLevel", paramHelp[1].data(), "20.0", false);
        addInParameter<double>("minDistSibling", paramHelp[2].data(), "10.0", false);
        addInParameter<double>("minDistCC", paramHelp[3].data(), "20.0", false);
        addInParameter<double>("pageRatio", paramHelp[4].data(), "1.0", false);
    }

    void beforeCall() override {
        auto *circular = static_cast<ogdf::CircularLayout *>(ogdfLayoutAlgo);

        if (dataSet != nullptr) {
            double val = 0;

            if (dataSet->get("minDistCircle", val)) {
                circular->minDistCircle(val);
            }

            if (dataSet->get("minDistLevel", val)) {
                circular->minDistLevel(val);
            }

            if (dataSet->get("minDistSibling", val)) {
                circular->minDistSibling(val);
            }

            if (dataSet->get("minDistCC", val)) {
                circular->minDistCC(val);
            }

            if (dataSet->get("pageRatio", val)) {
                circular->pageRatio(val);
            }
        }

        // graph must be simple or the layout algorithm crashes otherwise
        tlpToOGDF->makeOGDFGraphSimple();
    }
};

PLUGIN(OGDFCircular)
