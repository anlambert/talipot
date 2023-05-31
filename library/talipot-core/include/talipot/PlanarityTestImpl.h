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

#ifndef TALIPOT_PLANARITY_TEST_IMPL_H
#define TALIPOT_PLANARITY_TEST_IMPL_H

#include <map>
#include <list>
#include <vector>
#include <unordered_map>

#include <talipot/Edge.h>
#include <talipot/MutableContainer.h>
#include <talipot/BmdList.h>
#include <talipot/config.h>
#include <talipot/Node.h>

namespace tlp {
class Graph;
enum { NOT_VISITED, VISITED, TERMINAL, VISITED_IN_RBC };
static const node NULL_NODE = node();
static const edge NULL_EDGE = edge();

class TLP_SCOPE PlanarityTestImpl {

public:
  PlanarityTestImpl(Graph *sg);
  bool isPlanar(bool embedsg = false);
  static bool isPlanarEmbedding(const Graph *sG);
  std::list<edge> getObstructions();

private:
  bool compute(Graph *);
  void init();
  void restore();
  edge edgeReversal(edge e);
  void makeBidirected(Graph *sG);
  void swapNode(node &n1, node &n2);
  void findTerminalNodes(Graph *sG, node n, std::list<node> &listOfComponents,
                         std::map<node, std::list<node>> &terminalNodes);
  bool findObstruction(Graph *sG, node n, std::list<node> &terminalNodes);
  void setInfoForNewCNode(Graph *sG, node n, node newCNode, std::list<node> &terminalNodes);
  node findActiveCNode(node, node, std::list<node> &);
  void preProcessing(Graph *);
  tlp::BmdLink<node> *searchRBC(int, tlp::BmdLink<node> *, node, std::list<node> &);
  bool isT0Edge(Graph *, edge);
  bool isBackEdge(Graph *, edge);
  bool isCNode(node);
  void sortNodesIncreasingOrder(Graph *, MutableContainer<int> &, std::vector<node> &);
  node activeCNodeOf(bool, node);
  void addOldCNodeRBCToNewRBC(node, node, node, node, node, BmdList<node> &);
  void updateLabelB(node);
  void calcNewRBCFromTerminalNode(node, node, node, node, BmdList<node> &);
  node lastPNode(node, node);
  node lcaBetween(node, node, const MutableContainer<node> &);
  node lcaBetweenTermNodes(node, node);
  void calculateNewRBC(Graph *, node, node, std::list<node> &);
  node findNodeWithLabelBGreaterThanDfsN(bool, Graph *, node, node);
  void setPossibleK33Obstruction(node, node, node, node);
  bool testCNodeCounter(Graph *, node, node, node, node, node &, node &);
  bool testObstructionFromTerminalNode(Graph *, node, node, node);

  bool listEdgesUpwardT0(node n1, node n2);
  void extractBoundaryCycle(Graph *sG, node cNode, std::list<edge> &listEdges);

  void obstrEdgesTerminal(Graph *G, node w, node t, node u);
  void addPartOfBc(Graph *sG, node cNode, node n1, node n2, node n3);
  void sortByLabelB(node &n1, node &n2, node &n3);
  void obstrEdgesPNode(Graph *sG, node p, node u);
  void calcInfo3Terminals(node &t1, node &t2, node &t3, int &countMin, int &countF, node &cNode,
                          node &q);
  void obstructionEdgesT0(Graph *sG, node w, node t1, node t2, node t3, node v);
  void obstructionEdgesCountMin1(Graph *sG, node n, node cNode, node t1, node t2, node t3);
  void obstructionEdgesCountMin23(Graph *sG, node n, node cNode, node t1, node t2, node t3, node q,
                                  node v);

  void obstructionEdgesK5(Graph *sG, node w, node cNode, node t1, node t2, node t3);
  void obstructionEdgesPossibleObstrConfirmed(Graph *sG, node w, node t, node v);
  void obstructionEdgesCNodeCounter(Graph *sG, node cNode, node w, node jl, node jr, node t1,
                                    node t2);

