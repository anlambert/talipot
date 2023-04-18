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

%ModuleHeaderCode
extern PyObject *convertTlpDataSetToPyDict(const tlp::DataSet &dataSet, PyObject *dict = NULL);
extern tlp::DataSet *convertPyDictToTlpDataSet(PyObject *dict, tlp::DataSet *refDataSet = NULL,
                                               const std::string &algoName = "");
extern bool setDataSetEntryFromPyObject(tlp::DataSet *dataSet, const std::string &entry,
                                        PyObject *value, tlp::DataType *refDataType = NULL,
                                        const std::string &algoName = "");
%End

%ModuleCode
PyObject *convertTlpDataSetToPyDict(const tlp::DataSet &dataSet, PyObject *dict) {
  if (!dict)
    dict = PyDict_New();
  for (const std::pair<std::string, tlp::DataType *> &entry : dataSet.getValues()) {
    PyObject *key =
        sipConvertFromNewType(new std::string(entry.first), sipFindType("std::string"), NULL);
    PyObject *val = NULL;
    if (entry.second->getTypeName() == std::string(typeid(tlp::StringCollection).name())) {
      tlp::StringCollection *sc = static_cast<tlp::StringCollection *>(entry.second->value);
      val = sipConvertFromNewType(new std::string(sc->getCurrentString()),
                                  sipFindType("std::string"), NULL);
    } else {
      val = getPyObjectFromDataType(entry.second);
    }
    PyDict_SetItem(dict, key, val);
    Py_XDECREF(val);
  }
  return dict;
}

extern tlp::DataSet *convertPyDictToTlpDataSet(PyObject *dict, tlp::DataSet *refDataSet,
                                               const std::string &algoName) {
  PyObject *key = NULL;
  PyObject *val = NULL;
  Py_ssize_t pos = 0;
  int state = 0, err = 0;
  tlp::DataSet *ret = new tlp::DataSet();
  if (refDataSet) {
    *ret = *refDataSet;
  }
  enableErrorMessages(false);
  while (PyDict_Next(dict, &pos, &key, &val)) {
    const sipTypeDef *sipType = sipFindType("std::string");
    std::string *keyStr = static_cast<std::string *>(
        sipConvertToType(key, sipType, NULL, SIP_NOT_NONE, &state, &err));
    tlp::DataType *dataType = NULL;
    if (refDataSet) {
      dataType = refDataSet->getData(*keyStr);
    }
    if (!setDataSetEntryFromPyObject(ret, *keyStr, val, dataType, algoName)) {
      sipReleaseType(keyStr, sipType, state);
      delete ret;
      ret = NULL;
      break;
    }
    sipReleaseType(keyStr, sipType, state);
  }
  enableErrorMessages(true);
  return ret;
}

bool setDataSetEntryFromPyObject(tlp::DataSet *dataSet, const std::string &key, PyObject *value,
                                 tlp::DataType *refDataType, const std::string &algoName) {
  bool ret = true;
  tlp::DataType *dataType = NULL;
  if (refDataType) {
    dataType = refDataType;
  }
  if (dataType && dataType->getTypeName() == std::string(typeid(tlp::StringCollection).name()) &&
      PyUnicode_Check(value)) {

    tlp::StringCollection *sc = static_cast<tlp::StringCollection *>(dataType->value);
    std::string entry(convertPythonUnicodeObjectToStdString(value));

    std::vector<std::string> values = sc->getValues();
    if (std::find(values.begin(), values.end(), entry) != values.end()) {
      sc->setCurrent(entry);
      dataSet->set(key, *sc);
    } else {
      std::string msg = "Invalid value provided (\"" + entry + "\") to string parameter '" + key +
                        "' of algorithm '" + algoName + "'.\n";

      msg += "Possible values are : ";
      for (size_t i = 0; i < values.size(); ++i) {
        replaceAll(values[i], "\n", "\\n");
        msg += values[i];
        if (i != values.size() - 1) {
          msg += ", ";
        }
      }
      PyErr_SetString(PyExc_Exception, msg.c_str());
      ret = false;
    }
  } else {
    ValueSetter valSetter(dataSet, key);
    if (!setCppValueFromPyObject(value, valSetter, dataType)) {
      std::string msg;
      if (!dataType) {
        msg = "Object of type ";
        msg += std::string(value->ob_type->tp_name);
        msg += " can not be stored in a Talipot dataset.";
      } else {
        msg = "Invalid type for the value provided to parameter '" + key + "' ";
        msg += "for algorithm '" + algoName + "'\n";
        msg += "Provided Python type is '" + std::string(value->ob_type->tp_name) + "'\n";
        msg += "Expected C++ type is '" + tlp::demangleClassName(dataType->getTypeName()) + "'";
      }
      PyErr_SetString(PyExc_Exception, msg.c_str());
      ret = false;
    }
  }
  delete dataType;
  return ret;
}
%End

namespace tlp {

struct DataSet /TypeHint="Dict[str, Any]", TypeHintValue="{}"/ {
%Docstring

.. deprecated:: deprecated
   The direct use of that class is now deprecated as it is now mapped to a Python dictionary
   for commodity of use.

This class is used to store a set of parameters to transmit to a Talipot algorithm.
Below is an exhaustive list of data types that can be stored in a Talipot data set:

