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

#include <talipot/OGDFLayoutPluginBase.h>

#include <ogdf/energybased/FastMultipoleEmbedder.h>

static constexpr std::string_view paramHelp[] = {
    // number of iterations
    "The maximum number of iterations.",

    // number of coefficients
    "The number of coefficients for the expansions.",

    // randomize layout
    "If true, the initial layout will be randomized.",

    // default node size
    "The default node size.",

    // default edge length
    "The default edge length.",

    // number of threads
    "The number of threads to use during the computation of the layout."};

class OGDFFastMultipoleEmbedder : public tlp::OGDFLayoutPluginBase {

  public:
    PLUGININFORMATION(
        "Fast Multipole Embedder (OGDF)", "Martin Gronemann", "12/11/2007",
        "Implements the fast multipole embedder layout algorithm of Martin Gronemann. "
        "It uses the same repulsive forces as FM³ of Hachul and Jünger, but slightly "
        "modified attractive forces.",
        "1.0", "Force Directed")
    OGDFFastMultipoleEmbedder(const tlp::PluginContext *context)
        : OGDFLayoutPluginBase(context,
                               tlp::getOGDFLayoutModule<ogdf::FastMultipoleEmbedder>(context)),
          fme(static_cast<ogdf::FastMultipoleEmbedder *>(ogdfLayoutAlgo)) {
        addInParameter<int>("number of iterations", paramHelp[0].data(), "100");
        addInParameter<int>("number of coefficients", paramHelp[1].data(), "5");
        addInParameter<bool>("randomize layout", paramHelp[2].data(), "true");
        addInParameter<double>("default node size", paramHelp[3].data(), "20.0");
        addInParameter<double>("default edge length", paramHelp[4].data(), "1.0");
        addInParameter<int>("number of threads", paramHelp[5].data(), "3");
    }

    void beforeCall() override {

        if (dataSet != nullptr) {
            double dval = 0;
            int ival = 0;
            bool bval = 0;

            if (dataSet->get("number of iterations", ival)) {
                fme->setNumIterations(ival);
            }

            if (dataSet->get("number of coefficients", ival)) {
                fme->setMultipolePrec(ival);
            }

            if (dataSet->get("number of threads", ival)) {
                fme->setNumberOfThreads(ival);
            }

            if (dataSet->get("default node size", dval)) {
                fme->setDefaultNodeSize(dval);
            }

            if (dataSet->get("default edge length", dval)) {
                fme->setDefaultEdgeLength(dval);
            }

            if (dataSet->get("randomize layout", bval)) {
                fme->setRandomize(bval);
            }
        }

        // ensure the input graph is simple as the layout failed in non multi-threaded mode
        // otherwise
        tlpToOGDF->makeOGDFGraphSimple();
    }

  private:
    ogdf::FastMultipoleEmbedder *fme;
};

PLUGIN(OGDFFastMultipoleEmbedder)
