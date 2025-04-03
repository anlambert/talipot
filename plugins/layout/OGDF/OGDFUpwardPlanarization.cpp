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

#include <ogdf/upward/UpwardPlanarizationLayout.h>

#include <talipot/OGDFLayoutPluginBase.h>

static constexpr std::string_view paramHelp[] = {
    // transpose
    "If true, transpose the layout vertically."};

class OGDFUpwardPlanarization : public tlp::OGDFLayoutPluginBase {

  public:
    PLUGININFORMATION("Upward Planarization (OGDF)", "Hoi-Ming Wong", "12/11/2007",
                      "Implements an alternative to the classical Sugiyama approach. It adapts the "
                      "planarization approach for hierarchical graphs and produces significantly "
                      "less crossings than Sugiyama layout.",
                      "1.1", "Hierarchical")
    OGDFUpwardPlanarization(const tlp::PluginContext *context)
        : OGDFLayoutPluginBase(context,
                               tlp::getOGDFLayoutModule<ogdf::UpwardPlanarizationLayout>(context)) {
        addInParameter<bool>("transpose", paramHelp[0].data(), "false");
    }

    void afterCall() override {
        if (dataSet != nullptr) {
            bool bval = false;

            if (dataSet->get("transpose", bval)) {
                if (bval) {
                    transposeLayoutVertically();
                }
            }
        }
    }
};

PLUGIN(OGDFUpwardPlanarization)
