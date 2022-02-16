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

#include <talipot/OuterPlanarTest.h>
#include <talipot/Graph.h>
#include <talipot/PlanarityTestImpl.h>

using namespace std;
using namespace tlp;
//=================================================================
class OuterPlanarTestListener : public Observable {
public:
  // override of Observable::treatEvent to remove the cached result for a graph if it is modified.
  void treatEvent(const Event &) override;

  /**
   * @brief Stored results for graphs. When a graph is updated, its entry is removed from the map.
   **/
  std::unordered_map<const Graph *, bool> resultsBuffer;
};

void OuterPlanarTestListener::treatEvent(const Event &evt) {
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

      graph->removeListener(this);
      resultsBuffer.erase(graph);
      break;

    case GraphEventType::TLP_DEL_EDGE:
    case GraphEventType::TLP_DEL_NODE:

      if (resultsBuffer.find(graph) != resultsBuffer.end()) {
        if (!resultsBuffer[graph]) {
          return;
        }
      }
      [[fallthrough]];

    case GraphEventType::TLP_REVERSE_EDGE:
      graph->removeListener(this);
      resultsBuffer.erase(graph);
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
static OuterPlanarTestListener instance;
//=================================================================
bool OuterPlanarTest::isOuterPlanar(tlp::Graph *graph) {

  if (const auto it = instance.resultsBuffer.find(graph); it != instance.resultsBuffer.end()) {
    return it->second;
  } else if (graph->isEmpty()) {
    return instance.resultsBuffer[graph] = true;
  }

  PlanarityTestImpl planarTest(graph);

  if (!planarTest.isPlanar(true)) {
    return (instance.resultsBuffer[graph] = false);
  } else {
    Observable::holdObservers();
    node n = graph->addNode();
    for (auto current : graph->nodes()) {
      if (current != n) {
        graph->addEdge(n, current);
      }
    }
    instance.resultsBuffer[graph] = planarTest.isPlanar(true);
    graph->delNode(n);
    Observable::unholdObservers();
    graph->addListener(instance);
    return instance.resultsBuffer[graph];
  }
}
