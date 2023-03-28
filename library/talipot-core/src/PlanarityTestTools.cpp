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

#if defined(__GNUC__) && __GNUC__ >= 4 && \
    ((__GNUC_MINOR__ == 2 && __GNUC_PATCHLEVEL__ >= 1) || (__GNUC_MINOR__ >= 3))
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

#include <talipot/PlanarityTestImpl.h>
#include <talipot/StableIterator.h>
#include <talipot/Graph.h>

using namespace std;
using namespace tlp;

//================================================================
edge PlanarityTestImpl::edgeReversal(edge e) {
  return reversalEdge[e];
}
//=================================================================
static void dfsAux(Graph *sG, node n, MutableContainer<int> &dfsPre, MutableContainer<int> &dfsPos,
                   list<edge> &dfsEdges, uint &preCount, uint &postCount) {
  dfsPre.set(n.id, ++preCount);
  for (auto e : sG->getOutEdges(n)) {
    node target = sG->target(e);

    if (dfsPre.get(target.id) == 0) {
      dfsEdges.push_back(e);
      dfsAux(sG, target, dfsPre, dfsPos, dfsEdges, preCount, postCount);
    }
  }

  dfsPos.set(n.id, ++postCount);
}
//=================================================================
list<edge> posDFS(Graph *sG, MutableContainer<int> &dfsPos) {
  list<edge> dfsEdges;
  MutableContainer<int> dfsPre;
  dfsPre.setAll(0);
  uint preCount = 0;
  uint postCount = 0;
  for (auto n : sG->nodes()) {
    if (dfsPre.get(n.id) == 0) {
      dfsAux(sG, n, dfsPre, dfsPos, dfsEdges, preCount, postCount);
    }
  }

  return dfsEdges;
}
//=================================================================
void PlanarityTestImpl::makeBidirected(Graph *sG) {
  for (auto e : stableIterator(sG->getEdges())) {
    const auto &[src, tgt] = sG->ends(e);
    edge newEdge = sG->addEdge(tgt, src);
    bidirectedEdges[newEdge] = e;
    reversalEdge[newEdge] = e;
    reversalEdge[e] = newEdge;
  }
}
//=================================================================
/*
 * Returns true for all edges e in T_0,
 *         false otherwise.
 */
bool PlanarityTestImpl::isT0Edge(Graph *g, edge e) {
  const auto &[src, tgt] = g->ends(e);
  edge e1 = T0EdgeIn.get(tgt.id);

  if (e1.isValid()) {
    const auto &[src1, tgt1] = g->ends(e1);

    if (src1 == src && tgt1 == tgt) {
      return true;
    }
  }

  e1 = T0EdgeIn.get(src.id);

  if (e1.isValid()) {
    const auto &[src1, tgt1] = g->ends(e1);
    return tgt1 == src && src1 == tgt;
  }

  return false;
}
//=================================================================
/*
 * Returns true for all edges e in g - T_0,
 *         false otherwise.
 */
bool PlanarityTestImpl::isBackEdge(Graph *g, edge e) {
  return (e.isValid() && !isT0Edge(g, e));
}
//=================================================================
/*
 * Returns in sortedNodes the nodes n of g sorted in increasing
 * order by value[n].
 * Precondition:
 * - value[n] <= numberOfNodes for all nodes n of g, where n is
 * the number of
 * nodes in g
 */
