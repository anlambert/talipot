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

#include <talipot/PluginHeaders.h>
#include <talipot/PlanarConMap.h>
#include <talipot/ViewSettings.h>

#include "MixedModel.h"
#include "DatasetTools.h"

PLUGIN(MixedModel)

using namespace std;
using namespace tlp;

float spacing = 2;
float edgeNodeSpacing = 2;

//===============================================================

static constexpr std::string_view paramHelp[] = {
    // orientation
    "This parameter enables to choose the orientation of the drawing.",

    // y node-node spacing
    "This parameter defines the minimum y-spacing between any two nodes.",

    // x node-node and edge-node spacing
    "This parameter defines the minimum x-spacing between any two nodes or between a node and an "
    "edge.",

    // shape property
    "This parameter defines the property holding edges shapes."};

#define ORIENTATION "vertical;horizontal;"
//====================================================
MixedModel::MixedModel(const tlp::PluginContext *context) : LayoutAlgorithm(context) {
  addNodeSizePropertyParameter(this, true /* inout */);
  addInParameter<StringCollection>("orientation", paramHelp[0].data(), ORIENTATION, true,
                                   "<b>vertical</b> <br> <b>horizontal</b>");
  addInParameter<float>("y node-node spacing", paramHelp[1].data(), "2");
  addInParameter<float>("x node-node and edge-node spacing", paramHelp[2].data(), "2");
  addOutParameter<IntegerProperty>("shape property", paramHelp[3].data(), "viewShape");
  addDependency("Connected Components Packing", "1.0");
}
//====================================================
MixedModel::~MixedModel() = default;
//====================================================
bool MixedModel::run() {
  string orientation = "vertical";
  sizeResult = nullptr;
  shapeResult = nullptr;

  if (dataSet != nullptr) {
    getNodeSizePropertyParameter(dataSet, sizeResult);
    StringCollection tmp;

    if (dataSet->get("orientation", tmp)) {
      orientation = tmp.getCurrentString();
    }

    dataSet->get("y node-node spacing", spacing);
    dataSet->get("x node-node and edge-node spacing", edgeNodeSpacing);
    // "shape property" was previously called "node shape"
    if (!dataSet->get("node shape", shapeResult)) {
      dataSet->get("shape property", shapeResult);
    }
  }

  if (sizeResult == nullptr) {
    sizeResult = graph->getSizeProperty("viewSize");
  }

  if (shapeResult == nullptr) {
    shapeResult = graph->getLocalIntegerProperty("viewShape");
  }

  //=========================================================
  // rotate size if necessary
  if (orientation == "horizontal") {
    for (auto n : graph->nodes()) {
      const Size tmp = sizeResult->getNodeValue(n);
      sizeResult->setNodeValue(n, Size(tmp[1], tmp[0], tmp[2]));
    }
  }

  shapeResult->setAllEdgeValue(EdgeShape::Polyline);

  vector<edge> edge_planar;

  // give some empirical feedback of what we are doing 0,1 %
  if (pluginProgress->progress(1, 1000) != TLP_CONTINUE) {
    return pluginProgress->state() != TLP_CANCEL;
  }

  // compute the connected components's subgraphs
  auto components = ConnectedTest::computeConnectedComponents(graph);

  for (auto &component : components) {
    if (pluginProgress->progress(2, 1000) != TLP_CONTINUE) {
      return pluginProgress->state() != TLP_CANCEL;
    }

    //====================================================
    // don't compute the canonical ordering if the number of nodes is less than 3
    if (component.size() == 1) {
      result->setNodeValue(component[0], Coord(0, 0, 0));
      continue;
    } else if (component.size() < 4) {
      node n = component[0];
      const Coord &c = sizeResult->getNodeValue(n);
      result->setNodeValue(n, Coord(0, 0, 0));
      node n2 = component[1];
      const Coord &c2 = sizeResult->getNodeValue(n2);
      result->setNodeValue(n2, Coord(spacing + c.getX() / 2 + c2.getX() / 2, 0, 0));

      for (auto e : graph->getEdges(n, n2, false)) {
        edge_planar.push_back(e);
      }

      if (component.size() == 3) {
        node n3 = component[2];
        const Coord &c3 = sizeResult->getNodeValue(n2);
        result->setNodeValue(
            n3, Coord(2.f * spacing + c.getX() / 2.f + c2.getX() + c3.getX() / 2.f, 0, 0));
        edge e = graph->existEdge(n, n3, false);

        if (e.isValid()) {
          vector<Coord> bends(2);
          bends.clear();
          auto max = uint(c[1] > c2[1] ? c[1] / 2 : c2[1] / 2);
          max += uint(edgeNodeSpacing);

          if (graph->source(e) == n) {
            bends.push_back(Coord(0, float(max), 0));
            bends.push_back(
                Coord(2.f * spacing + c.getX() / 2.f + c2.getX() + c3.getX() / 2.f, float(max), 0));
          } else {
            bends.push_back(
                Coord(2.f * spacing + c.getX() / 2.f + c2.getX() + c3.getX() / 2.f, float(max), 0));
            bends.push_back(Coord(0, float(max), 0));
          }

          result->setEdgeValue(e, bends);
        }

        for (auto e : graph->getEdges(n, n3, false)) {
          edge_planar.push_back(e);
        }
        for (auto e : graph->getEdges(n2, n3, false)) {
          edge_planar.push_back(e);
        }
      }

      continue;
    }

    //====================================================
    Graph *currentGraph = graph->inducedSubGraph(component);

    planar = PlanarityTest::isPlanar(currentGraph);
    Graph *G;

    if (!planar) {
      G = currentGraph->addSubGraph();

      BooleanProperty resultatAlgoSelection(currentGraph);
      resultatAlgoSelection.setAllNodeValue(false);
      for (auto e : graph->bfsEdges()) {
        resultatAlgoSelection.setEdgeValue(e, true);
      }
      for (auto e : graph->edges()) {
        if (resultatAlgoSelection.getEdgeValue(e)) {
          const auto &[src, tgt] = currentGraph->ends(e);
          G->addNode(src);
          G->addNode(tgt);
          G->addEdge(e);
          edge_planar.push_back(e);
        } else {
          unplanar_edges.push_back(e);
        }
      }

      //===================================================

      graphMap = computePlanarConMap(G);
      vector<edge> re_added = getPlanarSubGraph(graphMap, unplanar_edges);

      for (auto e : re_added) {
        G->addEdge(e);
        resultatAlgoSelection.setEdgeValue(e, true);
        edge_planar.push_back(e);
        auto ite = find(unplanar_edges.begin(), unplanar_edges.end(), e);
        unplanar_edges.erase(ite);
      }

      delete graphMap;
    } else {
      G = currentGraph->addCloneSubGraph();
      for (auto e : currentGraph->edges()) {
        edge_planar.push_back(e);
      }
    }

    //===============================================

    // give some empirical feedback of what we are doing 0,5%
    if (pluginProgress->progress(5, 1000) != TLP_CONTINUE) {
      return pluginProgress->state() != TLP_CANCEL;
    }

    vector<edge> added_edges;

    if (!BiconnectedTest::isBiconnected(G)) {
      added_edges = BiconnectedTest::makeBiconnected(G);
    }

    assert(BiconnectedTest::isBiconnected(G));
    carte = computePlanarConMap(G);
    assert(BiconnectedTest::isBiconnected(G));
    assert(BiconnectedTest::isBiconnected(carte));

    // give some empirical feedback of what we are doing (5%)
    if (pluginProgress->progress(5, 100) != TLP_CONTINUE) {
      return pluginProgress->state() != TLP_CANCEL;
    }

    InPoints.clear();
    OutPoints.clear();
    EdgesIN.clear();
    EdgesOUT.clear();
    outl.clear();
    outr.clear();
    inl.clear();
    inr.clear();

    rank.clear();

    V.clear();

    NodeCoords.clear();

    initPartition();

    // give some empirical feedback of what we are doing (10%)
    if (pluginProgress->progress(10, 100) != TLP_CONTINUE) {
      return pluginProgress->state() != TLP_CANCEL;
    }

    assignInOutPoints();

    computeCoords();

    placeNodesEdges();

    for (auto e : dummy) {
      currentGraph->delEdge(e, true);
    }

    const vector<Coord> &dv = result->getEdgeDefaultValue();

    for (auto e : added_edges) {
      currentGraph->delEdge(e, true);
      result->setEdgeValue(e, dv);
    }

    delete carte;
    graph->delAllSubGraphs(currentGraph);
  }

  if (components.size() > 1) {
    string err;
    DataSet tmp;
    tmp.set("coordinates", result);
    graph->applyPropertyAlgorithm("Connected Components Packing", result, err, &tmp,
                                  pluginProgress);
    if (pluginProgress->state() != TLP_CONTINUE) {
      return pluginProgress->state() != TLP_CANCEL;
    }
  }

  // rotate layout and size
  if (orientation == "horizontal") {
    for (auto n : graph->nodes()) {
      const Size &size = sizeResult->getNodeValue(n);
      sizeResult->setNodeValue(n, Size(size[1], size[0], size[2]));
      const Coord &coord = result->getNodeValue(n);
      result->setNodeValue(n, Coord(-coord[1], coord[0], coord[2]));
    }
    for (auto e : graph->edges()) {
      const auto &tmp = result->getEdgeValue(e);
      LineType::RealType tmp2;

      for (auto coord : tmp) {
        tmp2.push_back(Coord(-coord[1], coord[0], coord[2]));
      }

      result->setEdgeValue(e, tmp2);
    }
  }

  dataSet->set("planar_edges", edge_planar);

  return true;
}

