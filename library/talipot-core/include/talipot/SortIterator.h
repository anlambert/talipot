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

#ifndef TALIPOT_SORT_ITERATOR_H
#define TALIPOT_SORT_ITERATOR_H

#include <vector>
#include <algorithm>

#include <talipot/Iterator.h>
#include <talipot/StableIterator.h>
#include <talipot/NumericProperty.h>
#include <talipot/Graph.h>
#include <talipot/Vector.h>

namespace tlp {
class Graph;

struct LessThan {
    LessThan(const tlp::NumericProperty *m) : metric(m) {}
    const tlp::NumericProperty *metric;
    bool operator()(const node &n1, const node &n2) const {
        return (metric->getNodeDoubleValue(n1) < metric->getNodeDoubleValue(n2));
    }
    bool operator()(const edge &e1, const edge &e2) const {
        return (metric->getEdgeDoubleValue(e1) < metric->getEdgeDoubleValue(e2));
    }
};

struct LessThanEdgeTargetMetric {
    LessThanEdgeTargetMetric(const Graph *sg, const tlp::NumericProperty *metric)
        : metric(metric), sg(sg) {}
    bool operator()(const edge &e1, const edge &e2) const {
        return (metric->getNodeDoubleValue(sg->target(e1)) <
                metric->getNodeDoubleValue(sg->target(e2)));
    }

  private:
    const tlp::NumericProperty *metric;
    const Graph *sg;
};

struct LessThanEdgeSourceMetric {
    LessThanEdgeSourceMetric(const Graph *sg, const tlp::NumericProperty *metric)
        : metric(metric), sg(sg) {}
    bool operator()(const edge &e1, const edge &e2) const {
        return (metric->getNodeDoubleValue(sg->source(e1)) <
                metric->getNodeDoubleValue(sg->source(e2)));
    }

  private:
    const tlp::NumericProperty *metric;
    const Graph *sg;
};

struct LessThanEdgeExtremitiesMetric {
    LessThanEdgeExtremitiesMetric(const Graph *sg, const tlp::NumericProperty *metric)
        : metric(metric), sg(sg) {}
    bool operator()(const edge &e1, const edge &e2) const {
        const auto &[src1, tgt1] = sg->ends(e1);
        Vec2d v1 = {metric->getNodeDoubleValue(src1), metric->getNodeDoubleValue(tgt1)};
        const auto &[src2, tgt2] = sg->ends(e2);
        Vec2d v2 = {metric->getNodeDoubleValue(src2), metric->getNodeDoubleValue(tgt2)};
        return v1 < v2;
    }

  private:
    const tlp::NumericProperty *metric;
    const Graph *sg;
};

/**
 * @brief This Iterator sorts the nodes in a sequence based on their values in a NumericProperty.
 **/
struct SortNodeIterator : public StableIterator<tlp::node> {

    SortNodeIterator(Iterator<tlp::node> *itIn, const tlp::NumericProperty *metric,
                     bool ascendingOrder = true)
        : StableIterator<tlp::node>(itIn) {
        LessThan tmp(metric);
        sort(sequenceCopy.begin(), sequenceCopy.end(), tmp);

        if (!ascendingOrder) {
            reverse(sequenceCopy.begin(), sequenceCopy.end());
        }

        copyIterator = sequenceCopy.begin();
    }

    ~SortNodeIterator() override = default;
};

/**
 * @brief This Iterator sorts the edges in a sequence based on their values in a NumericProperty.
 **/
struct SortEdgeIterator : public StableIterator<tlp::edge> {

    SortEdgeIterator(Iterator<tlp::edge> *itIn, const tlp::NumericProperty *metric,
                     bool ascendingOrder = true)
        : StableIterator<tlp::edge>(itIn) {
        LessThan tmp(metric);
        sort(sequenceCopy.begin(), sequenceCopy.end(), tmp);

        if (!ascendingOrder) {
            reverse(sequenceCopy.begin(), sequenceCopy.end());
        }

        copyIterator = sequenceCopy.begin();
    }

    ~SortEdgeIterator() override = default;
};

/**
 * @brief This Iterator sorts the edges based on the values of their target nodes in a
 *NumericProperty.
 **/
struct SortTargetEdgeIterator : public StableIterator<tlp::edge> {

