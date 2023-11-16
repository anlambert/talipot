/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "Random.h"

static constexpr std::string_view paramHelp[] = {
    // 3D layout
    "If true, the layout is computed in 3D, else it is computed in 2D."};

PLUGIN(Random)

using namespace std;
using namespace tlp;

Random::Random(const tlp::PluginContext *context) : LayoutAlgorithm(context) {
  addInParameter<bool>("3D layout", paramHelp[0].data(), "false");
}

bool Random::run() {
  bool is3D = false;

  if (dataSet != nullptr) {
    dataSet->get("3D layout", is3D);
  }

  result->setAllEdgeValue(vector<Coord>(0));
  // initialize a random sequence according the given seed
  tlp::initRandomSequence();

  for (auto n : graph->nodes()) {
    result->setNodeValue(
        n, Coord(randomNumber(1024), randomNumber(1024), is3D ? randomNumber(1024) : 0));
  }

  return true;
}
