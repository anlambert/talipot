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

#include <talipot/BooleanProperty.h>
#include <talipot/ConnectedTest.h>
#include <talipot/TreeTest.h>
#include <talipot/AcyclicTest.h>
#include <talipot/GraphTools.h>

using namespace std;
using namespace tlp;

//=================================================================
class TreeTestListener : public Observable {
public:
  // override of Observable::treatEvent to remove the cached result for a graph if it is modified.
  void treatEvent(const Event &) override;

  /**
   * @brief Stored results for graphs. When a graph is updated, its entry is removed from the map.
   **/
  flat_hash_map<const Graph *, bool> resultsBuffer;
};

void TreeTestListener::treatEvent(const Event &evt) {
  const auto *gEvt = dynamic_cast<const GraphEvent *>(&evt);

  if (gEvt) {
    Graph *graph = gEvt->getGraph();

    switch (gEvt->getType()) {
    case GraphEventType::TLP_ADD_NODE:
    case GraphEventType::TLP_DEL_NODE:
    case GraphEventType::TLP_ADD_EDGE:
    case GraphEventType::TLP_DEL_EDGE:
    case GraphEventType::TLP_REVERSE_EDGE:
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
static TreeTestListener instance;
//====================================================================
static bool treeTest(const Graph *graph) {
  if (graph->numberOfEdges() != graph->numberOfNodes() - 1) {
    return false;
  }

  bool rootNodeFound = false;

  for (auto tmp : graph->nodes()) {
    if (graph->indeg(tmp) > 1) {
      return false;
    }

    if (graph->indeg(tmp) == 0) {
      if (rootNodeFound) {
        return false;
      } else {
        rootNodeFound = true;
      }
    }
  }

  return AcyclicTest::acyclicTest(graph);
}
//====================================================================
bool TreeTest::isTree(const tlp::Graph *graph) {
  if (const auto it = instance.resultsBuffer.find(graph); it != instance.resultsBuffer.end()) {
    return it->second;
  }

  graph->addListener(instance);
  return instance.resultsBuffer[graph] = treeTest(graph);
}
//====================================================================
// Determines if a graph is a topological tree.  This means that
// if the graph was undirected, there would be no cycle
bool TreeTest::isFreeTree(const tlp::Graph *graph) {
  auto nbNodes = graph->numberOfNodes();
  auto nbEdges = graph->numberOfEdges();
  return nbNodes && (nbEdges == nbNodes - 1) && ConnectedTest::isConnected(graph);
}
//====================================================================
// simple structure to implement
// the further isFreeTree dfs loop
struct dfsFreeTreeStruct {
  node curRoot;
  node cameFrom;
  Iterator<node> *neighbours;

  dfsFreeTreeStruct(node root = node(), node from = node(), Iterator<node> *it = nullptr)
      : curRoot(root), cameFrom(from), neighbours(it) {}
  ~dfsFreeTreeStruct() {
    if (neighbours) {
      delete neighbours;
    }
  }
};
//====================================================================
// simple structure to implement
// the further makeRootedTree dfs loop
struct dfsMakeRootedTreeStruct {
  node curRoot;
  node cameFrom;
  Iterator<edge> *ioEdges;

  dfsMakeRootedTreeStruct(node root, node from, Iterator<edge> *it)
      : curRoot(root), cameFrom(from), ioEdges(it) {}
};

// given that a graph is topologically a tree, The function turns graph
// into a directed tree.
static void makeRootedTree(Graph *graph, node curRoot, vector<edge> *reversedEdges) {
  stack<dfsMakeRootedTreeStruct> dfsLevels;
  dfsMakeRootedTreeStruct curParams(curRoot, curRoot, graph->getInOutEdges(curRoot));
  dfsLevels.push(curParams);

  // dfs loop
  while (!dfsLevels.empty()) {
    curParams = dfsLevels.top();
    curRoot = curParams.curRoot;
    node cameFrom = curParams.cameFrom;
    Iterator<edge> *ioEdges = curParams.ioEdges;

    if (!ioEdges->hasNext()) {
      delete ioEdges;
      dfsLevels.pop();
    } else {
      // loop on remaining ioEdges
      while (ioEdges->hasNext()) {
        edge curEdge = ioEdges->next();
        node opposite = graph->opposite(curEdge, curRoot);

        if (opposite != cameFrom) {
          if (graph->target(curEdge) == curRoot) {
            graph->reverse(curEdge);

            if (reversedEdges) {
              reversedEdges->push_back(curEdge);
            }
          }

          // go deeper in the dfs traversal
          curParams.curRoot = opposite;
          curParams.cameFrom = curRoot;
          curParams.ioEdges = graph->getInOutEdges(opposite);
          dfsLevels.push(curParams);
          break;
        } // end if
      } // end while
    } // end else
  } // end while
}
//====================================================================
// Turns a topological tree graph into a directed tree starting at
// the node root.
void TreeTest::makeRootedTree(Graph *graph, node root) {
  graph->removeListener(instance);
  instance.resultsBuffer.erase(graph);

  if (!graph->isElement(root)) {
    tlp::warning() << "makeRootedTree:  Passed root is not an element of the graph" << endl;
    return;
  } // end if

  if (!TreeTest::isFreeTree(graph)) {
    tlp::warning()
        << "makeRootedTree: The graph is not topologically a tree, so rooted tree cannot be made."
        << endl;
    return;
  } // end if

  ::makeRootedTree(graph, root, nullptr);
  assert(treeTest(graph));
} // end makeRootedTree

//====================================================================
// this function is for internal purpose only
static Graph *computeTreeInternal(Graph *graph, Graph *rGraph, bool isConnected,
                                  PluginProgress *pluginProgress,
                                  vector<edge> *reversedEdges = nullptr) {
  // nothing todo if the graph is already
  if (TreeTest::isTree(graph)) {
    return graph;
  }

  // if needed, create a clone of the graph
  // as a working copy
  Graph *gClone = graph;

  if (!rGraph) {
// the graph attribute used to store the clone
#define CLONE_NAME "CloneForTree"
    rGraph = gClone = graph->addCloneSubGraph(CLONE_NAME);
// the graph attribute used to store added root node
#define CLONE_ROOT "CloneRoot"
    rGraph->setAttribute(CLONE_ROOT, node());
// the graph attribute used to store the reversed edge
#define REVERSED_EDGES "ReversedEdges"
    reversedEdges = new vector<edge>;
    rGraph->setAttribute(REVERSED_EDGES, reversedEdges);
  }

  // add a node for an empty graph
  if (graph->isEmpty()) {
    rGraph->setAttribute(CLONE_ROOT, rGraph->addNode());
    return rGraph;
  }

  // if the graph is topologically a tree, make it rooted
  // using a 'center' of the graph as root
  if (TreeTest::isFreeTree(gClone)) {
    makeRootedTree(gClone, graphCenterHeuristic(gClone), reversedEdges);
    return gClone;
  }

  // if the graph is connected,
  // extract a spanning tree,
  // and make it rooted
  if (isConnected || ConnectedTest::isConnected(gClone)) {
    BooleanProperty treeSelection(gClone);
    selectSpanningTree(gClone, &treeSelection, pluginProgress);

    if (pluginProgress && pluginProgress->state() != ProgressState::TLP_CONTINUE) {
      return nullptr;
    }

    return computeTreeInternal(gClone->addSubGraph(&treeSelection), rGraph, true, pluginProgress,
                               reversedEdges);
  }

  // graph is not connected
  // compute the connected components's subgraphs
  auto components = ConnectedTest::computeConnectedComponents(rGraph);

  for (const auto &component : components) {
    rGraph->inducedSubGraph(component);
  }

  // create a new subgraph for the tree
  Graph *tree = rGraph->addSubGraph();
  node root = tree->addNode();
  rGraph->setAttribute(CLONE_ROOT, root);

  // connected components subgraphs loop
  for (Graph *gConn : rGraph->subGraphs()) {
    if (gConn == tree) {
      continue;
    }

    // compute a tree for each subgraph
    // add each element of that tree
    // to our main tree
    // and connect the main root to each
    // subtree root
    Graph *sTree = computeTreeInternal(gConn, rGraph, true, pluginProgress, reversedEdges);

    if (pluginProgress && pluginProgress->state() != ProgressState::TLP_CONTINUE) {
      return nullptr;
    }

    for (auto n : sTree->nodes()) {
      tree->addNode(n);

      if (sTree->indeg(n) == 0) {
        tree->addEdge(root, n);
      }
    }
    tree->addEdges(sTree->edges());
  }
  assert(treeTest(tree));
  return tree;
}

// the documented functions
Graph *TreeTest::computeTree(tlp::Graph *graph, PluginProgress *pluginProgress) {
  return computeTreeInternal(graph, nullptr, false, pluginProgress);
}

// this one revert the updates due to tree computation
void TreeTest::cleanComputedTree(tlp::Graph *graph, tlp::Graph *tree) {
  if (graph == tree) {
    return;
  }

  // get the subgraph clone
  Graph *sg = tree;
  string nameAtt("name");
  string name;
  sg->getAttribute<string>(nameAtt, name);

  while (name != CLONE_NAME) {
    sg = sg->getSuperGraph();
    sg->getAttribute<string>(nameAtt, name);
  }

  Graph *rg = graph->getRoot();
  // get its added root
  node root;
  sg->getAttribute<node>(CLONE_ROOT, root);

  // delete it if needed
  if (root.isValid()) {
    rg->delNode(root);
  }

  // delete the reversed edges if any
  vector<edge> *reversedEdges = nullptr;

  if (sg->getAttribute<vector<edge> *>(REVERSED_EDGES, reversedEdges)) {
    sg->removeAttribute(REVERSED_EDGES);

    for (auto e : *reversedEdges) {
      rg->reverse(e);
    }

    delete reversedEdges;
  }

  // delete the clone
  graph->delAllSubGraphs(sg);
}
