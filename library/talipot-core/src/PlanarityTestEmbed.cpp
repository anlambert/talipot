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

#include <talipot/PlanarityTestImpl.h>
#include <talipot/MapIterator.h>
#include <talipot/StableIterator.h>
#include <talipot/Graph.h>

using namespace std;
using namespace tlp;

void sortEdges(Graph *graph, const vector<edge> &order, node_hash_map<edge, edge> &rev) {
  map<node, vector<edge>> graphMap;

  for (auto e : order) {
    auto &v = graphMap[graph->source(e)];
    v.push_back(e);
    v.push_back(rev[e]);
  }

  for (const auto &[n, edges] : graphMap) {
    graph->setEdgeOrder(n, edges);
  }
}
//=================================================================
/*
 * Embeds all back-edges from T's root and all remaining edges in G that weren't
 * embedded yet.
 * Preconditions:
 * - G is a graph with n nodes;
 * - G is biconnected.
 */
void PlanarityTestImpl::embedRoot(Graph *sG, int n) {
  if (n <= 2) {
    return;
  }

  list<node> traversedNodes;
  edge e;
  node r = nodeWithDfsPos.get(n), u;
  state.set(r.id, VISITED);

  for (auto e : stableIterator(sG->getOutEdges(r))) {
    u = sG->target(e);

    if (dfsPosNum.get(u.id) < dfsPosNum.get(r.id) && isBackEdge(sG, e)) {

      listBackEdges[r].push_back(edgeReversal(e));

      while (state.get(u.id) != VISITED) {
        // path compression;
        if (isCNode(parent.get(u.id))) {
          findActiveCNode(u, r, traversedNodes);
        }

        state.set(u.id, VISITED);
        traversedNodes.push_back(u);
        u = parent.get(u.id);
      }
    }
  }

  for (auto n : traversedNodes) {
    state.set(n.id, NOT_VISITED);
  }

  vector<edge> backEdge;
  int nH = sortBackEdgesByDfs(sG, r, r, listBackEdges[r], backEdge);

  BmdList<edge> el1, l1;

  for (int i = 1; i <= nH; ++i) {
    e = backEdge[i];

    if (e != NULL_EDGE) {
      node predU = sG->source(e);
      node u = parent.get(predU.id);
      embedList[r].push(e);
      embedList[r].append(edgeReversal(e));

      while (state.get(predU.id) == NOT_VISITED) {
        state.set(predU.id, VISITED);

        if (isCNode(u)) {
          u = activeCNodeOf(false, u);
          l1.conc(embedList[u]);
          u = parent.get(u.id);
        } else {
          l1.append(edgeReversal(T0EdgeIn.get(predU.id)));
          l1.append(T0EdgeIn.get(predU.id));
        }

        predU = u;
        u = parent.get(u.id);
      }

      l1.conc(el1);
      l1.swap(el1);
    }
  }

  el1.conc(embedList[r]);
  el1.swap(embedList[r]);

  vector<edge> finalEmbList;

  BmdListIt<edge> itBm(embedList[r]);

  while (itBm.hasNext()) {
    finalEmbList.push_back(itBm.next());
  }

  sortEdges(sG, finalEmbList, reversalEdge);
}
//=================================================================
/*
 * Calculates a partial embedding for the 2-connected component represented by
 * new_cnode (embed_list[new_cnode]).
 * Preconditions:
 * - u is not a root of T;
 * - state[u] == NOT_VISITED for all nodes u in T_w;
 * - for all nodes u in T_w, has_back_edge[u] == false;
 * - T_w is biconnected.
 */
