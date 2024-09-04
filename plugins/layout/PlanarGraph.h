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

#include <vector>
class PlanarConMap;
class SuperGraph;

namespace tlp {
/**
 * This function triangulate a planar graph. (ie. add edges in the graph until
 * each face of the planar map contains only 3 edges.
 * The input graph must be planr in order to call this function.
 */
void triangulate(SuperGraph *map, std::vector<edge> &addedEdges);
/**
 * Build the canonical ordering of a planar triangulated graph. For information
 * about canonical ordering the user must refer to the planar graph drawing book.
 */
void canonicalOrdering(SuperGraph *graph, std::vector<node> &order);
}
