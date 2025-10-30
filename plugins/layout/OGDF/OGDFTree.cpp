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

#include <ogdf/tree/TreeLayout.h>

#include <talipot/OGDFLayoutPluginBase.h>
#include <talipot/StringCollection.h>
#include <talipot/TreeTest.h>
#include <talipot/ConnectedTest.h>

using namespace std;

#define ELT_ORIENTATION "Orientation"
#define ELT_ORIENTATIONLIST "topToBottom;bottomToTop;leftToRight;rightToLeft"
static const vector<ogdf::Orientation> orientation = {
    ogdf::Orientation::topToBottom,
    ogdf::Orientation::bottomToTop,
    ogdf::Orientation::leftToRight,
    ogdf::Orientation::rightToLeft,
};

#define ELT_ROOTSELECTION "Root selection"
#define ELT_ROOTSELECTIONLIST "rootIsSource;rootByCoord"
static const vector<ogdf::TreeLayout::RootSelectionType> rootSelection = {
    ogdf::TreeLayout::RootSelectionType::Source,
    ogdf::TreeLayout::RootSelectionType::ByCoord,
};

static constexpr std::string_view paramHelp[] = {
    // siblings distance
    "The minimal required horizontal distance between siblings.",

    // subtrees distance
    "The minimal required horizontal distance between subtrees.",

    // levels distance
    "The minimal required vertical distance between levels.",

    // trees distance
    "The minimal required horizontal distance between trees in the forest.",

    // orthogonal layout
    "Indicates whether orthogonal edge routing style is used or not.",

    // Orientation
    "This parameter indicates the orientation of the layout.",

    // Root selection
    "This parameter indicates how the root is selected."};

static const char *orientationValuesDescription =
    "<b>topToBottom</b> <i>(Edges are oriented from top to bottom)</i><br>"
    "<b>bottomToTop</b> <i>(Edges are oriented from bottom to top)</i><br>"
    "<b>leftToRight</b> <i>(Edges are oriented from left to right)</i><br>"
    "<b>rightToLeft</b> <i>(Edges are oriented from right to left)</i>";

static const char *rootSelectionValuesDescription =
    "<b>rootIsSource</b> <i>(Select a source in the graph)</i><br>"
    "<b>rootIsSink</b> <i>(Select a sink in the graph)</i><br>"
    "<b>rootByCoord</b> <i>(Use the coordinates, e.g., select the topmost node if orientation is "
    "<b>topToBottom</b>)</i>";

class OGDFTree : public tlp::OGDFLayoutPluginBase {

public:
  PLUGININFORMATION("Improved Walker (OGDF)", "Christoph Buchheim", "12/11/2007",
                    "Implements a linear-time tree layout algorithm with straight-line or "
                    "orthogonal edge routing.",
                    "1.5", "Tree")
  OGDFTree(const tlp::PluginContext *context)
      : OGDFLayoutPluginBase(context, tlp::getOGDFLayoutModule<ogdf::TreeLayout>(context)) {
    addInParameter<double>("siblings distance", paramHelp[0].data(), "20");
    addInParameter<double>("subtrees distance", paramHelp[1].data(), "20");
    addInParameter<double>("levels distance", paramHelp[2].data(), "50");
    addInParameter<double>("trees distance", paramHelp[3].data(), "50");
    addInParameter<bool>("orthogonal layout", paramHelp[4].data(), "false");
    addInParameter<tlp::StringCollection>(ELT_ORIENTATION, paramHelp[5].data(), ELT_ORIENTATIONLIST,
                                          true, orientationValuesDescription);
    addInParameter<tlp::StringCollection>(ELT_ROOTSELECTION, paramHelp[6].data(),
                                          ELT_ROOTSELECTIONLIST, true,
                                          rootSelectionValuesDescription);
  }

  void beforeCall() override {
    auto *tree = static_cast<ogdf::TreeLayout *>(ogdfLayoutAlgo);

    if (dataSet != nullptr) {
      double dval = 0;
      bool bval = false;
      tlp::StringCollection sc;

      if (dataSet->get("siblings distance", dval)) {
        tree->siblingDistance(dval);
      }

      if (dataSet->get("subtrees distance", dval)) {
        tree->subtreeDistance(dval);
      }

      if (dataSet->get("levels distance", dval)) {
        tree->levelDistance(dval);
      }

      if (dataSet->get("trees distance", dval)) {
        tree->treeDistance(dval);
      }

      if (dataSet->get("orthogonal layout", bval)) {
        tree->orthogonalLayout(bval);
      }

      if (dataSet->get(ELT_ORIENTATION, sc)) {
        tree->orientation(orientation[sc.getCurrent()]);
      }

      if (dataSet->get(ELT_ROOTSELECTION, sc)) {
        tree->rootSelection(rootSelection[sc.getCurrent()]);
      }
    }
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
};

PLUGIN(OGDFTree)
