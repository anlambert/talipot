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

#include <ogdf/planarity/PlanarizationLayout.h>
#include <ogdf/planarity/EmbedderMaxFaceLayers.h>
#include <ogdf/planarity/EmbedderMinDepth.h>
#include <ogdf/planarity/EmbedderMinDepthMaxFaceLayers.h>
#include <ogdf/planarity/EmbedderMinDepthPiTa.h>
#include <ogdf/planarity/SimpleEmbedder.h>

#include <talipot/OGDFLayoutPluginBase.h>
#include <talipot/StringCollection.h>

using namespace std;

#define ELT_EMBEDDER "Embedder"
#define ELT_EMBEDDER_LIST                                                    \
    "SimpleEmbedder;EmbedderMaxFace;EmbedderMaxFaceLayers;EmbedderMinDepth;" \
    "EmbedderMinDepthMaxFace;EmbedderMinDepthMaxFaceLayers;EmbedderMinDepthPiTa"
static const vector<function<ogdf::EmbedderModule *()>> embedder = {
    []() { return new ogdf::SimpleEmbedder; },
    []() { return new ogdf::EmbedderMaxFace; },
    []() { return new ogdf::EmbedderMaxFaceLayers; },
    []() { return new ogdf::EmbedderMinDepth; },
    []() { return new ogdf::EmbedderMinDepthMaxFace; },
    []() { return new ogdf::EmbedderMinDepthMaxFaceLayers; },
    []() { return new ogdf::EmbedderMinDepthPiTa; },
};

static const char *embedderValuesDescription =
    "<b>SimpleEmbedder</b> <i>(Planar graph embedding from the algorithm of Boyer and "
    "Myrvold)</i><br>"
    "<b>EmbedderMaxFace</b> <i>(Planar graph embedding with maximum external face)</i><br>"
    "<b>EmbedderMaxFaceLayers</b> <i>(Planar graph embedding with maximum external face, plus "
    "layers approach)</i><br>"
    "<b>EmbedderMinDepth</b> <i>(Planar graph embedding with minimum block-nesting depth)</i><br>"
    "<b>EmbedderMinDepthMaxFace</b> <i>(Planar graph embedding with minimum block-nesting depth "
    "and maximum external face)</i><br>"
    "<b>EmbedderMinDepthMaxFaceLayers</b> <i>(Planar graph embedding with minimum block-nesting "
    "depth and maximum external face, plus layers approach)</i><br>"
    "<b>EmbedderMinDepthPiTa</b> <i>(Planar graph embedding with minimum block-nesting depth for "
    "given embedded blocks)</i>";

static constexpr std::string_view paramHelp[] = {
    // page ratio
    "Sets the option page ratio.",

    // Embedder
    "The result of the crossing minimization step is a planar graph, in which crossings are "
    "replaced by dummy nodes. The embedder then computes a planar embedding of this planar graph."};

class OGDFPlanarizationLayout : public tlp::OGDFLayoutPluginBase {

  public:
    PLUGININFORMATION("Planarization Layout (OGDF)", "Carsten Gutwenger", "12/11/2007",
                      "The planarization approach for drawing graphs.", "1.0", "Planar")
    OGDFPlanarizationLayout(const tlp::PluginContext *context)
        : OGDFLayoutPluginBase(context,
                               tlp::getOGDFLayoutModule<ogdf::PlanarizationLayout>(context)) {
        addInParameter<double>("page ratio", paramHelp[0].data(), "1.1");
        addInParameter<tlp::StringCollection>(ELT_EMBEDDER, paramHelp[1].data(), ELT_EMBEDDER_LIST,
                                              true, embedderValuesDescription);
    }

    void beforeCall() override {
        auto *pl = static_cast<ogdf::PlanarizationLayout *>(ogdfLayoutAlgo);

        if (dataSet != nullptr) {
            double dval = 0;
            tlp::StringCollection sc;

            if (dataSet->get("page ratio", dval)) {
                pl->pageRatio(dval);
            }

            if (dataSet->get(ELT_EMBEDDER, sc)) {
                pl->setEmbedder(embedder[sc.getCurrent()]());
            }
        }
    }
};

PLUGIN(OGDFPlanarizationLayout)