void PlanarityTestImpl::calculatePartialEmbedding(Graph *sG, node w, node newCNode,
                                                  list<edge> &listBackEdges,
                                                  list<node> &terminalNodes) {
  list<node> traversedNodes, listRepresentants;
  assert(!terminalNodes.empty());

  switch (terminalNodes.size()) {
  case 1: {
    map<node, node> backEdgeRepresentant;

    node term = terminalNodes.front();

    // marks as VISITED all nodes in the boundary cycle;
    markPathInT(term, w, backEdgeRepresentant, traversedNodes);

    map<node, list<edge>> bEdgesRepres = groupBackEdgesByRepr(
        sG, listBackEdges, backEdgeRepresentant, traversedNodes, listRepresentants);
    list<node> toEmbedLater;

    toEmbedLater =
        embedUpwardT(true, term, w, sG, w, bEdgesRepres, traversedNodes, embedList[newCNode]);

    // embeds all nodes in to_embed_later;
    for (auto t : toEmbedLater) {
      BmdList<edge> el1;
      embedBackEdges(false, sG, t, traversedNodes, bEdgesRepres[t], el1);
      el1.reverse();
      el1.conc(embedList[newCNode]);
      el1.swap(embedList[newCNode]);
      embedList[newCNode].conc(listBackEdgesOutW);
    }

  } break;

  case 2: {
    map<node, node> backEdgeRepresentant;
    node term1 = terminalNodes.front(), term2 = terminalNodes.back(), t1 = term1, t2 = term2,
         m = lcaBetweenTermNodes(t1, t2);

    // makes term1 with lowest dfspos_num
    // (as in calculate_new_RBC);
    if (isCNode(t1)) {
      t1 = parent.get(t1.id);
    }

    if (isCNode(t2)) {
      t2 = parent.get(t2.id);
    }

    assert(!(isCNode(t1) || isCNode(t2)));

    if (dfsPosNum.get(t1.id) > dfsPosNum.get(t2.id)) {
      swapNode(term1, term2);
    }

    t1 = lastPNode(term1, m);

    markPathInT(term2, w, backEdgeRepresentant, traversedNodes);
    markPathInT(term1, t1, backEdgeRepresentant, traversedNodes);

    map<node, list<edge>> bEdgesRepres = groupBackEdgesByRepr(
        sG, listBackEdges, backEdgeRepresentant, traversedNodes, listRepresentants);
    list<node> toEmbedLater;
    toEmbedLater =
        embedUpwardT(true, term2, w, sG, w, bEdgesRepres, traversedNodes, embedList[newCNode]);

    for (auto n : toEmbedLater) {
      embedBackEdges(true, sG, n, traversedNodes, bEdgesRepres[n], embedList[newCNode]);
    }

    if (t1 != m) {
      BmdList<edge> el1;
      embedBackEdges(false, sG, t1, traversedNodes, bEdgesRepres[t1], el1);
      el1.reverse();
      el1.conc(embedList[newCNode]);
      el1.swap(embedList[newCNode]);
      embedList[newCNode].conc(listBackEdgesOutW);
    }

    BmdList<edge> el1;
    embedUpwardT(false, term1, t1, sG, w, bEdgesRepres, traversedNodes, el1);
    el1.reverse();
    el1.conc(embedList[newCNode]);
    el1.swap(embedList[newCNode]);

    embedList[newCNode].conc(listBackEdgesOutW);
  } break;

  default: // sg is not planar (abort procedure);
    return;
  }

  // restores auxiliary variables;
  for (auto n : traversedNodes) {
    state.set(n.id, NOT_VISITED);
  }

  for (auto n : listRepresentants) {
    hasBackEdge.set(n.id, false);
  }
}
//=================================================================
/*
 * For all node u in path from t to w upward in T,
 * initializes backedge_representant[u] = u,
 * marks u as VISITED and appends u to list traversed_nodes.
 */
void PlanarityTestImpl::markPathInT(node t, node w, map<node, node> &backEdgeRepresentant,
                                    list<node> &traversedNodes) {
  state.set(w.id, VISITED);
  backEdgeRepresentant[w] = w;
  traversedNodes.push_back(w);
  node u = t;

  while (state.get(u.id) == NOT_VISITED) {
    state.set(u.id, VISITED);
    traversedNodes.push_back(u);
    backEdgeRepresentant[u] = u;
    u = parent.get(u.id);
  }
}
//=================================================================
/*
 * For each back-edge e in T_w (list_back_edges) calculates
 * backedge_representant[source(e)] and returns all back-edges grouped by
 * backedge_representant.
 * Marks as VISITED all nodes that are a representant of a back-edge.
 * Appends all nodes marked as VISITED in list traversed_nodes.
 * Precondition:
 * - for all nodes u in T_w, has_back_edge[u] == false.
 */
