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

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#include "talipot/PythonIncludes.h"
#include "talipot/PythonCppTypesConverter.h"

using namespace std;
using namespace tlp;

static const unordered_map<string, string> cppTypenameToSipTypename = {
    {demangleClassName<string>(), "std::string"},
    {demangleClassName<node>(), "tlp::node"},
    {demangleClassName<edge>(), "tlp::edge"},
    {demangleClassName<Vec3f>(), "tlp::Vec3f"},
    {demangleClassName<Coord>(), "tlp::Coord"},
    {demangleClassName<Size>(), "tlp::Size"},
    {demangleClassName<vector<int>>(), "std::vector<int>"},
    {demangleClassName<vector<uint>>(), "std::vector<uint>"},
    {demangleClassName<vector<long>>(), "std::vector<long>"},
    {demangleClassName<vector<unsigned long>>(), "std::vector<unsigned long>"},
    {demangleClassName<vector<float>>(), "std::vector<float>"},
    {demangleClassName<vector<double>>(), "std::vector<double>"},
    {demangleClassName<vector<bool>>(), "std::vector<bool>"},
    {demangleClassName<vector<string>>(), "std::vector<std::string>"},
    {demangleClassName<vector<node>>(), "std::vector<tlp::node>"},
    {demangleClassName<vector<edge>>(), "std::vector<tlp::edge>"},
    {demangleClassName<vector<Coord>>(), "std::vector<tlp::Coord>"},
    {demangleClassName<vector<Color>>(), "std::vector<tlp::Color>"},
    {demangleClassName<vector<Size>>(), "std::vector<tlp::Size>"},
    {demangleClassName<vector<Graph *>>(), "std::vector<tlp::Graph*>"},
    {demangleClassName<vector<ColorScale>>(), "std::vector<tlp::ColorScale>"},
    {demangleClassName<vector<StringCollection>>(), "std::vector<tlp::StringCollection>"},
    {demangleClassName<vector<BooleanProperty *>>(), "std::vector<tlp::BooleanProperty *>"},
    {demangleClassName<vector<ColorProperty *>>(), "std::vector<tlp::ColorProperty *>"},
    {demangleClassName<vector<DoubleProperty *>>(), "std::vector<tlp::DoubleProperty *>"},
    {demangleClassName<vector<IntegerProperty *>>(), "std::vector<tlp::IntegerProperty *>"},
    {demangleClassName<vector<LayoutProperty *>>(), "std::vector<tlp::LayoutProperty *>"},
    {demangleClassName<vector<SizeProperty *>>(), "std::vector<tlp::SizeProperty *>"},
    {demangleClassName<vector<StringProperty *>>(), "std::vector<tlp::StringProperty *>"},
    {demangleClassName<vector<PropertyInterface *>>(), "std::vector<tlp::PropertyInterface *>"},
    {demangleClassName<list<int>>(), "std::list<int>"},
    {demangleClassName<list<uint>>(), "std::list<uint>"},
    {demangleClassName<list<long>>(), "std::list<long>"},
    {demangleClassName<list<unsigned long>>(), "std::list<unsigned long>"},
    {demangleClassName<list<float>>(), "std::list<float>"},
    {demangleClassName<list<double>>(), "std::list<double>"},
    {demangleClassName<list<bool>>(), "std::list<bool>"},
    {demangleClassName<list<string>>(), "std::list<std::string>"},
    {demangleClassName<list<node>>(), "std::list<tlp::node>"},
    {demangleClassName<list<edge>>(), "std::list<tlp::edge>"},
    {demangleClassName<list<Coord>>(), "std::list<tlp::Coord>"},
    {demangleClassName<list<Color>>(), "std::list<tlp::Color>"},
    {demangleClassName<list<Size>>(), "std::list<tlp::Size>"},
    {demangleClassName<list<Graph *>>(), "std::list<tlp::Graph*>"},
    {demangleClassName<list<ColorScale>>(), "std::list<tlp::ColorScale>"},
    {demangleClassName<list<StringCollection>>(), "std::list<tlp::StringCollection>"},
    {demangleClassName<list<BooleanProperty *>>(), "std::list<tlp::BooleanProperty *>"},
    {demangleClassName<list<ColorProperty *>>(), "std::list<tlp::ColorProperty *>"},
    {demangleClassName<list<DoubleProperty *>>(), "std::list<tlp::DoubleProperty *>"},
    {demangleClassName<list<IntegerProperty *>>(), "std::list<tlp::IntegerProperty *>"},
    {demangleClassName<list<LayoutProperty *>>(), "std::list<tlp::LayoutProperty *>"},
    {demangleClassName<list<SizeProperty *>>(), "std::list<tlp::SizeProperty *>"},
    {demangleClassName<list<StringProperty *>>(), "std::list<tlp::StringProperty *>"},
    {demangleClassName<list<PropertyInterface *>>(), "std::list<tlp::PropertyInterface *>"},
    {demangleClassName<set<int>>(), "std::set<int>"},
    {demangleClassName<set<uint>>(), "std::set<uint>"},
    {demangleClassName<set<long>>(), "std::set<long>"},
    {demangleClassName<set<unsigned long>>(), "std::set<unsigned long>"},
    {demangleClassName<set<float>>(), "std::set<float>"},
    {demangleClassName<set<double>>(), "std::set<double>"},
    {demangleClassName<set<string>>(), "std::set<std::string>"},
    {demangleClassName<set<node>>(), "std::set<tlp::node>"},
    {demangleClassName<set<edge>>(), "std::set<tlp::edge>"},
    {demangleClassName<set<Coord>>(), "std::set<tlp::Coord>"},
    {demangleClassName<set<Color>>(), "std::set<tlp::Color>"},
    {demangleClassName<set<Size>>(), "std::set<tlp::Size>"},
    {demangleClassName<set<Graph *>>(), "std::set<tlp::Graph*>"}};

