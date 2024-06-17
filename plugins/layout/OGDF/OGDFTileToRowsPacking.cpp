/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <ogdf/basic/LayoutModule.h>

#include <talipot/OGDFLayoutPluginBase.h>

class SameLayout : public ogdf::LayoutModule {

public:
  void call(ogdf::GraphAttributes &GA) override {}
};

class OGDFTileToRowsPacking : public tlp::OGDFLayoutPluginBase {

public:
  PLUGININFORMATION("Tile To Rows Packing (OGDF)", "Carsten Gutwenger", "12/11/2007",
                    "The tile-to-rows algorithm for packing drawings of connected components.",
                    "1.0", "Misc")
  OGDFTileToRowsPacking(const tlp::PluginContext *context)
      // OGDFLayoutPluginBase wraps call to ogdf::ComponentSplitterLayout
      : OGDFLayoutPluginBase(context, tlp::getOGDFLayoutModule<SameLayout>(context), true) {}
};

PLUGIN(OGDFTileToRowsPacking)
