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

#include "GraphDimension.h"

#include <talipot/DoubleProperty.h>
#include <talipot/IntegerProperty.h>
#include <talipot/StringProperty.h>
#include <talipot/ConversionIterator.h>
#include <talipot/ConcatIterator.h>

using namespace tlp;
using namespace std;

map<Graph *, uint> GraphDimension::graphDimensionsMap;

GraphDimension::GraphDimension(Graph *graph, const string &dimName)
    : graph(graph), dimName(dimName) {
  nodeSorter = NodeMetricSorter::instance(graph);
  nodeSorter->sortNodesForProperty(dimName);
  propertyType = graph->getProperty(dimName)->getTypename();

  if (!graphDimensionsMap.contains(graph)) {
    graphDimensionsMap[graph] = 1;
  } else {
    ++graphDimensionsMap[graph];
  }
}

GraphDimension::~GraphDimension() {
  --graphDimensionsMap[graph];

  if (graphDimensionsMap[graph] == 0) {
    delete nodeSorter;
    graphDimensionsMap.erase(graph);
  }
}

void GraphDimension::updateNodesRank() {
  nodeSorter->sortNodesForProperty(dimName);
}

uint GraphDimension::numberOfItems() const {
  return graph->numberOfNodes();
}

uint GraphDimension::numberOfValues() const {
  return nodeSorter->getNbValuesForProperty(dimName);
}

template <typename PROPERTY>
double GraphDimension::getNodeValue(const node n) const {
  auto *dimValues = graph->getProperty<PROPERTY>(dimName);
  double d = dimValues->getNodeValue(n);
  double delta = maxValue() - minValue();
  return (d - minValue()) / delta;
}

std::string GraphDimension::getItemLabelAtRank(const uint rank) const {
  node n = nodeSorter->getNodeAtRankForProperty(rank, dimName);
  string label = graph->getStringProperty("viewLabel")->getNodeValue(n);
  return label;
}

std::string GraphDimension::getItemLabel(const uint itemId) const {
  string label = graph->getStringProperty("viewLabel")->getNodeValue(node(itemId));
  return label;
}

double GraphDimension::getItemValue(const uint itemId) const {
  if (propertyType == "double") {
    return getNodeValue<DoubleProperty>(node(itemId));
  } else if (propertyType == "int") {
    return getNodeValue<IntegerProperty>(node(itemId));
  } else {
    return 0;
  }
}

double GraphDimension::getItemValueAtRank(const uint rank) const {
  node n = nodeSorter->getNodeAtRankForProperty(rank, dimName);

  if (propertyType == "double") {
    return getNodeValue<DoubleProperty>(n);
  } else if (propertyType == "int") {
    return getNodeValue<IntegerProperty>(n);
  } else {
    return 0;
  }
}

uint GraphDimension::getItemIdAtRank(const uint rank) {
  node n = nodeSorter->getNodeAtRankForProperty(rank, dimName);
  return n.id;
}

uint GraphDimension::getRankForItem(const uint itemId) {
  return nodeSorter->getNodeRankForProperty(node(itemId), dimName);
}

double GraphDimension::minValue() const {
  if (propertyType == "double") {
    return graph->getDoubleProperty(dimName)->getNodeMin(graph);
  } else if (propertyType == "int") {
    return graph->getIntegerProperty(dimName)->getNodeMin(graph);
  } else {
    return 0;
  }
}

double GraphDimension::maxValue() const {
  if (propertyType == "double") {
    return graph->getDoubleProperty(dimName)->getNodeMax(graph);
  } else if (propertyType == "int") {
    return graph->getIntegerProperty(dimName)->getNodeMax(graph);
  } else {
    return 0;
  }
}

vector<uint> GraphDimension::links(const uint itemId) const {
  return iteratorVector(conversionIterator<uint>(
      concatIterator(graph->getInNodes(node(itemId)), graph->getOutNodes(node(itemId))),
      [](node n) { return n.id; }));
}