void *convertSipWrapperToCppType(PyObject *sipWrapper, const string &cppTypename,
                                 const bool transferTo) {
  PyObject *pyObject = sipWrapper;
  const sipTypeDef *kTypeDef = sipFindType(cppTypename.c_str());

  if (kTypeDef && sipCanConvertToType(pyObject, kTypeDef, transferTo ? 0 : SIP_NOT_NONE)) {
    int state = 0, err = 0;

    if (!transferTo) {
      return sipConvertToType(pyObject, kTypeDef, Py_None, SIP_NOT_NONE, &state, &err);
    } else {
      void *p = sipConvertToType(pyObject, kTypeDef, nullptr, 0, &state, &err);
      sipTransferTo(pyObject, pyObject);
      return p;
    }
  } else if (cppTypenameToSipTypename.contains(cppTypename)) {
    kTypeDef = sipFindType(cppTypenameToSipTypename.at(cppTypename).c_str());

    if (kTypeDef && sipCanConvertToType(pyObject, kTypeDef, transferTo ? 0 : SIP_NOT_NONE)) {
      int state = 0, err = 0;

      if (!transferTo) {
        return sipConvertToType(pyObject, kTypeDef, Py_None, SIP_NOT_NONE, &state, &err);
      } else {
        void *p = sipConvertToType(pyObject, kTypeDef, nullptr, 0, &state, &err);
        sipTransferTo(pyObject, pyObject);
        return p;
      }
    }
  }

  return nullptr;
}

PyObject *convertCppTypeToSipWrapper(void *cppObj, const string &cppTypename, bool fromNew) {
  const sipTypeDef *kTypeDef = sipFindType(cppTypename.c_str());

  if (kTypeDef) {
    if (fromNew) {
      return sipConvertFromNewType(cppObj, kTypeDef, nullptr);
    } else {
      return sipConvertFromType(cppObj, kTypeDef, nullptr);
    }
  } else if (cppTypenameToSipTypename.contains(cppTypename)) {
    kTypeDef = sipFindType(cppTypenameToSipTypename.at(cppTypename).c_str());

    if (kTypeDef) {
      if (fromNew) {
        return sipConvertFromNewType(cppObj, kTypeDef, nullptr);
      } else {
        return sipConvertFromType(cppObj, kTypeDef, nullptr);
      }
    }
  }

  return nullptr;
}

