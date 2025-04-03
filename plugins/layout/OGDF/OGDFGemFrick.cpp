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

#include <ogdf/energybased/GEMLayout.h>

#include <talipot/OGDFLayoutPluginBase.h>
#include <talipot/StringCollection.h>

#define ELT_ATTRACTIONFORMULA "Attraction formula"
#define ELT_ATTRACTIONFORMULALIST "Fruchterman/Reingold;GEM"

static constexpr std::string_view paramHelp[] = {
    // number of rounds
    "The maximal number of rounds per node.",

    // minimal temperature
    "The minimal temperature.",

    // initial temperature
    "The initial temperature to x; must be >= minimalTemperature.",

    // gravitational constant
    "Gravitational constant parameter.",

    // desired length
    "The desired edge length to x; must be >= 0.",

    // maximal disturbance
    "The maximal disturbance to x; must be >= 0.",

    // rotation angle
    "The opening angle for rotations to x (0 <= x <= pi / 2).",

    // oscillation angle
    "Sets the opening angle for oscillations to x (0 <= x <= pi / 2).",

    // rotation sensitivity
    "The rotation sensitivity to x (0 <= x <= 1).",

    // oscillation sensitivity
    "The oscillation sensitivity to x (0 <= x <= 1).",

    // Attraction formula
    "The formula for attraction.",

    // minDistCC
    "The minimal distance between connected components.",

    // pageRatio
    "The page ratio used for packing connected components."};

class OGDFGemFrick : public tlp::OGDFLayoutPluginBase {

  public:
    PLUGININFORMATION(
        "GEM Frick (OGDF)", "Christoph Buchheim", "15/11/2007",
        "Implements the GEM-2d layout algorithm first published as:<br/>"
        " <b>A fast, adaptive layout algorithm for undirected graphs</b>, A. Frick, A. "
        "Ludwig, and H. Mehldau, Graph Drawing'94, Volume 894 of Lecture Notes in "
        "Computer Science (1995).",
        "1.1", "Force Directed")
    OGDFGemFrick(const tlp::PluginContext *context);
    void beforeCall() override;
};

OGDFGemFrick::OGDFGemFrick(const tlp::PluginContext *context)
    : OGDFLayoutPluginBase(context, tlp::getOGDFLayoutModule<ogdf::GEMLayout>(context)) {
    addInParameter<int>("number of rounds", paramHelp[0].data(), "30000");
    addInParameter<double>("minimal temperature", paramHelp[1].data(), "0.005");
    addInParameter<double>("initial temperature", paramHelp[2].data(), "12.0");
    addInParameter<double>("gravitational constant", paramHelp[3].data(), "0.0625");
    addInParameter<double>("desired length", paramHelp[4].data(), "5.0");
    addInParameter<double>("maximal disturbance", paramHelp[5].data(), "0.0");
    addInParameter<double>("rotation angle", paramHelp[6].data(), "1.04719755");
    addInParameter<double>("oscillation angle", paramHelp[7].data(), "1.57079633");
    addInParameter<double>("rotation sensitivity", paramHelp[8].data(), "0.01");
    addInParameter<double>("oscillation sensitivity", paramHelp[9].data(), "0.3");
    addInParameter<tlp::StringCollection>(ELT_ATTRACTIONFORMULA, paramHelp[10].data(),
                                          ELT_ATTRACTIONFORMULALIST, true,
                                          "<b>Fruchterman/Reingold</b> <br> <b>GEM</b>");
    addInParameter<double>("minDistCC", paramHelp[11].data(), "20");
    addInParameter<double>("pageRatio", paramHelp[12].data(), "1.0");
}

void OGDFGemFrick::beforeCall() {
    auto *gem = static_cast<ogdf::GEMLayout *>(ogdfLayoutAlgo);

    if (dataSet != nullptr) {
        int ival = 0;
        double dval = 0;
        tlp::StringCollection sc;

        if (dataSet->get("number of rounds", ival)) {
            gem->numberOfRounds(ival);
        }

        if (dataSet->get("minimal temperature", dval)) {
            gem->minimalTemperature(dval);
        }

        if (dataSet->get("initial temperature", dval)) {
            gem->initialTemperature(dval);
        }

        if (dataSet->get("gravitational constant", dval)) {
            gem->gravitationalConstant(dval);
        }

        if (dataSet->get("desired length", dval)) {
            gem->desiredLength(dval);
        }

        if (dataSet->get("maximal disturbance", dval)) {
            gem->maximalDisturbance(dval);
        }

        if (dataSet->get("rotation angle", dval)) {
            gem->rotationAngle(dval);
        }

        if (dataSet->get("oscillation angle", dval)) {
            gem->oscillationAngle(dval);
        }

        if (dataSet->get("rotation sensitivity", dval)) {
            gem->rotationSensitivity(dval);
        }

        if (dataSet->get("oscillation sensitivity", dval)) {
            gem->oscillationSensitivity(dval);
        }

        if (dataSet->get(ELT_ATTRACTIONFORMULA, sc)) {
            gem->attractionFormula(sc.getCurrent() + 1);
        }

        if (dataSet->get("minDistCC", dval)) {
            gem->minDistCC(dval);
        }

        if (dataSet->get("pageRatio", dval)) {
            gem->pageRatio(dval);
        }
    }
}

PLUGIN(OGDFGemFrick)
