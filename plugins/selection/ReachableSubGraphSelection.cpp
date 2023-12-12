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

#include "ReachableSubGraphSelection.h"

#include <talipot/StringCollection.h>
#include <talipot/GraphTools.h>
#include <talipot/StableIterator.h>

using namespace tlp;

PLUGIN(ReachableSubGraphSelection)

static constexpr std::string_view paramHelp[] = {
    // edge direction
    "This parameter defines the navigation direction.",

    // starting nodes
    "This parameter defines the starting set of nodes used to walk in the graph.",

    // distance
    "This parameter defines the maximal distance of reachable nodes."};

static const char *directionValuesDescription =
    "<b>output edges</b> : <i>follow output edges (directed)</i><br>"
    "<b>input edges</b> : <i>follow input edges (reverse-directed)</i><br>"
    "<b>all edges</b> : <i>all edges (undirected)</i>";

static const char *edgesDirectionLabels[] = {"output edges", "input edges", "all edges"};

ReachableSubGraphSelection::ReachableSubGraphSelection(const tlp::PluginContext *context)
    : BooleanAlgorithm(context) {
  addInParameter<StringCollection>("edge direction", paramHelp[0].data(),
                                   "output edges;input edges;all edges", true,
                                   directionValuesDescription);
  addInParameter<BooleanProperty>("starting nodes", paramHelp[1].data(), "viewSelection");
  addInParameter<int>("distance", paramHelp[2].data(), "5");
  addOutParameter<uint>("#edges selected", "The number of newly selected edges");
  addOutParameter<uint>("#nodes selected", "The number of newly selected nodes");
  // old name
  declareDeprecatedName("Reachable Sub-Graph");
}

///===========================================================
bool ReachableSubGraphSelection::run() {
  uint maxDistance = 5;
  StringCollection edgeDirectionCollecion;
  EdgeType edgeDirection = EdgeType::DIRECTED;
  BooleanProperty *startNodes = graph->getBooleanProperty("viewSelection");

  if (dataSet != nullptr) {
    dataSet->get("distance", maxDistance);

    // Get the edge orientation
    bool found(false);

    if (dataSet->get("edge direction", edgeDirectionCollecion)) {
      found = true;
    } else {
      found = dataSet->get("edges direction", edgeDirectionCollecion); // former buggy parameter
    }
    // name

    if (found) {
      if (edgeDirectionCollecion.getCurrentString() == edgesDirectionLabels[0]) {
        edgeDirection = EdgeType::DIRECTED;
      } else if (edgeDirectionCollecion.getCurrentString() == edgesDirectionLabels[1]) {
        edgeDirection = EdgeType::INV_DIRECTED;
      } else if (edgeDirectionCollecion.getCurrentString() == edgesDirectionLabels[2]) {
        edgeDirection = EdgeType::UNDIRECTED;
      }
    } else {
      // If the new parameter is not defined search for the old one.
      int direction = 0;

      if (dataSet->get("direction", direction)) {
        switch (direction) {
        case 0:
          edgeDirection = EdgeType::DIRECTED;
          break;

        case 1:
          edgeDirection = EdgeType::INV_DIRECTED;
          break;

        case 2:
          edgeDirection = EdgeType::UNDIRECTED;
        }
      }
    }

    // keep startingnodes for compatibility
    if (!dataSet->get("starting nodes", startNodes)) {
      dataSet->get("startingnodes", startNodes);
    }
  }

  unsigned num_nodes = 0, num_edges = 0;

  if (startNodes) {
    // as the input selection property and the result property can be the same one,
    // if needed, use a stable iterator to keep a copy of the input selected nodes as all values
    // of the result property are reset to false below
    // delete done by the for loop
    Iterator<node> *itN = (result == startNodes) ? stableIterator(startNodes->getNodesEqualTo(true))
                                                 : startNodes->getNodesEqualTo(true);

    std::unordered_map<node, bool> reachables;

    result->setAllEdgeValue(false);
    result->setAllNodeValue(false);

    // iterate on startNodes add them and their reachables
    for (auto current : itN) {
      reachables[current] = true;
      markReachableNodes(graph, current, reachables, maxDistance, edgeDirection);
    }

    // select nodes
    for (const auto &itr : reachables) {
      result->setNodeValue(itr.first, true);
      ++num_nodes;
    }

    // select corresponding edges
    for (auto e : graph->edges()) {
      const auto &[src, tgt] = graph->ends(e);
      if (reachables.contains(src) && reachables.contains(tgt)) {
        result->setEdgeValue(e, true);
        ++num_edges;
      }
    }

  } else {
    result->setAllEdgeValue(false);
    result->setAllNodeValue(false);
  }

  // output some useful information
  if (dataSet != nullptr) {
    dataSet->set("#edges selected", num_edges);
    dataSet->set("#nodes selected", num_nodes);
  }

  return true;
}
