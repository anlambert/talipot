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

#include <stack>
#include <talipot/AcyclicTest.h>
#include <talipot/Graph.h>
#include <talipot/MutableContainer.h>

using namespace std;
using namespace tlp;

//**********************************************************************
class TestAcyclicListener : public Observable {
public:
  // override of Observable::treatEvent to remove the cached result for a graph if it is modified.
  void treatEvent(const Event &) override;

  /**
   * @brief Stored results for graphs. When a graph is updated, its entry is removed from the map.
   **/
  flat_hash_map<const Graph *, bool> resultsBuffer;
};

void TestAcyclicListener::treatEvent(const Event &evt) {
  const auto *graphEvent = dynamic_cast<const GraphEvent *>(&evt);

  if (graphEvent) {
    Graph *graph = graphEvent->getGraph();

    switch (graphEvent->getType()) {
    case GraphEventType::TLP_ADD_EDGE:

      if (!resultsBuffer[graph]) {
        return;
      }

      graph->removeListener(this);
      resultsBuffer.erase(graph);
      break;

    case GraphEventType::TLP_DEL_EDGE:

      if (resultsBuffer[graph]) {
        return;
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
//**********************************************************************
static TestAcyclicListener instance;
//**********************************************************************
bool AcyclicTest::isAcyclic(const Graph *graph) {
  if (instance.resultsBuffer.find(graph) == instance.resultsBuffer.end()) {
    instance.resultsBuffer[graph] = acyclicTest(graph);
    graph->addListener(instance);
  }

  return instance.resultsBuffer[graph];
}
//**********************************************************************
void AcyclicTest::makeAcyclic(Graph *graph, vector<edge> &reversed,
                              vector<tlp::SelfLoops> &selfLoops) {
  if (AcyclicTest::isAcyclic(graph)) {
    return;
  }

  std::vector<edge> edgesToDel;
  // replace self loops by three edges and two nodes.
  // do a loop on the already existing edges
  auto edges = graph->edges();
  // newly added edges during the loop
  // will not be taken into account
  uint nbEdges = edges.size();
  for (uint i = 0; i < nbEdges; ++i) {
    edge e = edges[i];
    const auto &[src, tgt] = graph->ends(e);

    if (src == tgt) {
      node n1 = graph->addNode();
      node n2 = graph->addNode();
      selfLoops.push_back(tlp::SelfLoops(n1, n2, graph->addEdge(src, n1), graph->addEdge(n1, n2),
                                         graph->addEdge(src, n2), e));
      edgesToDel.push_back(e);
    }
  }
  if (!edgesToDel.empty()) {
    graph->delEdges(edgesToDel);
  }

  // find obstruction edges
  reversed.clear();
  acyclicTest(graph, &reversed);

  if (reversed.size() > graph->numberOfEdges() / 2) {
    tlp::warning() << "[Warning]: " << __FUNCTION__ << ", is not efficient" << std::endl;
  }

  for (auto e : reversed) {
    graph->reverse(e);
  }

  assert(AcyclicTest::acyclicTest(graph));
}
//**********************************************************************
bool AcyclicTest::acyclicTest(const Graph *graph, vector<edge> *obstructionEdges) {
  MutableContainer<bool> visited;
  MutableContainer<bool> finished;
  visited.setAll(false);
  finished.setAll(false);
  bool result = true;
  // do a dfs traversal

  for (auto curNode : graph->nodes()) {

    if (!visited.get(curNode.id)) {
      stack<node> nodesToVisit;
      nodesToVisit.push(curNode);
      stack<Iterator<edge> *> neighboursToVisit;
      neighboursToVisit.push(graph->getOutEdges(curNode));

      while (!nodesToVisit.empty()) {
        node curNode = nodesToVisit.top();
        Iterator<edge> *ite = neighboursToVisit.top();

        // check if dfs traversal of curNode neighbours is finished
        if (!ite->hasNext()) {
          // unstack curNode
          nodesToVisit.pop();
          // delete & unstack neighbours iterator
          delete neighboursToVisit.top();
          neighboursToVisit.pop();
          // mark curNode as to be skipped
          // during further exploration
          finished.set(curNode.id, true);
        } else {
          visited.set(curNode.id, true);

          // loop on remaining neighbours
          while (ite->hasNext()) {
            edge tmp = ite->next();
            node neighbour = graph->target(tmp);

            // check if we are already in the exploration
            // of the neighbours of neighbour
            if (visited.get(neighbour.id)) {
              if (!finished.get(neighbour.id)) {
                // found a cycle
                result = false;

                if (obstructionEdges != nullptr) {
                  obstructionEdges->push_back(tmp);
                } else {
                  // it is finished if we don't need
                  // to collect obstruction edges
                  break;
                }
              }
            } else {
              // found a new neighbour to explore
              // go deeper in our dfs traversal
              nodesToVisit.push(neighbour);
              neighboursToVisit.push(graph->getOutEdges(neighbour));
              break;
            }
          }

          // it may be finished if we don't need
          // to collect obstruction edges
          if ((!result) && (obstructionEdges == nullptr)) {
            break;
          }
        }
      }

      // it may be finished if we don't need
      // to collect obstruction edges
      if ((!result) && (obstructionEdges == nullptr)) {
        // don't forget to delete remaining iterators
        while (!neighboursToVisit.empty()) {
          delete neighboursToVisit.top();
          neighboursToVisit.pop();
        }

        break;
      }
    }
  }

  return result;
}
