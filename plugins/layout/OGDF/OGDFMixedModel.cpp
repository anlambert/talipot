/**
 *
 * Copyright (C) 2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <ogdf/planarlayout/MixedModelLayout.h>
#include <ogdf/planarlayout/MMCBDoubleGrid.h>
#include <ogdf/planarlayout/MMCBLocalStretch.h>

#include <talipot/StringCollection.h>

#include "OGDFPlanarLayoutBase.h"

#define ELT_CROSSINGS_BEAUTIFIER "crossings beautifier"
#define ELT_CROSSINGS_BEAUTIFIER_LIST "MMDummyCrossingsBeautifier;MMCBDoubleGrid;MMCBLocalStretch"
static const std::vector<std::function<ogdf::MixedModelCrossingsBeautifierModule *()>>
    crossingsBeautifier = {
        []() { return new ogdf::MMDummyCrossingsBeautifier; },
        []() { return new ogdf::MMCBDoubleGrid; },
        []() { return new ogdf::MMCBLocalStretch; },
};
static const char *crossingsBeautifierValuesDescription =
    "<b>MMDummyCrossingsBeautifier</b> <i>(does no beautification at "
    "all)</i><br><b>MMCBDoubleGrid</b> <i>(crossings beautifier using grid "
    "doubling)</i><br><b>MMCBLocalStretch</b> <i>(crossings beautifier using a local stretch "
    "strategy)</i>";

static constexpr std::string_view paramHelp[] = {
    // crossings beautifiers
    "The crossings beautifier is applied as preprocessing to dummy nodes in the graph that "
    "actually represent crossings. By default, crossings might look weird, since they are not "
    "drawn as two crossing horizontal and vertical lines; the other available crossings beautifier "
    "correct this.",
};

class OGDFMixedModelLayout : public OGDFPlanarLayoutBase {

public:
  PLUGININFORMATION("Mixed Model (OGDF)", "", "", "", "1.0", "Planar")
  OGDFMixedModelLayout(const tlp::PluginContext *context)
      : OGDFPlanarLayoutBase(context, tlp::getOGDFLayoutModule<ogdf::MixedModelLayout>(context)),
        mixedModel(static_cast<ogdf::MixedModelLayout *>(ogdfLayoutAlgo)) {
    addInParameter<tlp::StringCollection>(ELT_CROSSINGS_BEAUTIFIER, paramHelp[0].data(),
                                          ELT_CROSSINGS_BEAUTIFIER_LIST, true,
                                          crossingsBeautifierValuesDescription);
  }

  void beforeCall() override {
    tlp::StringCollection sc;
    if (dataSet && dataSet->get(ELT_CROSSINGS_BEAUTIFIER, sc)) {
      mixedModel->setCrossingsBeautifier(crossingsBeautifier[sc.getCurrent()]());
    }
    tlpToOGDF->makeOGDFGraphSimple();
  }

private:
  ogdf::MixedModelLayout *mixedModel;
};

PLUGIN(OGDFMixedModelLayout)
