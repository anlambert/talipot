/**
 *
 * Copyright (C) 2022 The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef GRAPHVIZ_LAYOUT_BASE_H
#define GRAPHVIZ_LAYOUT_BASE_H

#include <talipot/PluginHeaders.h>

extern bool applyGraphvizLayout(tlp::Graph *graph, tlp::LayoutProperty *result,
                                const std::string &layoutName, tlp::PluginProgress *pluginProgress);

class GraphvizLayoutBase : public tlp::LayoutAlgorithm {
public:
  GraphvizLayoutBase(const tlp::PluginContext *context, const std::string &layoutName)
      : tlp::LayoutAlgorithm(context), _layoutName(layoutName) {}
  ~GraphvizLayoutBase() = default;

  bool run() override {
    return applyGraphvizLayout(graph, result, _layoutName, pluginProgress);
  }

protected:
  std::string _layoutName;
};

#endif // GRAPHVIZ_LAYOUT_BASE_H