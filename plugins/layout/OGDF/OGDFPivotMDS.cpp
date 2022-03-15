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

#include <ogdf/energybased/PivotMDS.h>

#include <talipot/OGDFLayoutPluginBase.h>

static constexpr std::string_view paramHelp[] = {
    // number of pivots
    "Sets the number of pivots. If the new value is smaller or equal 0 the default value (250) is "
    "used.",

    // use edge costs
    "Sets if the edge costs attribute has to be used.",

    // edge costs
    "Sets the desired distance between adjacent nodes. If the new value is smaller or equal 0 the "
    "default value (100) is used.",

    // 3D layout
    "Indicates if a three-dimensional layout should be computed."};

class OGDFPivotMDS : public tlp::OGDFLayoutPluginBase {

public:
  PLUGININFORMATION("Pivot MDS (OGDF)", "Mark Ortmann", "29/05/2015",
                    "By setting the number of pivots to infinity this algorithm behaves just like "
                    "classical MDS. See Brandes and Pich: Eigensolver methods for progressive "
                    "multidimensional scaling of large data.",
                    "1.0", "Force Directed")
  OGDFPivotMDS(const tlp::PluginContext *context)
      : OGDFLayoutPluginBase(context, tlp::getOGDFLayoutModule<ogdf::PivotMDS>(context)),
        pivotMds(static_cast<ogdf::PivotMDS *>(ogdfLayoutAlgo)) {
    addInParameter<int>("number of pivots", paramHelp[0].data(), "250", false);
    addInParameter<bool>("use edge costs", paramHelp[1].data(), "false", false);
    addInParameter<double>("edge costs", paramHelp[2].data(), "100", false);
    addInParameter<bool>("3D layout", paramHelp[3].data(), "false", false);
  }

  void beforeCall() override {

    if (dataSet != nullptr) {
      bool bval = false;
      int ival = 0;
      double val = 0;

      if (dataSet->get("number of pivots", ival)) {
        pivotMds->setNumberOfPivots(ival);
      }

      if (dataSet->get("edge costs", val)) {
        pivotMds->setEdgeCosts(ival);
      }

      if (dataSet->get("use edge costs", bval)) {
        pivotMds->useEdgeCostsAttribute(bval);
      }

      if (dataSet->get("3D layout", bval)) {
        tlpToOGDF->enable3DLayout(bval);
      }
    }
  }

private:
  ogdf::PivotMDS *pivotMds;
};

PLUGIN(OGDFPivotMDS)
