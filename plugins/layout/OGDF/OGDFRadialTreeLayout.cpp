/**
 *
 * Copyright (C) 2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <ogdf/tree/RadialTreeLayout.h>

#include <talipot/OGDFLayoutPluginBase.h>
#include <talipot/ConnectedTest.h>
#include <talipot/StringCollection.h>
#include <talipot/TreeTest.h>

using namespace std;

#define ELT_ROOTSELECTION "root selection"
#define ELT_ROOTSELECTIONLIST "Center;Source;Sink"
static const vector<ogdf::RadialTreeLayout::RootSelectionType> rootSelectionType = {
    ogdf::RadialTreeLayout::RootSelectionType::Center,
    ogdf::RadialTreeLayout::RootSelectionType::Source,
    ogdf::RadialTreeLayout::RootSelectionType::Sink,
};

static constexpr string_view paramHelp[] = {
    // level distance
    "The minimal distance between levels",
    // root selection
    "Specifies how to determine the root"};

static const char *rootSelectionValuesDescription =
    "<b>Center</b> <i>(Select the center of the tree)</i>"
    "<b>Source</b> <i>(Select a source in the graph)</i><br>"
    "<b>Sink</b> <i>(Select a sink in the graph)</i><br>";

class OGDFRadialTreeLayout : public tlp::OGDFLayoutPluginBase {

public:
  PLUGININFORMATION(
      "Radial Tree (OGDF)", "Carsten Gutwenger, Mirko H. Wagner", "",
      "Linear time layout algorithm for free trees based on chapter 3.1.1 Radial Drawings "
      "of Graph Drawing by Di Battista, Eades, Tamassia, Tollis.",
      "1.0 ", "Hierarchical")
  OGDFRadialTreeLayout(const tlp::PluginContext *context)
      : OGDFLayoutPluginBase(context, tlp::getOGDFLayoutModule<ogdf::RadialTreeLayout>(context)),
        radialTree(static_cast<ogdf::RadialTreeLayout *>(ogdfLayoutAlgo)) {
    addInParameter<double>("level distance", paramHelp[0].data(), "30", false);
    addInParameter<tlp::StringCollection>(ELT_ROOTSELECTION, paramHelp[1].data(),
                                          ELT_ROOTSELECTIONLIST, true,
                                          rootSelectionValuesDescription);
  }

  bool check(string &errorMsg) {
    auto connectedComponents = tlp::ConnectedTest::computeConnectedComponents(graph);
    for (const auto &connectedComponent : connectedComponents) {
      auto *sg = graph->inducedSubGraph(connectedComponent);
      if (!tlp::TreeTest::isTree(sg)) {
        graph->delSubGraph(sg);
        errorMsg = "Each connected component must be a tree.";
        return false;
      }
      graph->delSubGraph(sg);
    }
    return true;
  }

  void beforeCall() override {

    if (dataSet != nullptr) {
      double val = 30;
      tlp::StringCollection stringCollection;

      if (dataSet->get("level distance", val)) {
        radialTree->levelDistance(val);
      }

      if (dataSet->get(ELT_ROOTSELECTION, stringCollection)) {
        radialTree->rootSelection(rootSelectionType[stringCollection.getCurrent()]);
      }
    }
  }

private:
  ogdf::RadialTreeLayout *radialTree;
};

PLUGIN(OGDFRadialTreeLayout)
