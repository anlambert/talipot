/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/Graph.h>
#include <talipot/TriconnectedTest.h>
#include <talipot/BiconnectedTest.h>

using namespace std;
using namespace tlp;
//=================================================================
class TriconnectedTestListener : public Observable {
public:
  // override of Observable::treatEvent to remove the cached result for a graph if it is modified.
  void treatEvent(const Event &) override;

  /**
   * @brief Stored results for graphs. When a graph is updated, its entry is removed from the map.
   **/
  std::unordered_map<const Graph *, bool> resultsBuffer;
};

//=================================================================
void TriconnectedTestListener::treatEvent(const Event &evt) {
  const auto *gEvt = dynamic_cast<const GraphEvent *>(&evt);

  if (gEvt) {
    Graph *graph = gEvt->getGraph();

    switch (gEvt->getType()) {
    case GraphEventType::TLP_ADD_EDGE:

      if (resultsBuffer.find(graph) != resultsBuffer.end()) {
        if (resultsBuffer[graph]) {
          return;
        }
      }
      [[fallthrough]];

    case GraphEventType::TLP_DEL_EDGE:
    case GraphEventType::TLP_DEL_NODE:
      graph->removeListener(this);
      resultsBuffer.erase(graph);
      break;

    case GraphEventType::TLP_ADD_NODE:
      resultsBuffer[graph] = false;
      break;

    default:
      // we don't care about other events
      break;
    }
  } else {

    auto *graph = static_cast<Graph *>(evt.sender());

    if (evt.type() == EventType::TLP_DELETE) {
      resultsBuffer.erase(graph);
    }
  }
}
//=================================================================
static TriconnectedTestListener instance;
//=================================================================
bool TriconnectedTest::isTriconnected(Graph *graph) {
  if (const auto it = instance.resultsBuffer.find(graph); it != instance.resultsBuffer.end()) {
    return it->second;
  }

  if (graph->isEmpty()) {
    return false;
  }

  bool result = true;
  Graph *tmp = graph->addCloneSubGraph();

  for (auto n : graph->nodes()) {
    tmp->delNode(n);

    if (!BiconnectedTest::isBiconnected(tmp)) {
      result = false;
      break;
    }

    tmp->addNode(n);

    for (auto e : graph->incidence(n)) {
      tmp->addEdge(e);
    }
  }

  graph->delSubGraph(tmp);
  graph->addListener(instance);
  return instance.resultsBuffer[graph] = result;
}