//====================================================
bool MixedModel::check(std::string &err) {
  if (!SimpleTest::isSimple(graph)) {
    err = "The graph must be simple and without self-loop ";
    return false;
  }
  return true;
}

//====================================================
vector<edge> MixedModel::getPlanarSubGraph(tlp::PlanarConMap *sg,
                                           const std::vector<tlp::edge> &unplanar_edges) {
  vector<edge> res;

  for (auto e : unplanar_edges) {
    const auto &[src, tgt] = sg->ends(e);
    Face f = sg->sameFace(src, tgt);

    if (f != Face()) {
      sg->splitFace(f, e);
      res.push_back(e);
    }
  }

  return res;
}

//====================================================
void MixedModel::placeNodesEdges() {
  float maxX = 0, maxY = 0;

  for (auto n : carte->nodes()) {
    Coord c = nodeSize.get(n.id);
    c[0] -= edgeNodeSpacing;
    graph->getSizeProperty("viewSize")->setNodeValue(n, Size(c[0], c[1], 0.3f));
    result->setNodeValue(n, NodeCoords[n]);
  }

  for (auto e : carte->edges()) {
    const auto &[src, tgt] = carte->ends(e);
    Coord cs, ct, c;
    uint rs = rank[src], rt = rank[tgt];

    if (rs != rt) {
      vector<Coord> bends;

      if (rs > rt) {
        cs = InPoints[e][0] + NodeCoords[src];
        ct = OutPoints[e] + NodeCoords[tgt];
        c = Coord(ct.getX(), cs.getY(), 0);
      } else {
        ct = InPoints[e][0] + NodeCoords[tgt];
        cs = OutPoints[e] + NodeCoords[src];
        c = Coord(cs.getX(), ct.getY(), 0);
      }

      if (ct.getX() >= maxX) {
        maxX = ct.getX();
      }

      if (cs.getX() >= maxX) {
        maxX = cs.getX();
      }

      if (ct.getY() >= maxY) {
        maxY = ct.getY();
      }

      if (cs.getY() >= maxY) {
        maxY = cs.getY();
      }

      if ((cs != NodeCoords[src]) && (cs != ct)) {
        bends.push_back(cs);
      }

      if ((c != cs) && (c != ct)) {
        bends.push_back(c);
      }

      if ((ct != NodeCoords[tgt]) && (ct != cs)) {
        bends.push_back(ct);
      }

      if (!bends.empty()) {
        result->setEdgeValue(e, bends);
      }
    }

    // rs == rt, même partition donc pas de points intermédiaire à  calculer
    // en case de post-processing, il faudra pe y changer
  }

  if (!planar) {
    float z_size = (maxX + maxY) / 3.f; // sqrt(maxX + maxY);
    maxX /= 8;
    maxY /= 8;

    for (auto e : unplanar_edges) {
      const auto &[n, v] = carte->ends(e);
      auto c_n = NodeCoords[n];
      auto c_v = NodeCoords[v];
      vector<Coord> bends;
      bends.push_back(Coord(-maxX + (c_n.getX() + c_v.getX()) / 2.f,
                            -maxY + (c_n.getY() + c_v.getY()) / 2.f, -z_size));
      result->setEdgeValue(e, bends);
      graph->getIntegerProperty("viewShape")->setEdgeValue(e, EdgeShape::BezierCurve);
      graph->getColorProperty("viewColor")->setEdgeValue(e, Color(218, 218, 218));
    }
  }
}

