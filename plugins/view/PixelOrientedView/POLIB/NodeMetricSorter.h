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

#ifndef NODE_METRIC_SORTER_H
#define NODE_METRIC_SORTER_H

#include <talipot/Graph.h>
#include <talipot/Observable.h>
#include <talipot/TypeInterface.h>

#include <map>
#include <vector>

template <typename PROPERTYTYPE, typename PROPERTY>
class NodeMetricPropertyOrderRelation {

  public:
    NodeMetricPropertyOrderRelation(tlp::Graph *graph, const std::string &dimName) {
        nodeProperty = graph->getProperty<PROPERTY>(dimName);
    }

    bool operator()(tlp::node n1, tlp::node n2) const {
        REAL_TYPE(PROPERTYTYPE) v1 = nodeProperty->getNodeValue(n1);
        REAL_TYPE(PROPERTYTYPE) v2 = nodeProperty->getNodeValue(n2);
        return v1 < v2;
    }

  private:
    PROPERTY *nodeProperty;
};

class NodeMetricSorter {

  public:
    static NodeMetricSorter *instance(tlp::Graph *graph);

    ~NodeMetricSorter();

    void sortNodesForProperty(const std::string &propertyName);
    void cleanupSortNodesForProperty(const std::string &propertyName);
    uint getNbValuesForProperty(const std::string &propertyName);
    tlp::node getNodeAtRankForProperty(const uint rank, const std::string &propertyName);
    uint getNodeRankForProperty(tlp::node n, const std::string &propertyName);

  private:
    NodeMetricSorter(tlp::Graph *graph);

    void reset();

    tlp::Graph *graph;
    std::map<std::string, std::vector<tlp::node>> nodeSortingMap;
    std::map<std::string, uint> nbValuesPropertyMap;

    static std::map<tlp::Graph *, NodeMetricSorter *> instances;
};

#endif // NODE_METRIC_SORTER_H