map<node, list<edge>> PlanarityTestImpl::groupBackEdgesByRepr(Graph *sG, list<edge> &listBackEdges,
                                                              map<node, node> &backEdgeRepresentant,
                                                              list<node> &traversedNodes,
                                                              list<node> &listRepresentants) {
  list<node> nl;

  for (auto e : listBackEdges) {
    list<node> S;
    node u = sG->source(e), pNode = u; // pNode is never a c-node;

    while (state.get(u.id) == NOT_VISITED) {
      if (!isCNode(u)) {
        pNode = u;
      }

      if (state.get(u.id) == NOT_VISITED) {
        state.set(u.id, VISITED);
        nl.push_back(u);
        S.push_front(u);
      }

      u = parent.get(u.id);
    }

    node repr;

    if (!isCNode(backEdgeRepresentant[u])) {
      repr = backEdgeRepresentant[u];
    } else {
      repr = pNode;
      traversedNodes.push_back(repr);
    }

    for (auto u : S) {
      if (isCNode(u) && isCNode(parent.get(u.id))) {
        backEdgeRepresentant[u] = backEdgeRepresentant[parent.get(u.id)];
      } else {
        backEdgeRepresentant[u] = repr;
      }
    }

    if (!hasBackEdge.get(repr.id)) {
      hasBackEdge.set(repr.id, true);
      listRepresentants.push_back(repr);
    }
  }

  map<node, list<edge>> listEdges;

  for (auto e : listBackEdges) {
    node v = sG->source(e);
    listEdges[backEdgeRepresentant[v]].push_back(e);
  }

  for (auto n : nl) {
    state.set(n.id, NOT_VISITED);
  }

  for (auto n : listRepresentants) {
    state.set(n.id, VISITED);
  }

  return listEdges;
}
//=================================================================
/*
 * Embeds all edges in path P' from t1 to t2 and all back-edges with
 * representant in P' (emb_list).
 * emb_back_edges_out_w is false only in case of 2 terminal
 * nodes, for one of the two terminals (see calculate_partial_embedding).
 * Returns an ordered list of all representants to be embedded later
 * (see add_old_cnode_to_embedding).
 * Precondition:
 * - if u is a node in P' and e is a back-edge with representant u then e is in list
 *   b_edges_repres[u].
 */
list<node> PlanarityTestImpl::embedUpwardT(bool embBackEdgesOutW, node t1, node t2, Graph *sG,
                                           node w, map<node, list<edge>> &bEdgesRepres,
                                           list<node> &traversedNodes,
                                           tlp::BmdList<edge> &embList) {
  list<node> toEmbedLater;
  node u = t1, predU = NULL_NODE;

  while (predU != t2) {
    if (isCNode(u)) {
      node f = predU, oldCNode = activeCNodeOf(false, u);
      addOldCNodeToEmbedding(embBackEdgesOutW, sG, w, oldCNode, f, bEdgesRepres, traversedNodes,
                             toEmbedLater, embList);
      u = parent.get(oldCNode.id);

      if (u == t2) {
        return toEmbedLater;
      }
    } else {
      if (predU != NULL_NODE) {

        embList.push(edgeReversal(T0EdgeIn.get(predU.id)));

        if (u != w) {
          embList.push(T0EdgeIn.get(predU.id));
        } else {
          embList.append(T0EdgeIn.get(predU.id));
        }
      }
    }

    if (hasBackEdge.get(u.id) && u != t2) {
      embedBackEdges(embBackEdgesOutW, sG, u, traversedNodes, bEdgesRepres[u], embList);
    }

    predU = u;
    u = parent.get(u.id);
  }

  return toEmbedLater;
}
//=================================================================
/*
 * Moves embedding of oldCNode to embList - note that oldCNode may
 * flip - and embeds all edges in path from w (starting with a back-edge from w) and
 * ending in a node in RBC[oldCNode].
 * All representants to be embedded later is ordered and inserted in toEmbedLater.
 * emb_back_edges_out_w is false only in case of 2 terminal
 * nodes, for one of the two terminals (see calculate_partial_embedding).
 * Preconditions:
 * - for a back-edge e, if u is representant of source(e) and  u in RBC[oldCNode],
 *   then e is in list b_edges_repres[u];
 * - the first element in list RBC[oldCNode] is parent[oldCNode];
 * - if u != nil then u is a node in RBC[oldCNode] s.t. label_b[u] > dfspos_num[w]
 *   and there exists a path from u to w that doesn't contain any node in the
 *   2-connected component represented by oldCNode, except u.
 */