bool convertPyObjectToBool(PyObject *pyObject, bool &cppObject) {
  if (PyBool_Check(pyObject)) {
    cppObject = (pyObject == Py_True);
    return true;
  }

  return false;
}

PyObject *convertBoolToPyObject(bool cppObject) {
  return PyBool_FromLong(cppObject);
}

bool convertPyObjectToLong(PyObject *pyObject, long &cppObject) {
  if (PyLong_Check(pyObject)) {
    cppObject = PyLong_AsLong(pyObject);
    return true;
  }

  return false;
}

bool convertPyObjectToUnsignedLong(PyObject *pyObject, ulong &cppObject) {
  if (PyLong_Check(pyObject)) {
    cppObject = PyLong_AsUnsignedLong(pyObject);
    return true;
  }

  return false;
}

PyObject *convertLongToPyObject(long cppObject) {
  return PyLong_FromLong(cppObject);
}

PyObject *convertUnsignedLongToPyObject(ulong cppObject) {
  return PyLong_FromUnsignedLong(cppObject);
}

bool convertPyObjectToDouble(PyObject *pyObject, double &cppObject) {
  if (PyFloat_Check(pyObject)) {
    cppObject = PyFloat_AsDouble(pyObject);
    return true;
  }

  return false;
}

PyObject *convertDoubleToPyObject(double cppObject) {
  return PyFloat_FromDouble(cppObject);
}

template <typename T>
PyObject *getPyObjectFromCppObject(const T &cppObject) {
  PyObject *pyObj = nullptr;
  CppObjectToPyObjectConverter<T> converter;
  converter.convert(cppObject, pyObj);
  return pyObj;
}

template <typename T>
T getCppObjectFromPyObject(PyObject *pyObj) {
  T v{};
  PyObjectToCppObjectConverter<T> converter;
  converter.convert(pyObj, v);
  return v;
}

template <typename T>
T *getCppPointerFromPyObject(PyObject *pyObj) {
  T *v = nullptr;
  PyObjectToCppObjectConverter<T *> converter;
  converter.convert(pyObj, v);
  return v;
}

#define CHECK_BASE_CPP_TYPE_CONVERSION(T)                                          \
  if (!pyObj && dataType && dataType->getTypeName() == string(typeid(T).name())) { \
    pyObj = getPyObjectFromCppObject(*(static_cast<T *>(dataType->value)));        \
  }

#define CHECK_SIP_CPP_TYPE_CONVERSION(T)                                           \
  if (!pyObj && dataType && dataType->getTypeName() == string(typeid(T).name())) { \
    if (noCopy) {                                                                  \
      pyObj = getPyObjectFromCppObject(static_cast<T *>(dataType->value));         \
    } else {                                                                       \
      pyObj = getPyObjectFromCppObject(*(static_cast<T *>(dataType->value)));      \
    }                                                                              \
  }