void PlanarityTestImpl::sortNodesIncreasingOrder(Graph *g, MutableContainer<int> &value,
                                                 vector<node> &sortedNodes) {

  // Counting sort;
  int numberOfNodes = g->numberOfNodes();
  vector<int> c(numberOfNodes + 1);

  for (int i = 1; i <= numberOfNodes; ++i) {
    c[i] = 0;
  }

  vector<node> a(numberOfNodes + 1);
  int j = 0;

  for (auto n : g->nodes()) {
    a[++j] = n;
  }

  for (int i = 1; i <= numberOfNodes; ++i) {
    uint tmp = value.get(a[i].id);
    ++c[tmp];
  }

  for (int i = 2; i <= numberOfNodes; ++i) {
    c[i] += c[i - 1];
  }

  for (int i = numberOfNodes; i > 0; i--) {
    sortedNodes[c[value.get(a[i].id)]] = a[i];
    c[value.get(a[i].id)]--;
  }
}
//=================================================================
/*
 * Calculates:
 * - DFS tree T_0 (initial T) of g represented as parent[n] for
 * all nodes n in g;
 * - dfspos_num[n] for all nodes n in T_0;
 * - node_with_dfspos[i] = n if dfspos_num[n] == i, for i = 1, ..,
 * numberOfNodes, where numberOfNodes is the number of nodes in g;
 * - largest_neighbor[n] for all nodes n in T_0;
 * - label_b[u] for all nodes n in T_0 (see PlanarityTest.h for
 * more details);
 * - nodeLabelB[n] for all nodes n in T_0 (see PlanarityTest.h for
 * more details);
 * - childrenInT0[u] for each node n in T_0 (children of n in T_0
 * in decreasing
 *   order by labelB -- needed by updateLabelB);
 */
