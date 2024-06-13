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

#ifndef OGDF_PLANAR_LAYOUT_BASE_H
#define OGDF_PLANAR_LAYOUT_BASE_H

#include <talipot/OGDFLayoutPluginBase.h>

#include <talipot/ConnectedTest.h>
#include <talipot/PlanarityTest.h>

class OGDFPlanarLayoutBase : public tlp::OGDFLayoutPluginBase {

public:
  OGDFPlanarLayoutBase(const tlp::PluginContext *context, ogdf::LayoutModule *ogdfLayoutAlgo)
      : OGDFLayoutPluginBase(context, ogdfLayoutAlgo) {}

  bool check(std::string &errorMsg) override {
    auto connectedComponents = tlp::ConnectedTest::computeConnectedComponents(graph);
    for (const auto &connectedComponent : connectedComponents) {
      auto *sg = graph->inducedSubGraph(connectedComponent);
      if (!tlp::PlanarityTest::isPlanar(sg)) {
        graph->delSubGraph(sg);
        errorMsg = "Each connected component must be planar.";
        return false;
      }
      graph->delSubGraph(sg);
    }
    return true;
  }
};

#endif // OGDF_PLANAR_LAYOUT_BASE_H