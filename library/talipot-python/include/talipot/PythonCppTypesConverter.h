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

#ifndef TALIPOT_PYTHON_CPP_TYPES_CONVERTER_H
#define TALIPOT_PYTHON_CPP_TYPES_CONVERTER_H

#include <talipot/Coord.h>
#include <talipot/Color.h>
#include <talipot/ColorScale.h>
#include <talipot/StringCollection.h>
#include <talipot/Size.h>
#include <talipot/Node.h>
#include <talipot/Edge.h>
#include <talipot/BooleanProperty.h>
#include <talipot/ColorProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/IntegerProperty.h>
#include <talipot/LayoutProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/StringProperty.h>
#include <talipot/NumericProperty.h>
#include <talipot/TlpTools.h>

TLP_PYTHON_SCOPE void *convertSipWrapperToCppType(PyObject *sipWrapper,
                                                  const std::string &cppTypename,
                                                  const bool transferTo = false);
TLP_PYTHON_SCOPE PyObject *convertCppTypeToSipWrapper(void *cppObj, const std::string &cppTypename,
                                                      bool fromNew = false);

TLP_PYTHON_SCOPE bool convertPyObjectToBool(PyObject *pyObject, bool &cppObject);
TLP_PYTHON_SCOPE PyObject *convertBoolToPyObject(bool cppObject);

TLP_PYTHON_SCOPE bool convertPyObjectToDouble(PyObject *pyObject, double &cppObject);
TLP_PYTHON_SCOPE PyObject *convertDoubleToPyObject(double cppObject);

TLP_PYTHON_SCOPE bool convertPyObjectToLong(PyObject *pyObject, long &cppObject);
TLP_PYTHON_SCOPE PyObject *convertLongToPyObject(long cppObject);

TLP_PYTHON_SCOPE bool convertPyObjectToUnsignedLong(PyObject *pyObject, ulong &cppObject);
TLP_PYTHON_SCOPE PyObject *convertUnsignedLongToPyObject(ulong cppObject);

class TLP_PYTHON_SCOPE ValueSetter {

public:
  ValueSetter(tlp::DataSet *dataSet, const std::string &key)
      : dataSet(dataSet), graph(nullptr), key(key) {}

  ValueSetter(tlp::Graph *graph, const std::string &key)
      : dataSet(nullptr), graph(graph), key(key) {}

  template <typename T>
  void setValue(const T &value) {
    if (dataSet) {
      dataSet->set(key, value);
    } else if (graph) {
      graph->setAttribute(key, value);
    }
  }

private:
  tlp::DataSet *dataSet;
  tlp::Graph *graph;
  std::string key;
};

TLP_PYTHON_SCOPE PyObject *getPyObjectFromDataType(const tlp::DataType *dataType,
                                                   bool noCopy = false);

template <typename T>
PyObject *getPyObjectFromCppPointer(const T *val) {
  tlp::TypedData<T *> dataType(new T *(const_cast<T *>(val)));
  return getPyObjectFromDataType(&dataType);
}

template <typename T>
PyObject *getPyObjectFromCppReference(const T &val) {
  tlp::TypedData<T> dataType(new T(val));
  return getPyObjectFromDataType(&dataType);
}

TLP_PYTHON_SCOPE bool setCppValueFromPyObject(PyObject *pyObj, ValueSetter &valSetter,
                                              tlp::DataType *dataType = nullptr);

template <typename T>
class PyObjectToCppObjectConverter {

public:
  bool convert(PyObject *pyObject, T &cppObject) {
    std::string className = tlp::demangleClassName<T>();

    void *pointer = convertSipWrapperToCppType(pyObject, className);

    if (pointer) {
      T *cppObjectPointer = static_cast<T *>(pointer);
      cppObject = *cppObjectPointer;
      delete cppObjectPointer;
      return true;
    }

    return false;
  }
};

template <typename T>
class PyObjectToCppObjectConverter<T *> {

public:
  bool convert(PyObject *pyObject, T *&cppObject) {
    std::string className = tlp::demangleClassName<T>();

    void *cppObjPointer = convertSipWrapperToCppType(pyObject, className, true);

    if (cppObjPointer) {
      cppObject = static_cast<T *>(cppObjPointer);
      return true;
    }

    return false;
  }
};

template <>
class PyObjectToCppObjectConverter<PyObject *> {
public:
  bool convert(PyObject *pyObject, PyObject *&cppObject) {
    cppObject = pyObject;
    return true;
  }
};

