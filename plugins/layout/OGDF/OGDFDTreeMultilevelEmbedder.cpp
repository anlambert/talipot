/**
 *
 * Copyright (C) 2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <ogdf/energybased/dtree/DTreeForceTypes.h>
#include <ogdf/energybased/DTreeMultilevelEmbedder.h>

#include <talipot/OGDFLayoutPluginBase.h>

using namespace ogdf::energybased::dtree;

static constexpr std::string_view paramHelp[] = {
    // 3D layout
    "Indicates if a three-dimensional layout should be computed."};

class OGDFDTreeMultilevelEmbedder : public tlp::OGDFLayoutPluginBase {

public:
  PLUGININFORMATION("DTreeMultilevelEmbedder (OGDF)", "OGDF developers", "15/03/2022", "", "1.0",
                    "Force Directed")
  OGDFDTreeMultilevelEmbedder(const tlp::PluginContext *context)
      : OGDFLayoutPluginBase(context,
                             tlp::getOGDFLayoutModule<ogdf::DTreeMultilevelEmbedder2D>(context)) {
    addInParameter<bool>("3D layout", paramHelp[0].data(), "false");
  }

  void beforeCall() override {
    if (dataSet != nullptr) {
      bool val = false;

      if (dataSet->get("3D layout", val) && val) {
        delete ogdfLayoutAlgo;
        ogdfLayoutAlgo = new ogdf::DTreeMultilevelEmbedder3D();
      }
    }
  }
};

PLUGIN(OGDFDTreeMultilevelEmbedder)
