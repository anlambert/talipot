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

#include <ogdf/energybased/SpringEmbedderKK.h>

#include <talipot/OGDFLayoutPluginBase.h>

static constexpr std::string_view paramHelp[] = {
    // stop tolerance
    "The value for the stop tolerance, below which the system is regarded stable (balanced) and "
    "the optimization stopped.",

    // used layout
    "If set to true, the given layout is used for the initial positions.",

    // zero length
    "If set != 0, value zerolength is used to determine the desirable edge length by L = "
    "zerolength / max distance_ij. Otherwise, zerolength is determined using the node number and "
    "sizes.",

    // edge length
    "The desirable edge length.",

    // compute max iterations
    "If set to true, the number of iterations is computed depending on G.",

    // global iterations
    "The number of global iterations.",

    // local iterations
    "The number of local iterations."};

class OGDFKamadaKawai : public tlp::OGDFLayoutPluginBase {

  public:
    PLUGININFORMATION("Kamada Kawai (OGDF)", "Karsten Klein", "12/11/2007",
                      "Implements the Kamada-Kawai layout algorithm.<br/>It is a force-directed "
                      "layout algorithm that tries to place vertices with a distance corresponding "
                      "to their graph theoretic distance. ",
                      "1.0", "Force Directed")
    OGDFKamadaKawai(const tlp::PluginContext *context)
        : OGDFLayoutPluginBase(context, tlp::getOGDFLayoutModule<ogdf::SpringEmbedderKK>(context)) {
        addInParameter<double>("stop tolerance", paramHelp[0].data(), "0.001");
        addInParameter<bool>("used layout", paramHelp[1].data(), "true");
        addInParameter<double>("zero length", paramHelp[2].data(), "0");
        addInParameter<double>("edge length", paramHelp[3].data(), "0");
        addInParameter<bool>("compute max iterations", paramHelp[4].data(), "true");
        addInParameter<int>("global iterations", paramHelp[5].data(), "50");
        addInParameter<int>("local iterations", paramHelp[6].data(), "50");
    }

    void beforeCall() override {
        auto *kamada = static_cast<ogdf::SpringEmbedderKK *>(ogdfLayoutAlgo);

        if (dataSet != nullptr) {
            double dval = 0;
            int ival = 0;
            bool bval = false;

            if (dataSet->get("stop tolerance", dval)) {
                kamada->setStopTolerance(dval);
            }

            if (dataSet->get("use layout", bval)) {
                kamada->setUseLayout(bval);
            }

            if (dataSet->get("zero length", dval)) {
                kamada->setZeroLength(dval);
            }

            if (dataSet->get("edge length", dval)) {
                kamada->setDesLength(dval);
            }

            if (dataSet->get("compute max iterations", bval)) {
                kamada->computeMaxIterations(bval);
            }

            if (dataSet->get("global iterations", ival)) {
                kamada->setMaxGlobalIterations(ival);
            }

            if (dataSet->get("local iterations", ival)) {
                kamada->setMaxLocalIterations(ival);
            }
        }
    }
};

PLUGIN(OGDFKamadaKawai)