  * boolean
  * integer
  * float
  * string
  * :class:`tlp.node`
  * :class:`tlp.edge`
  * :class:`tlp.Graph`
  * :class:`tlp.Coord`
  * :class:`tlp.Color`
  * :class:`tlp.Size`
  * :class:`tlp.DataSet`
  * :class:`tlp.ColorScale`
  * :class:`tlp.StringCollection`
  * :class:`tlp.BooleanProperty`
  * :class:`tlp.ColorProperty`
  * :class:`tlp.DoubleProperty`
  * :class:`tlp.IntegerProperty`
  * :class:`tlp.LayoutProperty`
  * :class:`tlp.SizeProperty`
  * :class:`tlp.StringProperty`
  * :class:`tlp.PropertyInterface`
  * list of those types

If you try to store an unsupported data type, an exception will be thrown.
Each value stored in a data set is associated to a string key. Reading / Writing a value in
a data set is done through the [] operator as illustrated below::

  # data set creation
  dataSet = tlp.DataSet()

  # writing integer values
  dataSet["height"] = 100
  dataSet["width"] = 300

  # reading integer values
  area = dataSet["height"] * dataSet["width"]

When reading a data set value, a reference is returned not a copy.

%End

%TypeCode
#include <talipot/PythonCppTypesConverter.h>
%End

%ConvertToTypeCode

PyObject *key = NULL;
PyObject *val = NULL;
Py_ssize_t pos = 0;

if (sipIsErr == NULL) {
  if (sipCanConvertToType(sipPy, sipFindType("tlp::DataSet"), SIP_NOT_NONE | SIP_NO_CONVERTORS)) {
    return 1;
  } else if (PyDict_Check(sipPy)) {
    while (PyDict_Next(sipPy, &pos, &key, &val)) {
      if (!sipCanConvertToType(key, sipFindType("std::string"), SIP_NOT_NONE)) {
        return 0;
      }
    }
    return 1;
  }
  return 0;
}

int state = 0, err = 0;

if (PyDict_Check(sipPy)) {
  *sipCppPtr = convertPyDictToTlpDataSet(sipPy);
} else {
  const sipTypeDef *sipType = sipFindType("tlp::DataSet");
  tlp::DataSet *ds = static_cast<tlp::DataSet *>(
      sipConvertToType(sipPy, sipType, NULL, SIP_NOT_NONE | SIP_NO_CONVERTORS, &state, &err));
  *sipCppPtr = new tlp::DataSet(*ds);
  sipReleaseType(ds, sipType, state);
}

if (!*sipCppPtr) {
  *sipIsErr = 1;
}

return sipGetState(sipTransferObj);
%End

  DataSet();

  DataSet(const tlp::DataSet &set);

  bool exists(const std::string &str) const;
%Docstring
tlp.DataSet.exists(key)

Checks if the data set contains a value associated to a particular key.

:param key:
   the key to check

:type key:
   string

:rtype:
   boolean
%End

  void remove(const std::string &str);
%Docstring
tlp.DataSet.remove(key)

Remove the value associated to a particular key from the data set if it exists.

:param key:
   the key of the value to remove.

:type key:
   string
%End

  uint size() const;
%Docstring
tlp.DataSet.size()

Returns the number of values stored in the data set.

:rtype:
   integer
%End

  std::vector<std::string> getKeys() const /TypeHint="List[str]"/;
%Docstring
tlp.DataSet.getKeys()

Returns the list of keys associated to the values stored in the data set.

:rtype:
   list of string
%End

%MethodCode
std::vector<std::string> ret;
for (const std::pair<std::string, tlp::DataType *> &p : sipCpp->getValues()) {
  ret.push_back(p.first);
}
sipRes = new std::vector<std::string>(ret);
%End

  void __setitem__(const std::string &attributeName, SIP_PYOBJECT po);
%MethodCode
sipIsErr = setDataSetEntryFromPyObject(sipCpp, *a0, a1) ? 0 : 1;
%End

  SIP_PYOBJECT __getitem__(const std::string &attributeName);
%MethodCode
sipRes = NULL;
tlp::DataType *dataType = NULL;
for (const std::pair<std::string, tlp::DataType *> &p : sipCpp->getValues()) {
  if (p.first == *a0) {
    dataType = p.second;
  }
}

sipRes = getPyObjectFromDataType(dataType, true);

if (!sipRes) {
  std::ostringstream oss;
  oss << "Dataset entry \"" << *a0 << "\" does not exist.";
  sipIsErr = 1;
  PyErr_SetString(PyExc_AttributeError, oss.str().c_str());
}
%End

  SIP_PYOBJECT toDict();
%Docstring
tlp.DataSet.toDict()

Converts that data set to a Python dictionary.

:rtype:
   a dictionary reflecting the data set content
%End
%MethodCode
sipRes = convertTlpDataSetToPyDict(*sipCpp);
%End

  SIP_PYOBJECT __repr__() const;
%MethodCode
PyObject *pyDict = convertTlpDataSetToPyDict(*sipCpp);
sipRes = PyObject_Repr(pyDict);
Py_XDECREF(pyDict);
%End

};
};
