/**
 *
 * Copyright (C) 2024-2025  The Talipot developers
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

#include <talipot/OGDFUtils.h>

class OGDFImportBase : public tlp::ImportModule {

public:
  OGDFImportBase(tlp::PluginContext *context) : tlp::ImportModule(context) {}
  virtual bool importOGDFGraph() = 0;

  bool importGraph() override {
    if (importOGDFGraph()) {
      convertOGDFGraphToTalipotGraph(G, graph);
      return true;
    }
    return false;
  }

protected:
  ogdf::Graph G;
};