void PlanarityTestImpl::preProcessing(Graph *g) {
  int numberOfNodes = g->numberOfNodes();
  list<edge> edgeInT0; // list of edges in T_0;
  edgeInT0 = posDFS(g, dfsPosNum);

  for (auto e : edgeInT0) {
    const auto &[n, v] = g->ends(e);
    parent.set(v.id, n);
    T0EdgeIn.set(v.id, e);
  }

#ifndef NDEBUG

  for (auto e : edgeInT0) {
    assert(!(isBackEdge(g, e) || isBackEdge(g, edgeReversal(e))));
  }

#endif

  for (auto n : g->nodes()) {
    int dfsPos = dfsPosNum.get(n.id);
    nodeWithDfsPos.set(dfsPos, n);
    largestNeighbor.set(n.id, dfsPos);
    labelB.set(n.id, dfsPos);

    if (parent.get(n.id).isValid()) { // if u is not the root of T_0;
      largestNeighbor.set(n.id, dfsPosNum.get((parent.get(n.id)).id));
    }

    if (embed) {
      p0.set(n.id, parent.get(n.id));
    }

    // saves T_0 for obstruction edges if g is not planar;
  }

  for (int i = 1; i <= numberOfNodes; ++i) {
    node n = nodeWithDfsPos.get(i);
    for (auto e : g->getOutEdges(n)) {
      node v = g->target(e);

      if (labelB.get(n.id) < labelB.get(v.id)) {
        labelB.set(n.id, labelB.get(v.id));
      }

      if (largestNeighbor.get(n.id) < dfsPosNum.get(v.id)) {
        largestNeighbor.set(n.id, dfsPosNum.get(v.id));
      }
    }
  }

  vector<node> sortedNodes(numberOfNodes + 1);
  sortedNodes[0] = node();
  sortNodesIncreasingOrder(g, labelB, sortedNodes);

  for (int i = numberOfNodes; i >= 1; i--) {
    node n = sortedNodes[i];
    node v = parent.get(n.id);

    if (v.isValid()) {
      childrenInT0[v].push_back(n);
    }
  }

  if (embed) { // needed to calculate obstruction edges;
    for (int i = 1; i <= numberOfNodes; ++i) {
      node n = nodeWithDfsPos.get(i);

      if (labelB.get(n.id) == largestNeighbor.get(n.id)) {
        nodeLabelB.set(n.id, n);
      } else {
        node v = childrenInT0[n].front();
        nodeLabelB.set(n.id, nodeLabelB.get(v.id));
      }
    }
  }
}
//=================================================================
void PlanarityTestImpl::swapNode(node &n1, node &n2) {
  node tmp = n1;
  n1 = n2;
  n2 = tmp;
}
//=================================================================
bool PlanarityTestImpl::isCNode(node n) {
  return (n.isValid() && (dfsPosNum.get(n.id) < 0));
}
//=================================================================
node PlanarityTestImpl::activeCNodeOf(bool calculatingObstruction, node n) {
  node cNode = n;

  if (!isCNode(n)) {
    cNode = parent.get(n.id);
  }

  if (!isCNode(cNode)) {
    return node(); // zero
  }

  // debug
  if (calculatingObstruction) {
    while (isCNode(parent.get(cNode.id))) {
      cNode = parent.get(cNode.id);
    }
  } else {
    assert(!isCNode(parent.get(cNode.id)));
  }

  return cNode;
}
//=================================================================
void PlanarityTestImpl::addOldCNodeRBCToNewRBC(node oldCNode, node, node n, node n1, node n2,
                                               tlp::BmdList<node> &nodeList) {
  // compress RBC[oldCNode]:
  // removes all nodes v in RBC[oldCNode] s.t. label_b[v] < dfspos_num[n]
  // or v = parent[oldCNode].

  BmdLink<node> *firstItem = RBC[oldCNode].firstItem();
  BmdLink<node> *predItem = RBC[oldCNode].cyclicPred(firstItem, nullptr);
  BmdLink<node> *succItem = RBC[oldCNode].cyclicSucc(firstItem, nullptr);

  node predNode = predItem->getData();
  node succNode = succItem->getData();
  node ul;
  node ur;

  // goes to the left;
  while (labelB.get(predNode.id) == dfsPosNum.get(n.id) && predNode != n1 && predNode != n2) {
    if (!ul.isValid()) {
      ul = predNode;
    }

    BmdLink<node> *tmp = predItem;
    predItem = RBC[oldCNode].cyclicPred(predItem, firstItem);
    predNode = predItem->getData();
    RBC[oldCNode].delItem(tmp);
  }

  // goes to right;
  while (labelB.get(succNode.id) == dfsPosNum.get(n.id) && succNode != n1 && succNode != n2) {
    if (!ur.isValid()) {
      ur = succNode;
    }

    BmdLink<node> *tmp = succItem;
    succItem = RBC[oldCNode].cyclicSucc(succItem, firstItem);
    succNode = succItem->getData();
    RBC[oldCNode].delItem(tmp);
  }

  RBC[oldCNode].delItem(RBC[oldCNode].firstItem());

  // endpoint to correctly concatenates with RBC[new_cnode];
  node first = n1;

  if (!n1.isValid()) {
    if (ul.isValid()) {
      first = predNode;
    } else {
      first = succNode;
    }
  }

  // note that oldCNode may have flipped;
  if (RBC[oldCNode].lastItem()->getData() == first) {
    RBC[oldCNode].reverse();
  }

  // reoves n1 and n2 from RBC[oldCNode];
  if (n1.isValid()) {
    RBC[oldCNode].delItem(RBC[oldCNode].firstItem());
  }

  if (n2.isValid()) {
    RBC[oldCNode].delItem(RBC[oldCNode].lastItem());
  }

  nodeList.conc(RBC[oldCNode]);
}
//=================================================================
void PlanarityTestImpl::updateLabelB(node n) {
  if (!n.isValid()) {
    return;
  }

  labelB.set(n.id, largestNeighbor.get(n.id));

  if (embed) {
    nodeLabelB.set(n.id, n); // needed to calculate obstruction edges if G is not planar;
  }

  node v;

  if (!childrenInT0[n].empty()) {
    v = childrenInT0[n].front();
  } else {
    return;
  }

  while (v.isValid()) {
    // or v is not a descendant of n in T,
    // or v is a child of n in T,
    // or v is a child of a c-node whose parent is n;
    node pv = parent.get(v.id);

    if (pv.isValid() && isCNode(pv) && parent.get(pv.id) == n) {
      v = pv; // active c-node;
      break;
    }

    if (pv == n) {
      break;
    }

    // v became sibling of n;
    childrenInT0[n].pop_front();

    if (!childrenInT0[n].empty()) {
      v = childrenInT0[n].front();
    } else {
      v = node();
    }
  }

  if (v.isValid() && labelB.get(n.id) < labelB.get(v.id)) {
    labelB.set(n.id, labelB.get(v.id));

    if (embed) {
      nodeLabelB.set(n.id, nodeLabelB.get(v.id)); // needed to calculate obstruction edges;
    }
  }
}
//=================================================================
void PlanarityTestImpl::calcNewRBCFromTerminalNode(node newCNode, node n, node n1, node n2,
                                                   tlp::BmdList<node> &nodeList) {
  node t = n1;
  node predT;
  node parentT;

  while (t != n2) {
    parentT = parent.get(t.id);

    if (isCNode(t)) {
      t = activeCNodeOf(false, t);
      addOldCNodeRBCToNewRBC(t, newCNode, n, predT, NULL_NODE, nodeList);
      parentT = parent.get(t.id);
      parent.set(t.id, newCNode);

      if (labelB.get(t.id) > labelB.get(newCNode.id)) {
        labelB.set(newCNode.id, labelB.get(t.id));

        if (embed) { // needed to calculate obstruction edges;
          nodeLabelB.set(newCNode.id, nodeLabelB.get(t.id));
        }
      }
    } else {
      parent.set(t.id, newCNode);
      updateLabelB(t);

      if (labelB.get(t.id) > dfsPosNum.get(n.id)) {
        BmdLink<node> *item = nodeList.append(t);
        ptrItem.set(t.id, item); // nodeList.append(t));
      }

      if (labelB.get(t.id) > labelB.get(newCNode.id)) {
        labelB.set(newCNode.id, labelB.get(t.id));

        if (embed) { // needed to calculate obstruction edges;
          nodeLabelB.set(newCNode.id, nodeLabelB.get(t.id));
        }
      }
    }

    if (!isCNode(t)) {
      predT = t; // predT is never a c-node;
    }

    t = parentT;
  }
}
//=================================================================
node PlanarityTestImpl::lastPNode(node n1, node n2) {
  if (n1 == n2) {
    return isCNode(n1) ? node() : n1;
  }

  list<node> S;
  node n = n1;

  while (n.isValid() && n != n2) {
    S.push_front(n);
    n = parent.get(n.id);
  }

  if (!n.isValid()) {
    return n;
  }

  n = n2; // inutile

#ifndef NDEBUG
  int count = 0;
#endif

  while (isCNode(n)) {
    assert(++count <= 2);

    if (S.empty()) {
      return node();
    }

    n = S.front();
    S.pop_front();
  }

  return n;
}
//=================================================================
node PlanarityTestImpl::lcaBetween(node n1, node n2, const MutableContainer<node> &p) {
  if (isCNode(n1)) {
    node n = activeCNodeOf(false, n1);
    n1 = p.get(n.id);
  }

  if (isCNode(n2)) {
    node n = activeCNodeOf(false, n2);
    n2 = p.get(n.id);
  }

  if (dfsPosNum.get(n1.id) > dfsPosNum.get(n2.id)) {
    swapNode(n1, n2);
  }

  list<node> nl;

  while (dfsPosNum.get(n1.id) < dfsPosNum.get(n2.id)) {
    nl.push_front(n1);
    n1 = p.get(n1.id);
  }

  node u;

  if (!nl.empty()) {
    u = nl.front();
    nl.pop_front();
  }

  while (n2 != u && n2 != n1 && dfsPosNum.get(n2.id) < dfsPosNum.get(n1.id)) {
    nl.push_front(n2);
    n2 = p.get(n2.id);
  }

  if (n2 == u || n2 == n1) {
    return n2;
  }

  return nl.front();
}
//=================================================================
node PlanarityTestImpl::lcaBetweenTermNodes(node n1, node n2) {
  node lca = lastVisited.get(n1.id);

  if (dfsPosNum.get((lastVisited.get(n2.id)).id) < dfsPosNum.get(lca.id)) {
    lca = lastVisited.get(n2.id);
  }

  assert(lca == lcaBetween(n1, n2, parent));

  return lca;
}
//=================================================================
void PlanarityTestImpl::calculateNewRBC(Graph *, node newCNode, node n, list<node> &terminalNodes) {
  switch (terminalNodes.size()) {
  case 1: {
    node n1 = terminalNodes.front();
    terminalNodes.pop_front();
    calcNewRBCFromTerminalNode(newCNode, n, n1, n, RBC[newCNode]);
  } break;

  case 2: {
    node m;
    node t1 = terminalNodes.front();
    terminalNodes.pop_front();
    node t2 = terminalNodes.front();
    terminalNodes.pop_front();
    node aux1 = t1;
    node aux2 = t2;

    // makes t1 with lower dfspos_num than t2;
    if (isCNode(aux1)) {
      aux1 = activeCNodeOf(false, aux1);
      aux1 = parent.get(aux1.id);
    }

    if (isCNode(aux2)) {
      aux2 = activeCNodeOf(false, aux2);
      aux2 = parent.get(aux2.id);
    }

    if (dfsPosNum.get(aux1.id) > dfsPosNum.get(aux2.id)) {
      swapNode(t1, t2);
    }

    m = lcaBetweenTermNodes(t1, t2);
    node t12 = lastPNode(t1, m), t22 = lastPNode(t2, m);
    BmdList<node> auxNodeList;
    calcNewRBCFromTerminalNode(newCNode, n, t1, t12, auxNodeList);

    calcNewRBCFromTerminalNode(newCNode, n, t2, t22, RBC[newCNode]);

    // adds m to RBC[newCNode];
    if (isCNode(m)) {
      m = activeCNodeOf(false, m);
      parent.set(t12.id, newCNode);
      parent.set(t22.id, newCNode);
      updateLabelB(t12);

      if (labelB.get(t12.id) > dfsPosNum.get(n.id)) {
        ptrItem.set(t12.id, auxNodeList.append(t12));
      }

      updateLabelB(t22);

      if (labelB.get(t22.id) > dfsPosNum.get(n.id)) {
        ptrItem.set(t22.id, RBC[newCNode].append(t22));
      }

      addOldCNodeRBCToNewRBC(m, newCNode, n, t22, t12, RBC[newCNode]);
      parent.set(m.id, newCNode);
    } else {
      parent.set(m.id, newCNode);
      updateLabelB(m);

      if (labelB.get(m.id) > dfsPosNum.get(n.id)) {
        ptrItem.set(m.id, RBC[newCNode].append(m));
      }
    }

    if (labelB.get(m.id) > labelB.get(newCNode.id)) {
      labelB.set(newCNode.id, labelB.get(m.id));

      if (embed) { // needed to calculate obstruction edges;
        nodeLabelB.set(newCNode.id, nodeLabelB.get(m.id));
      }
    }

    auxNodeList.reverse();
    RBC[newCNode].conc(auxNodeList);
  } break;

  default:
    assert(false);
  }
}
//=================================================================
node PlanarityTestImpl::findNodeWithLabelBGreaterThanDfsN(bool saveLastNodeTraversed, Graph *,
                                                          node n, node t) {
  node result = NULL_NODE;
  node vPred = t;
  node v = parent.get(t.id);

  list<node> nl, nl2; // to restore parent, label_b and node_label_b;
  map<node, node> p, nb;
  map<node, int> b;

  p[t] = parent.get(t.id); // saves info;
  nl.push_back(t);
  parent.set(t.id, node());

  while (v != n) {
    if (isCNode(v)) {
      v = activeCNodeOf(false, v);
      BmdListIt<node> it(RBC[v]);

      while (it.hasNext()) {
        node u = it.next();

        if (labelB.get(u.id) > dfsPosNum.get(n.id) && u != t && u != parent.get(v.id)) {
          result = u;
          break;
        }
      }
    } else {
      nl2.push_back(v);
      b[v] = labelB.get(v.id); // saves info

      if (embed) {             // needed to calculate obstruction edges;
        nb[v] = nodeLabelB.get(v.id);
      }

      updateLabelB(v);

      if (labelB.get(v.id) > dfsPosNum.get(n.id)) {
        result = v;
        break;
      }
    }

    vPred = v;
    v = parent.get(v.id);
    p[vPred] = parent.get(vPred.id);
    nl.push_back(vPred);
    parent.set(vPred.id, NULL_NODE);
  }

  // restores parent;
  for (auto n : nl) {
    parent.set(n.id, p[n]);
  }

  if (result.isValid()) {
    return result;
  }

  // restores info if result is nil;
  for (auto n : nl2) {
    labelB.set(n.id, b[n]);

    if (embed) { // needed to calculate obstruction edges;
      nodeLabelB.set(n.id, nb[n]);
    }
  }

  // to update label_b of n correctly in future iteration
  // (vPred is the child of n in T that belongs to Q_linha);
  if (saveLastNodeTraversed && vPred != t) {
    lastNodeInQLinha = vPred; // see sets_info_for_new_cnode;
  } else {
    lastNodeInQLinha = node();
  }

  return node();
}
//=================================================================
void PlanarityTestImpl::setPossibleK33Obstruction(node cNode, node n, node nl, node nr) {
  // K_33: X = {w, parent[cnode], u}
  //       Y = {ul, ur, ?};
  obstructionNodes.clear();
  obstructionNodes.push_back(n);
  obstructionNodes.push_back(nl);
  obstructionNodes.push_back(nr);
  cNodeOfPossibleK33Obstruction = cNode;
}
//=================================================================
bool PlanarityTestImpl::testCNodeCounter(Graph *, node cNode, node n, node n1, node n2, node &jl,
                                         node &jr) {
  assert(!(parent.get(cNode.id) != RBC[cNode].firstItem()->getData()));

  // no chance for cNode to be an obstruction;
  /// code ajoute
  //  if (RBC.get(cNode.id) == 0)
  //    RBC.set(cNode.id, new BmdList<node>());
  ////
  if (RBC[cNode].size() <= 2) {
    return false;
  }

  BmdLink<node> *it1 = RBC[cNode].firstItem();
  BmdLink<node> *it1l = RBC[cNode].cyclicPred(it1, nullptr);
  BmdLink<node> *it1r = RBC[cNode].cyclicSucc(it1, nullptr);

  jl = it1l->getData();
  jr = it1r->getData();
  node ul;
  node ur;
  int count = 0;

  // goes to the left;
  BmdLink<node> *a = nullptr, *s = it1;

  while (labelB.get(jl.id) <= dfsPosNum.get(n.id)) {
    if (labelB.get(jl.id) == dfsPosNum.get(n.id)) {
      ul = jl;
      ++count;
    }

    a = it1l;
    it1l = RBC[cNode].cyclicPred(it1l, s);
    s = a;
    jl = it1l->getData();
  }

  // goes to right;
  BmdLink<node> *p = it1;

  while (labelB.get(jr.id) <= dfsPosNum.get(n.id)) {
    if (labelB.get(jr.id) == dfsPosNum.get(n.id)) {
      ur = jr;
      ++count;
    }

    a = it1r;
    it1r = RBC[cNode].cyclicSucc(it1r, p);
    p = a;
    jr = it1r->getData();
  }

  if (jl == n1 || jl == n2) {
    ++count;
  }

  if (jr != jl && (jr == n1 || jr == n2)) {
    ++count;
  }

  if (count != counter.get(cNode.id)) {
    return true;
  }

  node u;

  if (ul.isValid() && ur.isValid()) {
    u = jl; // or u = jr;
  } else if (ul.isValid() && n1.isValid() && n1 != jl) {
    u = jl;
    ur = n1;
  } else if (ur.isValid() && n1.isValid() && n1 != jr) {
    u = jr;
    ul = n1;
  }

  if (cNodeOfPossibleK33Obstruction == NULL_NODE && u != NULL_NODE) {
    setPossibleK33Obstruction(cNode, u, ul, ur);
  }

  return false;
}
//=================================================================
bool PlanarityTestImpl::testObstructionFromTerminalNode(Graph *sG, node w, node terminal, node u) {
  node v = terminal, predV = NULL_NODE;

  while (v != u) {
    if (isCNode(v)) {
      v = activeCNodeOf(false, v);
      node jl, jr;

      if (testCNodeCounter(sG, v, w, predV, NULL_NODE, jl, jr)) {
        if (embed) {
          obstructionEdgesCNodeCounter(sG, v, w, jl, jr, terminal, NULL_NODE);
        }

        return true;
      }

      if (cNodeOfPossibleK33Obstruction != NULL_NODE) {
        break;
      }

      v = parent.get(v.id);

      if (v == u) {
        break;
      }
    }

    predV = v; // never a c-node;
    v = parent.get(v.id);
  }

  return false;
}

