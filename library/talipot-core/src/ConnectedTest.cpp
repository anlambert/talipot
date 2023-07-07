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

#include <talipot/ConnectedTest.h>
#include <talipot/ConnectedTestListener.h>
#include <talipot/VectorProperty.h>

using namespace std;
using namespace tlp;
//=================================================================
static ConnectedTestListener instance;
//=================================================================
bool ConnectedTest::isConnected(const tlp::Graph *const graph) {
  if (instance.resultsBuffer.find(graph) != instance.resultsBuffer.end()) {
    return instance.resultsBuffer[graph];
  }

  if (graph->isEmpty()) {
    return true;
  }

  // graph cannot be connected with that configuration
  if (graph->numberOfEdges() < graph->numberOfNodes() - 1) {
    return false;
  }

  graph->addListener(instance);
  return instance.resultsBuffer[graph] = (graph->bfs().size() == graph->numberOfNodes());
}
//=================================================================
vector<edge> ConnectedTest::makeConnected(Graph *graph) {
  vector<edge> addedEdges;
  if (!isConnected(graph)) {
    graph->removeListener(instance);
    instance.resultsBuffer.erase(graph);
    auto components = computeConnectedComponents(graph);
    for (uint i = 1; i < components.size(); ++i) {
      addedEdges.push_back(graph->addEdge(components[i - 1][0], components[i][0]));
    }
    graph->addListener(instance);
  }

  assert(isConnected(graph));
  instance.resultsBuffer[graph] = true;
  return addedEdges;
}
//=================================================================
uint ConnectedTest::numberOfConnectedComponents(const tlp::Graph *const graph) {
  if (graph->isEmpty()) {
    return 0u;
  }
  graph->removeListener(instance);
  uint result = computeConnectedComponents(graph).size();
  graph->addListener(instance);
  return result;
}
//======================================================================
vector<vector<node>> ConnectedTest::computeConnectedComponents(const tlp::Graph *graph) {
  vector<vector<node>> components;
  auto visited = NodeVectorProperty<bool>(graph);
  visited.setAll(false);
  // do a bfs traversal for each node
  for (auto n : graph->nodes()) {
    // check if curNode has been already visited
    if (!visited[n]) {
      // add a new component by doing a bfs traversal from this node
      components.push_back(graph->bfs(n));
      for (auto nc : components.back()) {
        visited[nc] = true;
      }
    }
  }
  return components;
}
//=================================================================

// algorithm implementation adapted from https://cp-algorithms.com/graph/bridge-searching.html
vector<edge> ConnectedTest::computeBridges(const Graph *graph) {
  auto visited = NodeVectorProperty<bool>(graph);
  auto tin = NodeVectorProperty<uint>(graph);
  auto low = NodeVectorProperty<uint>(graph);
  uint timer = 0;
  vector<edge> bridges;

  function<void(node, node)> dfsBridges = [&](node n, node p) {
    visited[n] = true;
    tin[n] = low[n] = timer++;
    for (edge e : graph->incidence(n)) {
      node m = graph->opposite(e, n);
      if (m == p) {
        continue;
      }
      if (visited[m]) {
        low[n] = min(low[n], tin[m]);
      } else {
        dfsBridges(m, n);
        low[n] = min(low[n], low[m]);
        if (low[m] > tin[n]) {
          bridges.push_back(e);
        }
      }
    }
  };

  visited.setAll(false);
  tin.setAll(-1);
  low.setAll(-1);

  for (auto n : graph->nodes()) {
    if (!visited[n]) {
      dfsBridges(n, node());
    }
  }

  return bridges;
}