//====================================================
void MixedModel::initPartition() {
  V = tlp::computeCanonicalOrdering(carte, &dummy, pluginProgress);

  if (pluginProgress->state() == TLP_CANCEL) {
    return;
  }

  for (uint i = 0; i < V.size(); ++i) {
    for (uint j = 0; j < V[i].size(); ++j) {
      rank[V[i][j]] = i;
    }
  }
}

//====================================================
void MixedModel::assignInOutPoints() { // on considère qu'il n'y a pas d'arc double
  assert(carte);
  vector<node> C; // chemin courant

  // empirical feedback (95% -> 99%)
  int minProgress = 950, deltaProgress = 40;

  for (uint k = 0; k < V.size(); ++k) {
    // give pluginProgress feedback
    if (pluginProgress->progress(minProgress + (deltaProgress * k) / V.size(), 1000) !=
        TLP_CONTINUE) {
      return;
    }

    uint p = V[k].size();

    vector<node>::iterator il, ir;
    node nl = node(), nr = node();

    if (k != 0) {
      // ordonne les arcs "in"
      uint i;

      for (i = 0; !nl.isValid() && (i < C.size() - 1); ++i) {
        if (existEdge(C[i], V[k][0]).isValid()) {
          nl = C[i];
        }
      }

      assert(nl.isValid());

      for (i = C.size() - 1; !nr.isValid() && (i > 0); i--) {
        if (existEdge(C[i], V[k][p - 1]).isValid()) {
          nr = C[i];
        }
      }

      assert(nr.isValid());

      il = find(C.begin(), C.end(), nl);
      assert(il != C.end());
      ir = find(il, C.end(), nr);
      assert(ir != C.end());
    }

    for (uint i = 0; i < p; ++i) {
      node v = V[k][i];
      unsigned nbIn = 0, nbOut = 0;
      vector<edge> listOfEdgesIN;
      vector<edge> listOfEdgesOUT;

      listOfEdgesIN.clear();
      listOfEdgesOUT.clear();

      // build in-edge vector and out-edge vector in the good order
      vector<edge> tmp;
      char pred = 'p';

      tmp.clear();

      for (auto e : carte->getInOutEdges(v)) {
        const auto &[src, tgt] = carte->ends(e);

        node n = (src == v) ? tgt : src;

        bool trouve = false;
        uint r = rank[n];

        if (r < k) {
          ++nbIn;
          trouve = true;

          if (pred == 'o') {
            pred = 'i';
            listOfEdgesOUT.insert(listOfEdgesOUT.begin(), tmp.begin(), tmp.end());
            tmp.clear();
          } else if (pred == 'p') {
            pred = 'i';
          }

          tmp.push_back(e);
        }

        if (r > k && !trouve) {
          ++nbOut;
          trouve = true;

          if (pred == 'i') {
            pred = 'o';
            listOfEdgesIN.insert(listOfEdgesIN.begin(), tmp.begin(), tmp.end());
            tmp.clear();
          } else if (pred == 'p') {
            pred = 'o';
          }

          tmp.push_back(e);
        }

        if ((i != 0) && (!trouve)) { // test le voisin de gauche
          if (V[k][i - 1] == n) {
            ++nbIn;
            trouve = true;

            if (pred == 'o') {
              pred = 'i';
              listOfEdgesOUT.insert(listOfEdgesOUT.begin(), tmp.begin(), tmp.end());
              tmp.clear();
            } else if (pred == 'p') {
              pred = 'i';
            }

            tmp.push_back(e);
          }
        }

        if ((i != V[k].size() - 1) && !trouve) { // test le voisin de droite
          if (V[k][i + 1] == n) {
            ++nbIn;

            if (pred == 'o') {
              pred = 'i';
              listOfEdgesOUT.insert(listOfEdgesOUT.begin(), tmp.begin(), tmp.end());
              tmp.clear();
            } else if (pred == 'p') {
              pred = 'i';
            }

            tmp.push_back(e);
          }
        }
      }

      if (pred == 'o') {
        listOfEdgesOUT.insert(listOfEdgesOUT.begin(), tmp.begin(), tmp.end());
      } else if (pred == 'i') {
        listOfEdgesIN.insert(listOfEdgesIN.begin(), tmp.begin(), tmp.end());
      }

      if (k != 0) {
        if (i == 0) {
          edge e = existEdge(nl, v);
          auto eil = find(listOfEdgesIN.begin(), listOfEdgesIN.end(), e);
          assert(eil != listOfEdgesIN.end());

          if (!((*listOfEdgesIN.begin()) == e)) {
            vector<edge> tmp;
            tmp.insert(tmp.begin(), eil, listOfEdgesIN.end());
            tmp.insert(tmp.end(), listOfEdgesIN.begin(), eil);
            listOfEdgesIN.swap(tmp);
          }
        } else if (i == p - 1) {
          edge e = existEdge(nr, v);
          auto eir = find(listOfEdgesIN.begin(), listOfEdgesIN.end(), e);
          assert(eir != listOfEdgesIN.end());

          if (!((*listOfEdgesIN.rbegin()) == e)) {
            vector<edge> tmp;
            tmp.insert(tmp.begin(), eir + 1, listOfEdgesIN.end());
            tmp.insert(tmp.end(), listOfEdgesIN.begin(), eir + 1);
            listOfEdgesIN.swap(tmp);
          }
        } else {
          edge e = existEdge(V[k][i - 1], v);

          if (e.isValid()) {
            if (!((*listOfEdgesIN.begin()) == e)) {
              vector<edge> tmp;
              auto ei = find(listOfEdgesIN.begin(), listOfEdgesIN.end(), e);
              tmp.insert(tmp.begin(), ei, listOfEdgesIN.end());
              tmp.insert(tmp.end(), listOfEdgesIN.begin(), ei);
              listOfEdgesIN.swap(tmp);
            }
          } else {
            e = existEdge(V[k][i + 1], v);
            assert(e.isValid());

            if (!((*listOfEdgesIN.rbegin()) == e)) {
              vector<edge> tmp;
              auto ei = find(listOfEdgesIN.begin(), listOfEdgesIN.end(), e);
              tmp.insert(tmp.begin(), ei + 1, listOfEdgesIN.end());
              tmp.insert(tmp.end(), listOfEdgesIN.begin(), ei + 1);
              listOfEdgesIN.swap(tmp);
            }
          }
        }
      }

      int out_l = 0, out_r = 0, dl = 0, dr = 0;
      // determinate the coords out-in points

      float dtmp = (nbOut - 1.f) / 2.f;
      int outPlus = int(dtmp);

      int outMoins = (0 >= outPlus ? 0 : outPlus);

      if (dtmp != float(outPlus)) {
        ++outPlus; // valeur entière supérieure
      }

      if (nbIn >= 2) {
        dl = 1;
        dr = 1;
        out_l = outMoins;
        out_r = outPlus;
      } else if (nbIn == 0) {
        dl = dr = 0;
        out_l = outMoins;
        out_r = outPlus;
      } else { // case nbIn == 1
        if (k == 0) {
          if (i == 0) { // (Z0,Z1), existe forcément par définition
            dl = 0;
            dr = 1;
            out_l = outPlus;
            out_r = outMoins;
          } else if (i == V[k].size() - 1) {
            dl = 1;
            dr = 0;
            out_l = outMoins;
            out_r = outPlus;
          } else {
            bool trouve = false;
            edge e = existEdge(v, V[k][i - 1]); // (Zi, Zi-1)

            if (!e.isValid()) {
              trouve = true;
              dl = 0;
              dr = 1;
              out_l = outPlus;
              out_r = outMoins;
            }

            if (!trouve) {
              edge e = existEdge(v, V[k][i + 1]); // (Zi, Zi+1)

              if (!e.isValid()) {
                dl = 1;
                dr = 0;
                out_l = outMoins;
                out_r = outPlus;
              }
            }
          }
        } else {
          if (i == 0) { // existe un arc entre left(V[k]),Z0
            dl = 1;
            dr = 0;
            out_l = outMoins;
            out_r = outPlus;
          } else if (i == V[k].size() - 1) {
            dl = 0;
            dr = 1;
            out_l = outPlus;
            out_r = outMoins;
          } else {
            bool trouve = false;
            edge e = existEdge(v, V[k][i - 1]); // (Zi, Zi-1)

            if (!e.isValid()) {
              trouve = true;
              dl = 0;
              dr = 1;
              out_l = outPlus;
              out_r = outMoins;
            }

            if (!trouve) {
              edge e = existEdge(v, V[k][i + 1]); // (Zi, Zi+1)

              if (!e.isValid()) {
                dl = 1;
                dr = 0;
                out_l = outMoins;
                out_r = outPlus;
              }
            }
          }
        }
      }

      outl[v] = out_l;
      outr[v] = out_r;

      if (nbOut >= 1) {
        Coord c;

        for (int x = -out_l, y = dl; x <= -1; ++x, ++y) {
          c.set(float(x), float(y));
          out_points[v].push_back(c);
        }

        c.set(0, (out_l + dl - 1 > out_r + dr - 1) ? float(out_l + dl - 1) : float(out_r + dr - 1));
        out_points[v].push_back(c);

        for (int x = 1, y = out_r + dr - 1; y >= dr; ++x, --y) {
          c.set(float(x), float(y));
          out_points[v].push_back(c);
        }
      }

      if (k != 0) {
        for (auto e_tmp : listOfEdgesIN) {
          const auto &[src, tgt] = carte->ends(e_tmp);
          node n_tmp = (src == v) ? tgt : src;

          if (rank[n_tmp] < k) {
            if (i == 0) {
              uint t = out_points[n_tmp].size();

              if (n_tmp == nl) {
                OutPoints[e_tmp] = out_points[n_tmp][t - 1];
                out_points[n_tmp].pop_back();
              }

              else {
                OutPoints[e_tmp] = out_points[n_tmp][0];
                out_points[n_tmp].erase(out_points[n_tmp].begin());
              }
            } else if (i == p - 1) {
              // n_tmp is equal to nr
              OutPoints[e_tmp] = out_points[n_tmp][0];
              out_points[n_tmp].erase(out_points[n_tmp].begin());
            }
          }
        }
      }

      int in_l = (nbIn - 3) / 2;
      float ftmp = (nbIn - 3.f) / 2.f;
      int in_r = in_l;

      if (0 > in_l) {
        in_l = 0;
      }

      if (ftmp != float(in_r)) {
        ++in_r;
      }

      if (0 > in_r) {
        in_r = 0;
      }

      inr[v] = in_r;
      inl[v] = in_l;

      if (nbIn > 3) {
        uint j = 0;
        Coord c = {-float(in_l), 0};
        InPoints[listOfEdgesIN[j]].push_back(c);
        ++j;

        for (int x = -in_l, y = -1; x <= -1; ++x, --y) {
          c.set(float(x), float(y));
          InPoints[listOfEdgesIN[j]].push_back(c);
          ++j;
        }

        c.set(0, -float(in_r));
        InPoints[listOfEdgesIN[j]].push_back(c);
        ++j;

        for (int x = 1, y = -in_r; x <= in_r; ++x, ++y) {
          c.set(float(x), float(y));
          InPoints[listOfEdgesIN[j]].push_back(c);
          ++j;
        }

        c.set(float(in_r), 0);
        InPoints[listOfEdgesIN[j]].push_back(c);
        ++j;

        assert(j == nbIn);
      } else {
        for (uint j = 0; j < nbIn; ++j) {
          InPoints[listOfEdgesIN[j]].push_back(Coord());
        }
      }

      EdgesIN[v] = listOfEdgesIN;
      unsigned s = listOfEdgesOUT.size();
      EdgesOUT[v] = vector<edge>(s);

      for (uint i = 0; i < s; ++i) {
        EdgesOUT[v][s - (i + 1)] = listOfEdgesOUT[i];
      }
    }

    // mise à jour du contour
    if (k == 0) {
      C = V[0];
    } else {
      C.erase(il + 1, ir);
      ir = find(C.begin(), C.end(), nr);
      C.insert(ir, V[k].begin(), V[k].end());
    }
  }
}

