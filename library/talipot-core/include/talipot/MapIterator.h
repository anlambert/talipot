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

#include <talipot/Iterator.h>
#include <talipot/config.h>
#include <talipot/Edge.h>
#include <list>
#include <vector>

#ifndef TALIPOT_MAP_ITERATOR_H
#define TALIPOT_MAP_ITERATOR_H

namespace tlp {

struct node;
class Graph;

/**
 * That function enables to obtain the next edge on a face of the embedding. It uses
 * the EdgeMapIterators.
 *
 * @see NodeMapIterator
 * @see EdgeMapIterator
 * @see PlanarConMap
 */
TLP_SCOPE edge nextFaceEdge(Graph *g, edge source, node target);

/**
 * @class NodeMapIterator
 * @brief Iterator that enables to traverse the graph taking into account the ordering of edges
 * around nodes
 * @param sg the considered graph
 * @param source the node from witch one arrives on target
 * @param target the node the considered node (one will obtain an iterator on the neighbours of
 * that node)
 *
 * Since Talipot enables to order the edges around nodes, it is possible to traverse the nodes
 * according
 * to that ordering. It is necessary to use that function if one wants to take into account the
 * embedding
 * of the graph. Such functionality is really useful when dealing with planar graphs. However if
 * one wants
 * more efficient data structure for planar graphs one should consider using PlanarConMap.
 *
 * @see EdgeMapIterator
 * @see PlanarConMap
 */
struct TLP_SCOPE NodeMapIterator : public Iterator<node> {
    ///
    NodeMapIterator(Graph *sg, node source, node target);
    ~NodeMapIterator() override;
    /// Return the next element
    node next() override;
    /// Return true if it exist a next element
    bool hasNext() override;

  private:
    std::list<node> cloneIt;
    std::list<node>::iterator itStl;
};

/**
 * @class EdgeMapIterator
 * @brief Iterator that enables to traverse the graph taking into account the ordering of edges
 * around nodes
 * @param sg the considered graph
 * @param source the edge from witch one arrives on target
 * @param target the node the considered node (one will obtain an iterator on the neighbours of
 * that node)
 *
 * Since Talipot enables to order the edges around nodes, it is possible to traverse the nodes
 * according
 * to that ordering. It is necessary to use that function if one wants to take into account the
 * embedding
 * of the graph. Such functionality is really useful when dealing with planar graphs. However if
 * one wants
 * more efficient data structure for planar graphs one should consider using PlanarConMap.
 *
 * @see EdgeMapIterator
 * @see PlanarConMap
 */
struct TLP_SCOPE EdgeMapIterator : public Iterator<edge> {
    ///
    EdgeMapIterator(const Graph *sg, edge source, node target);
    /// Return the next element
    edge next() override;
    /// Return true if it exist a next element
    bool hasNext() override;

  private:
    std::vector<edge> adj;
    edge start;
    int treat;
    uint pos;
    bool finished;
};
}
#endif // TALIPOT_MAP_ITERATOR_H
