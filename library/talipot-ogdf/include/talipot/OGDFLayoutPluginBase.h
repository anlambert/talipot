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

#ifndef TALIPOT_OGDF_LAYOUT_PLUGIN_BASE_H
#define TALIPOT_OGDF_LAYOUT_PLUGIN_BASE_H

#include <talipot/config.h>
#include <talipot/PropertyAlgorithm.h>
#include <talipot/TalipotToOGDF.h>

namespace ogdf {
class LayoutModule;
class GraphAttributes;
class SimpleCCPacker;
}

namespace tlp {

template <class OGDFLayoutModule>
static inline OGDFLayoutModule *getOGDFLayoutModule(const PluginContext *context) {
  return context ? new OGDFLayoutModule() : nullptr;
}

class TLP_OGDF_SCOPE OGDFLayoutPluginBase : public LayoutAlgorithm {
public:
  OGDFLayoutPluginBase(const PluginContext *context, ogdf::LayoutModule *ogdfLayoutAlgo);
  ~OGDFLayoutPluginBase() override;

  bool run() override;

protected:
  virtual void beforeCall() {}
  virtual void callOGDFLayoutAlgorithm(ogdf::GraphAttributes &gAttributes);
  virtual void afterCall() {}

  void transposeLayoutVertically();

  TalipotToOGDF *tlpToOGDF;
  ogdf::LayoutModule *ogdfLayoutAlgo;
  ogdf::SimpleCCPacker *simpleCCPacker;
};
}
#endif // TALIPOT_OGDF_LAYOUT_PLUGIN_BASE_H