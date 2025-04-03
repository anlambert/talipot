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

#ifndef TALIPOT_FILTER_ITERATOR_H
#define TALIPOT_FILTER_ITERATOR_H

#include <talipot/Iterator.h>
#include <talipot/StlIterator.h>
#include <talipot/MemoryPool.h>

namespace tlp {
/**
 * @class FilterIterator
 * @ingroup Iterators
 * @brief Iterator that enables to filter an other Iterator
 * @param it the iterator that should be filtered
 * @param filter the functor or lambda function that enables to test whether or not an element is
 *filtered
 *
 * The functor function should have the following form:
 * @code
 * class AFilterFunctor {
 *  bool operator()(TYPE a) {
 *    return true if a should be iterated, false if a should be removed;
 *  }
 * };
 * @endcode
 **/
template <typename TYPE, typename FILTER>
class FilterIterator : public Iterator<TYPE> {
  public:
    FilterIterator(Iterator<TYPE> *it, FILTER filter) : _it(it), _filter(filter) {
        update();
    }
    ~FilterIterator() {
        delete _it;
    }
    TYPE next() {
        TYPE tmp = _curVal;
        update();
        return tmp;
    }
    bool hasNext() {
        return _hasNext;
    }

  private:
    void update() {
        _hasNext = false;

        while (_it->hasNext()) {
            _curVal = _it->next();

            if (_filter(_curVal)) {
                _hasNext = true;
                break;
            }
        }
    }

    bool _hasNext;
    Iterator<TYPE> *_it;
    TYPE _curVal;
    FILTER _filter;
    size_t _nbele;
};

template <typename TYPE, typename FILTER>
class MPFilterIterator : public FilterIterator<TYPE, FILTER>,
                         public MemoryPool<MPFilterIterator<TYPE, FILTER>> {
  public:
    MPFilterIterator(Iterator<TYPE> *it, FILTER filter)
        : FilterIterator<TYPE, FILTER>(it, filter) {}
};

/**
 * @brief Convenient function for creating a FilterIterator.
 * @ingroup Iterators
 *
 * Creates a FilterIterator from another iterator and a filter function.
 * The returned iterator takes ownership of the one provided as parameter.
 *
 * @param it a Talipot iterator
 * @param filter the functor or lambda function that enables to test whether or not an element is
 *filtered
 * @return a FilterIterator
 **/
template <typename TYPE, typename FILTER>
inline Iterator<TYPE> *filterIterator(Iterator<TYPE> *it, FILTER filter) {
    return new MPFilterIterator<TYPE, FILTER>(it, filter);
}

/**
 * @brief Convenient function for creating a FilterIterator from a STL container.
 * @ingroup Iterators
 *
 * Creates a FilterIterator from a STL container (std::list, std::vector, std::set, std::map, ...)
 * and a filter function.
 *
 * @param stlContainer any STL container
 * @param filter the functor or lambda function that enables to test whether or not an element is
 *filtered
 * @return a FilterIterator
 **/
template <typename Container, typename FILTER>
inline Iterator<typename Container::value_type> *filterIterator(const Container &stlContainer,
                                                                FILTER filter) {
    return new MPFilterIterator<typename Container::value_type, FILTER>(stlIterator(stlContainer),
                                                                        filter);
}
}
#endif // TALIPOT_FILTER_ITERATOR_H
