/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <algorithm>
#include "NodeMetricSorter.h"

#include <talipot/DoubleProperty.h>
#include <talipot/IntegerProperty.h>

using namespace std;
using namespace tlp;

namespace pocore {

map<Graph *, NodeMetricSorter *> NodeMetricSorter::instances;

NodeMetricSorter *NodeMetricSorter::getInstance(Graph *graph) {
  if (instances.find(graph) == instances.end()) {
    instances[graph] = new NodeMetricSorter(graph);
  }

  return instances[graph];
}

NodeMetricSorter::NodeMetricSorter(Graph *graph) : graph(graph) {}

NodeMetricSorter::~NodeMetricSorter() {
  reset();
  instances.erase(graph);
}

void NodeMetricSorter::sortNodesForProperty(const string &propertyName) {
  cleanupSortNodesForProperty(propertyName);
  nodeSortingMap[propertyName] = graph->nodes();

  const string &propertyType = graph->getProperty(propertyName)->getTypename();

  if (propertyType == "double") {
    sort(nodeSortingMap[propertyName].begin(), nodeSortingMap[propertyName].end(),
         NodeMetricPropertyOrderRelation<DoubleType, DoubleProperty>(graph, propertyName));
  } else if (propertyType == "int") {
    sort(nodeSortingMap[propertyName].begin(), nodeSortingMap[propertyName].end(),
         NodeMetricPropertyOrderRelation<IntegerType, IntegerProperty>(graph, propertyName));
  }
}

void NodeMetricSorter::cleanupSortNodesForProperty(const std::string &propertyName) {
  nodeSortingMap.erase(propertyName);
}

node NodeMetricSorter::getNodeAtRankForProperty(const unsigned int rank,
                                                const string &propertyName) {
  if (nodeSortingMap.find(propertyName) == nodeSortingMap.end()) {
    sortNodesForProperty(propertyName);
  }

  return nodeSortingMap[propertyName][rank];
}

unsigned int NodeMetricSorter::getNbValuesForProperty(const string &propertyName) {
  if (nbValuesPropertyMap.find(propertyName) == nbValuesPropertyMap.end()) {
    unsigned int count = 0;
    const string &propertyType = graph->getProperty(propertyName)->getTypename();

    if (propertyType == "double") {
      set<double> sd;

      for (auto n : graph->nodes()) {
        sd.insert(graph->getProperty<DoubleProperty>(propertyName)->getNodeValue(n));
      }

      count = sd.size();
    } else if (propertyType == "int") {
      set<int> si;

      for (auto n : graph->nodes()) {
        si.insert(graph->getProperty<IntegerProperty>(propertyName)->getNodeValue(n));
      }

      count = si.size();
    }

    nbValuesPropertyMap[propertyName] = count;
  }

  return nbValuesPropertyMap[propertyName];
}

void NodeMetricSorter::reset() {
  nodeSortingMap.clear();
}

unsigned int NodeMetricSorter::getNodeRankForProperty(tlp::node n, const string &propertyName) {
  if (nodeSortingMap.find(propertyName) == nodeSortingMap.end()) {
    sortNodesForProperty(propertyName);
  }

  for (unsigned int i = 0; i < nodeSortingMap[propertyName].size(); ++i) {
    if (nodeSortingMap[propertyName][i] == n) {
      return i;
    }
  }

  return 0;
}
} // namespace pocore
