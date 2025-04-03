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

#include <ogdf/upward/DominanceLayout.h>

#include <talipot/OGDFLayoutPluginBase.h>

using namespace std;

static constexpr std::string_view paramHelp[] = {
    // minimum grid distance
    "The minimum grid distance.",

    // transpose
    "If true, transpose the layout vertically."};

class OGDFDominance : public tlp::OGDFLayoutPluginBase {

  public:
    PLUGININFORMATION(
        "Dominance (OGDF)", "Hoi-Ming Wong", "12/11/2007",
        "Implements a simple upward drawing algorithm based on dominance drawings of st-digraphs.",
        "1.0", "Hierarchical")
    OGDFDominance(const tlp::PluginContext *context)
        : OGDFLayoutPluginBase(context, tlp::getOGDFLayoutModule<ogdf::DominanceLayout>(context)) {
        addInParameter<int>("minimum grid distance", paramHelp[0].data(), "1");
        addInParameter<bool>("transpose", paramHelp[1].data(), "false");
    }

    void beforeCall() override {
        auto *dominance = static_cast<ogdf::DominanceLayout *>(ogdfLayoutAlgo);

        if (dataSet != nullptr) {
            int ival = 0;

            if (dataSet->get("minimum grid distance", ival)) {
                dominance->setMinGridDistance(ival);
            }
        }
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

PLUGIN(OGDFDominance)
