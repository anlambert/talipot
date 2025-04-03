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

#ifndef TALIPOT_ITERATOR_H
#define TALIPOT_ITERATOR_H

#include <cassert>
#include <list>
#include <set>
#include <tuple>
#include <vector>

#include <talipot/config.h>

namespace tlp {

/**
 * @brief Interface for Talipot iterators.
 * Allows basic iteration operations only.
 *
 * Below are some examples about how to use Talipot iterators in C++ code.
 *
 * @code
 *
 * // graph is a pointer to a tlp::Graph instance
 *
 * // shortest syntax using C++11 for range based loops
 * for (auto n : graph->getNodes()) {
 *   // do something with n
 * }
 *
 *
 * // no syntactic sugar syntax (needs to explicitly delete the iterator
 * // after its use)
 * tlp::Iterator<tl::node> *nodesIt = graph->getNodes();
 * while (nodesIt->hasNext()) {
 *   tlp::node n = nodes->next();
 *   // do something with n
 * }
 * delete nodesIt;
 * @endcode
 **/
template <typename T>
struct Iterator {

    virtual ~Iterator() = default;
    /**
     * @brief Moves the Iterator on the next element.
     *
     * @return The current element pointed by the Iterator.
     **/
    virtual T next() = 0;

    /**
     * @brief Tells if the sequence is at its end.
     *
     * @return bool Whether there are more elements to iterate.
     **/
    virtual bool hasNext() = 0;

  private:
    // glue code in order to use Talipot iterators with C++11 for range based loops
    struct iterator_t {

        enum IteratorStatus { Begin = 0, Finished = 1, End = 3 };

        IteratorStatus _iteratorStatus;
        Iterator<T> *_it;

        iterator_t(Iterator<T> *it, IteratorStatus iteratorStatus = End)
            : _iteratorStatus(iteratorStatus), _it(it) {
            if (_iteratorStatus == Begin && !_it->hasNext()) {
                _iteratorStatus = Finished;
            }
        }

        ~iterator_t() {
            if (_iteratorStatus != End) {
                delete _it;
            }
        }

        bool operator!=(const iterator_t &it) const {
            return ((_iteratorStatus & it._iteratorStatus) == 0) || (_it != it._it);
        }

        const iterator_t &operator++() {
            if (!_it->hasNext()) {
                _iteratorStatus = Finished;
            }
            return *this;
        }

        T operator*() const {
            assert(_iteratorStatus != Finished);
            return _it->next();
        }
    };

  public:
    iterator_t begin() {
        return iterator_t(this, iterator_t::Begin);
    }

    iterator_t end() {
        return iterator_t(this);
    }
};

// as Iterator is only accessible through pointer
// we must have a specific definition of begin and end
template <typename T>
auto begin(Iterator<T> *it) -> decltype(it->begin()) {
    return it->begin();
}

template <typename T>
auto end(Iterator<T> *it) -> decltype(it->end()) {
    return it->end();
}

/**
 * @brief Counts the number of iterated elements
 * @ingroup Iterators
 *
 * Counts the number of elements iterated by the provided iterator.
 * That function takes ownership of the iterator and will delete it.
 *
 * @param it a Talipot iterator
 *
 * @return The number of iterated elements
 **/
template <typename T>
uint iteratorCount(Iterator<T> *it) {
    uint count = 0;
    while (it->hasNext()) {
        ++count;
        it->next();
    }
    delete it;
    return count;
}

/**
 * @brief Checks a minimum amount of iterated elements
 * @ingroup Iterators
 *
 * Checks if the iterator returns at least n values.
 * That function takes ownership of the iterator and will delete it.
 *
 * @param it a Talipot iterator
 *
 * @return true if the iterator returns at least n values
 **/
template <typename T>
bool iteratorCountCheck(Iterator<T> *it, uint minNbElements) {
    uint count = 0;
    while (it->hasNext()) {
        ++count;
        if (count == minNbElements) {
            delete it;
            return true;
        }
        it->next();
    }
    delete it;
    return false;
}

/**
 * @brief Checks if an iterator is empty
 * @ingroup Iterators
 *
 * Checks if the iterator does not return any value.
 * That function takes ownership of the iterator and will delete it.
 *
 * @param it a Talipot iterator
 *
 * @return true if the iterator is empty
 **/
template <typename T>
bool iteratorEmpty(Iterator<T> *it) {
    return !iteratorCountCheck(it, 1);
}

/**
 * @brief Applies a function to each iterated element
 * @ingroup Iterators
 *
 * Applies a function to each element iterated by the provided iterator.
 * That function takes ownership of the iterator and will delete it.
 *
 * @param it a Talipot iterator
 * @param mapFunction functor or lambda function taking one parameter of the same type of the
 *iterated elements
 *
 *
 **/
template <typename T, class MapFunction>
void iteratorMap(Iterator<T> *it, MapFunction &&mapFunction) {
    for (auto v : it) {
        mapFunction(v);
    }
}

/**
 * @brief Reduces iterated elements to a single value
 * @ingroup Iterators
 *
 * Produces a single value from the iterated elements (e.g. sum).
 * That function takes ownership of the iterator and will delete it.
 *
 * @param it a Talipot iterator
 * @param initVal initial value of the reduced result
 * @param reduceFunction functor or lambda function taking two parameters : first one being the
 *current reduced value,
 * second one being the current iterated element and returning intermediate reduced value
 *
 * @return the reduced value from the iterated elements
 *
 **/
template <typename T, typename reduceType, class ReduceFunction>
reduceType iteratorReduce(Iterator<T> *it, const reduceType &initVal,
                          ReduceFunction reduceFunction) {
    reduceType val = initVal;
    for (auto v : it) {
        val = reduceFunction(val, v);
    }

    return val;
}

/**
 * @brief Converts an iterator to a std::list
 * @ingroup Iterators
 *
 * Returns a std::list filled with the iterated elements.
 * That function takes ownership of the iterator and will delete it.
 *
 * @param it a Talipot iterator
 *
 * @return a std::list filled with iterated elements
 **/
template <typename T>
std::list<T> iteratorList(Iterator<T> *it) {
    std::list<T> ret;
    while (it->hasNext()) {
        ret.push_back(it->next());
    }
    delete it;
    return ret;
}

/**
 * @brief Converts an iterator to a std::vector
 * @ingroup Iterators
 *
 * Returns a std::vector filled with the iterated elements.
 * That function takes ownership of the iterator and will delete it.
 *
 * @param it a Talipot iterator
 *
 * @return a std::vector filled with iterated elements
 **/
template <typename T>
std::vector<T> iteratorVector(Iterator<T> *it) {
    std::vector<T> ret;
    while (it->hasNext()) {
        ret.push_back(it->next());
    }
    delete it;
    return ret;
}

/**
 * @brief Converts an iterator to a std::set
 * @ingroup Iterators
 *
 * Returns a std::set filled with the iterated elements.
 * That function takes ownership of the iterator and will delete it.
 *
 * @param it a Talipot iterator
 *
 * @return a std::set filled with iterated elements
 **/
template <typename T>
std::set<T> iteratorSet(Iterator<T> *it) {
    std::set<T> ret;
    while (it->hasNext()) {
        ret.insert(it->next());
    }
    delete it;
    return ret;
}
}

#endif // TALIPOT_ITERATOR_H
