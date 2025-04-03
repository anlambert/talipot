/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "NodeMetric.h"

PLUGIN(NodeMetric)

using namespace std;
using namespace tlp;

//====================================================================
NodeMetric::NodeMetric(const tlp::PluginContext *context) : DoubleAlgorithm(context) {}

// structure below is used to implement dfs loop
struct dfsStruct {
    node current;
    Iterator<node> *outNodes;
    double res;

    dfsStruct(node n, Iterator<node> *nodes) : current(n), outNodes(nodes), res(1.0) {}
};
//=======================================================================
// original recursive algorithm
/*double NodeMetric::getNodeValue(const tlp::node n) {
  if (result->getNodeValue(n)!=0)
      return result->getNodeValue(n);
    double res = 1;
    for(const node &m : graph->getOutNodes(n))
      res += getNodeValue(m);
    result->setNodeValue(n, res);
    return res;
  }*/
//=======================================================================
double NodeMetric::getNodeValue(tlp::node current) {
    double value = result->getNodeValue(current);

    if (value != 0.0) {
        return value;
    }

    // dfs loop
    stack<dfsStruct> dfsLevels;
    Iterator<node> *outNodes = graph->getOutNodes(current);
    dfsStruct dfsParams(current, outNodes);
    double res = 1.0;
    dfsLevels.push(dfsParams);

    while (!dfsLevels.empty()) {
        while (outNodes->hasNext()) {
            node neighbour = outNodes->next();
            value = result->getNodeValue(neighbour);

            // compute res
            if (value != 0.0) {
                res += value;
            } else {
                // store res for current
                dfsLevels.top().res = res;
                // push new dfsParams on stack
                current = dfsParams.current = neighbour;
                outNodes = dfsParams.outNodes = graph->getOutNodes(neighbour);
                res = dfsParams.res = 1.0;
                dfsLevels.push(dfsParams);
                // and go deeper
                break;
            }
        }

        if (outNodes->hasNext()) {
            // new dfsParams has been pushed on stack
            continue;
        }

        // save current res
        result->setNodeValue(current, res);
        // unstack current dfsParams
        delete outNodes;
        dfsLevels.pop();

        if (dfsLevels.empty()) {
            break;
        }

        // get dfsParams on top of dfsLevels
        dfsParams = dfsLevels.top();
        current = dfsParams.current;
        outNodes = dfsParams.outNodes;
        // update new current res if any
        dfsParams.res += res;
        res = dfsParams.res;
    }

    return res;
}
//====================================================================
bool NodeMetric::run() {
    result->setAllEdgeValue(0);
    result->setAllNodeValue(0);
    for (auto n : graph->nodes()) {
        result->setNodeValue(n, getNodeValue(n));
    }
    return true;
}
//====================================================================
bool NodeMetric::check(std::string &erreurMsg) {
    if (!AcyclicTest::isAcyclic(graph)) {
        erreurMsg = "The graph must be acyclic.";
        return false;
    }
    return true;
}
//===================================================================
