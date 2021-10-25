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

#include "HierarchicalClustering.h"

using namespace std;
using namespace tlp;

PLUGIN(HierarchicalClustering)

//================================================================================
HierarchicalClustering::HierarchicalClustering(PluginContext *context) : Algorithm(context) {}
//================================================================================
HierarchicalClustering::~HierarchicalClustering() = default;
//================================================================================

class LessThan {
public:
  DoubleProperty *metric;
  bool operator()(node n1, node n2) const {
    return (metric->getNodeValue(n1) < metric->getNodeValue(n2));
  }
};

bool HierarchicalClustering::split(DoubleProperty *metric, list<node> &orderedNode) {

  for (auto n : graph->nodes()) {
    orderedNode.push_back(n);
  }

  LessThan comp;
  comp.metric = metric;
  orderedNode.sort(comp);

  list<node>::iterator itListNode;
  double tmpDbl;

  // We split the sorted list in two parts
  int nbElement = orderedNode.size();
  nbElement /= 2;

  if (nbElement < 10) {
    return (true);
  }

  itListNode = orderedNode.begin();
  tmpDbl = metric->getNodeValue(*itListNode);
  ++itListNode;
  --nbElement;

  while ((itListNode != orderedNode.end()) &&
         ((nbElement > 0) || (tmpDbl == metric->getNodeValue(*itListNode)))) {
    tmpDbl = metric->getNodeValue(*itListNode);
    ++itListNode;
    --nbElement;
  }

  orderedNode.erase(itListNode, orderedNode.end());
  return false;
}
//================================================================================
bool HierarchicalClustering::run() {

  string tmp1, tmp2;
  DoubleProperty *metric = graph->getDoubleProperty("viewMetric");
  tmp1 = "Hierar Sup";
  tmp2 = "Hierar Inf";
  bool result = false;

  while (!result) {
    list<node> badNodeList;
    result = split(metric, badNodeList);

    if (!result) {
      BooleanProperty sel1(graph);
      BooleanProperty sel2(graph);
      BooleanProperty splitRes(graph);

      sel1.setAllNodeValue(true);
      sel1.setAllEdgeValue(true);
      sel2.setAllNodeValue(true);
      sel2.setAllEdgeValue(true);
      splitRes.setAllNodeValue(true);
      splitRes.setAllEdgeValue(true);

      for (auto n : badNodeList) {
        splitRes.setNodeValue(n, false);
      }

      for (auto nit : graph->nodes()) {

        if (splitRes.getNodeValue(nit)) {
          sel2.setNodeValue(nit, false);

          for (auto ite : graph->incidence(nit)) {
            sel2.setEdgeValue(ite, false);
          }

        } else {
          sel1.setNodeValue(nit, false);

          for (auto ite : graph->incidence(nit)) {
            sel1.setEdgeValue(ite, false);
          }
        }
      }

      Graph *tmpSubGraph;
      tmpSubGraph = graph->addSubGraph(&sel1);
      tmpSubGraph->setAttribute<string>("name", tmp1);
      (graph->addSubGraph(&sel2))->setAttribute<string>("name", tmp2);
      graph = tmpSubGraph;
    }
  }

  return true;
}
