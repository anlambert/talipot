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

#ifndef PATH_ALGORITHM_H
#define PATH_ALGORITHM_H

#include <talipot/Node.h>
#include <cfloat>

namespace tlp {
class BooleanProperty;
class DoubleProperty;
class Graph;

/**
 * @brief An facade for any path finding algorithm.
 * This class will initiate and run the correct path finding algorithm relative to the parameters
 * given.
 */
class PathAlgorithm {
public:
  /**
   * By default, Tulip works on directed edges. This behavior can be overloaded by forcing the edges
   * to be directed, undirected or reversed.
   */
  enum EdgeOrientation { Directed, Undirected, Reversed };

  /**
   * A path algorithm can look for only one (shortest) path or all the shortest paths.
   */
  enum PathType { OneShortest, AllShortest };

  /**
   * Compute a path between two nodes.
   * @param pathType the type of path to look for.
   * @param edgesOrientation The edge orientation policy.
   * @param src The source node
   * @param tgt The target node
   * @param result Nodes and edges located in the path will be set to true in a resulting boolean
   * property.
   * @param weights The edges weights
   * @return a boolean indicating if at least one path has been found
   *
   * @see PathType
   * @see EdgeOrientation
   */
  static bool computePath(tlp::Graph *graph, PathType pathType, EdgeOrientation edgesOrientation,
                          tlp::node src, tlp::node tgt, tlp::BooleanProperty *result,
                          tlp::DoubleProperty *weights = nullptr);
};
}
#endif // PATH_ALGORITHM_H
