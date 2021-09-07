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

#include <talipot/ConnectedTest.h>
#include <talipot/ConnectedTestListener.h>
#include <talipot/VectorProperty.h>

using namespace std;
using namespace tlp;
//=================================================================
static ConnectedTestListener instance;
//=================================================================
static uint connectedTest(const Graph *const graph, node n, NodeVectorProperty<bool> &visited) {
  list<node> nodesToVisit;
  visited[n] = true;
  nodesToVisit.push_front(n);
  uint count = 1;

  while (!nodesToVisit.empty()) {
    node r = nodesToVisit.front();
    nodesToVisit.pop_front();
    // loop on all neighbours
    for (auto neighbour : graph->getInOutNodes(r)) {
      // check if neighbour has been visited
      if (!visited[neighbour]) {
        // mark neighbour as already visited
        visited[neighbour] = true;
        // push it for further deeper exploration
        nodesToVisit.push_back(neighbour);
        ++count;
      }
    }
  }

  return count;
}
//=================================================================
bool ConnectedTest::isConnected(const tlp::Graph *const graph) {
  if (instance.resultsBuffer.find(graph) != instance.resultsBuffer.end()) {
    return instance.resultsBuffer[graph];
  }

  if (graph->isEmpty()) {
    return true;
  }

  // graph can not be connected with that configuration
  if (graph->numberOfEdges() < graph->numberOfNodes() - 1) {
    return false;
  }

  NodeVectorProperty<bool> visited(graph);
  visited.setAll(false);
  uint count = connectedTest(graph, graph->getOneNode(), visited);
  bool result = (count == graph->numberOfNodes());
  graph->addListener(instance);
  return instance.resultsBuffer[graph] = result;
}
//=================================================================
vector<edge> ConnectedTest::makeConnected(Graph *graph) {
  vector<edge> addedEdges;
  graph->removeListener(instance);
  instance.resultsBuffer.erase(graph);
  vector<node> toLink;
  connect(graph, toLink);

  for (uint i = 1; i < toLink.size(); ++i) {
    addedEdges.push_back(graph->addEdge(toLink[i - 1], toLink[i]));
  }

  assert(isConnected(graph));
  return addedEdges;
}
//=================================================================
uint ConnectedTest::numberOfConnectedComponents(const tlp::Graph *const graph) {
  if (graph->isEmpty()) {
    return 0u;
  }

  graph->removeListener(instance);
  vector<node> toLink;
  connect(graph, toLink);
  uint result;

  if (!toLink.empty()) {
    result = toLink.size();
  } else {
    result = 1u;
  }

  instance.resultsBuffer[graph] = (result == 1u);
  graph->addListener(instance);
  return result;
}
//======================================================================
vector<vector<node>> ConnectedTest::computeConnectedComponents(const tlp::Graph *graph) {
  vector<vector<node>> components;
  NodeVectorProperty<bool> visited(graph);
  visited.setAll(false);
  // do a bfs traversal for each node
  TLP_MAP_NODES_AND_INDICES(graph, [&](node n, uint i) {
    // check if curNode has been already visited
    if (!visited[i]) {
      // add a new component
      components.push_back(std::vector<node>());
      std::vector<node> &component = components.back();
      // and initialize it with current node
      component.push_back(n);
      // do a bfs traversal this node
      list<node> nodesToVisit;
      visited[i] = true;
      nodesToVisit.push_front(n);

      while (!nodesToVisit.empty()) {
        n = nodesToVisit.front();
        nodesToVisit.pop_front();

        // loop on all neighbours
        for (auto neighbour : graph->getInOutNodes(n)) {
          uint neighPos = graph->nodePos(neighbour);
          // check if neighbour has been visited
          if (!visited[neighPos]) {
            // mark neighbour as already visited
            visited[neighPos] = true;
            // insert it in current component
            component.push_back(neighbour);
            // push it for further deeper exploration
            nodesToVisit.push_back(neighbour);
          }
        }
      }
    }
  });
  return components;
}
//=================================================================
void ConnectedTest::connect(const tlp::Graph *const graph, vector<node> &toLink) {

  if (const auto it = instance.resultsBuffer.find(graph); it != instance.resultsBuffer.end()) {
    if (it->second) {
      return;
    }
  }

  if (graph->isEmpty()) {
    return;
  }

  NodeVectorProperty<bool> visited(graph);
  visited.setAll(false);

  TLP_MAP_NODES_AND_INDICES(graph, [&](node n, uint i) {
    if (!visited[i]) {
      toLink.push_back(n);
      connectedTest(graph, n, visited);
    }
  });
}