//=================================================================
BmdLink<node> *PlanarityTestImpl::searchRBC(int dir, BmdLink<node> *it, node n,
                                            list<node> &traversedNodesInRBC) {
  if (it != nullptr &&
      (it->prev() == nullptr || it->succ() == nullptr)) { // 1st or last item in RBC;
    return it;
  }

  BmdLink<node> *prev = it;
  BmdLink<node> *aux1 = it->prev();
  BmdLink<node> *aux2 = it->succ();

  if (dir == 1) {
    // leda_swap(aux1, aux2);
    BmdLink<node> *tmp = aux1;
    aux1 = aux2;
    aux2 = tmp;
  }

  it = aux1;
  node u = it->getData();
  int b = labelB.get(u.id);

  while (it != nullptr && (b <= dfsPosNum.get(n.id) || dir != 1) &&
         state.get(u.id) == NOT_VISITED) {
    aux1 = it->prev();

    if (aux1 == prev) {
      aux1 = it->succ();
    }

    prev = it;
    it = aux1;
    u = prev->getData();
    state.set(u.id, VISITED_IN_RBC);
    traversedNodesInRBC.push_back(u);

    if (it != nullptr) {
      u = it->getData();
      b = labelB.get(u.id);
    }
  }

  if (it == nullptr) {
    return prev;
  } else if (state.get(u.id) != NOT_VISITED || (it->prev() == nullptr || it->succ() == nullptr)) {
    return it;
  } else {
    return nullptr;
  }
}
//=================================================================
/*
 * Returns the active c-node of RBC list containing "u".
 * Traverses only nodes with label_b <= dfspos_num[w].
 * Let z be the first element of RBC.
 * All traversed nodes, but z, are inserted in "nl".
 * Pre-conditions:
 * - "u" is not the 1st element in RBC list;
 * - ptritem[u] is the pointer to the item containing "u";
 * - "u" is a node that is an ancestor of a neighbor of w;
 * - state[v] == NOT_VISITED for each node v in RBC list that was not
 *   traversed.
 */
