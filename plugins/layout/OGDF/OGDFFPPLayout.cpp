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

#include <ogdf/planarlayout/FPPLayout.h>

#include "OGDFPlanarLayoutBase.h"

// static constexpr std::string_view paramHelp[] = {};

class OGDFFPPLayout : public OGDFPlanarLayoutBase {

public:
  PLUGININFORMATION("FPP (OGDF)", "", "", "", "1.0", "Planar")
  OGDFFPPLayout(const tlp::PluginContext *context)
      : OGDFPlanarLayoutBase(context, tlp::getOGDFLayoutModule<ogdf::FPPLayout>(context)),
        fppLayout(static_cast<ogdf::FPPLayout *>(ogdfLayoutAlgo)) {}

  void beforeCall() override {}

private:
  ogdf::FPPLayout *fppLayout;
};

PLUGIN(OGDFFPPLayout)
