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

#include <talipot/Graph.h>
#include <talipot/PlanarityTest.h>
#include <talipot/PlanarityTestImpl.h>
#include <talipot/BiconnectedTest.h>

using namespace std;
using namespace tlp;
//=================================================================
class PlanarityTestListener : public Observable {
public:
  // override of Observable::treatEvent to remove the cached result for a graph if it is modified.
  void treatEvent(const Event &) override;

  /**
   * @brief Stored results for graphs. When a graph is updated, its entry is removed from the map.
   **/
  node_hash_map<const Graph *, bool> resultsBuffer;
};

void PlanarityTestListener::treatEvent(const Event &evt) {
  const auto *gEvt = dynamic_cast<const GraphEvent *>(&evt);

  if (gEvt) {
    Graph *graph = gEvt->getGraph();

    switch (gEvt->getType()) {
    case GraphEventType::TLP_DEL_EDGE:
    case GraphEventType::TLP_DEL_NODE:

      if (resultsBuffer.contains(graph)) {
        if (resultsBuffer[graph]) {
          return;
        }
      }

      graph->removeListener(this);
      resultsBuffer.erase(graph);
      break;

    case GraphEventType::TLP_ADD_EDGE:

      if (resultsBuffer.contains(graph)) {
        if (!resultsBuffer[graph]) {
          return;
        }
      }

      graph->removeListener(this);
      resultsBuffer.erase(graph);
      break;

    default:
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
static PlanarityTestListener instance;
//=================================================================
bool PlanarityTest::isPlanar(Graph *graph) {
  if (const auto itr = instance.resultsBuffer.find(graph); itr != instance.resultsBuffer.end()) {
    return itr->second;
  }

  uint nbOfNodes = graph->numberOfNodes();

  if (nbOfNodes == 0) {
    return instance.resultsBuffer[graph] = true;
  }

  // quick test
  if ((nbOfNodes >= 3) && (graph->numberOfEdges() > ((3 * nbOfNodes) - 6))) {
    graph->addListener(instance);
    return instance.resultsBuffer[graph] = false;
  }

  Observable::holdObservers();
  vector<edge> addedEdges = BiconnectedTest::makeBiconnected(graph);
  PlanarityTestImpl planarTest(graph);
  instance.resultsBuffer[graph] = planarTest.isPlanar(true);

  for (auto e : addedEdges) {
    graph->delEdge(e, true);
  }

  Observable::unholdObservers();
  graph->addListener(instance);
  return instance.resultsBuffer[graph];
}
bool PlanarityTest::isPlanarEmbedding(const tlp::Graph *graph) {
  return PlanarityTestImpl::isPlanarEmbedding(graph);
}
//=================================================================
bool PlanarityTest::planarEmbedding(Graph *graph) {
  if (!PlanarityTest::isPlanar(graph)) {
    return false;
  }
  Observable::holdObservers();
  vector<edge> addedEdges = BiconnectedTest::makeBiconnected(graph);
  PlanarityTestImpl planarTest(graph);
  planarTest.isPlanar(true);

  for (auto e : addedEdges) {
    graph->delEdge(e, true);
  }

  Observable::unholdObservers();
  return true;
}
//=================================================================
list<edge> PlanarityTest::getObstructionsEdges(Graph *graph) {
  if (PlanarityTest::isPlanar(graph)) {
    return list<edge>();
  }

  Observable::holdObservers();
  vector<edge> addedEdges = BiconnectedTest::makeBiconnected(graph);
  PlanarityTestImpl planarTest(graph);
  planarTest.isPlanar(true);
  list<edge> tmpList = planarTest.getObstructions();
  for (auto e : addedEdges) {
    graph->delEdge(e, true);
  }
  Observable::unholdObservers();
  set<edge> tmpAdded(addedEdges.begin(), addedEdges.end());
  list<edge> result;

  for (auto e : tmpList) {
    if (!tmpAdded.contains(e))
      result.push_back(e);
  }

  return result;
}
