/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "ReverseEdges.h"

#include <talipot/BooleanProperty.h>

using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // selection
    "Only edges selected in this property (or all edges if no property is given) will be "
    "reversed."};

ReverseEdges::ReverseEdges(tlp::PluginContext *context) : Algorithm(context) {
  addInParameter<BooleanProperty>("selection", paramHelp[0].data(), "viewSelection", false);
}

bool ReverseEdges::run() {
  BooleanProperty *selection = nullptr;

  if (dataSet) {
    dataSet->get<BooleanProperty *>("selection", selection);
  }

  Iterator<edge> *ite = selection ? selection->getEdgesEqualTo(true) : graph->getEdges();

  pluginProgress->showPreview(false);

  int step = 0, max_step = graph->numberOfEdges();
  for (auto e : ite) {
    if ((++step % 100) == 0) {
      ProgressState state = pluginProgress->progress(step, max_step);

      if (state != ProgressState::TLP_CONTINUE) {
        return state != ProgressState::TLP_CANCEL;
      }
    }

    graph->reverse(e);
  }

  return true;
}

PLUGIN(ReverseEdges)
