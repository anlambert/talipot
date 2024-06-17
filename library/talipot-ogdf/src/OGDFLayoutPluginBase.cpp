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

#include <talipot/OGDFLayoutPluginBase.h>
#include <talipot/LayoutProperty.h>

#include <ogdf/packing/SimpleCCPacker.h>

#include <talipot/DrawingTools.h>

using namespace std;
using namespace tlp;

OGDFLayoutPluginBase::OGDFLayoutPluginBase(const PluginContext *context,
                                           ogdf::LayoutModule *ogdfLayoutAlgo, bool importEdgeBends)
    : LayoutAlgorithm(context), tlpToOGDF(nullptr), ogdfLayoutAlgo(ogdfLayoutAlgo),
      simpleCCPacker(ogdfLayoutAlgo ? new ogdf::SimpleCCPacker(ogdfLayoutAlgo) : nullptr) {
  // convert Tulip Graph to OGDF Graph including attributes
  if (graph) {
    tlpToOGDF = new TalipotToOGDF(graph, importEdgeBends);
  }
}

OGDFLayoutPluginBase::~OGDFLayoutPluginBase() {
  delete tlpToOGDF;
  delete simpleCCPacker;
}

bool OGDFLayoutPluginBase::run() {
  if (pluginProgress) {
    // user cannot interact while computing
    pluginProgress->showPreview(false);
    pluginProgress->showStops(false);
  }

  ogdf::GraphAttributes &gAttributes = tlpToOGDF->getOGDFGraphAttr();

  beforeCall();

  try {
    // run the algorithm on the OGDF Graph with attributes
    callOGDFLayoutAlgorithm(gAttributes);
  } catch (ogdf::AlgorithmFailureException &afce) {
    std::string msg;

    switch (afce.exceptionCode()) {
    case ogdf::AlgorithmFailureCode::IllegalParameter:
      msg = "function parameter is illegal";
      break;

    case ogdf::AlgorithmFailureCode::NoFlow:
      msg = "min-cost flow could not find a legal flow";
      break;

    case ogdf::AlgorithmFailureCode::Sort:
      msg = "sequence not sorted";
      break;

    case ogdf::AlgorithmFailureCode::Label:
      msg = "labelling failed";
      break;

    case ogdf::AlgorithmFailureCode::ExternalFace:
      msg = "external face not correct";
      break;

    case ogdf::AlgorithmFailureCode::ForbiddenCrossing:
      msg = "crossing forbidden but necessary";
      break;

    case ogdf::AlgorithmFailureCode::TimelimitExceeded:
      msg = "it took too long";
      break;

    case ogdf::AlgorithmFailureCode::NoSolutionFound:
      msg = "couldn't solve the problem";
      break;

    default:
      msg = "unknown error";
    }

    pluginProgress->setError(std::string("Error\n") + msg);
    return false;
  }

  // retrieve nodes coordinates computed by the OGDF Layout Algorithm
  // and store them in the Tulip Layout Property
  for (auto n : graph->nodes()) {
    Coord nodeCoord = tlpToOGDF->getNodeCoordFromOGDFGraphAttr(n);
    result->setNodeValue(n, nodeCoord);
  }

  // same operation as above but with edges
  for (auto e : graph->edges()) {
    vector<Coord> edgeCoord = tlpToOGDF->getEdgeCoordFromOGDFGraphAttr(e);
    result->setEdgeValue(e, edgeCoord);
  }

  afterCall();

  return true;
}

void OGDFLayoutPluginBase::callOGDFLayoutAlgorithm(ogdf::GraphAttributes &gAttributes) {
  simpleCCPacker->call(gAttributes);
}

void OGDFLayoutPluginBase::transposeLayoutVertically() {

  BoundingBox graphBB = computeBoundingBox(graph, result, graph->getSizeProperty("viewSize"),
                                           graph->getDoubleProperty("viewRotation"));
  float midY = (graphBB[0][1] + graphBB[1][1]) / 2.f;

  for (auto n : graph->nodes()) {
    Coord nodeCoord = result->getNodeValue(n);
    nodeCoord[1] = midY - (nodeCoord[1] - midY);
    result->setNodeValue(n, nodeCoord);
  }

  for (auto e : graph->edges()) {
    std::vector<Coord> bends = result->getEdgeValue(e);

    if (bends.size()) {
      for (auto &bend : bends) {
        bend[1] = midY - (bend[1] - midY);
      }

      result->setEdgeValue(e, bends);
    }
  }
}