void PlanarityTestImpl::addOldCNodeToEmbedding(bool embBackEdgesOutW, Graph *sG, node w,
                                               node oldCNode, node u,
                                               map<node, list<edge>> &bEdgesRepres,
                                               list<node> &traversedNodes, list<node> &toEmbedLater,
                                               tlp::BmdList<edge> &embList) {

  BmdLink<node> *it = RBC[oldCNode].firstItem();
  BmdLink<node> *itl = RBC[oldCNode].cyclicPred(it, nullptr);
  BmdLink<node> *itr = RBC[oldCNode].cyclicSucc(it, nullptr);
  node jl = itl->getData();
  node jr = itr->getData();

  list<node> listNodesL, listNodesR;
  // goes to the left;
  BmdLink<node> *aux = nullptr;
  BmdLink<node> *s = it;

  while (labelB.get(jl.id) <= dfsPosNum.get(w.id)) {
    assert(jl != u);

    if (labelB.get(jl.id) == dfsPosNum.get(w.id)) {
      listNodesL.push_back(jl);
    }

    aux = itl;
    itl = RBC[oldCNode].cyclicPred(itl, s);
    s = aux;
    jl = itl->getData();
  }

  // goes to the right;
  BmdLink<node> *p = it;

  while (labelB.get(jr.id) <= dfsPosNum.get(w.id)) {
    assert(jr != u);

    if (labelB.get(jr.id) == dfsPosNum.get(w.id)) {
      listNodesR.push_back(jr);
    }

    aux = itr;
    itr = RBC[oldCNode].cyclicSucc(itr, p);
    p = aux;
    jr = itr->getData();
  }

  // checks if need to flip oldCNode;
  // u == nil when oldCNode is a terminal node;
  bool flipped =
      ((!listNodesL.empty() && (jl == u || u == NULL_NODE)) || (jr != u && u != NULL_NODE));

  if (flipped) {
    listNodesL.swap(listNodesR);
  }

  // embeds all back-edges in oldCNode;
  listNodesR.reverse();
  node t;

  for (auto t : listNodesR) {
    embedBackEdges(embBackEdgesOutW, sG, t, traversedNodes, bEdgesRepres[t], embList);
  }

  if (flipped) {
    embedList[oldCNode].reverse();
  }

  embedList[oldCNode].conc(embList);
  embedList[oldCNode].swap(embList);

  toEmbedLater.splice(toEmbedLater.begin(), listNodesL);
}
//=================================================================
/*
 * Embeds all edges in a path from w, starting in a back-edge in listBackEdges
 * and ending in repr.
 * Inserts all embedded edges in list embList.
 * Appends all nodes marked as VISITED in list traversed_nodes.
 * emb_back_edges_out_w is false only in case of 2 terminal
 * nodes, for one of the two terminals (see calculate_partial_embedding).
 */
void PlanarityTestImpl::embedBackEdges(bool embBackEdgesOutW, Graph *sG, node repr,
                                       list<node> &traversedNodes, list<edge> &listBackEdges,
                                       tlp::BmdList<edge> &embList) {
  if (listBackEdges.empty()) {
    return;
  }

  edge e;
  BmdList<edge> el1, l1, wl1;
  node w = sG->target(listBackEdges.front());

  vector<edge> backEdge;
  int n = sortBackEdgesByDfs(sG, w, repr, listBackEdges, backEdge);

  for (int i = 1; i <= n; ++i) {
    e = backEdge[i];

    if (e != NULL_EDGE) {
      if (embBackEdgesOutW) {
        wl1.append(edgeReversal(e));
      } else {
        listBackEdgesOutW.push(edgeReversal(e));
      }
    }
  }

  for (int i = n; i >= 1; i--) {
    e = backEdge[i];

    if (e != NULL_EDGE) {
      l1.push(e);
      node predU = sG->source(e);
      node u = parent.get(predU.id);

      while (state.get(predU.id) == NOT_VISITED) {
        state.set(predU.id, VISITED);
        traversedNodes.push_back(predU);

        if (isCNode(u)) {
          u = activeCNodeOf(false, u);
          embedList[u].conc(l1);
          embedList[u].swap(l1);
          u = parent.get(u.id);
        } else {
          l1.push(T0EdgeIn.get(predU.id));
          l1.push(edgeReversal(T0EdgeIn.get(predU.id)));
        }

        predU = u;
        u = parent.get(u.id);
      }

      el1.conc(l1);
    }
  }

  el1.conc(embList);
  el1.swap(embList);
  embList.conc(wl1);
}
//=================================================================
/*
 * Sorts all back-edges with representant repr by depth first search traversal in
 * (T_repr - P), denoted as T_v^*.
 */
