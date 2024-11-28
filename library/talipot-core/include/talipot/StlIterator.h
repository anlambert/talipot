/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_STL_ITERATOR_H
#define TALIPOT_STL_ITERATOR_H

#include <map>
#include <type_traits>

#include <talipot/Iterator.h>
#include <talipot/MemoryPool.h>

namespace tlp {

/**
 * @class StlIterator
 * @ingroup Iterators
 *
 * @brief StlIterator wraps a stl iterator.
 *
 * Creates a Talipot iterator from two stl iterators.
 *
 * @param startIt a stl iterator from which to begin the iteration
 * @param endIt a stl iterator on which to end the iteration
 *
 **/
template <typename T, typename ITERATOR>
struct StlIterator : public Iterator<T> {
  StlIterator(const ITERATOR &startIt, const ITERATOR &endIt) : it(startIt), itEnd(endIt) {}
  T next() {
    return *it++;
  }
  bool hasNext() {
    return (itEnd != it);
  }

private:
  ITERATOR it, itEnd;
};
//=================================================
template <typename T, typename ITERATOR>
struct MPStlIterator : public StlIterator<T, ITERATOR>,
                       public MemoryPool<MPStlIterator<T, ITERATOR>> {
  MPStlIterator(const ITERATOR &startIt, const ITERATOR &endIt)
      : StlIterator<T, ITERATOR>(startIt, endIt) {}
};
//=================================================

/**
 * @brief Convenient function for creating a StlIterator from a stl container.
 * @ingroup Iterators
 *
 * Creates a StlIterator from a STL container (std::list, std::vector, std::set, std::map, ...).
 *
 * @param stlContainer any STL container
 * @return a StlIterator
 **/
template <typename Container>
inline Iterator<typename Container::value_type> *stlIterator(const Container &stlContainer) {
  return new MPStlIterator<typename Container::value_type, typename Container::const_iterator>(
      stlContainer.begin(), stlContainer.end());
}

//=================================================
template <typename Map>
struct StlMapIterator
    : public Iterator<std::pair<typename Map::key_type, typename Map::mapped_type>> {
  StlMapIterator(typename Map::const_iterator startIt, typename Map::const_iterator endIt)
      : it(startIt), itEnd(endIt) {}

  std::pair<typename Map::key_type, typename Map::mapped_type> next() {
    return *it++;
  }

  bool hasNext() {
    return (itEnd != it);
  }

private:
  typename Map::const_iterator it, itEnd;
};
//=================================================
template <typename Map>
inline StlMapIterator<Map> *stlMapIterator(const Map &map) {
  return new StlMapIterator<Map>(map.begin(), map.end());
}
//=================================================
template <typename Map>
struct StlMapKeyIterator : public tlp::Iterator<typename Map::key_type> {
  StlMapKeyIterator(typename Map::const_iterator startIt, typename Map::const_iterator endIt)
      : it(startIt), itEnd(endIt) {}

  typename Map::key_type next() {
    auto tmp = it->first;
    ++it;
    return tmp;
  }

  bool hasNext() {
    return it != itEnd;
  }

private:
  typename Map::const_iterator it, itEnd;
};
//=================================================
template <typename Map>
inline StlMapKeyIterator<Map> *stlMapKeyIterator(const Map &map) {
  return new StlMapKeyIterator<Map>(map.begin(), map.end());
}
//=================================================
template <typename Map>
struct StlMapValueIterator : public tlp::Iterator<typename Map::mapped_type> {
  StlMapValueIterator(typename Map::const_iterator startIt, typename Map::const_iterator endIt)
      : it(startIt), itEnd(endIt) {}

  typename Map::mapped_type next() {
    auto tmp = it->second;
    ++it;
    return tmp;
  }

  bool hasNext() {
    return it != itEnd;
  }

private:
  typename Map::const_iterator it, itEnd;
};
//=================================================
template <typename Map>
inline StlMapValueIterator<Map> *stlMapValueIterator(const Map &map) {
  return new StlMapValueIterator<Map>(map.begin(), map.end());
}
//=================================================
template <typename Container>
struct reverse_wrapper {
  Container &_c;
  reverse_wrapper(Container &c) : _c(c) {}

  typename Container::reverse_iterator begin() {
    return _c.rbegin();
  }
  typename Container::reverse_iterator end() {
    return _c.rend();
  }
};

template <typename Container>
struct reverse_wrapper_copy {
  Container _c;
  reverse_wrapper_copy(Container c) : _c(std::move(c)) {}

  typename Container::reverse_iterator begin() {
    return _c.rbegin();
  }
  typename Container::reverse_iterator end() {
    return _c.rend();
  }
};

template <typename Container>
struct const_reverse_wrapper {
  const Container &_c;
  const_reverse_wrapper(const Container &c) : _c(c) {}

  typename Container::const_reverse_iterator begin() const {
    return _c.rbegin();
  }
  typename Container::const_reverse_iterator end() const {
    return _c.rend();
  }
};

template <typename Container>
struct const_reverse_wrapper_copy {
  const Container _c;
  const_reverse_wrapper_copy(const Container c) : _c(std::move(c)) {}

  typename Container::const_reverse_iterator begin() const {
    return _c.rbegin();
  }
  typename Container::const_reverse_iterator end() const {
    return _c.rend();
  }
};

template <typename Container>
inline reverse_wrapper<Container> reversed(Container &c) {
  return reverse_wrapper<Container>(c);
}

template <typename Container>
inline reverse_wrapper_copy<Container> reversed(Container &&c) {
  return reverse_wrapper_copy<Container>(std::move(c));
}

template <typename Container>
inline const_reverse_wrapper<Container> reversed(const Container &c) {
  return const_reverse_wrapper<Container>(c);
}

template <typename Container>
inline const_reverse_wrapper_copy<Container> reversed(const Container &&c) {
  return const_reverse_wrapper_copy<Container>(std::move(c));
}

}

#endif // TALIPOT_STL_ITERATOR_H
