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

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <vector>
#include <set>
#include <climits>
#include <talipot/Graph.h>
#include <talipot/Vector.h>
#include <talipot/LayoutProperty.h>
#include <talipot/VectorProperty.h>
#include <talipot/ParallelTools.h>

class Dijkstra {

  public:
    //============================================================
    Dijkstra(tlp::Graph *graph) : graph(graph) {
        nodeDistance.alloc(graph);
        forbiddenNodes.alloc(graph);
        usedEdges.alloc(graph);
        resultNodes.alloc(graph);
        resultEdges.alloc(graph);
        mapDik.alloc(graph);
    }

    ~Dijkstra() = default;

    void initDijkstra(const tlp::Graph *const forbiddenNodes, tlp::node src,
                      const tlp::EdgeVectorProperty<double> &weights,
                      const std::set<tlp::node> &focus);

    //========================================================
    void searchPaths(tlp::node n, tlp::EdgeVectorProperty<uint> &depth);
    void searchPath(tlp::node n, std::vector<tlp::node> &vNodes);
    //=============================================================
  private:
    struct DijkstraElement {
        DijkstraElement(const double dist = DBL_MAX, const tlp::node previous = tlp::node(),
                        const tlp::node n = tlp::node())
            : dist(dist), previous(previous), n(n) {}
        //=========================================================
        bool operator==(const DijkstraElement &b) const {
            return n == b.n;
        }
        bool operator!=(const DijkstraElement &b) const {
            return n != b.n;
        }
        double dist;
        tlp::node previous;
        tlp::node n;
        std::vector<tlp::edge> usedEdge;
    };

    struct LessDijkstraElement {
        bool operator()(const DijkstraElement *const a, const DijkstraElement *const b) const {
            if (fabs(a->dist - b->dist) > 1.E-9) {
                return (a->dist < b->dist);
            } else {
                return (a->n.id < b->n.id);
            }
        }
    };

    tlp::Graph *graph;

    tlp::node src;

    tlp::NodeVectorProperty<double> nodeDistance;
    tlp::NodeVectorProperty<bool> forbiddenNodes;
    tlp::EdgeVectorProperty<bool> usedEdges;
    tlp::NodeVectorProperty<bool> resultNodes;
    tlp::EdgeVectorProperty<bool> resultEdges;
    tlp::NodeVectorProperty<DijkstraElement *> mapDik;
};

#endif // DIJKSTRA_H
