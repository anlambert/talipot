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

#include <ogdf/energybased/SpringEmbedderFRExact.h>

#include <talipot/OGDFLayoutPluginBase.h>
#include <talipot/StringCollection.h>

using namespace std;

#define ELT_COOLING "Cooling function"
#define ELT_COOLINGLIST "Factor;Logarithmic"
static const vector<ogdf::SpringEmbedderFRExact::CoolingFunction> coolingFunction = {
    ogdf::SpringEmbedderFRExact::CoolingFunction::Factor,
    ogdf::SpringEmbedderFRExact::CoolingFunction::Logarithmic,
};

static constexpr std::string_view paramHelp[] = {
    // iterations
    "The number of iterations.",

    // noise
    "Sets the parameter noise.",

    // use node weights
    "Indicates if the node weights have to be used.",

    // node weights
    "The metric containing node weights.",

    // Cooling function
    "Sets the parameter cooling function",

    // ideal edge length
    "The ideal edge length.",

    // minDistCC
    "The minimal distance between connected components.",

    // pageRatio
    "The page ratio used for packing connected components.",

    // check convergence
    "Indicates if the convergence has to be checked.",

    // convergence tolerance
    "The convergence tolerance parameter."};

class OGDFFrutchermanReingold : public tlp::OGDFLayoutPluginBase {

  public:
    PLUGININFORMATION(
        "Frutcherman Reingold (OGDF)", "Stephan Hachul", "15/11/2007",
        "Implements the Fruchterman and Reingold layout algorithm, first published "
        "as:<br/><b>Graph Drawing by Force-Directed Placement</b>, Fruchterman, Thomas "
        "M. J., Reingold, Edward M., Software – Practice & Experience (Wiley) Volume "
        "21, Issue 11, pages 1129–1164, (1991)",
        "1.1", "Force Directed")
    OGDFFrutchermanReingold(const tlp::PluginContext *context);
    void beforeCall() override;
};

PLUGIN(OGDFFrutchermanReingold)

OGDFFrutchermanReingold::OGDFFrutchermanReingold(const tlp::PluginContext *context)
    : OGDFLayoutPluginBase(context,
                           tlp::getOGDFLayoutModule<ogdf::SpringEmbedderFRExact>(context)) {
    addInParameter<int>("iterations", paramHelp[0].data(), "1000");
    addInParameter<bool>("noise", paramHelp[1].data(), "true");
    addInParameter<bool>("use node weights", paramHelp[2].data(), "false");
    addInParameter<tlp::NumericProperty *>("node weights", paramHelp[3].data(), "viewMetric");
    addInParameter<tlp::StringCollection>(ELT_COOLING, paramHelp[4].data(), ELT_COOLINGLIST, true,
                                          "<b>Factor</b> <br> <b>Logarithmic</b>");
    addInParameter<double>("ideal edge length", paramHelp[5].data(), "10.0");
    addInParameter<double>("minDistCC", paramHelp[6].data(), "20.0");
    addInParameter<double>("pageRatio", paramHelp[7].data(), "1.0");
    addInParameter<bool>("check convergence", paramHelp[8].data(), "true");
    addInParameter<double>("convergence tolerance", paramHelp[9].data(), "0.01");
}

void OGDFFrutchermanReingold::beforeCall() {
    auto *sefr = static_cast<ogdf::SpringEmbedderFRExact *>(ogdfLayoutAlgo);

    if (dataSet != nullptr) {
        int ival = 0;
        double dval = 0;
        bool bval = false;
        tlp::StringCollection sc;

        if (dataSet->get("iterations", ival)) {
            sefr->iterations(ival);
        }

        if (dataSet->get("noise", bval)) {
            sefr->noise(bval);
        }

        if (dataSet->get("minDistCC", dval)) {
            sefr->minDistCC(dval);
        }

        if (dataSet->get("pageRatio", dval)) {
            sefr->pageRatio(dval);
        }

        if (dataSet->get(ELT_COOLING, sc)) {
            sefr->coolingFunction(coolingFunction[sc.getCurrent()]);
        }

        if (dataSet->get("use node weights", bval)) {
            sefr->nodeWeights(bval);

            tlp::NumericProperty *metric = nullptr;

            if (bval && dataSet->get("node weights", metric)) {
                tlpToOGDF->copyTlpNumericPropertyToOGDFNodeWeight(metric);
            }
        }

        if (dataSet->get("check convergence", bval)) {
            sefr->checkConvergence(bval);
        }

        if (dataSet->get("convergence tolerance", dval)) {
            sefr->convTolerance(dval);
        }
    }
}
