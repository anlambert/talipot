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

#ifndef TALIPOT_CONCAT_ITERATOR_H
#define TALIPOT_CONCAT_ITERATOR_H

#include <talipot/Iterator.h>

namespace tlp {

/**
 * @brief This Iterator iterates over the sequence formed by the concatenation of the sequences it
 *is given.
 * @warning This class takes ownership of the Iterators it is given.
 **/
template <class T>
struct ConcatIterator : public Iterator<T> {

    /**
     * @brief Creates an Iterator that iterates over the concatenation of the two sequences it is
     *given.
     *
     * @param itOne The first sequence to iterate upon.
     * @param itTwo The second sequence, which will be iterated upon after the first sequence has
     *been completely iterated upon.
     **/
    ConcatIterator(Iterator<T> *itOne, Iterator<T> *itTwo) : itOne(itOne), itTwo(itTwo) {}

    /**
     * @brief Deletes the two iterators it was given at construction.
     **/
    ~ConcatIterator() override {
        delete itOne;
        delete itTwo;
    }

    T next() override {
        if (itOne->hasNext()) {
            return itOne->next();
        } else {
            return itTwo->next();
        }
    }

    bool hasNext() override {
        return (itOne->hasNext() || itTwo->hasNext());
    }

  private:
    Iterator<T> *itOne;
    Iterator<T> *itTwo;
};

/**
 * @brief Convenient function for creating a ConcatIterator.
 * @ingroup Iterators
 *
 * Creates a ConcatIterator from two other Iterators.
 * The returned Iterator takes ownership of the one provided as parameter.
 *
 * @param itOne the first input Iterator
 * @param itTwo the second input Iterator
 * @return a ConcatIterator
 **/
template <class T>
Iterator<T> *concatIterator(Iterator<T> *itOne, Iterator<T> *itTwo) {
    return new ConcatIterator<T>(itOne, itTwo);
}
}
#endif // TALIPOT_CONCAT_ITERATOR_H
