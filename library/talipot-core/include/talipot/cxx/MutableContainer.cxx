/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
#include <parallel_hashmap/phmap.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

//===================================================================
// we implement 2 templates with IteratorValue as parent class
// for the two kinds of storage used in a MutableContainer
// one for vector storage
template <typename TYPE, typename INDEX_TYPE>
class IteratorVect : public tlp::IteratorValue<INDEX_TYPE> {
public:
  IteratorVect(const TYPE &value, bool equal,
               std::deque<typename tlp::StoredType<TYPE>::Value> *vData, uint minIndex)
      : _value(value), _equal(equal), _pos(minIndex), vData(vData), it(vData->begin()) {
    while (it != (*vData).end() && tlp::StoredType<TYPE>::equal((*it), _value) != _equal) {
      ++it;
      ++_pos;
    }
  }
  bool hasNext() override {
    return (_pos < UINT_MAX && it != (*vData).end());
  }
  INDEX_TYPE next() override {
    INDEX_TYPE tmp = _pos;

    do {
      ++it;
      ++_pos;
    } while (it != (*vData).end() && tlp::StoredType<TYPE>::equal((*it), _value) != _equal);

    return tmp;
  }
  INDEX_TYPE nextValue(tlp::DataMem &val) override {
    static_cast<tlp::TypedValueContainer<TYPE> &>(val).value = tlp::StoredType<TYPE>::get(*it);
    INDEX_TYPE pos = _pos;

    do {
      ++it;
      ++_pos;
    } while (it != (*vData).end() && tlp::StoredType<TYPE>::equal((*it), _value) != _equal);

    return pos;
  }

private:
  const TYPE _value;
  bool _equal;
  INDEX_TYPE _pos;
  std::deque<typename tlp::StoredType<TYPE>::Value> *vData;
  typename std::deque<typename tlp::StoredType<TYPE>::Value>::const_iterator it;
};

// one for hash storage
template <typename TYPE, typename INDEX_TYPE>
class IteratorHash : public tlp::IteratorValue<INDEX_TYPE> {
public:
  IteratorHash(const TYPE &value, bool equal,
               phmap::flat_hash_map<INDEX_TYPE, typename tlp::StoredType<TYPE>::Value> *hData)
      : _value(value), _equal(equal), hData(hData) {
    it = (*hData).begin();

    while (it != (*hData).end() && tlp::StoredType<TYPE>::equal((*it).second, _value) != _equal) {
      ++it;
    }
  }
  bool hasNext() override {
    return (it != (*hData).end());
  }
  INDEX_TYPE next() override {
    INDEX_TYPE tmp = (*it).first;

    do {
      ++it;
    } while (it != (*hData).end() && tlp::StoredType<TYPE>::equal((*it).second, _value) != _equal);

    return tmp;
  }
  INDEX_TYPE nextValue(tlp::DataMem &val) override {
    static_cast<tlp::TypedValueContainer<TYPE> &>(val).value =
        tlp::StoredType<TYPE>::get((*it).second);
    INDEX_TYPE pos = (*it).first;

    do {
      ++it;
    } while (it != (*hData).end() && tlp::StoredType<TYPE>::equal((*it).second, _value) != _equal);

    return pos;
  }

private:
  const TYPE _value;
  bool _equal;
  phmap::flat_hash_map<INDEX_TYPE, typename tlp::StoredType<TYPE>::Value> *hData;
  typename phmap::flat_hash_map<INDEX_TYPE, typename tlp::StoredType<TYPE>::Value>::const_iterator
      it;
};

//===================================================================
template <typename TYPE, typename INDEX_TYPE>
tlp::MutableContainer<TYPE, INDEX_TYPE>::MutableContainer()
    : vData(new std::deque<typename StoredType<TYPE>::Value>()), hData(nullptr), minIndex(UINT_MAX),
      maxIndex(UINT_MAX), defaultValue(StoredType<TYPE>::defaultValue()), state(VECT),
      elementInserted(0),
      ratio(double(sizeof(typename tlp::StoredType<TYPE>::Value)) /
            (3.0 * double(sizeof(void *)) + double(sizeof(typename tlp::StoredType<TYPE>::Value)))),
      compressing(false) {}