node PlanarityTestImpl::findActiveCNode(node u, node w, list<node> &nl) {
  list<node> traversedNodesInRBC;
  assert(isCNode(parent.get(u.id)));

  if (state.get(u.id) != NOT_VISITED) {
    assert(!isCNode(parent.get(parent.get(u.id).id)));
    return parent.get(u.id);
  }

  BmdLink<node> *it1 = ptrItem.get(u.id); // ptrItem[u];
  assert(it1 != nullptr);

  state.set(u.id, VISITED_IN_RBC);
  traversedNodesInRBC.push_back(u);
  BmdLink<node> *it = searchRBC(1, it1, w, traversedNodesInRBC);

  if (it == nullptr) {
    it = searchRBC(0, it1, w, traversedNodesInRBC);
  }

  assert(it != nullptr);

  node v = it->getData();
  node cNode;

  if (it->prev() != nullptr && it->succ() != nullptr) {
    cNode = parent.get(v.id); // path compressed;
  } else {
    cNode = activeCNode[it];
  }

  assert(cNode != NULL_NODE);

  node first = RBC[cNode].firstItem()->getData();

  // path compression;
  for (auto n : traversedNodesInRBC) {
    if (n != first) {
      if (n != u) {
        nl.push_back(v);
      }

      parent.set(n.id, cNode);
    } else {
      state.set(n.id, NOT_VISITED);
    }
  }

  return cNode;
}
//=================================================================
