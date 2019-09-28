/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "LoopSelection.h"

PLUGIN(LoopSelection)

using namespace std;
using namespace tlp;

LoopSelection::LoopSelection(const tlp::PluginContext *context) : BooleanAlgorithm(context) {
  addOutParameter<unsigned int>("#edges selected", "The number of loops selected");
}
//============================================
bool LoopSelection::run() {
  result->setAllNodeValue(false);
  unsigned cpt = 0;
  for (auto e : graph->edges()) {
    auto eEnds = graph->ends(e);

    if (eEnds.first == eEnds.second) {
      result->setEdgeValue(e, true);
      ++cpt;
    }
  }

  // output some useful information
  if (dataSet != nullptr) {
    dataSet->set("#edges selected", cpt);
  }

  return true;
}
//============================================