//====================================================
void MixedModel::computeCoords() {
  NodeCoords.clear();

  nodeSize.setAll(Coord()); // permet de conserver une taille relative pout les sommets

  for (auto n : carte->nodes()) {
    Coord c = sizeResult->getNodeValue(n);
    c[0] += edgeNodeSpacing;
    nodeSize.set(n.id, c);
  }

  unordered_map<node, node> father; // permet de connaître le noeud de référence
  father.clear();
  assert(!V.empty());

  vector<node> C; // chemin courant
  double out_r_moins1 = 0.0;

  //-------------  initialisation --------------------------
  for (uint i = 0; i < V[0].size(); ++i) {
    node v = V[0][i];

    double out_l = outl[v], out_r = outr[v];
    Coord c = nodeSize.get((V[0][i]).id);
    float x;

    if (i == 0) {
      x = (out_l < double(c.getX() / 2.f)) ? (c.getX() / 2.f) : float(out_l);
    } else {
      x = float(out_r_moins1) +
          ((out_l < double(c.getX() / 2.f)) ? (c.getX() / 2.f) : float(out_l)) + spacing;
    }

    NodeCoords[v] = Coord(x, 0, 0); // y(vi) = 0
    out_r_moins1 = (out_r < (c.getX() / 2.) ? (c.getX() / 2.) : out_r);
  }

  C = V[0]; // initiation du contour C0

  uint size_V = V.size();

  for (uint k = 1; k < size_V; ++k) { // parcours de chaque partition

    uint p = V[k].size(); // taille de la partition Vk
    node cl = leftV(k),
         cr = rightV(k); // recherche du left et right de Vk, qui est cherché à l'aide de EdgesIN

    auto il = find(C.begin(), C.end(), cl);
    assert(il != C.end());
    auto ir = find(il, C.end(), cr); // par définition, il<ir
    assert(ir != C.end());
    Coord co = nodeSize.get((*il).id);
    float max_y = NodeCoords[(*il)].getY() + co.getY() / 2.f;
    float sum = 0; // relative to cl

    for (auto i = il + 1; i != ir + 1; ++i) {
      Coord co2 = nodeSize.get((*i).id);
      float y = NodeCoords[(*i)].getY() + co2.getY() / 2.f; // recherche max des y(ci) dans
                                                            // [cl...cr]

      if (max_y < y) {
        max_y = y;
      }

      // compute x-coords [cl+1.. cr] temporarily
      sum += NodeCoords[(*i)].getX();
      NodeCoords[(*i)].setX(sum); // somme x(cj) dans [cl+1...ci]
    }

    node Z0 = V[k][0];
    co = nodeSize.get(Z0.id);
    int max_y_taille = int((inr[Z0] < co.getY() / 2.) ? co.getY() / 2. : inr[Z0]);

    for (uint i = 0; i < p; ++i) {
      co = nodeSize.get((V[k][i]).id);
      int taille_tmp = int((inr[V[k][i]] < co.getY() / 2.) ? co.getY() / 2. : inr[V[k][i]]);

      if (taille_tmp > max_y_taille) {
        max_y_taille = taille_tmp;
      }
    }

    for (uint i = 0; i < p; ++i) {
      // assign y-coords
      NodeCoords[V[k][i]] = Coord(0, max_y_taille + max_y + spacing, 0);
    }

    // assign x-coords :
    uint n = EdgesIN[V[k][p - 1]].size();
    int dxl = int(OutPoints[EdgesIN[V[k][0]][0]].getX()),
        dxr = int(OutPoints[EdgesIN[V[k][p - 1]][n - 1]].getX());

    if (EdgesIN[Z0].size() >= 3) {
      assert(p == 1);

      int in_l = inl[Z0];
      auto it = il;
      int t = 1;

      for (; t < in_l + 2 && it != ir; ++t) {
        do {
          ++it;
        } while (!existEdge(Z0, (*it)).isValid());
      }

      assert(t == in_l + 2);

      edge e = EdgesIN[V[k][0]][t - 1];
      float dxt = OutPoints[e].getX();
      int out_l = outl[V[k][0]], out_r = outr[V[k][0]];

      float tmp = NodeCoords[(*it)].getX() + dxt;
      float ftmp =
          ((out_l < (nodeSize.get(Z0.id)).getX() / 2.f) ? (nodeSize.get(Z0.id)).getX() / 2.f
                                                        : out_l) +
          dxl;

      if (tmp >= ftmp) {
        NodeCoords[Z0].setX(tmp);
      } else {
        NodeCoords[Z0].setX(ftmp);
      }

      float delta = NodeCoords[Z0].getX() - (dxt + NodeCoords[(*it)].getX());

      if (delta > 0) {
        for (auto i = it; i != ir + 1; ++i) {
          NodeCoords[(*i)].setX(NodeCoords[(*i)].getX() + delta);
        }
      } else {
        delta = 0;
      }

      tmp = NodeCoords[(*ir)].getX() + delta - NodeCoords[Z0].getX();
      ftmp = ((out_r < (nodeSize.get(Z0.id)).getX() / 2.f) ? (nodeSize.get(Z0.id)).getX() / 2.f
                                                           : out_r) -
             dxr;

      if (tmp >= ftmp) {
        NodeCoords[(*ir)].setX(tmp);
      } else {
        NodeCoords[(*ir)].setX(ftmp);
      }

      float xZ0 = NodeCoords[Z0].getX();

      for (auto i = il + 1; i != ir; ++i) { // it; i++){
        NodeCoords[(*i)].setX(NodeCoords[(*i)].getX() - xZ0);
        father[(*i)] = Z0;
      }
    } else {
      int out_r = 0, out_r_moins1 = 0;
      float sum = 0;

      // assign x(zi)
      for (uint i = 0; i < p; ++i) {
        int out_l = outl[V[k][i]];
        out_r = outr[V[k][i]];
        Coord co2 = nodeSize.get((V[k][i]).id);
        double x;

        if (i == 0) {
          x = ((out_l < co2.getX() / 2.) ? co2.getX() / 2. : out_l) + dxl;
        } else {
          x = out_r_moins1 + ((out_l < co2.getX() / 2.) ? co2.getX() / 2. : out_l) + 1;
        }

        NodeCoords[V[k][i]].setX(float(x));
        sum += float(x);

        out_r_moins1 = int((out_r < co2.getX() / 2.) ? co2.getX() / 2. : out_r);
      }

      // assign x(cr)
      co = nodeSize.get((V[k][p - 1]).id);
      float tmp = ((out_r < co.getX() / 2.f) ? co.getX() / 2.f : out_r) - dxr;

      float xtmp = NodeCoords[cr].getX();
      double x;

      if (tmp > xtmp - sum) {
        x = tmp;
      } else {
        x = xtmp - sum;
      }

      NodeCoords[cr].setX(float(x));

      float xZ0 = NodeCoords[Z0].getX();

      for (auto i = il + 1; i != ir; ++i) {
        NodeCoords[(*i)].setX(NodeCoords[(*i)].getX() - xZ0);
        father[(*i)] = Z0;
      }
    }

    C.erase(il + 1, ir);
    ir = find(C.begin(), C.end(), cr);
    C.insert(ir, V[k].begin(), V[k].end());
  }

  float sum = 0;

  for (auto n : C) {
    sum += NodeCoords[n].getX();
    NodeCoords[n].setX(sum);
  }

  for (int k = size_V - 1; k >= 0; k--) {
    uint p = V[k].size();

    for (uint i = 0; i < p; ++i) {
      node v = V[k][i];

      if (find(C.begin(), C.end(), v) == C.end()) {
        if (father.find(v) != father.end()) {
          NodeCoords[v].setX(NodeCoords[v].getX() + NodeCoords[father[v]].getX());
        }
      }
    }
  }
}

//====================================================
node MixedModel::leftV(uint k) {
  assert((0 < k) && (k < V.size()));
  edge el = EdgesIN[V[k][0]][0];
  const auto &[src, tgt] = carte->ends(el);
  return (src == V[k][0]) ? tgt : src;
}

//====================================================
node MixedModel::rightV(uint k) {
  assert((0 < k) && (k < V.size()));
  uint p = V[k].size();
  uint n = EdgesIN[V[k][p - 1]].size();
  edge er = EdgesIN[V[k][p - 1]][n - 1];
  const auto &[src, tgt] = carte->ends(er);
  return (src == V[k][p - 1]) ? tgt : src;
}
//====================================================
