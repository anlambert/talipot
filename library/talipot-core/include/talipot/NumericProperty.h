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

#ifndef TALIPOT_NUMERIC_PROPERTY_H
#define TALIPOT_NUMERIC_PROPERTY_H

#include <talipot/PropertyInterface.h>

namespace tlp {

/**
 * @brief Interface all numerical properties.
 * Property values are always returned as double
 **/
class TLP_SCOPE NumericProperty : public PropertyInterface {
  public:
    /**
     * @brief Returns the value associated with the node n in this property.
     * @param n The node for which we want to get the value of the property.
     **/
    virtual double getNodeDoubleValue(const node n) const = 0;

    /**
     * @brief Gets the default node value of the property.
     * @return The default value of nodes.
     */
    virtual double getNodeDoubleDefaultValue() const = 0;

    /**
     * @brief Gets the minimum value on the nodes.
     * @param graph The graph on which to compute.
     * @return The minimal value on this graph for this property.
     **/
    virtual double getNodeDoubleMin(const Graph *graph = nullptr) = 0;

    /**
     * @brief Gets the maximum value on the nodes.
     * @param graph The graph on which to compute.
     * @return The maximal value on this graph for this property.
     **/
    virtual double getNodeDoubleMax(const Graph *graph = nullptr) = 0;

    /**
     * @brief Returns the value associated with the edge e in this property.
     * @param e The edge for which we want to get the value of the property.
     **/
    virtual double getEdgeDoubleValue(const edge e) const = 0;

    /**
     * @brief Gets the default edge value of the property.
     * @return The default value of edges.
     */
    virtual double getEdgeDoubleDefaultValue() const = 0;

    /**
     * @brief Gets the minimum value on the edges.
     * @param graph The graph on which to compute.
     * @return The minimal value on this graph for this property.
     **/
    virtual double getEdgeDoubleMin(const Graph *graph = nullptr) = 0;

    /**
     * @brief Gets the maximum value on the edges.
     * @param graph The graph on which to compute.
     * @return The maximal value on this graph for this property.
     **/
    virtual double getEdgeDoubleMax(const Graph *graph = nullptr) = 0;

    /**
     * @brief computes a uniform quantification for the nodes
     * associated values
     */
    virtual void nodesUniformQuantification(uint) = 0;

    /**
     * @brief computes a uniform quantification for the edges
     * associated values
     */
    virtual void edgesUniformQuantification(uint) = 0;

    /**
     * @brief computes a uniform quantification for the nodes/edges
     * associated values
     */
    void uniformQuantification(uint k) {
        nodesUniformQuantification(k);
        edgesUniformQuantification(k);
    }

    /**
     * @brief Creates a property of the same type (e.g. tlp::DoubleProperty)
     * The new property will be a copy of this property's values for all
     * the elements of the graph.
     * @param graph The Graph in which to create the new property.
     * @return The newly created property.
     */
    virtual NumericProperty *copyProperty(Graph *graph) = 0;

    /**
     * @brief Gets an iterator sorting nodes according to their values in that numeric property.
     *
     * @param sg If provided, returns an iterator on the subset of nodes defined by that subgraph.
     * @return An iterator over graph nodes.
     **/
    virtual Iterator<node> *getSortedNodes(const Graph *sg = nullptr, bool ascendingOrder = true);

    /**
     * @brief Gets an iterator sorting edges according to their values in that numeric property.
     *
     * @param sg If provided, returns an iterator on the subset of edges defined by that subgraph.
     * @param ascendingOrder defines the sort ordering (ascending or descending).
     * @return An iterator over graph edges.
     **/
    virtual Iterator<edge> *getSortedEdges(const Graph *sg = nullptr, bool ascendingOrder = true);

    /**
     * @brief Gets an iterator sorting edges according to the values of their source nodes in that
     *numeric property.
     *
     * @param sg If provided, returns an iterator on the subset of edges defined by that subgraph.
     * @param ascendingOrder defines the sort ordering (ascending or descending).
     * @return An iterator over graph edges.
     **/
    virtual Iterator<edge> *getSortedEdgesBySourceValue(const Graph *sg = nullptr,
                                                        bool ascendingOrder = true);

    /**
     * @brief Gets an iterator sorting edges according to the values of their target nodes in that
     *numeric property.
     *
     * @param sg If provided, returns an iterator on the subset of edges defined by that subgraph.
     * @param ascendingOrder defines the sort ordering (ascending or descending).
     * @return An iterator over graph edges.
     **/
    virtual Iterator<edge> *getSortedEdgesByTargetValue(const Graph *sg = nullptr,
                                                        bool ascendingOrder = true);

    /**
     * @brief Gets an iterator sorting edges according to the values of their extremities in that
     *numeric property.
     * Vectors of two numbers (first element being the source node value, second one the target node
     *value) are compared in that case.
     *
     * @param sg If provided, returns an iterator on the subset of edges defined by that subgraph.
     * @param ascendingOrder defines the sort ordering (ascending or descending).
     * @return An iterator over graph edges.
     **/
    virtual Iterator<edge> *getSortedEdgesByExtremitiesValues(const Graph *sg = nullptr,
                                                              bool ascendingOrder = true);
};
}

#endif // TALIPOT_NUMERIC_PROPERTY_H
