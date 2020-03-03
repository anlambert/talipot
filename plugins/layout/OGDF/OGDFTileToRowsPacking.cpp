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

#include <ogdf/packing/ComponentSplitterLayout.h>

#include "talipot2ogdf/OGDFLayoutPluginBase.h"

class SameLayout : public ogdf::LayoutModule {

public:
  void call(GraphAttributes &GA) override {}
};

class OGDFTileToRowsPacking : public OGDFLayoutPluginBase {

public:
  PLUGININFORMATION("Tile To Rows Packing (OGDF)", "Carsten Gutwenger", "12/11/2007",
                    "The tile-to-rows algorithm for packing drawings of connected components.",
                    "1.0", "Misc")
  OGDFTileToRowsPacking(const tlp::PluginContext *context)
      : OGDFLayoutPluginBase(context, new ogdf::ComponentSplitterLayout()) {
    ogdf::ComponentSplitterLayout *csl =
        static_cast<ogdf::ComponentSplitterLayout *>(ogdfLayoutAlgo);
    // ComponentSplitterLayout takes ownership of the SameLayout instance
    csl->setLayoutModule(new SameLayout());
  }
};

PLUGIN(OGDFTileToRowsPacking)