int PlanarityTestImpl::sortBackEdgesByDfs(Graph *sG, node, node repr, list<edge> &listBackEdges,
                                          vector<edge> &backEdge) {
  // constructs a DFS tree of T_v^* to sort back-edges to embed;
  Graph *D = tlp::newGraph();

  list<node> listNodes, listCNodes;
  map<node, node> nodeInD, nodeInG;

  nodeInD[repr] = D->addNode();
  nodeInG[nodeInD[repr]] = repr;

  for (auto e : listBackEdges) {
    node u = sG->source(e);
    node predU = NULL_NODE;

    while (state.get(u.id) == NOT_VISITED) {
      if (isCNode(u)) {
        u = activeCNodeOf(false, u);

        if (state.get(u.id) == NOT_VISITED) {
          listCNodes.push_back(u);
        }
      }

      if (state.get(u.id) == NOT_VISITED) {
        state.set(u.id, VISITED);
        listNodes.push_back(u);
        nodeInD[u] = D->addNode();
        nodeInG[nodeInD[u]] = u;
      }

      if (predU != NULL_NODE) {
        D->addEdge(nodeInD[u], nodeInD[predU]);
      }

      predU = u;
      u = parent.get(u.id);
    }

    if (predU != NULL_NODE) {
      D->addEdge(nodeInD[u], nodeInD[predU]);
    }
  }

  for (auto n : listNodes) {
    state.set(n.id, NOT_VISITED);
  }

  map<node, bool> isInD;

  for (auto v : listCNodes) {
    BmdListIt<node> bmItn(RBC[v]);

    while (bmItn.hasNext()) {
      isInD[bmItn.next()] = false;
    }

    list<edge> el;

    for (auto e : stableIterator(D->getOutEdges(nodeInD[v]))) {
      isInD[nodeInG[D->target(e)]] = true;
      el.push_back(e);
    }

    for (auto e : el) {
      D->delEdge(e);
    }

    BmdListRevIt<node> itR(RBC[v]);

    while (itR.hasNext()) {
      node u = itR.next();

      if (isInD[u]) {
        D->addEdge(nodeInD[v], nodeInD[u]);
      }
    }
  }

  // orders back-edges by DFS number
  MutableContainer<int> dfsPos;
  dfsPos.setAll(0);

  posDFS(D, dfsPos);
  int tot = D->numberOfNodes();
  backEdge.resize(tot + 1);
  backEdge[0] = NULL_EDGE;

  for (int i = 1; i <= tot; ++i) {
    backEdge[i] = NULL_EDGE;
  }

  for (auto e : listBackEdges) {
    node v = sG->source(e);
    backEdge[dfsPos.get(nodeInD[v].id)] = e;
  }

  delete D;
  return tot;
}
//=================================================================
/*
 * Algebraic criteria to check the plane map...
 */
bool PlanarityTestImpl::isPlanarEmbedding(const tlp::Graph *sG) {
  int n = sG->numberOfNodes();

  if (n == 1) {
    return true;
  }

  int m = sG->numberOfEdges();
  uint count = 0;
  MutableContainer<char> considered;
  MutableContainer<bool> sens;
  considered.setAll(0);
  sens.setAll(false);
  int fc = 0;

  for (int k = 0; k < 2; ++k) {

    for (auto e : sG->edges()) {

      if (considered.get(e.id) < 2) {
        count = 0;
        edge e1 = e;
        node n, n_tmp;

        if (sens.get(e.id)) {
          n = sG->target(e1);
        } else {
          n = sG->source(e1);
        }

        n_tmp = n;

        do {
          considered.add(e1.id, 1);
          EdgeMapIterator it(sG, e1, n);
          e1 = it.next();
          n = sG->opposite(e1, n);

          if (sG->source(e1) == n) {
            sens.set(e1.id, true);
          }

          ++count;

          if (count > 2 * sG->numberOfEdges() + 1) {
            break; // needed for trees or non biconnected graphs
          }
        } while ((e1 != e) || (n != n_tmp));

        ++fc;
      }
    }
  }

  return fc == m - n + 2;
}
//=================================================================
