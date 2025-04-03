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

#ifndef GRAPH_DIMENSION_H
#define GRAPH_DIMENSION_H

#include <talipot/Graph.h>

#include <string>
#include <map>

#include "DimensionBase.h"
#include "NodeMetricSorter.h"

class GraphDimension : public DimensionBase {

  public:
    GraphDimension(tlp::Graph *graph, const std::string &dimensionName);
    ~GraphDimension() override;

    uint numberOfItems() const override;
    uint numberOfValues() const override;
    std::string getItemLabelAtRank(const uint rank) const override;
    std::string getItemLabel(const uint itemId) const override;
    double getItemValue(const uint itemId) const override;
    double getItemValueAtRank(const uint rank) const override;
    uint getItemIdAtRank(const uint rank) override;
    uint getRankForItem(const uint itemId) override;
    double minValue() const override;
    double maxValue() const override;
    std::vector<uint> links(const uint itemId) const override;
    std::string getDimensionName() const override {
        return dimName;
    }
    tlp::Graph *getGraph() const {
        return graph;
    }
    void updateNodesRank();

  private:
    template <typename PROPERTY>
    double getNodeValue(const tlp::node n) const;

    tlp::Graph *graph;
    std::string dimName;
    std::string propertyType;
    std::vector<tlp::node> dataOrder;

    NodeMetricSorter *nodeSorter;
    static std::map<tlp::Graph *, uint> graphDimensionsMap;
};

#endif // GRAPH_DIMENSION_H
