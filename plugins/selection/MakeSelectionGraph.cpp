/**
 *
 * Copyright (C) 2019-2020  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "MakeSelectionGraph.h"

#include <talipot/GraphTools.h>
using namespace tlp;

static const char *paramHelp[] = {
    // selection
    "The property indicating the selected elements"};

MakeSelectionGraph::MakeSelectionGraph(const tlp::PluginContext *context)
    : BooleanAlgorithm(context) {
  addInParameter<BooleanProperty>("selection", paramHelp[0], "viewSelection");
  addOutParameter<unsigned int>("#elements selected",
                                "The number of graph elements (nodes + edges) selected");
}
bool MakeSelectionGraph::run() {
  BooleanProperty *sel = graph->getBooleanProperty("viewSelection");

  if (dataSet != nullptr) {
    dataSet->get("selection", sel);
  }

  result->copy(sel);
  unsigned added = makeSelectionGraph(graph, result);

  // output some useful information
  if (dataSet != nullptr) {
    dataSet->set("#elements added to the selection", added);
  }

  return true;
}

isGraphTest::isGraphTest(const tlp::PluginContext *context) : tlp::GraphTest(context) {
  addInParameter<BooleanProperty>("selection", paramHelp[0], "viewSelection");
}

bool isGraphTest::test() {
  BooleanProperty *sel = graph->getBooleanProperty("viewSelection");

  if (dataSet != nullptr) {
    dataSet->get("selection", sel);
  }

  bool test;
  makeSelectionGraph(graph, sel, &test);
  return test;
}

PLUGIN(MakeSelectionGraph)
PLUGIN(isGraphTest)
