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

#include <ogdf/energybased/FastMultipoleEmbedder.h>

#include <talipot/OGDFLayoutPluginBase.h>

static constexpr std::string_view paramHelp[] = {
    // number of threads
    "The number of threads to use during the computation of the layout.",

    // multilevel nodes bound
    "The bound for the number of nodes in a multilevel step."};

class OGDFFastMultipoleMultiLevelEmbedder : public tlp::OGDFLayoutPluginBase {

  public:
    PLUGININFORMATION(
        "Fast Multipole Multilevel Embedder (OGDF)", "Martin Gronemann", "12/11/2007",
        "The FMME layout algorithm is a variant of multilevel, force-directed layout, "
        "which utilizes various tools to speed up the computation.",
        "1.1", "Multilevel")
    OGDFFastMultipoleMultiLevelEmbedder(const tlp::PluginContext *context)
        : OGDFLayoutPluginBase(
              context, tlp::getOGDFLayoutModule<ogdf::FastMultipoleMultilevelEmbedder>(context)),
          fmme(static_cast<ogdf::FastMultipoleMultilevelEmbedder *>(ogdfLayoutAlgo)) {
        addInParameter<int>("number of threads", paramHelp[0].data(), "2");
        addInParameter<int>("multilevel nodes bound", paramHelp[1].data(), "10");
    }

    void beforeCall() override {

        if (dataSet != nullptr) {
            int ival = 0;

            if (dataSet->get("number of threads", ival)) {
                fmme->maxNumThreads(ival);
            }

            if (dataSet->get("multilevel nodes bound", ival)) {
                fmme->multilevelUntilNumNodesAreLess(ival);
            }
        }

        // ensure the input graph is simple as the layout failed in non multi-threaded mode
        // otherwise
        tlpToOGDF->makeOGDFGraphSimple();
    }

  private:
    ogdf::FastMultipoleMultilevelEmbedder *fmme;
};

PLUGIN(OGDFFastMultipoleMultiLevelEmbedder)