    SortTargetEdgeIterator(Iterator<tlp::edge> *itIn, const Graph *sg,
                           const tlp::NumericProperty *metric, bool ascendingOrder = true)
        : StableIterator<tlp::edge>(itIn) {
        LessThanEdgeTargetMetric tmp(sg, metric);
        sort(sequenceCopy.begin(), sequenceCopy.end(), tmp);

        if (!ascendingOrder) {
            reverse(sequenceCopy.begin(), sequenceCopy.end());
        }

        copyIterator = sequenceCopy.begin();
    }

    ~SortTargetEdgeIterator() override = default;
};

/**
 * @brief This Iterator sorts the edges based on the values of their source nodes in a
 *NumericProperty.
 **/
struct SortSourceEdgeIterator : public StableIterator<tlp::edge> {

    SortSourceEdgeIterator(Iterator<tlp::edge> *itIn, const Graph *sg,
                           const tlp::NumericProperty *metric, bool ascendingOrder = true)
        : StableIterator<tlp::edge>(itIn) {
        LessThanEdgeSourceMetric tmp(sg, metric);
        sort(sequenceCopy.begin(), sequenceCopy.end(), tmp);

        if (!ascendingOrder) {
            reverse(sequenceCopy.begin(), sequenceCopy.end());
        }

        copyIterator = sequenceCopy.begin();
    }

    ~SortSourceEdgeIterator() override = default;
};

/**
 * @brief This Iterator sorts the edges based on the values of their extremities nodes in a
 *NumericProperty.
 **/
struct SortExtremitiesEdgeIterator : public StableIterator<tlp::edge> {

    SortExtremitiesEdgeIterator(Iterator<tlp::edge> *itIn, const Graph *sg,
                                const tlp::NumericProperty *metric, bool ascendingOrder = true)
        : StableIterator<tlp::edge>(itIn) {
        LessThanEdgeExtremitiesMetric tmp(sg, metric);
        sort(sequenceCopy.begin(), sequenceCopy.end(), tmp);

        if (!ascendingOrder) {
            reverse(sequenceCopy.begin(), sequenceCopy.end());
        }

        copyIterator = sequenceCopy.begin();
    }

    ~SortExtremitiesEdgeIterator() override = default;
};

/**
 * @class SortIterator
 * @ingroup Iterators
 * @brief Iterator that wraps an existing one and sorts its iterated elements based on comparison
 *function.
 *
 * @param it the iterator to sort
 * @param comp functor or lambda function taking two parameters of type const T& and returning a
 *boolean:
 * true if the first parameter is lesser or equal than the second one, false otherwise
 *
 **/
template <typename T, typename CompareFunction>
struct SortIterator : public tlp::StableIterator<T> {

    SortIterator(Iterator<T> *itIn, CompareFunction &&compFunc) : StableIterator<T>(itIn) {
        sort(this->sequenceCopy.begin(), this->sequenceCopy.end(), compFunc);
        this->copyIterator = this->sequenceCopy.begin();
    }

    ~SortIterator() = default;
};

/**
 * @brief Convenient function for creating a SortIterator.
 * @ingroup Iterators
 *
 * Creates a SortIterator from another Iterator and a comparison function.
 * The returned Iterator takes ownership of the one provided as parameter.
 *
 * @param it a Talipot iterator
 * @param compFunc functor or lambda function taking two parameters of type const T& and returning a
 *boolean:
 * true if the first parameter is lesser or equal than the second one, false otherwise
 *
 * @return a SortIterator
 *
 **/
template <typename T, typename CompareFunction>
inline Iterator<T> *sortIterator(Iterator<T> *it, CompareFunction &&compFunc) {
    return new SortIterator<T, CompareFunction>(it, compFunc);
}

/**
 * @brief Convenient function for creating a SortIterator from a STL container.
 * @ingroup Iterators
 *
 * Creates a SortIterator from another Iterator and a comparison function.
 *
 * @param stlContainer any STL container
 * @param compFunc functor or lambda function taking two parameters of type const T& and returning a
 *boolean:
 * true if the first parameter is lesser or equal than the second one, false otherwise
 *
 * @return a SortIterator
 *
 **/
template <typename Container, typename CompareFunction>
inline Iterator<typename Container::value_type> *sortIterator(const Container &stlContainer,
                                                              CompareFunction &&compFunc) {
    return new SortIterator<typename Container::value_type, CompareFunction>(
        stlIterator(stlContainer), compFunc);
}
}
#endif // TALIPOT_SORT_ITERATOR_H
