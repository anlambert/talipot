/**
 *
 * Copyright (C) 2019-2020  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <ogdf/upward/VisibilityLayout.h>
#include <ogdf/packing/ComponentSplitterLayout.h>

#include <talipot/OGDFLayoutPluginBase.h>

static const char *paramHelp[] = {
    // minimum grid distance
    "The minimum grid distance.",

    // transpose
    "If true, transpose the layout vertically."};

class OGDFVisibility : public tlp::OGDFLayoutPluginBase {

public:
  PLUGININFORMATION("Visibility (OGDF)", "Hoi-Ming Wong", "12/11/2007",
                    "Implements a simple upward drawing algorithm based on visibility "
                    "representations (horizontal segments for nodes, vectical segments for edges).",
                    "1.1", "Hierarchical")
  OGDFVisibility(const tlp::PluginContext *context)
      : OGDFLayoutPluginBase(context, new ogdf::ComponentSplitterLayout()),
        visibility(new ogdf::VisibilityLayout()) {
    addInParameter<int>("minimum grid distance", paramHelp[0], "1");
    addInParameter<bool>("transpose", paramHelp[1], "false");
    ogdf::ComponentSplitterLayout *csl =
        static_cast<ogdf::ComponentSplitterLayout *>(ogdfLayoutAlgo);
    // ComponentSplitterLayout takes ownership of the VisibilityLayout instance
    csl->setLayoutModule(visibility);
  }

  void beforeCall() override {

    if (dataSet != nullptr) {
      int ival = 0;

      if (dataSet->get("minimum grid distance", ival))
        visibility->setMinGridDistance(ival);
    }
  }

  void afterCall() override {
    if (dataSet != nullptr) {
      bool bval = false;

      if (dataSet->get("transpose", bval)) {
        if (bval) {
          transposeLayoutVertically();
        }
      }
    }
  }

private:
  ogdf::VisibilityLayout *visibility;
};

PLUGIN(OGDFVisibility)