  void embedRoot(Graph *sG, int n);
  void calculatePartialEmbedding(Graph *sG, node w, node newCNode, std::list<edge> &listBackEdges,
                                 std::list<node> &terminalNodes);
  void markPathInT(node t, node w, std::map<node, node> &backEdgeRepresentant,
                   std::list<node> &traversedNodes);
  std::map<node, std::list<edge>> groupBackEdgesByRepr(Graph *sG, std::list<edge> &listBackEdges,
                                                       std::map<node, node> &backEdgeRepresentant,
                                                       std::list<node> &traversedNodes,
                                                       std::list<node> &listRepresentants);
  std::list<node> embedUpwardT(bool embBackEdgesOutW, node t1, node t2, Graph *sG, node w,
                               std::map<node, std::list<edge>> &bEdgesRepres,
                               std::list<node> &traversedNodes, BmdList<edge> &embList);
  void addOldCNodeToEmbedding(bool embBackEdgesOutW, Graph *sG, node w, node oldCNode, node u,
                              std::map<node, std::list<edge>> &bEdgesRepres,
                              std::list<node> &traversedNodes, std::list<node> &toEmbedLater,
                              BmdList<edge> &embList);
  void embedBackEdges(bool embBackEdgesOutW, Graph *sG, node repr, std::list<node> &traversedNodes,
                      std::list<edge> &listBackEdges, BmdList<edge> &embList);
  int sortBackEdgesByDfs(Graph *sG, node w, node repr, std::list<edge> &listBackEdges,
                         std::vector<edge> &backEdge);

  Graph *sg;
  int totalCNodes;
  bool embed, biconnected;
  node lastNodeInQLinha;
  std::unordered_map<edge, edge> bidirectedEdges;
  std::unordered_map<edge, edge> reversalEdge;

  // auxiliary variable to help detecting obstruction;
  node cNodeOfPossibleK33Obstruction;

  // for each node u in T, children is the list of u's children
  // ordered in decreasing order by label_b
  // (it helps to update label_b's in constant time)
  std::unordered_map<node, std::list<node>> childrenInT0;

  // for each 2-connected component represented by r,
  // list_back_edges[r] is the list of all back-edges in component r
  // (it helps to calculate an embedding of G, if G is planar);
  std::map<node, std::list<edge>> listBackEdges;

  // the Representative Boundary Cycle for each c-node;
  std::map<node, BmdList<node>> RBC;

  // for each node u in G, the algorithm calculates the
  // clockwise ordering of edges with source u around u, such that
  // G.sort_edges(embed_list) is a plane map, if it exists
  std::unordered_map<node, BmdList<edge>> embedList;

  // to avoid path compression of c-nodes;
  std::unordered_map<tlp::BmdLink<node> *, node> activeCNode;

  // (it helps to calculate an embedding of G, if G is planar, in
  // case of 2 terminal nodes);
  BmdList<edge> listBackEdgesOutW;

  // list of nodes in an obstruction found in G if G is not planar
  // (it helps to calculate "obstruction_edges");
  std::list<node> obstructionNodes;

  // list of edges in an obstruction found int G if G is not planar;
  std::list<edge> obstructionEdges;

  MutableContainer<tlp::BmdLink<node> *> ptrItem;

  MutableContainer<int> dfsPosNum;

  MutableContainer<node> nodeWithDfsPos;

  // to help calculate an embedding or an obstruction;
  MutableContainer<edge> T0EdgeIn;

  // p0 saves initial DFS tree T_0 of G;
  MutableContainer<node> parent;
  MutableContainer<node> p0;

  // for each node u in T,
  // largest_neighbor[u] = max{dfspos_num[v] : v is a neighbor of u in G};
  MutableContainer<int> largestNeighbor;

  // for each node u in T,
  // label_b[u] = max{largest_neighbor[v] : v is a descendant of u in T_u}
  // where T_u is the subtree of T rooted at u;
  MutableContainer<int> labelB;

  // for each node u in T, node_label_b[u] = v
  // where v is a descendant of u in T and largest_neighbor[v] == label_b[u]
  // (it helps to find an obstruction in G, if G is not planar);
  MutableContainer<node> nodeLabelB;

  // to help find the lca between two terminal nodes;
  MutableContainer<node> lastVisited;

  // given w, for each terminal node u of w, neighbor_w_terminal[u] is
  // a descendant of u that is a neighbor of w in G;
  MutableContainer<node> neighborWTerminal;

  // to help search for terminal nodes and calculate an embedding of G if G is
  // planar (states: VISITED, NOT_VISITED, TERMINAL);
  MutableContainer<int> state;

  // for each (active) c-node d, counter[d] is the number of children of d
  // with a descendant that are neighbor of w in G;
  MutableContainer<int> counter;

  // (it helps to calculate an embedding of G, if G is planar);
  MutableContainer<bool> hasBackEdge;
  uint numberOfNodesInG;
};
}

std::list<tlp::edge> posDFS(tlp::Graph *sG, tlp::MutableContainer<int> &dfsPos);

#endif // TALIPOT_PLANARITY_TEST_IMPL_H