//===================================================================
template <typename TYPE, typename INDEX_TYPE>
tlp::MutableContainer<TYPE, INDEX_TYPE>::~MutableContainer<TYPE, INDEX_TYPE>() {
  switch (state) {
  case VECT:

    if (StoredType<TYPE>::isPointer) {
      // delete stored values
      auto it = vData->begin();

      while (it != vData->end()) {
        if ((*it) != defaultValue) {
          StoredType<TYPE>::destroy(*it);
        }

        ++it;
      }
    }

    delete vData;
    vData = nullptr;
    break;

  case HASH:

    if (StoredType<TYPE>::isPointer) {
      // delete stored values
      auto it = hData->begin();

      while (it != hData->end()) {
        StoredType<TYPE>::destroy(it->second);
        ++it;
      }
    }

    delete hData;
    hData = nullptr;
    break;

  default:
    assert(false);
    tlp::error() << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
    break;
  }

  StoredType<TYPE>::destroy(defaultValue);
}
//===================================================================
template <typename TYPE, typename INDEX_TYPE>
void tlp::MutableContainer<TYPE, INDEX_TYPE>::setDefault(
    typename StoredType<TYPE>::ConstReference value) {
  StoredType<TYPE>::destroy(defaultValue);
  defaultValue = StoredType<TYPE>::clone(value);
}
//===================================================================
template <typename TYPE, typename INDEX_TYPE>
void tlp::MutableContainer<TYPE, INDEX_TYPE>::setAll(
    typename StoredType<TYPE>::ConstReference value) {
  switch (state) {
  case VECT:

    if (StoredType<TYPE>::isPointer) {
      // delete stored values
      auto it = vData->begin();

      while (it != vData->end()) {
        if ((*it) != defaultValue) {
          StoredType<TYPE>::destroy(*it);
        }

        ++it;
      }
    }

    vData->clear();
    break;

  case HASH:

    if (StoredType<TYPE>::isPointer) {
      // delete stored values
      for (const auto &[id, type] : *hData) {
        StoredType<TYPE>::destroy(type);
      }
    }

    delete hData;
    hData = nullptr;
    vData = new std::deque<typename StoredType<TYPE>::Value>();
    break;

  default:
    assert(false);
    tlp::error() << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
    break;
  }

  StoredType<TYPE>::destroy(defaultValue);
  defaultValue = StoredType<TYPE>::clone(value);
  state = VECT;
  maxIndex = UINT_MAX;
  minIndex = UINT_MAX;
  elementInserted = 0;
}
//===================================================================
// this method is private and used as is by GraphUpdatesRecorder class
// it is also used to implement findAll
template <typename TYPE, typename INDEX_TYPE>
tlp::IteratorValue<INDEX_TYPE> *tlp::MutableContainer<TYPE, INDEX_TYPE>::findAllValues(
    typename StoredType<TYPE>::ConstReference value, bool equal) const {
  if (equal && StoredType<TYPE>::equal(defaultValue, value))
    // error
    return nullptr;
  else {
    switch (state) {
    case VECT:
      return new IteratorVect<TYPE, INDEX_TYPE>(value, equal, vData, minIndex);
      break;

    case HASH:
      return new IteratorHash<TYPE, INDEX_TYPE>(value, equal, hData);
      break;

    default:
      assert(false);
      tlp::error() << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
      return nullptr;
    }
  }
}
//===================================================================
// this method is visible for any class
template <typename TYPE, typename INDEX_TYPE>
tlp::Iterator<INDEX_TYPE> *
tlp::MutableContainer<TYPE, INDEX_TYPE>::findAll(typename StoredType<TYPE>::ConstReference value,
                                                 bool equal) const {
  return findAllValues(value, equal);
}
//===================================================================
template <typename TYPE, typename INDEX_TYPE>
void tlp::MutableContainer<TYPE, INDEX_TYPE>::vectset(const INDEX_TYPE i,
                                                      typename StoredType<TYPE>::Value value) {
  if (minIndex == UINT_MAX) {
    minIndex = i;
    maxIndex = i;
    (*vData).push_back(value);
    ++elementInserted;
  } else {
    // the time performance of these two attempts of nicer coding
    // in this commented code seems worse than the loops below (about 15%)
    // if ( i > maxIndex ) {
    //-- 1st attempt --
    //   vData->resize(i+1 - minIndex, defaultValue);
    //-- 2nd attempt
    //   vData->insert(vData->end(), i - maxIndex, defaultValue);
    //   maxIndex = i;
    // } else if (i < minIndex) {
    //   vData->insert(vData->begin(), minIndex - i, defaultValue);
    //   minIndex = i;
    // }
    while (i > maxIndex) {
      (*vData).push_back(defaultValue);
      ++maxIndex;
    }

    while (i < minIndex) {
      (*vData).push_front(defaultValue);
      --minIndex;
    }

    typename StoredType<TYPE>::Value val = (*vData)[i - minIndex];
    (*vData)[i - minIndex] = value;

    if (val != defaultValue) {
      StoredType<TYPE>::destroy(val);
    } else {
      ++elementInserted;
    }
  }
}
//===================================================================
template <typename TYPE, typename INDEX_TYPE>
void tlp::MutableContainer<TYPE, INDEX_TYPE>::set(const INDEX_TYPE i,
                                                  typename StoredType<TYPE>::ConstReference value,
                                                  bool forceDefaultValueRemoval) {
  // Test if after insertion we need to resize
  if (!compressing && !StoredType<TYPE>::equal(defaultValue, value)) {
    compressing = true;
    compress(std::min(i, minIndex), std::max(i, maxIndex), elementInserted);
    compressing = false;
  }

  if (StoredType<TYPE>::equal(defaultValue, value)) {

    switch (state) {
    case VECT:

      if (i <= maxIndex && i >= minIndex) {
        typename StoredType<TYPE>::Value val = (*vData)[i - minIndex];

        if (val != defaultValue) {
          (*vData)[i - minIndex] = defaultValue;
          StoredType<TYPE>::destroy(val);
          --elementInserted;
        } else if (forceDefaultValueRemoval)
          --elementInserted;
      }

      return;

    case HASH: {
      auto it = hData->find(i);

      if (it != hData->end()) {
        StoredType<TYPE>::destroy(it->second);
        hData->erase(it);
        --elementInserted;
      }

      break;
    }

    default:
      assert(false);
      tlp::error() << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
      break;
    }
  } else {
    typename StoredType<TYPE>::Value newVal = StoredType<TYPE>::clone(value);

    switch (state) {
    case VECT:

      vectset(i, newVal);

      return;

    case HASH: {
      auto it = hData->find(i);

      if (it != hData->end()) {
        StoredType<TYPE>::destroy(it->second);
        it->second = newVal;
      } else {
        ++elementInserted;
        (*hData)[i] = newVal;
      }
      break;
    }

    default:
      assert(false);
      tlp::error() << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
      break;
    }

    maxIndex = std::max(maxIndex, i);
    minIndex = std::min(minIndex, i);
  }
}
//===================================================================
template <typename TYPE, typename INDEX_TYPE>
void tlp::MutableContainer<TYPE, INDEX_TYPE>::add(const INDEX_TYPE i, TYPE val) {
  if constexpr (!static_cast<bool>(tlp::StoredType<TYPE>::isPointer)) {
    if (maxIndex == UINT_MAX) {
      assert(state == VECT);
      minIndex = i;
      maxIndex = i;
      (*vData).push_back(defaultValue + val);
      ++elementInserted;
      return;
    }

    switch (state) {
    case VECT: {
      if (i > maxIndex || i < minIndex) {
        set(i, defaultValue + val);
        return;
      }

      TYPE &oldVal = (*vData)[i - minIndex];

      if (oldVal == defaultValue) {
        set(i, defaultValue + val);
        return;
      }

      oldVal += val;

      return;
    }

    case HASH: {
      auto it = hData->find(i);

      if (it != hData->end()) {
        // check default value
        if ((it->second + val) == defaultValue) {
          StoredType<TYPE>::destroy(it->second);
          hData->erase(it);
          --elementInserted;
        } else {
          it->second += val;
        }
      } else {
        set(i, defaultValue + val);
      }

      return;
    }

    default:
      assert(false);
      std::cerr << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
    }
  } else {
    std::ignore = i;
    std::ignore = val;
  }

  assert(false);
  std::cerr << __PRETTY_FUNCTION__ << "not implemented" << std::endl;
}
//===================================================================
template <typename TYPE, typename INDEX_TYPE>
typename tlp::StoredType<TYPE>::ConstReference
tlp::MutableContainer<TYPE, INDEX_TYPE>::get(const INDEX_TYPE i) const {
  if (maxIndex == UINT_MAX) {
    return StoredType<TYPE>::get(defaultValue);
  }

  switch (state) {
  case VECT:

    if (i > maxIndex || i < minIndex) {
      return StoredType<TYPE>::get(defaultValue);
    } else {
      return StoredType<TYPE>::get((*vData)[i - minIndex]);
    }

  case HASH: {
    auto it = hData->find(i);

    if (it != hData->end()) {
      return StoredType<TYPE>::get(it->second);
    } else {
      return StoredType<TYPE>::get(defaultValue);
    }
  }

  default:
    assert(false);
    tlp::error() << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
    return StoredType<TYPE>::get(defaultValue);
  }
}
//===================================================================
template <typename TYPE, typename INDEX_TYPE>
void tlp::MutableContainer<TYPE, INDEX_TYPE>::invertBooleanValue(const INDEX_TYPE i) {
  if constexpr (std::is_same<typename StoredType<TYPE>::Value, bool>::value) {
    switch (state) {
    case VECT: {
      if (i > maxIndex || i < minIndex) {
        vectset(i, !defaultValue);
      } else {
        typename StoredType<TYPE>::Value val = (*vData)[i - minIndex];

        if (val != defaultValue) {
          --elementInserted;
        } else {
          ++elementInserted;
        }
        (*vData)[i - minIndex] = !val;
      }
      return;
    }

    case HASH: {
      auto it = hData->find(i);

      if (it != hData->end()) {
        hData->erase(it);
        --elementInserted;
      } else {
        (*hData)[i] = !defaultValue;
        ++elementInserted;
      }
      return;
    }

    default:
      assert(false);
      tlp::error() << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
      break;
    }
  } else {
    std::ignore = i;
  }

  assert(false);
  std::cerr << __PRETTY_FUNCTION__ << "not implemented" << std::endl;
}
//===================================================================
template <typename TYPE, typename INDEX_TYPE>
typename tlp::StoredType<TYPE>::Reference
tlp::MutableContainer<TYPE, INDEX_TYPE>::getDefault() const {
  return StoredType<TYPE>::get(defaultValue);
}
//===================================================================
template <typename TYPE, typename INDEX_TYPE>
bool tlp::MutableContainer<TYPE, INDEX_TYPE>::hasNonDefaultValue(const INDEX_TYPE i) const {
  if (maxIndex == UINT_MAX) {
    return false;
  }

  switch (state) {
  case VECT:
    return (i <= maxIndex && i >= minIndex && (((*vData)[i - minIndex]) != defaultValue));

  case HASH:
    return ((hData->find(i)) != hData->end());

  default:
    assert(false);
    tlp::error() << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
    return false;
  }
}
//===================================================================
template <typename TYPE, typename INDEX_TYPE>
typename tlp::StoredType<TYPE>::Reference
tlp::MutableContainer<TYPE, INDEX_TYPE>::get(const INDEX_TYPE i, bool &notDefault) const {
  if (maxIndex == UINT_MAX) {
    notDefault = false;
    return StoredType<TYPE>::get(defaultValue);
  }

  switch (state) {
  case VECT:

    if (i > maxIndex || i < minIndex) {
      notDefault = false;
      return StoredType<TYPE>::get(defaultValue);
    } else {
      typename StoredType<TYPE>::Reference val = StoredType<TYPE>::get((*vData)[i - minIndex]);
      notDefault = val != StoredType<TYPE>::get(defaultValue);
      return val;
    }

  case HASH: {
    auto it = hData->find(i);

    if (it != hData->end()) {
      notDefault = true;
      return StoredType<TYPE>::get(it->second);
    } else {
      notDefault = false;
      return StoredType<TYPE>::get(defaultValue);
    }
  }

  default:
    assert(false);
    notDefault = false;
    tlp::error() << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
    return StoredType<TYPE>::get(defaultValue);
  }
}
//===================================================================
template <typename TYPE, typename INDEX_TYPE>
uint tlp::MutableContainer<TYPE, INDEX_TYPE>::numberOfNonDefaultValues() const {
  return elementInserted;
}
//===================================================================
template <typename TYPE, typename INDEX_TYPE>
void tlp::MutableContainer<TYPE, INDEX_TYPE>::vecttohash() {
  hData = new phmap::flat_hash_map<INDEX_TYPE, typename StoredType<TYPE>::Value>(elementInserted);

  INDEX_TYPE newMaxIndex = 0;
  INDEX_TYPE newMinIndex = UINT_MAX;
  elementInserted = 0;

  for (INDEX_TYPE i = minIndex; i <= maxIndex; ++i) {
    if ((*vData)[i - minIndex] != defaultValue) {
      (*hData)[i] = (*vData)[i - minIndex];
      newMaxIndex = std::max(newMaxIndex, i);
      newMinIndex = std::min(newMinIndex, i);
      ++elementInserted;
    }
  }

  maxIndex = newMaxIndex;
  minIndex = newMinIndex;
  delete vData;
  vData = nullptr;
  state = HASH;
}
//===================================================================
template <typename TYPE, typename INDEX_TYPE>
void tlp::MutableContainer<TYPE, INDEX_TYPE>::hashtovect() {
  vData = new std::deque<typename StoredType<TYPE>::Value>();
  minIndex = UINT_MAX;
  maxIndex = UINT_MAX;
  elementInserted = 0;
  state = VECT;

  for (const auto &[id, type] : *hData) {
    if (type != defaultValue) {
      vectset(id, type);
    }
  }

  delete hData;
  hData = nullptr;
}
//===================================================================
template <typename TYPE, typename INDEX_TYPE>
void tlp::MutableContainer<TYPE, INDEX_TYPE>::compress(INDEX_TYPE min, INDEX_TYPE max,
                                                       uint nbElements) {
  if (max == UINT_MAX || (max - min) < 10) {
    return;
  }

  double limitValue = ratio * (double(max - min + 1.0));

  switch (state) {
  case VECT:

    if (double(nbElements) < limitValue) {
      vecttohash();
    }

    break;

  case HASH:

    if (double(nbElements) > limitValue * 1.5) {
      hashtovect();
    }

    break;

  default:
    assert(false);
    tlp::error() << __PRETTY_FUNCTION__ << "unexpected state value (serious bug)" << std::endl;
    break;
  }
}

template <typename TYPE, typename INDEX_TYPE>
typename tlp::StoredType<TYPE>::ConstReference
tlp::MutableContainer<TYPE, INDEX_TYPE>::operator[](const INDEX_TYPE i) const {
  return get(i);
}
