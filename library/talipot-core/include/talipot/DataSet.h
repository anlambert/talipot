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

#ifndef TALIPOT_DATA_SET_H
#define TALIPOT_DATA_SET_H

#include <talipot/config.h>
#include <talipot/StlIterator.h>

#include <map>
#include <string>
#include <sstream>
#include <typeinfo>
#include <talipot/hash.h>

namespace tlp {

class Graph;

// basic interface to embed a data of any type
struct TLP_SCOPE DataMem {
  DataMem() = default;
  virtual ~DataMem() = default;
};

// basic template to embed a value of a known type
template <typename TYPE>
struct TypedValueContainer : public DataMem {
  TYPE value;
  TypedValueContainer() = default;
  TypedValueContainer(const TYPE &val) : value(val) {}
  ~TypedValueContainer() override = default;
};

/**
 * @ingroup Structures
 * @brief Describes a value of any type
 */
struct TLP_SCOPE DataType : public DataMem {
  DataType() = default;
  DataType(void *value) : value(value) {}

  /**
   * @return A deep copy of the stored value.
   */
  virtual DataType *clone() const = 0;

  /**
   * @return The C++ typename of the stored element
   */
  virtual std::string getTypeName() const = 0;

  /**
   * @brief indicates if it is a Talipot property
   */
  static bool isTalipotProperty(const std::string &typeName);

  /**
   * @brief indicates if it is a Talipot property
   */
  bool isTalipotProperty() const {
    return isTalipotProperty(getTypeName());
  }

  /**
   * @brief The actual pointer to the element's data
   */
  void *value;
};

// template class to embed value of known type
template <typename T>
struct TypedData : public DataType {
  TypedData(void *value) : DataType(value) {}
  ~TypedData() override {
    delete static_cast<T *>(value);
  }
  DataType *clone() const override {
    return new TypedData<T>(new T(getValue()));
  }

  std::string getTypeName() const override {
    return std::string(typeid(T).name());
  }

  const T &getValue() const {
    return *(static_cast<const T *>(this->value));
  }
};

// forward declaration of DataSet
class DataSet;

// Basic class for serialization of DataType embedded value
struct DataTypeSerializer {
  // the readable type name the serializer is designed for
  std::string outputTypeName;
  DataTypeSerializer(const std::string &otn) : outputTypeName(otn) {}
  virtual ~DataTypeSerializer() = default;
  // return a copy of this
  virtual DataTypeSerializer *clone() const = 0;
  // write the DataType embedded value into the output stream
  virtual void writeData(std::ostream &os, const DataType *data) = 0;
  // return the DataType embedded value as a std::string
  virtual std::string toString(const DataType *data) {
    std::stringstream ss;
    writeData(ss, data);
    return ss.str();
  }
  // build a DataType embedding value read from input stream
  virtual DataType *readData(std::istream &is) = 0;
  // set a value into a DataSet
  virtual bool setData(DataSet &ds, const std::string &prop, const std::string &value) = 0;
};

// a template class designs to simplify the developer's work
// when writing a serializer class
template <typename T>
struct TypedDataSerializer : public DataTypeSerializer {
  TypedDataSerializer(const std::string &otn) : DataTypeSerializer(otn) {}
  // declare new serialization virtual functions
  virtual void write(std::ostream &os, const T &value) = 0;
  // return true if the read of value succeeded, false if not
  virtual bool read(std::istream &is, T &value) = 0;
  // define virtually inherited functions using the previous ones
  void writeData(std::ostream &os, const DataType *data) override {
    write(os, *(static_cast<T *>(data->value)));
  }
  DataType *readData(std::istream &is) override {
    T value;
    bool ok = read(is, value);

    if (ok) {
      return new TypedData<T>(new T(value));
    }

    return nullptr;
  }
  // set a value into a DataSet
  bool setData(DataSet &ds, const std::string &prop, const std::string &value) override = 0;
};

// This class is there to ensure the destruction of DataTypeSerializer objects
// when program ends
class DataTypeSerializerContainer {

public:
  ~DataTypeSerializerContainer() {
    for (const auto &[type, serializer] : tnTodts) {
      delete serializer;
    }
  }