template <>
class PyObjectToCppObjectConverter<bool> {
public:
  bool convert(PyObject *pyObject, bool &cppObject) {
    return convertPyObjectToBool(pyObject, cppObject);
  }
};

template <>
class PyObjectToCppObjectConverter<double> {
public:
  bool convert(PyObject *pyObject, double &cppObject) {
    return convertPyObjectToDouble(pyObject, cppObject);
  }
};

template <>
class PyObjectToCppObjectConverter<float> {
public:
  bool convert(PyObject *pyObject, float &cppObject) {
    double val = 0;
    PyObjectToCppObjectConverter<double> converter;
    bool ok = converter.convert(pyObject, val);
    cppObject = val;
    return ok;
  }
};

template <>
class PyObjectToCppObjectConverter<long> {

public:
  bool convert(PyObject *pyObject, long &cppObject) {
    return convertPyObjectToLong(pyObject, cppObject);
  }
};

template <>
class PyObjectToCppObjectConverter<int> {
public:
  bool convert(PyObject *pyObject, int &cppObject) {
    long val = 0;
    PyObjectToCppObjectConverter<long> converter;
    bool ok = converter.convert(pyObject, val);
    cppObject = val;
    return ok;
  }
};

template <>
class PyObjectToCppObjectConverter<ulong> {
public:
  bool convert(PyObject *pyObject, ulong &cppObject) {
    return convertPyObjectToUnsignedLong(pyObject, cppObject);
  }
};

template <>
class PyObjectToCppObjectConverter<uint> {
public:
  bool convert(PyObject *pyObject, uint &cppObject) {
    ulong val = 0;
    PyObjectToCppObjectConverter<ulong> converter;
    bool ok = converter.convert(pyObject, val);
    cppObject = val;
    return ok;
  }
};

template <typename T>
class CppObjectToPyObjectConverter {

public:
  bool convert(const T &cppObject, PyObject *&pyObject) {
    std::string className = tlp::demangleClassName<T>();

    T *objCopy = new T(cppObject);
    PyObject *pyObj = convertCppTypeToSipWrapper(objCopy, className, true);

    if (pyObj) {
      pyObject = pyObj;
      return true;
    } else {
      delete objCopy;
    }

    return false;
  }
};

template <typename T>
class CppObjectToPyObjectConverter<T *> {

public:
  bool convert(T *cppObject, PyObject *&pyObject) {
    std::string className = tlp::demangleClassName<T>();

    PyObject *pyObj = convertCppTypeToSipWrapper(cppObject, className);

    if (pyObj) {
      pyObject = pyObj;
      return true;
    }

    return false;
  }
};

template <>
class CppObjectToPyObjectConverter<PyObject *> {
public:
  bool convert(const PyObject *&cppObject, PyObject *&pyObject) {
    pyObject = const_cast<PyObject *>(cppObject);
    return true;
  }
};

template <>
class CppObjectToPyObjectConverter<bool> {
public:
  bool convert(const bool &cppObject, PyObject *&pyObject) {
    pyObject = convertBoolToPyObject(cppObject);
    return true;
  }
};

template <>
class CppObjectToPyObjectConverter<long> {
public:
  bool convert(const long &cppObject, PyObject *&pyObject) {
    pyObject = convertLongToPyObject(cppObject);
    return true;
  }
};

template <>
class CppObjectToPyObjectConverter<int> {
public:
  bool convert(const int &cppObject, PyObject *&pyObject) {
    pyObject = convertLongToPyObject(cppObject);
    return true;
  }
};

template <>
class CppObjectToPyObjectConverter<uint> {
public:
  bool convert(const uint &cppObject, PyObject *&pyObject) {
    pyObject = convertUnsignedLongToPyObject(cppObject);
    return true;
  }
};

template <>
class CppObjectToPyObjectConverter<ulong> {
public:
  bool convert(const ulong &cppObject, PyObject *&pyObject) {
    pyObject = convertUnsignedLongToPyObject(cppObject);
    return true;
  }
};

template <>
class CppObjectToPyObjectConverter<double> {
public:
  bool convert(const double &cppObject, PyObject *&pyObject) {
    pyObject = convertDoubleToPyObject(cppObject);
    return true;
  }
};

template <>
class CppObjectToPyObjectConverter<float> {
public:
  bool convert(const float &cppObject, PyObject *&pyObject) {
    pyObject = convertDoubleToPyObject(cppObject);
    return true;
  }
};

#endif // TALIPOT_PYTHON_CPP_TYPES_CONVERTER_H
