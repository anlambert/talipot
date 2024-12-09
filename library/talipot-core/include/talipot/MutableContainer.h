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

#ifndef TALIPOT_MUTABLE_CONTAINER_H
#define TALIPOT_MUTABLE_CONTAINER_H

#include <deque>
#include <iostream>
#include <string>
#include <cassert>
#include <climits>
#include <cstring>

#include <talipot/hash.h>

#include <talipot/config.h>
#include <talipot/PropertyTypes.h>
#include <talipot/DataSet.h>
#include <talipot/Iterator.h>

namespace tlp {

//===================================================================
// we first define an interface
// to make easier the iteration on values
// stored in a MutableContainer for the GraphUpdatesRecorder
template <typename INDEX_TYPE>
class IteratorValue : public Iterator<INDEX_TYPE> {
public:
  virtual INDEX_TYPE nextValue(DataMem &) = 0;
};
//===================================================================
template <typename TYPE, typename INDEX_TYPE = uint>
class MutableContainer {
  friend class MutableContainerTest;
  friend class GraphUpdatesRecorder;

public:
  MutableContainer();
  ~MutableContainer();

  /**
   * Set the default value
   */
  void setDefault(typename StoredType<TYPE>::ConstReference value);
  /**
   * set the same value to all elements and modify the default value
   */
  void setAll(typename StoredType<TYPE>::ConstReference value);
  /**
   * set the value associated to i
   */
  void set(const INDEX_TYPE i, typename StoredType<TYPE>::ConstReference value,
           bool forceDefaultValueRemoval = false);
  /**
   * add val to the value associated to i
   */
  void add(const INDEX_TYPE i, TYPE val);
  /**
   * get the value associated to i
   */
  typename StoredType<TYPE>::ConstReference get(const INDEX_TYPE i) const;
  /**
   * get the value associated to i and indicates if it is not the default one
   */
  typename StoredType<TYPE>::Reference get(const INDEX_TYPE i, bool &isNotDefault) const;
  /**
   * get the default value
   */
  typename StoredType<TYPE>::Reference getDefault() const;
  /**
   * return if the value associated to i is not the default one
   */
  bool hasNonDefaultValue(const INDEX_TYPE i) const;
  /**
   * return a pointer on an iterator for all the elements whose associated value
   * is equal to a given value or different from the default value.
   * A null pointer is returned in case of an iteration on all the elements
   * whose value is equal to the default value.
   */
  Iterator<INDEX_TYPE> *findAll(typename StoredType<TYPE>::ConstReference value,
                                bool equal = true) const;
  /**
   * return the number of non default values
   */
  uint numberOfNonDefaultValues() const;

  /**
   * return whether a non default value exists
   */
  bool hasNonDefaultValues() const {
    return numberOfNonDefaultValues() != 0;
  }

  /**
   * invert the boolean value set to i (do nothing for non boolean value)
   */
  void invertBooleanValue(const INDEX_TYPE i);

private:
  MutableContainer(const MutableContainer<TYPE> &) {}
  void operator=(const MutableContainer<TYPE> &) {}
  typename StoredType<TYPE>::ConstReference operator[](const INDEX_TYPE i) const;
  void vecttohash();
  void hashtovect();
  void compress(INDEX_TYPE min, INDEX_TYPE max, uint nbElements);
  void vectset(const INDEX_TYPE i, typename StoredType<TYPE>::Value value);
  IteratorValue<INDEX_TYPE> *findAllValues(typename StoredType<TYPE>::ConstReference value,
                                           bool equal = true) const;

private:
  std::deque<typename StoredType<TYPE>::Value> *vData;
  flat_hash_map<INDEX_TYPE, typename StoredType<TYPE>::Value> *hData;
  INDEX_TYPE minIndex, maxIndex;
  typename StoredType<TYPE>::Value defaultValue;
  enum State { VECT = 0, HASH = 1 };
  State state;
  uint elementInserted;
  double ratio;
  bool compressing;
};

DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<edge>), TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<node>), TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<uint>), TLP_TEMPLATE_DECLARE_SCOPE)

DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(BooleanType)>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(BooleanVectorType)>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(ColorType)>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(ColorVectorType)>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(DoubleType)>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(DoubleVectorType)>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(IntegerType)>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(IntegerVectorType)>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(LineType)>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(PointType)>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(SizeType)>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(SizeVectorType)>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(StringType)>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(StringVectorType)>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(BooleanType), node>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(BooleanVectorType), node>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(ColorType), node>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(ColorVectorType), node>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(DoubleType), node>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(DoubleVectorType), node>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(IntegerType), node>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(IntegerVectorType), node>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(LineType), node>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(PointType), node>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(SizeType), node>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(SizeVectorType), node>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(StringType), node>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(StringVectorType), node>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(BooleanType), edge>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(BooleanVectorType), edge>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(ColorType), edge>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(ColorVectorType), edge>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(DoubleType), edge>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(DoubleVectorType), edge>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(IntegerType), edge>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(IntegerVectorType), edge>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(LineType), edge>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(PointType), edge>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(SizeType), edge>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(SizeVectorType), edge>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(StringType), edge>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MutableContainer<REAL_TYPE(StringVectorType), edge>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

}

#ifdef DLL_TALIPOT
#include "cxx/MutableContainer.cxx"
#endif

#endif // TALIPOT_MUTABLE_CONTAINER_H
