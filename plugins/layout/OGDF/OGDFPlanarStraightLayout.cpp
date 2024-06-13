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

#include <ogdf/planarlayout/PlanarStraightLayout.h>

#include "OGDFPlanarLayoutBase.h"

// static constexpr std::string_view paramHelp[] = {};

class OGDFPlanarStraightLayout : public OGDFPlanarLayoutBase {

public:
  PLUGININFORMATION("Planar Straight (OGDF)", "", "", "", "1.0", "Planar")
  OGDFPlanarStraightLayout(const tlp::PluginContext *context)
      : OGDFPlanarLayoutBase(context,
                             tlp::getOGDFLayoutModule<ogdf::PlanarStraightLayout>(context)),
        planarStraightLayout(static_cast<ogdf::PlanarStraightLayout *>(ogdfLayoutAlgo)) {}

  void beforeCall() override {}

private:
  ogdf::PlanarStraightLayout *planarStraightLayout;
};

PLUGIN(OGDFPlanarStraightLayout)
