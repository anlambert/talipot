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

#include <stack>

#include <talipot/BiconnectedTest.h>
#include <talipot/ConnectedTest.h>
#include <talipot/ConnectedTestListener.h>
#include <talipot/MutableContainer.h>
#include <talipot/StableIterator.h>

using namespace std;
using namespace tlp;

//=================================================================
static ConnectedTestListener instance;
//=================================================================
// structure below is used to implement dfs loop
struct dfsBiconnectStruct {
  node from, u, first;
  uint depth;
  Iterator<node> *inOutNodes;

  dfsBiconnectStruct(Graph *graph, node n, uint d = 0, node u = node(), node first = node())
      : from(n), u(u), first(first), depth(d),
        inOutNodes(stableIterator(graph->getInOutNodes(from))) {}
};

static void makeBiconnectedDFS(Graph *graph, vector<edge> &addedEdges) {
  // the graph is already connected
  // so get any node to begin
  node from = graph->getOneNode();

  if (!from.isValid()) {
    return;
  }

  MutableContainer<int> low;
  MutableContainer<int> depth;
  depth.setAll(-1);
  MutableContainer<node> supergraph;
  supergraph.setAll(node());

  // dfs loop
  stack<dfsBiconnectStruct> dfsLevels;
  dfsBiconnectStruct dfsParams(graph, from);
  dfsLevels.push(dfsParams);
  depth.set(from.id, 0);
  low.set(from.id, 0);

  while (!dfsLevels.empty()) {
    dfsParams = dfsLevels.top();
    from = dfsParams.from;
    node u = dfsParams.first;

    // for every node connected to from
    Iterator<node> *itN = dfsParams.inOutNodes;

    while (itN->hasNext()) {
      node to = itN->next();

      // if there is a loop, ignore it
      if (from == to) {
        continue;
      }

      if (!u.isValid()) {
        dfsLevels.top().first = u = to;
      }

      // if the destination node has not been visited, visit it
      if (depth.get(to.id) == -1) {
        supergraph.set(to.id, from);
        dfsParams.from = to;
        dfsParams.first = node();
        dfsParams.u = u;
        uint currentDepth = dfsParams.depth + 1;
        dfsParams.depth = currentDepth;
        depth.set(to.id, currentDepth);
        low.set(to.id, currentDepth);
        dfsParams.inOutNodes = stableIterator(graph->getInOutNodes(to));
        break;
      } else {
        low.set(from.id, std::min(low.get(from.id), depth.get(to.id)));
      }
    }

    if (from != dfsParams.from) {
      dfsLevels.push(dfsParams);
      continue;
    }

    delete itN;

    // pop the current dfsParams
    node to = dfsParams.from;
    from = supergraph.get(to.id);

    if (from.isValid()) {
      u = dfsParams.u;

      if (low.get(to.id) == depth.get(from.id)) {
        if (to == u && supergraph.get(from.id).isValid()) {
          addedEdges.push_back(graph->addEdge(to, supergraph.get(from.id)));
        }

        if (to != u) {
          addedEdges.push_back(graph->addEdge(u, to));
        }
      }

      low.set(from.id, std::min(low.get(from.id), low.get(to.id)));
    }

    dfsLevels.pop();
  }
}

//=================================================================
bool biconnectedTest(const Graph *graph, node v, MutableContainer<uint> &low,
                     MutableContainer<uint> &dfsNumber, MutableContainer<node> &supergraph,
                     uint &count) {
  uint vDfs = count++;
  dfsNumber.set(v.id, vDfs);
  low.set(v.id, vDfs);

  for (auto w : graph->getInOutNodes(v)) {

    if (dfsNumber.get(w.id) == UINT_MAX) {
      if (vDfs == 1) {
        if (count != 2) {
          return false;
        }
      }

      supergraph.set(w.id, v);

      if (!biconnectedTest(graph, w, low, dfsNumber, supergraph, count)) {
        return false;
      }

      if (vDfs != 1) {
        if (low.get(w.id) >= dfsNumber.get(v.id)) {
          return false;
        } else {
          low.set(v.id, std::min(low.get(v.id), low.get(w.id)));
        }
      }
    } else if (supergraph.get(v.id) != w) {
      low.set(v.id, std::min(low.get(v.id), dfsNumber.get(w.id)));
    }
  }

  return true;
}
//=================================================================
static bool biconnectedTest(const Graph *graph) {
  MutableContainer<uint> low;
  MutableContainer<uint> dfsNumber;
  dfsNumber.setAll(UINT_MAX);
  MutableContainer<node> supergraph;
  uint count = 1;
  return (biconnectedTest(graph, graph->nodes()[0], low, dfsNumber, supergraph, count) &&
          (count == graph->numberOfNodes() + 1));
}
//=================================================================
bool BiconnectedTest::isBiconnected(const tlp::Graph *graph) {
  if (graph->isEmpty()) {
    return true;
  }

  if (const auto it = instance.resultsBuffer.find(graph); it != instance.resultsBuffer.end()) {
    return it->second;
  }

  graph->addListener(instance);
  return instance.resultsBuffer[graph] = biconnectedTest(graph);
}
//=================================================================
vector<edge> BiconnectedTest::makeBiconnected(Graph *graph) {
  graph->removeListener(instance);
  instance.resultsBuffer.erase(graph);
  vector<edge> addedEdges = ConnectedTest::makeConnected(graph);
  makeBiconnectedDFS(graph, addedEdges);
  assert(BiconnectedTest::isBiconnected(graph));
  return addedEdges;
}
