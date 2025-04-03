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

#include "PathLengthMetric.h"

PLUGIN(PathLengthMetric)

using namespace std;
using namespace tlp;

//=======================================
PathLengthMetric::PathLengthMetric(const tlp::PluginContext *context) : DoubleAlgorithm(context) {
    // Leaf metric needed
    addDependency("Leaf", "1.0");
}

// structure below is used to implement dfs loop
struct dfsStruct {
    node current;
    Iterator<node> *outNodes;
    double res;

    dfsStruct(node n, Iterator<node> *nodes) : current(n), outNodes(nodes), res(0.0) {}
};
//=======================================
// original recursive algorithm
/*double PathLengthMetric::getNodeValue(const tlp::node n) {
    if (graph->outdeg(n)==0) return 0.0;
    if (result->getNodeValue(n) > 0.1)
      return result->getNodeValue(n);
    for (auto child : graph->getOutNodes(n)) {
      res += getNodeValue(child);
    }
    res += leafMetric->getNodeValue(n);
    result->setNodeValue(n, res);
    return res;
  }*/
//=======================================================================
double PathLengthMetric::getNodeValue(tlp::node current, tlp::DoubleProperty *leafMetric) {
    if (graph->outdeg(current) == 0) {
        return 0.0;
    }

    double value = result->getNodeValue(current);

    if (value > 0.1) {
        return value;
    }

    // dfs loop
    stack<dfsStruct> dfsLevels;
    Iterator<node> *outNodes = graph->getOutNodes(current);
    dfsStruct dfsParams(current, outNodes);
    double res = 0.0;
    dfsLevels.push(dfsParams);

    while (!dfsLevels.empty()) {
        while (outNodes->hasNext()) {
            node neighbour = outNodes->next();
            value = result->getNodeValue(neighbour);

            // compute result
            if (value > 0.1) {
                res += value;
            } else {
                outNodes = graph->getOutNodes(neighbour);

                if (outNodes->hasNext()) {
                    // store result for current
                    dfsLevels.top().res = res;
                    // push new dfsParams on stack
                    current = dfsParams.current = neighbour;
                    dfsParams.outNodes = outNodes;
                    res = dfsParams.res = 0.0;
                    dfsLevels.push(dfsParams);
                    // and go deeper
                    break;
                } else {
                    delete outNodes;
                    outNodes = dfsParams.outNodes;
                }
            }
        }

        if (outNodes->hasNext()) {
            // new dfsParams has been pushed on stack
            continue;
        }

        res += leafMetric->getNodeValue(current);
        // save current result
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
        // update new current result if any
        dfsParams.res += res;
        res = dfsParams.res;
    }

    return res;
}
//=======================================
bool PathLengthMetric::run() {
    result->setAllNodeValue(0);
    result->setAllEdgeValue(0);
    DoubleProperty leafMetric(graph);
    string errorMsg;

    if (!graph->applyPropertyAlgorithm("Leaf", &leafMetric, errorMsg)) {
        tlp::warning() << errorMsg << endl;
        return false;
    }

    for (auto n : graph->nodes()) {
        getNodeValue(n, &leafMetric);
    }
    return true;
}
//=======================================
bool PathLengthMetric::check(std::string &erreurMsg) {
    if (AcyclicTest::isAcyclic(graph)) {
        return true;
    } else {
        erreurMsg = "The graph must be acyclic.";
        return false;
    }
}
//=======================================