PyObject *getPyObjectFromDataType(const DataType *dataType, bool noCopy) {
  PyObject *pyObj = nullptr;
  CHECK_BASE_CPP_TYPE_CONVERSION(bool)
  CHECK_BASE_CPP_TYPE_CONVERSION(int)
  CHECK_BASE_CPP_TYPE_CONVERSION(long)
  CHECK_BASE_CPP_TYPE_CONVERSION(uint)
  CHECK_BASE_CPP_TYPE_CONVERSION(ulong)
  CHECK_BASE_CPP_TYPE_CONVERSION(double)
  CHECK_BASE_CPP_TYPE_CONVERSION(float)
  CHECK_BASE_CPP_TYPE_CONVERSION(string)
  CHECK_SIP_CPP_TYPE_CONVERSION(node)
  CHECK_SIP_CPP_TYPE_CONVERSION(edge)
  CHECK_BASE_CPP_TYPE_CONVERSION(Graph *)
  CHECK_BASE_CPP_TYPE_CONVERSION(PluginProgress *)
  CHECK_SIP_CPP_TYPE_CONVERSION(DataSet)
  CHECK_SIP_CPP_TYPE_CONVERSION(Coord)
  CHECK_SIP_CPP_TYPE_CONVERSION(Color)
  CHECK_SIP_CPP_TYPE_CONVERSION(Size)
  CHECK_SIP_CPP_TYPE_CONVERSION(ColorScale)
  CHECK_SIP_CPP_TYPE_CONVERSION(StringCollection)
  CHECK_BASE_CPP_TYPE_CONVERSION(BooleanProperty *)
  CHECK_BASE_CPP_TYPE_CONVERSION(ColorProperty *)
  CHECK_BASE_CPP_TYPE_CONVERSION(DoubleProperty *)
  CHECK_BASE_CPP_TYPE_CONVERSION(IntegerProperty *)
  CHECK_BASE_CPP_TYPE_CONVERSION(NumericProperty *)
  CHECK_BASE_CPP_TYPE_CONVERSION(LayoutProperty *)
  CHECK_BASE_CPP_TYPE_CONVERSION(SizeProperty *)
  CHECK_BASE_CPP_TYPE_CONVERSION(StringProperty *)
  CHECK_BASE_CPP_TYPE_CONVERSION(BooleanVectorProperty *)
  CHECK_BASE_CPP_TYPE_CONVERSION(ColorVectorProperty *)
  CHECK_BASE_CPP_TYPE_CONVERSION(DoubleVectorProperty *)
  CHECK_BASE_CPP_TYPE_CONVERSION(IntegerVectorProperty *)
  CHECK_BASE_CPP_TYPE_CONVERSION(CoordVectorProperty *)
  CHECK_BASE_CPP_TYPE_CONVERSION(SizeVectorProperty *)
  CHECK_BASE_CPP_TYPE_CONVERSION(StringVectorProperty *)
  CHECK_BASE_CPP_TYPE_CONVERSION(PropertyInterface *)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<Graph *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<node>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<edge>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<string>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<Coord>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<Color>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<Size>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<ColorScale>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<StringCollection>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<BooleanProperty *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<ColorProperty *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<DoubleProperty *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<IntegerProperty *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<LayoutProperty *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<SizeProperty *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<StringProperty *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<PropertyInterface *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<bool>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<int>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<long>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<uint>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<ulong>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<double>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<float>)
  CHECK_SIP_CPP_TYPE_CONVERSION(set<Graph *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(set<node>)
  CHECK_SIP_CPP_TYPE_CONVERSION(set<edge>)
  CHECK_SIP_CPP_TYPE_CONVERSION(set<string>)
  CHECK_SIP_CPP_TYPE_CONVERSION(set<Coord>)
  CHECK_SIP_CPP_TYPE_CONVERSION(set<Color>)
  CHECK_SIP_CPP_TYPE_CONVERSION(set<Size>)
  CHECK_SIP_CPP_TYPE_CONVERSION(set<int>)
  CHECK_SIP_CPP_TYPE_CONVERSION(set<long>)
  CHECK_SIP_CPP_TYPE_CONVERSION(set<uint>)
  CHECK_SIP_CPP_TYPE_CONVERSION(set<ulong>)
  CHECK_SIP_CPP_TYPE_CONVERSION(set<double>)
  CHECK_SIP_CPP_TYPE_CONVERSION(set<float>)
  CHECK_SIP_CPP_TYPE_CONVERSION(list<Graph *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(list<node>)
  CHECK_SIP_CPP_TYPE_CONVERSION(list<edge>)
  CHECK_SIP_CPP_TYPE_CONVERSION(list<string>)
  CHECK_SIP_CPP_TYPE_CONVERSION(list<Coord>)
  CHECK_SIP_CPP_TYPE_CONVERSION(list<Color>)
  CHECK_SIP_CPP_TYPE_CONVERSION(list<Size>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<ColorScale>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<StringCollection>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<BooleanProperty *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<ColorProperty *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<DoubleProperty *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<IntegerProperty *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<LayoutProperty *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<SizeProperty *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<StringProperty *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(vector<PropertyInterface *>)
  CHECK_SIP_CPP_TYPE_CONVERSION(list<int>)
  CHECK_SIP_CPP_TYPE_CONVERSION(list<long>)
  CHECK_SIP_CPP_TYPE_CONVERSION(list<bool>)
  CHECK_SIP_CPP_TYPE_CONVERSION(list<uint>)
  CHECK_SIP_CPP_TYPE_CONVERSION(list<ulong>)
  CHECK_SIP_CPP_TYPE_CONVERSION(list<double>)
  CHECK_SIP_CPP_TYPE_CONVERSION(list<float>)
  return pyObj;
}

#define CHECK_SIP_TYPE_CONVERSION(CPP_TYPE, SIP_TYPE_STR)                          \
  if (sipCanConvertToType(pyObj, sipFindType(SIP_TYPE_STR), SIP_NOT_NONE)) {       \
    if (!dataType || dataType->getTypeName() == string(typeid(CPP_TYPE).name())) { \
      valSetter.setValue(getCppObjectFromPyObject<CPP_TYPE>(pyObj));               \
      return true;                                                                 \
    }                                                                              \
  }

#define CHECK_SIP_POINTER_TYPE_CONVERSION(CPP_TYPE, SIP_TYPE_STR)                    \
  if (sipCanConvertToType(pyObj, sipFindType(SIP_TYPE_STR), 0)) {                    \
    if (!dataType || dataType->getTypeName() == string(typeid(CPP_TYPE *).name())) { \
      valSetter.setValue(getCppPointerFromPyObject<CPP_TYPE>(pyObj));                \
      return true;                                                                   \
    }                                                                                \
  }

#define CHECK_SIP_VECTOR_LIST_CONVERSION(CPP_TYPE, SIP_TYPE_STR)                              \
  if (sipCanConvertToType(pyObj,                                                              \
                          sipFindType((string("std::vector<") + SIP_TYPE_STR + ">").c_str()), \
                          SIP_NOT_NONE)) {                                                    \
    if (dataType && dataType->getTypeName() == string(typeid(list<CPP_TYPE>).name())) {       \
      valSetter.setValue(getCppObjectFromPyObject<list<CPP_TYPE>>(pyObj));                    \
    } else {                                                                                  \
      valSetter.setValue(getCppObjectFromPyObject<vector<CPP_TYPE>>(pyObj));                  \
    }                                                                                         \
    return true;                                                                              \
  }

bool setCppValueFromPyObject(PyObject *pyObj, ValueSetter &valSetter, DataType *dataType) {

  if (PyBool_Check(pyObj)) {
    valSetter.setValue(getCppObjectFromPyObject<bool>(pyObj));
    return true;
  }

  if (PyLong_Check(pyObj)) {
    long val = getCppObjectFromPyObject<long>(pyObj);

    if (dataType && dataType->getTypeName() == string(typeid(int).name())) {
      valSetter.setValue(int(val));
    } else if (dataType && dataType->getTypeName() == string(typeid(uint).name())) {
      valSetter.setValue(getCppObjectFromPyObject<uint>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(long).name())) {
      valSetter.setValue(val);
    } else if (dataType && dataType->getTypeName() == string(typeid(ulong).name())) {
      valSetter.setValue(getCppObjectFromPyObject<ulong>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(float).name())) {
      valSetter.setValue(float(val));
    } else if (dataType && dataType->getTypeName() == string(typeid(double).name())) {
      valSetter.setValue(double(val));
    } else {
      valSetter.setValue(int(val));
    }

    return true;
  }

  if (PyFloat_Check(pyObj)) {
    if (dataType && dataType->getTypeName() == string(typeid(float).name())) {
      valSetter.setValue(getCppObjectFromPyObject<float>(pyObj));
    } else {
      valSetter.setValue(getCppObjectFromPyObject<double>(pyObj));
    }

    return true;
  }

  CHECK_SIP_VECTOR_LIST_CONVERSION(bool, "bool")

  if (sipCanConvertToType(pyObj, sipFindType("std::vector<long>"), SIP_NOT_NONE)) {
    if (dataType && dataType->getTypeName() == string(typeid(vector<int>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<vector<int>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(vector<uint>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<vector<uint>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(vector<long>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<vector<long>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(vector<ulong>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<vector<ulong>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(vector<float>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<vector<float>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(vector<double>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<vector<double>>(pyObj));

    } else if (dataType && dataType->getTypeName() == string(typeid(list<int>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<list<int>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(list<uint>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<list<uint>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(list<long>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<list<long>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(list<ulong>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<list<ulong>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(list<float>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<list<float>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(list<double>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<list<double>>(pyObj));
    } else {
      valSetter.setValue(getCppObjectFromPyObject<vector<int>>(pyObj));
    }

    return true;
  }

  if (sipCanConvertToType(pyObj, sipFindType("std::vector<double>"), SIP_NOT_NONE)) {
    if (dataType && dataType->getTypeName() == string(typeid(vector<float>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<vector<float>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(list<float>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<list<float>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(list<double>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<list<double>>(pyObj));
    } else {
      valSetter.setValue(getCppObjectFromPyObject<vector<double>>(pyObj));
    }

    return true;
  }

  if (sipCanConvertToType(pyObj, sipFindType("std::set<long>"), SIP_NOT_NONE)) {
    if (dataType && dataType->getTypeName() == string(typeid(set<int>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<set<int>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(set<uint>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<set<uint>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(set<long>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<set<long>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(set<ulong>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<set<ulong>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(set<float>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<set<float>>(pyObj));
    } else if (dataType && dataType->getTypeName() == string(typeid(set<double>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<set<double>>(pyObj));
    } else {
      valSetter.setValue(getCppObjectFromPyObject<set<int>>(pyObj));
    }

    return true;
  }

  if (sipCanConvertToType(pyObj, sipFindType("std::set<double>"), SIP_NOT_NONE)) {
    if (dataType && dataType->getTypeName() == string(typeid(set<float>).name())) {
      valSetter.setValue(getCppObjectFromPyObject<set<float>>(pyObj));
    } else {
      valSetter.setValue(getCppObjectFromPyObject<set<double>>(pyObj));
    }

    return true;
  }

  CHECK_SIP_TYPE_CONVERSION(string, "std::string")
  CHECK_SIP_TYPE_CONVERSION(node, "tlp::node")
  CHECK_SIP_TYPE_CONVERSION(edge, "tlp::edge")
  CHECK_SIP_TYPE_CONVERSION(Coord, "tlp::Coord")
  CHECK_SIP_TYPE_CONVERSION(Size, "tlp::Size")
  CHECK_SIP_TYPE_CONVERSION(Vec3f, "tlp::Vec3f")
  CHECK_SIP_TYPE_CONVERSION(Color, "tlp::Color")
  CHECK_SIP_POINTER_TYPE_CONVERSION(Graph, "tlp::Graph")
  CHECK_SIP_TYPE_CONVERSION(DataSet, "tlp::DataSet")
  CHECK_SIP_TYPE_CONVERSION(ColorScale, "tlp::ColorScale")
  CHECK_SIP_TYPE_CONVERSION(StringCollection, "tlp::StringCollection")
  CHECK_SIP_POINTER_TYPE_CONVERSION(BooleanProperty, "tlp::BooleanProperty")
  CHECK_SIP_POINTER_TYPE_CONVERSION(ColorProperty, "tlp::ColorProperty")
  CHECK_SIP_POINTER_TYPE_CONVERSION(DoubleProperty, "tlp::DoubleProperty")
  CHECK_SIP_POINTER_TYPE_CONVERSION(IntegerProperty, "tlp::IntegerProperty")
  CHECK_SIP_POINTER_TYPE_CONVERSION(NumericProperty, "tlp::NumericProperty")
  CHECK_SIP_POINTER_TYPE_CONVERSION(LayoutProperty, "tlp::LayoutProperty")
  CHECK_SIP_POINTER_TYPE_CONVERSION(SizeProperty, "tlp::SizeProperty")
  CHECK_SIP_POINTER_TYPE_CONVERSION(StringProperty, "tlp::StringProperty")
  CHECK_SIP_POINTER_TYPE_CONVERSION(BooleanVectorProperty, "tlp::BooleanVectorProperty")
  CHECK_SIP_POINTER_TYPE_CONVERSION(ColorVectorProperty, "tlp::ColorVectorProperty")
  CHECK_SIP_POINTER_TYPE_CONVERSION(DoubleVectorProperty, "tlp::DoubleVectorProperty")
  CHECK_SIP_POINTER_TYPE_CONVERSION(IntegerVectorProperty, "tlp::IntegerVectorProperty")
  CHECK_SIP_POINTER_TYPE_CONVERSION(CoordVectorProperty, "tlp::CoordVectorProperty")
  CHECK_SIP_POINTER_TYPE_CONVERSION(SizeVectorProperty, "tlp::SizeVectorProperty")
  CHECK_SIP_POINTER_TYPE_CONVERSION(StringVectorProperty, "tlp::StringVectorProperty")
  CHECK_SIP_POINTER_TYPE_CONVERSION(PropertyInterface, "tlp::PropertyInterface")

  CHECK_SIP_VECTOR_LIST_CONVERSION(string, "std::string")
  CHECK_SIP_VECTOR_LIST_CONVERSION(node, "tlp::node")
  CHECK_SIP_VECTOR_LIST_CONVERSION(edge, "tlp::edge")
  CHECK_SIP_VECTOR_LIST_CONVERSION(Coord, "tlp::Coord")
  CHECK_SIP_VECTOR_LIST_CONVERSION(Color, "tlp::Color")
  CHECK_SIP_VECTOR_LIST_CONVERSION(Size, "tlp::Size")
  CHECK_SIP_VECTOR_LIST_CONVERSION(Graph *, "tlp::Graph*")
  CHECK_SIP_VECTOR_LIST_CONVERSION(DataSet, "tlp::DataSet")
  CHECK_SIP_VECTOR_LIST_CONVERSION(ColorScale, "tlp::ColorScale")
  CHECK_SIP_VECTOR_LIST_CONVERSION(StringCollection, "tlp::StringCollection")
  CHECK_SIP_VECTOR_LIST_CONVERSION(BooleanProperty *, "tlp::BooleanProperty*")
  CHECK_SIP_VECTOR_LIST_CONVERSION(ColorProperty *, "tlp::ColorProperty*")
  CHECK_SIP_VECTOR_LIST_CONVERSION(DoubleProperty *, "tlp::DoubleProperty*")
  CHECK_SIP_VECTOR_LIST_CONVERSION(IntegerProperty *, "tlp::IntegerProperty*")
  CHECK_SIP_VECTOR_LIST_CONVERSION(LayoutProperty *, "tlp::LayoutProperty*")
  CHECK_SIP_VECTOR_LIST_CONVERSION(SizeProperty *, "tlp::SizeProperty*")
  CHECK_SIP_VECTOR_LIST_CONVERSION(StringProperty *, "tlp::StringProperty*")
  CHECK_SIP_VECTOR_LIST_CONVERSION(PropertyInterface *, "tlp::PropertyInterface*")

  CHECK_SIP_TYPE_CONVERSION(set<string>, "std::set<std::string>")
  CHECK_SIP_TYPE_CONVERSION(set<node>, "std::set<tlp::node>")
  CHECK_SIP_TYPE_CONVERSION(set<edge>, "std::set<tlp::edge>")
  CHECK_SIP_TYPE_CONVERSION(set<Coord>, "std::set<tlp::Coord>")
  CHECK_SIP_TYPE_CONVERSION(set<Color>, "std::set<tlp::Color>")
  CHECK_SIP_TYPE_CONVERSION(set<Size>, "std::set<tlp::Size>")

  return false;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
