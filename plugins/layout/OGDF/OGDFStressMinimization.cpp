/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <ogdf/energybased/StressMinimization.h>

#include <talipot/OGDFLayoutPluginBase.h>
#include <talipot/StringCollection.h>
#include <talipot/DoubleProperty.h>

static constexpr std::string_view paramHelp[] = {
    // terminationCriterion
    "Tells which TERMINATION_CRITERIA should be used.",

    // fixXCoordinates
    "Tells whether the x coordinates are allowed to be modified or not.",

    // fixYCoordinates
    "Tells whether the y coordinates are allowed to be modified or not.",

    // hasInitialLayout
    "Tells whether the current layout should be used or the initial layout needs to be computed.",

    // layoutComponentsSeparately
    "Sets whether the graph components should be layouted separately or a dummy distance should be "
    "used for nodes within different components.",

    // numberOfIterations
    "Sets a fixed number of iterations for stress minimization. If the new value is smaller or "
    "equal 0 the default value (200) is used.",

    // edgeCosts
    "Sets the desired distance between adjacent nodes. If the new value is smaller or equal 0 the "
    "default value (100) is used.",

    // useEdgeCostsProperty
    "Tells whether the edge costs are uniform or defined in an edge costs property.",

    // edgeCostsProperty
    "The numeric property that holds the desired cost for each edge.",

    // 3D layout
    "Indicates if a three-dimensional layout should be computed."

};

class OGDFStressMinimization : public tlp::OGDFLayoutPluginBase {

public:
  PLUGININFORMATION("Stress Minimization (OGDF)", "Karsten Klein", "12/11/2007",
                    "Implements an alternative to force-directed layout which is a distance-based "
                    "layout realized by the stress minimization approach. ",
                    "2.0", "Force Directed")
  OGDFStressMinimization(const tlp::PluginContext *context)
      : OGDFLayoutPluginBase(context, tlp::getOGDFLayoutModule<ogdf::StressMinimization>(context)) {
    addInParameter<tlp::StringCollection>(
        "terminationCriterion", paramHelp[0].data(), "None;PositionDifference;Stress", true,
        "<b>None</b> <br> <b>PositionDifference</b> <br> <b>Stress</b>");
    addInParameter<bool>("fixXCoordinates", paramHelp[1].data(), "false");
    addInParameter<bool>("fixYCoordinates", paramHelp[2].data(), "false");
    addInParameter<bool>("hasInitialLayout", paramHelp[3].data(), "false");
    addInParameter<bool>("layoutComponentsSeparately", paramHelp[4].data(), "false");
    addInParameter<int>("numberOfIterations", paramHelp[5].data(), "200");
    addInParameter<double>("edgeCosts", paramHelp[6].data(), "100");
    addInParameter<bool>("useEdgeCostsProperty", paramHelp[7].data(), "false");
    addInParameter<tlp::NumericProperty *>("edgeCostsProperty", paramHelp[8].data(), "viewMetric");
    addInParameter<bool>("3D layout", paramHelp[5].data(), "false");
  }

  void beforeCall() override {
    auto *stressm = static_cast<ogdf::StressMinimization *>(ogdfLayoutAlgo);

    if (dataSet != nullptr) {
      double dval = 0;
      int ival = 0;
      bool bval = false;
      tlp::StringCollection sc;
      tlp::NumericProperty *edgeCosts = graph->getDoubleProperty("viewMetric");

      if (dataSet->get("terminationCriterion", sc)) {
        if (sc.getCurrentString() == "PositionDifference") {
          stressm->convergenceCriterion(
              ogdf::StressMinimization::TerminationCriterion::PositionDifference);
        } else if (sc.getCurrentString() == "Stress") {
          stressm->convergenceCriterion(ogdf::StressMinimization::TerminationCriterion::Stress);
        } else {
          stressm->convergenceCriterion(ogdf::StressMinimization::TerminationCriterion::None);
        }
      }

      if (dataSet->get("fixXCoordinates", bval)) {
        stressm->fixXCoordinates(bval);
      }

      if (dataSet->get("fixYCoordinates", bval)) {
        stressm->fixXCoordinates(bval);
      }

      if (dataSet->get("hasInitialLayout", bval)) {
        stressm->hasInitialLayout(bval);
      }

      if (dataSet->get("layoutComponentsSeparately", bval)) {
        stressm->layoutComponentsSeparately(bval);
      }

      if (dataSet->get("numberOfIterations", ival)) {
        stressm->setIterations(ival);
      }

      if (dataSet->get("edgeCosts", dval)) {
        stressm->setEdgeCosts(dval);
      }

      if (dataSet->get("useEdgeCostsProperty", bval)) {
        stressm->useEdgeCostsAttribute(bval);

        if (bval) {
          dataSet->get("edgeCostsProperty", edgeCosts);
          tlpToOGDF->copyTlpNumericPropertyToOGDFEdgeLength(edgeCosts);
        }
      }

      if (dataSet->get("3D layout", bval)) {
        tlpToOGDF->enable3DLayout(bval);
      }
    }
  }
};

PLUGIN(OGDFStressMinimization)
