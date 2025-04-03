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

#include <ogdf/misclayout/BertaultLayout.h>

#include <talipot/OGDFLayoutPluginBase.h>

static constexpr std::string_view paramHelp[] = {
    // impred
    "Sets impred option.",

    // iterno
    "The number of iterations. If 0, the number of iterations will be set as 10 times the number "
    "of nodes.",

    // reqlength
    "The required edge length."};

class OGDFBertaultLayout : public tlp::OGDFLayoutPluginBase {

  public:
    PLUGININFORMATION(
        "Bertault (OGDF)", "Smit Sanghavi", "29/05/2015",
        "Computes a force directed layout (Bertault Layout) for preserving the planar "
        "embedding in the graph.",
        "1.0", "Force Directed")
    OGDFBertaultLayout(const tlp::PluginContext *context)
        : OGDFLayoutPluginBase(context, tlp::getOGDFLayoutModule<ogdf::BertaultLayout>(context)) {
        addInParameter<bool>("impred", paramHelp[0].data(), "false", false);
        addInParameter<int>("iterno", paramHelp[1].data(), "20", false);
        addInParameter<double>("reqlength", paramHelp[2].data(), "0.0", false);
    }

    void beforeCall() override {
        auto *bertault = static_cast<ogdf::BertaultLayout *>(ogdfLayoutAlgo);

        if (dataSet != nullptr) {
            bool bval = false;
            int ival = 20;
            double val = 0;

            if (dataSet->get("impred", bval)) {
                bertault->setImpred(bval);
            }

            if (dataSet->get("iterno", ival)) {
                bertault->iterno(ival);
            }

            if (dataSet->get("reqlength", val)) {
                bertault->reqlength(val);
            }
        }
    }
};

PLUGIN(OGDFBertaultLayout)
