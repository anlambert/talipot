/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/BooleanProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/GraphTools.h>

#include "PathAlgorithm.h"

using namespace tlp;
using namespace std;

bool PathAlgorithm::computePath(Graph *graph, PathType pathType, EdgeOrientation edgesOrientation,
                                node src, node tgt, BooleanProperty *result,
                                DoubleProperty *weights) {

  bool retVal = false;
  tlp::ShortestPathType spt;

  if (pathType == AllShortest) {
    switch (edgesOrientation) {
    case Directed:
      spt = ShortestPathType::AllDirectedPaths;
      break;
    case Undirected:
      spt = ShortestPathType::AllPaths;
      break;
    case Reversed:
    default:
      spt = ShortestPathType::AllReversedPaths;
    }
  } else {
    switch (edgesOrientation) {
    case Directed:
      spt = ShortestPathType::OneDirectedPath;
      break;
    case Undirected:
      spt = ShortestPathType::OnePath;
      break;
    case Reversed:
    default:
      spt = ShortestPathType::OneReversedPath;
    }
  }
  graph->push();
  retVal = selectShortestPaths(graph, src, tgt, spt, weights, result);
  if (!retVal) {
    graph->pop();
  }
  return retVal;
}
