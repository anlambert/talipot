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

#include <talipot/ImportModule.h>

using namespace tlp;

class EmptyGraphImport : public ImportModule {
  public:
    PLUGININFORMATION("Empty graph", "Tulip team", "05/09/2012",
                      "A no-op plugin to import empty graphs", "1.0", "")

    EmptyGraphImport(const tlp::PluginContext *context) : ImportModule(context) {}

    bool importGraph() override {
        return true;
    }
};
PLUGIN(EmptyGraphImport)
