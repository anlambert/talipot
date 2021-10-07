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

#include <stack>

#include <talipot/DoubleProperty.h>
#include <talipot/PropertyAlgorithm.h>

using namespace std;
using namespace tlp;

// simple structure to implement
// the dfs biconnected component loop
struct dfsBicoTestStruct {
  node v;
  node opp;
  Iterator<edge> *ite;

  dfsBicoTestStruct(node n, node o, Iterator<edge> *it) : v(n), opp(o), ite(it) {}
};
// dfs biconnected component loop
static void bicoTestAndLabeling(const Graph &graph, node v, MutableContainer<int> &compnum,
                                MutableContainer<int> &dfsnum, MutableContainer<int> &lowpt,
                                MutableContainer<node> &father, stack<node> &current, int &count1,
                                int &count2) {
  Iterator<edge> *it = graph.getInOutEdges(v);
  stack<dfsBicoTestStruct> dfsLevels;
  dfsBicoTestStruct dfsParams(v, node(), it);
  dfsLevels.push(dfsParams);
  lowpt.set(v.id, dfsnum.get(v.id));

  while (!dfsLevels.empty()) {
    dfsParams = dfsLevels.top();
    v = dfsParams.v;
    it = dfsParams.ite;

    if (it->hasNext()) {
      edge e = it->next();
      node w = graph.opposite(e, v);

      if (dfsnum.get(w.id) == -1) {
        dfsnum.set(w.id, ++count1);
        current.push(w);
        father.set(w.id, v);
        dfsParams.v = w;
        dfsParams.opp = v;
        dfsParams.ite = graph.getInOutEdges(w);
        dfsLevels.push(dfsParams);
        lowpt.set(w.id, dfsnum.get(w.id));
      } else {
        lowpt.set(v.id, std::min(lowpt.get(v.id), dfsnum.get(w.id)));
      }
    } else {
      delete it;
      dfsLevels.pop();
      node opp = dfsParams.opp;

      if (opp.isValid()) {
        lowpt.set(opp.id, std::min(lowpt.get(opp.id), lowpt.get(v.id)));
      }

      if (father.get(v.id).isValid() && (lowpt.get(v.id) == dfsnum.get(father.get(v.id).id))) {
        node w;

        do {
          w = current.top();
          current.pop();
          it = graph.getInOutEdges(w);

          while (it->hasNext()) {
            edge e = it->next();

            if (dfsnum.get(w.id) > dfsnum.get(graph.opposite(e, w).id)) {
              compnum.set(e.id, count2);
            }
          }

          delete it;
        } while (w != v);

        count2++;
      }
    }
  }
}

//=============================================================================================
int biconnectedComponents(const Graph &graph, MutableContainer<int> &compnum) {
  stack<node> current;
  MutableContainer<int> dfsnum;
  dfsnum.setAll(-1);
  MutableContainer<int> lowpt;
  lowpt.setAll(0);
  MutableContainer<node> father;
  father.setAll(node());
  int count1 = 0;
  int count2 = 0;
  int num_isolated = 0;
  node v;
  Iterator<node> *it = graph.getNodes();

  while (it->hasNext()) {
    v = it->next();

    if (dfsnum.get(v.id) == -1) {
      dfsnum.set(v.id, ++count1);
      bool is_isolated = true;

      for (auto e : graph.getInOutEdges(v)) {
        if (graph.opposite(e, v) != v) {
          is_isolated = false;
          break;
        }
      }

      if (is_isolated) {
        num_isolated++;
      } else {
        current.push(v);
        bicoTestAndLabeling(graph, v, compnum, dfsnum, lowpt, father, current, count1, count2);
        current.pop();
      }
    }
  }

  delete it;
  return (count2 + num_isolated);
}
//=============================================================================================

/** This plugin is an implementation of a biconnected component decomposition algorithm. It assigns
 *  the same value to all the edges in the same component.
 *
 */
class BiconnectedComponents : public DoubleAlgorithm {
public:
  PLUGININFORMATION("Biconnected Components", "David Auber", "03/01/2005",
                    "Implements a biconnected component decomposition."
                    "It assigns the same value to all the edges in the same component.",
                    "1.0", "Component")
  BiconnectedComponents(const tlp::PluginContext *context) : DoubleAlgorithm(context) {
    addOutParameter<uint>("#biconnected components", "Number of biconnected components found");
  }
  bool run() override {
    MutableContainer<int> compo;
    compo.setAll(-1);
    biconnectedComponents(*graph, compo);
    result->setAllEdgeValue(-1);

    int maxVal = -1;
    for (auto e : graph->edges()) {
      int val = compo.get(e.id);
      result->setEdgeValue(e, val);
      maxVal = std::max(val, maxVal);
    }

    if (dataSet != nullptr) {
      dataSet->set("#biconnected components", uint(maxVal + 1));
    }

    return true;
  }
};

PLUGIN(BiconnectedComponents)
