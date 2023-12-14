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

#include <libvpsc/generate-constraints.h>
#include <libvpsc/remove_rectangle_overlap.h>

#include "FastOverlapRemoval.h"

using namespace std;
using namespace tlp;

PLUGIN(FastOverlapRemoval)

static constexpr std::string_view paramHelp[] = {
    // overlap removal type
    "Overlap removal type.",

    // layout
    "The property used for the input layout of nodes and edges.",

    // node size
    "The property used for node sizes.",

    // Rotation
    "The property defining rotation angles of nodes around the z-axis.",

    // Iterations
    "The algorithm will be applied N times, each time increasing node size to attain original size "
    "at the final iteration. This greatly enhances the layout.",

    // x border
    "The minimal x border value that will separate the graph nodes after application of the "
    "algorithm.",

    // y border
    "The minimal y border value that will separate the graph nodes after application of the "
    "algorithm."};

#define OVERLAP_TYPE "X-Y;X;Y"

static const char *overlapRemovalTypeValuesDescription =
    "<b>X-Y</b> <i>(Remove overlaps in both X and Y directions)</i><br>"
    "<b>X</b> <i>(Remove overlaps only in X direction)</i><br>"
    "<b>Y</b> <i>(Remove overlaps only in Y direction)</i>";

FastOverlapRemoval::FastOverlapRemoval(const tlp::PluginContext *context)
    : tlp::LayoutAlgorithm(context) {
  addInParameter<StringCollection>("overlap removal type", paramHelp[0].data(), OVERLAP_TYPE, true,
                                   overlapRemovalTypeValuesDescription);
  addInParameter<LayoutProperty>("layout", paramHelp[1].data(), "viewLayout");
  addInParameter<SizeProperty>("bounding box", paramHelp[2].data(), "viewSize");
  addInParameter<DoubleProperty>("rotation", paramHelp[3].data(), "viewRotation");
  addInParameter<int>("number of passes", paramHelp[4].data(), "5");
  addInParameter<double>("x border", paramHelp[5].data(), "0.0");
  addInParameter<double>("y border", paramHelp[6].data(), "0.0");
}

/**
 * The following function transfers the node set into vpsc rectangles
 * and runs fast overlap removal.  This vpsc code was a port of Dwyer
 * used in the InkScape Open Source Software.
 */
bool FastOverlapRemoval::run() {
  if (pluginProgress) {
    // user cannot interact while computing
    pluginProgress->showPreview(false);
    pluginProgress->showStops(false);
  }

  tlp::StringCollection stringCollection(OVERLAP_TYPE);
  stringCollection.setCurrent(0);
  LayoutProperty *viewLayout = nullptr;
  SizeProperty *viewSize = nullptr;
  DoubleProperty *viewRot = nullptr;
  double xBorder = 0.;
  double yBorder = 0.;
  int nbPasses = 5;

  if (dataSet != nullptr) {

    if (dataSet->exists("overlaps removal type")) {
      dataSet->get("overlaps removal type", stringCollection);
    } else {
      dataSet->get("overlap removal type", stringCollection);
    }

    dataSet->get("layout", viewLayout);

    if (!dataSet->get("bounding box", viewSize)) {
      // old name of the parameter
      dataSet->get("boundingBox", viewSize);
    }

    dataSet->get("rotation", viewRot);
    dataSet->get("number of passes", nbPasses);
    dataSet->get("x border", xBorder);
    dataSet->get("y border", yBorder);
  }

  if (viewLayout == nullptr) {
    viewLayout = graph->getLayoutProperty("viewLayout");
  }

  if (viewSize == nullptr) {
    viewSize = graph->getSizeProperty("viewSize");
  }

  if (viewRot == nullptr) {
    viewRot = graph->getDoubleProperty("viewRotation");
  }

  // initialize result for edges
  result->setAllEdgeValue(viewLayout->getEdgeDefaultValue());
  for (auto e : viewLayout->getNonDefaultValuatedEdges()) {
    result->setEdgeValue(e, viewLayout->getEdgeValue(e));
  }

  NodeVectorProperty<vpsc::Rectangle> nodeRectangles(graph);
  for (float passIndex = 1; passIndex <= nbPasses; ++passIndex) {
    // initialization
    TLP_PARALLEL_MAP_NODES(graph, [&](node curNode) {
      Size sz = viewSize->getNodeValue(curNode) * passIndex / float(nbPasses);
      const Coord &pos = viewLayout->getNodeValue(curNode);
      double curRot = viewRot->getNodeValue(curNode);
      Size rotSize = Size(sz.getW() * fabs(cos(curRot * M_PI / 180.0)) +
                              sz.getH() * fabs(sin(curRot * M_PI / 180.0)),
                          sz.getW() * fabs(sin(curRot * M_PI / 180.0)) +
                              sz.getH() * fabs(cos(curRot * M_PI / 180.0)),
                          1.0f);
      double maxX = pos.getX() + rotSize.getW() / 2.0;
      double maxY = pos.getY() + rotSize.getH() / 2.0;
      double minX = pos.getX() - rotSize.getW() / 2.0;
      double minY = pos.getY() - rotSize.getH() / 2.0;

      nodeRectangles[curNode] = vpsc::Rectangle(minX, maxX, minY, maxY, xBorder, yBorder);
    });

    size_t nbNodes = graph->numberOfNodes();

    // actually apply fast overlap removal
    if (stringCollection.getCurrentString() == "X-Y") {
      removeRectangleOverlap(nbNodes, nodeRectangles.data(), xBorder, yBorder);
    } else if (stringCollection.getCurrentString() == "X") {
      removeRectangleOverlapX(nbNodes, nodeRectangles.data(), xBorder, yBorder);
    } else {
      removeRectangleOverlapY(nbNodes, nodeRectangles.data(), yBorder);
    }

    for (auto n : graph->nodes()) {
      Coord newPos = Coord(nodeRectangles[n].getCentreX(), nodeRectangles[n].getCentreY());
      LayoutAlgorithm::result->setNodeValue(n, newPos);
    }
  }

  return true;
} // end run
