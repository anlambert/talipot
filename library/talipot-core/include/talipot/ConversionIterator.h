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

#ifndef TALIPOT_CONVERSION_ITERATOR_H
#define TALIPOT_CONVERSION_ITERATOR_H

#include <talipot/Iterator.h>
#include <talipot/StlIterator.h>
#include <talipot/MemoryPool.h>

namespace tlp {
/**
 * @class ConversionIterator
 * @ingroup Iterators
 * @brief Iterator that enables to convert an Iterator of type TYPEIN to an Iterator of type TYPEOUT
 * @param it the iterator that should be converted
 * @param convFunctor the functor or lamnda function that enables to convert TYPEIN to TYPEOUT
 *
 * The functor function should have the following form:
 * @code
 * class AConversionFunc {
 *  TYPEOUT operator()(TYPEIN a) {
 *    return a converted in TYPEOUT;
 *  }
 * };
 * @endcode
 **/
template <typename TYPEIN, typename TYPEOUT, typename ConversionFunc>
class ConversionIterator : public Iterator<TYPEOUT> {
  public:
    ConversionIterator(Iterator<TYPEIN> *it, ConversionFunc convFunctor)
        : _it(it), _convFunctor(convFunctor) {}
    ~ConversionIterator() override {
        delete _it;
    }
    bool hasNext() override {
        return _it->hasNext();
    }
    TYPEOUT next() override {
        return _convFunctor(_it->next());
    }

  private:
    tlp::Iterator<TYPEIN> *_it;
    ConversionFunc _convFunctor;
};
/**
 * @class MPConversionIterator
 * @ingroup Iterators
 * @brief MPConversionIterator implements memory pool for ConversionIterator
 * @warning never inherit from that class
 * @see ConversionIterator
 **/
template <typename TIN, typename TOUT, typename ConversionFunc>
class MPConversionIterator : public ConversionIterator<TIN, TOUT, ConversionFunc>,
                             public MemoryPool<MPConversionIterator<TIN, TOUT, ConversionFunc>> {
  public:
    MPConversionIterator(Iterator<TIN> *it, ConversionFunc convFunctor)
        : ConversionIterator<TIN, TOUT, ConversionFunc>(it, convFunctor) {}
};

/**
 * @brief Convenient function for creating a ConversionIterator.
 * @ingroup Iterators
 *
 * Creates a ConversionIterator from another Iterator and a conversion function.
 * The returned Iterator takes ownership of the one provided as parameter.
 *
 * @param it a Talipot iterator
 * @param convFunc functor or lambda function converting a value of type TYPEIN to type TYPEOUT
 *
 * @return a ConversionIterator
 *
 **/
template <typename TOUT, typename ConversionFunc, typename TIN>
inline Iterator<TOUT> *conversionIterator(tlp::Iterator<TIN> *it, ConversionFunc convFunc) {
    return new MPConversionIterator<TIN, TOUT, ConversionFunc>(it, convFunc);
}

/**
 * @brief Convenient function for creating a ConversionIterator from a STL container.
 * @ingroup Iterators
 *
 * Creates a ConversionIterator from a STL container (std::list, std::vector, std::set, std::map,
 *...)
 * and a conversion function.
 *
 * @param stlContainer any STL container
 * @param convFunc functor or lambda function converting a value from the container to type TYPEOUT
 *
 * @return a ConversionIterator
 *
 **/
template <typename TOUT, typename ConversionFunc, typename Container>
inline Iterator<TOUT> *conversionIterator(const Container &stlContainer, ConversionFunc convFunc) {
    return new MPConversionIterator<typename Container::value_type, TOUT, ConversionFunc>(
        stlIterator(stlContainer), convFunc);
}
}
#endif // TALIPOT_CONVERSION_ITERATOR_H
