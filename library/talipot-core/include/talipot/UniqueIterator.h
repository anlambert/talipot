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

#ifndef TALIPOT_UNIQUE_ITERATOR_H
#define TALIPOT_UNIQUE_ITERATOR_H

#include <set>

#include <talipot/Iterator.h>
#include <talipot/StlIterator.h>
#include <talipot/MemoryPool.h>

//===============================================================================
namespace tlp {
/**
 * @brief That class implement a default functor that always returns true
 *
 * @see UniqueIterator
 */
template <typename T>
struct CheckAllFunctor {
  bool operator()(const T &) {
    return true;
  }
};

/**
 * @class UniqueIterator
 * @ingroup Iterators
 *
 * @brief UniqueIterator enables to remove duplicated elements in an iterator
 *
 * @param it the iterator in which we want to filter out duplicated elements
 * @param checkFunctor a functor or a lambda function that enables to indicate whether or not the
 *element could be duplicated (default test all elements)
 *
 * The functor function should have the following form
 * @code
 * template <typename T>
 * class ACheckFunctor {
 *  bool operator(const T &a) {
 *   return true if a could be duplicated else false;
 *  }
 * };
 * @endcode
 * checkFunctor are used for optimization purpose to prevent to log(n) test for all elements when
 *not necessary.
 **/
template <typename T, typename CheckFunc = CheckAllFunctor<T>>
class UniqueIterator : public Iterator<T> {
public:
  UniqueIterator(Iterator<T> *it, CheckFunc checkFunctor = CheckFunc())
      : _it(it), _checkFunctor(checkFunctor) {
    update();
  }
  //================================================
  ~UniqueIterator() {
    delete _it;
  }
  //================================================
  T next() {
    T tmp = curVal;
    update();
    return tmp;
  }
  //================================================
  bool hasNext() {
    return _hasNext;
  }
  //================================================
  void update() {
    _hasNext = false;

    while (_it->hasNext()) {
      curVal = _it->next();

      if (_checkFunctor(curVal)) {
        if (!_flag.contains(curVal)) {
          _hasNext = true;
          _flag.insert(curVal);
          return;
        }
      } else {
        _hasNext = true;
        return;
      }
    }
  }

private:
  std::set<T> _flag;
  bool _hasNext;
  Iterator<T> *_it;
  T curVal;
  CheckFunc _checkFunctor;
};

template <typename T, typename CheckFunc = CheckAllFunctor<T>>
class MPUniqueIterator : public UniqueIterator<T, CheckFunc>,
                         public MemoryPool<MPUniqueIterator<T, CheckFunc>> {
public:
  MPUniqueIterator(Iterator<T> *it, CheckFunc checkFunctor = CheckFunc())
      : UniqueIterator<T, CheckFunc>(it, checkFunctor) {}
};

/**
 * @brief Convenient function for creating a UniqueIterator.
 * @ingroup Iterators
 *
 * Creates a UniqueIterator from another Iterator.
 * The returned Iterator takes ownership of the one provided as parameter.
 *
 * @param it a Talipot iterator
 * @param checkFunctor a functor or a lambda function that enables to indicate whether or not the
 *element could be duplicated (default test all elements)
 *
 * @return a UniqueIterator
 *
 **/
template <typename T, typename CheckFunc = CheckAllFunctor<T>>
inline Iterator<T> *uniqueIterator(Iterator<T> *it, CheckFunc checkFunctor = CheckFunc()) {
  return new MPUniqueIterator<T, CheckFunc>(it, checkFunctor);
}

/**
 * @brief Convenient function for creating a UniqueIterator from a STL container.
 * @ingroup Iterators
 *
 * Creates a UniqueIterator from a STL container (std::list, std::vector, std::set, std::map, ...).
 *
 * @param stlContainer any STL container
 * @param checkFunctor a functor or a lambda function that enables to indicate whether or not the
 *element could be duplicated (default test all elements)
 *
 * @return a UniqueIterator
 *
 **/
template <typename Container, typename CheckFunc = CheckAllFunctor<typename Container::value_type>>
inline Iterator<typename Container::value_type> *
uniqueIterator(const Container &stlContainer, CheckFunc checkFunctor = CheckFunc()) {
  return new MPUniqueIterator<typename Container::value_type, CheckFunc>(stlIterator(stlContainer),
                                                                         checkFunctor);
}
}
#endif // TALIPOT_UNIQUE_ITERATOR_H