  flat_hash_map<std::string, DataTypeSerializer *> tnTodts;
  flat_hash_map<std::string, DataTypeSerializer *> otnTodts;
};

/**
 * @ingroup Structures
 * @brief A container that can store data from any type.
 *
 * The DataSet aggregate data of various types into a single structure and map each value to a key
 *(std::string) describing its name.
 * DataSet is mainly used in plugins. When creating a plugin, one can add input parameters (using
 *tlp::WithParameter methods) and retrieve them from the dataSet member variable once they have been
 *set by the user.
 **/
class TLP_SCOPE DataSet {
  // Internal map of key to data
  std::map<std::string, DataType *> data;
  /* management of data serialization
      two hashmap to retrieve data serializer from their
      type names and output type names
      tnTodsts => typename to data type serializer
      otnTodts => output type name to data type serializer */
  static DataTypeSerializerContainer serializerContainer;
  static void registerDataTypeSerializer(const std::string &typeName, DataTypeSerializer *dts);

public:
  DataSet() = default;
  DataSet(const DataSet &set);
  ~DataSet();

  /**
   * @brief Performs a deep copy of a DataSet into another
   */
  DataSet &operator=(const DataSet &set);

  /**
   * @brief Returns the stored value associated with the given key.
   * The stored value is a copy of the original value that was set.
   * If there is no value associated with the given key, the input value is left untouched.
   *
   * @param key The key with which the data we want to retrieve is associated.
   * @param value A variable which will be overwritten with the value to retrieve.
   * @return bool Whether there is a value associated with given key or not.
   **/
  template <typename T>
  bool get(const std::string &key, T &value) const;

  /**
   * @brief Returns the stored value, and deletes the stored copy.
   * If no value is found, nothing is deleted.
   *
   * @param key The key with which the data we want to retrieve is associated.
   * @param value A variable which will be overwritten with the value to retrieve.
   * @return bool Whether there is a value associated with given key or not.
   **/
  template <typename T>
  bool getAndFree(const std::string &key, T &value);

  /**
   * @brief Stores a copy of the given param, associated with the key.
   * The value must have a well-formed copy constructor.
   *
   * @param key The key which can be used to retrieve the data.
   * @param value The data to store.
   * @return void
   **/
  template <typename T>
  void set(const std::string &key, const T &value);

  /**
   * @brief Returns the mangled name of a type.
   *.
   * @return std::string the mangled name (typeid(T).name())
   **/
  template <typename T>
  std::string getTypeName() const {
    return std::string(typeid(T).name());
  }

  /**
   * @brief Registers a serializer for a known type
   *
   * Serializers are used to write/read from std::ostream objects when saving DataSet instances.
   */
  template <typename T>
  static void registerDataTypeSerializer(const DataTypeSerializer &serializer) {
    registerDataTypeSerializer(std::string(typeid(T).name()), serializer.clone());
  }

  /**
   * @brief write an embedded value
   */
  void writeData(std::ostream &os, const std::string &prop, const DataType *dt) const;

  /**
   * @brief Serializes a DataSet into a stream
   */
  static void write(std::ostream &os, const DataSet &ds);

  /**
   * @brief Read a value and stores it into the specified type
   */
  bool readData(std::istream &is, const std::string &prop, const std::string &outputTypeName);

  /**
   * @brief Reads a stream and stores its contents into a DataSet
   */
  static bool read(std::istream &is, DataSet &ds);

  /**
   * @param str the name of the member to look for
   * @return true if str exists into the DataSet.
   */
  bool exists(const std::string &str) const;

  /**
   * @param str the name of the member to look for
   * @return the mangled name of the type if str exists into the DataSet. An empty string if not.
   */
  std::string getTypeName(const std::string &str) const;

  /**
   * @brief Removes an element from the DataSet
   * @param str the name of the element to remove
   */
  void remove(const std::string &str);

  /**
   * @brief Removes and destroys all values stored in the dataset.
   */
  void clear();

  /**
   * @param str The name of the element to retrieve
   * @return A untyped value for a given element name. nullptr if the element does not exist
   */
  DataType *getData(const std::string &str) const;

  /**
   * @brief Set from an untyped value
   */
  void setData(const std::string &str, const DataType *value);

  /**
   * @return An iterator over stored values as a std::pair name => untyped value
   */
  tlp::Iterator<std::pair<std::string, DataType *>> *getValues() const;

  /**
   * @brief Returns the number of registered values
   */
  uint size() const;

  /**
   * @brief Indicates whether the set is empty of not
   */
  bool empty() const;

  /**
   * @return the data type serializer associated to the given typename. nullptr if no serializer is
   * found
   */
  static DataTypeSerializer *typenameToSerializer(const std::string &name);

  /**
   * @return A std::string representation of a DataSet
   */
  std::string toString() const;
};
}

#include "cxx/DataSet.cxx"

#endif // TALIPOT_DATA_SET_H
