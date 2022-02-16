/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

// std::list
template<TYPE *>
%MappedType std::list<TYPE *> /TypeHint="List[TYPE]"/ {
%TypeHeaderCode
#include <list>
%End

%ConvertFromTypeCode
  PyObject *l;

  const sipTypeDef* kpTypeDef = getSipType("TYPE");

  if (!kpTypeDef) {
    return NULL;
  }

  // Create the Python list of the correct length.
  if ((l = PyList_New(sipCpp -> size())) == NULL) {
    return NULL;
  }

  int i = 0;
  for (TYPE *cpp : *sipCpp) {
    PyObject *pobj;

    // Get the Python wrapper for the Type instance, creating a new
    // one if necessary, and handle any ownership transfer.
    if ((pobj = sipConvertFromType(cpp, kpTypeDef, sipTransferObj)) == NULL) {
      // There was an error so garbage collect the Python list.
      Py_XDECREF(l);
      return NULL;
    }

    // Add the wrapper to the list.
    PyList_SET_ITEM(l, i++, pobj);
  }

  // Return the Python list.
  return l;
%End

%ConvertToTypeCode

  const sipTypeDef* kpTypeDef = getSipType("TYPE");

  if (!kpTypeDef) {
    return 0;
  }

  // Check if type is compatible
  if (sipIsErr == NULL) {
    if (!PyList_Check(sipPy)) {
      return 0;
    }
    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
      PyObject *item = PyList_GET_ITEM(sipPy, i);
      if (!sipCanConvertToType(item, kpTypeDef, SIP_NOT_NONE)) {
        return 0;
      }
    }
    return 1;
  }

  // Convert Python list of TYPE to  std::list<TYPE*>
  std::list<TYPE*> *l = new std::list<TYPE*>();

  for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
    int state;
    PyObject *item = PyList_GET_ITEM(sipPy, i);
    TYPE* p = static_cast<TYPE*>(sipConvertToType(item, kpTypeDef, NULL, SIP_NOT_NONE, &state, sipIsErr));

    if (*sipIsErr) {
      sipReleaseType(p, kpTypeDef, state);
      delete l;
      return 0;
    }

    sipTransferTo(item, item);
    l->push_back(p);

    sipReleaseType(p, kpTypeDef, state);
  }

  *sipCppPtr = l;
  return sipGetState(sipTransferObj);
%End
};

template<TYPE>
%MappedType std::list<TYPE> /TypeHint="List[TYPE]"/ {
%TypeHeaderCode
#include <list>
%End

%ConvertFromTypeCode
  PyObject *l;

  const sipTypeDef* kpTypeDef = getSipType("TYPE");

  if (!kpTypeDef) {
    return NULL;
  }

  // Create the Python list of the correct length.
  if ((l = PyList_New(sipCpp -> size())) == NULL) {
    return NULL;
  }

  int i = 0;
  for (const TYPE &v : *sipCpp) {
    TYPE *cpp = new TYPE(v);
    PyObject *pobj;

    // Get the Python wrapper for the Type instance, creating a new
    // one if necessary, and handle any ownership transfer.
    if ((pobj = sipConvertFromNewType(cpp, kpTypeDef, sipTransferObj)) == NULL) {
      // There was an error so garbage collect the Python list.
      Py_XDECREF(l);
      return NULL;
    }

    // Add the wrapper to the list.
    PyList_SET_ITEM(l, i++, pobj);
  }

  // Return the Python list.
  return l;
%End

%ConvertToTypeCode

  const sipTypeDef* kpTypeDef = getSipType("TYPE");

  if (!kpTypeDef) {
    return 0;
  }

  // Check if type is compatible
  if (sipIsErr == NULL) {
    if (!PyList_Check(sipPy)) {
      return 0;
    }
    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
      PyObject *item = PyList_GET_ITEM(sipPy, i);
      if (!sipCanConvertToType(item, kpTypeDef, SIP_NOT_NONE)) {
        return 0;
      }
    }
    return 1;
  }

  // Convert Python list of TYPE to  std::list<TYPE>
  std::list<TYPE> *l = new std::list<TYPE>();
  for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
    int state;
    TYPE* p = static_cast<TYPE*>(sipConvertToType(PyList_GET_ITEM(sipPy, i), kpTypeDef, sipTransferObj, SIP_NOT_NONE, &state, sipIsErr));

    if (*sipIsErr) {
      sipReleaseType(p, kpTypeDef, state);
      delete l;
      return 0;
    }

    l->push_back(*p);

    sipReleaseType(p, kpTypeDef, state);
  }

  *sipCppPtr = l;
  return sipGetState(sipTransferObj);
%End
};

// ****************************************************
// Specialization for std::list<double>
// ****************************************************

%MappedType std::list<double> /TypeHint="List[float]"/ {
%TypeHeaderCode
#include <list>
%End

%ConvertFromTypeCode
  PyObject *l;

  // Create the Python list of the correct length.
  if ((l = PyList_New(sipCpp -> size())) == NULL) {
    return NULL;
  }

  // Go through each element in the C++ instance and convert it to a
  // wrapped object.
  int i = 0;
  for (double d : *sipCpp) {
    // Add the wrapper to the list.
    PyList_SET_ITEM(l, i++, PyFloat_FromDouble(d));
  }

  // Return the Python list.
  return l;
%End

%ConvertToTypeCode
  // Check if type is compatible
  if (sipIsErr == NULL) {
    if (!PyList_Check(sipPy)) {
      return 0;
    }
    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
      PyObject *item = PyList_GET_ITEM(sipPy, i);
      if (!PyNumber_Check(item)) {
        return 0;
      }
    }
    return 1;
  }

  // Convert Python list of numbers to a std::vector<double>
  std::list<double> *v = new std::list<double>();
  for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
    PyObject *f = PyNumber_Float(PyList_GET_ITEM(sipPy, i));
    v->push_back(PyFloat_AsDouble(f));
    Py_XDECREF(f);
  }

  *sipCppPtr = v;
  return sipGetState(sipTransferObj);
%End
};

// ****************************************************
// Specialization for std::list<float>
// ****************************************************

%MappedType std::list<float> /TypeHint="List[float]"/ {
%TypeHeaderCode
#include <list>
%End

%ConvertFromTypeCode
  PyObject *l;

  // Create the Python list of the correct length.
  if ((l = PyList_New(sipCpp -> size())) == NULL) {
    return NULL;
  }

  // Go through each element in the C++ instance and convert it to a
  // wrapped object.
  int i = 0;
  for (float f : *sipCpp) {
    // Add the wrapper to the list.
    PyList_SET_ITEM(l, i++, PyFloat_FromDouble(f));
  }

  // Return the Python list.
  return l;
%End

%ConvertToTypeCode
  // Check if type is compatible
  if (sipIsErr == NULL) {
    if (!PyList_Check(sipPy)) {
      return 0;
    }
    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
      PyObject *item = PyList_GET_ITEM(sipPy, i);
      if (!PyNumber_Check(item)) {
        return 0;
      }
    }
    return 1;
  }

  // Convert Python list of numbers to a std::vector<double>
  std::list<float> *v = new std::list<float>();
  for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
    PyObject *f = PyNumber_Float(PyList_GET_ITEM(sipPy, i));
    v->push_back(float(PyFloat_AsDouble(f)));
    Py_XDECREF(f);
  }

  *sipCppPtr = v;
  return sipGetState(sipTransferObj);
%End
};


// ****************************************************
// Specialization for std::list<int>
// ****************************************************

%MappedType std::list<int> /TypeHint="List[int]"/ {
%TypeHeaderCode
#include <list>
%End

%ConvertFromTypeCode
  PyObject *l;

  // Create the Python list of the correct length.
  if ((l = PyList_New(sipCpp -> size())) == NULL) {
    return NULL;
  }

  // Go through each element in the C++ instance and convert it to a
  // wrapped object.
  int i = 0;
  for (int n : *sipCpp) {
    // Add the wrapper to the list.
    PyList_SET_ITEM(l, i++, PyLong_FromLong(n));
  }

  // Return the Python list.
  return l;
%End

%ConvertToTypeCode
  // Check if type is compatible
  if (sipIsErr == NULL) {
    if (!PyList_Check(sipPy)) {
      return 0;
    }
    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
      PyObject *item = PyList_GET_ITEM(sipPy, i);
      if (!PyLong_Check(item)) {
          return 0;
        }
      }
      return 1;
    }

    // Convert Python list of integers to a std::vector<int>
    std::list<int> *v = new std::list<int>();
    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
      v->push_back(int(PyLong_AsLong(PyList_GET_ITEM(sipPy, i))));
    }

    *sipCppPtr = v;
    return sipGetState(sipTransferObj);
%End

};

// ****************************************************
// Specialization for std::list<uint>
// ****************************************************

%MappedType std::list<uint> /TypeHint="List[int]"/ {
%TypeHeaderCode
#include <list>
%End

%ConvertFromTypeCode
  PyObject *l;

  // Create the Python list of the correct length.
  if ((l = PyList_New(sipCpp -> size())) == NULL) {
    return NULL;
  }

  // Go through each element in the C++ instance and convert it to a
  // wrapped object.
  int i = 0;
  for (uint n : *sipCpp) {
    // Add the wrapper to the list.
    PyList_SET_ITEM(l, i++, PyLong_FromUnsignedLong(n));
  }

  // Return the Python list.
  return l;
%End

%ConvertToTypeCode
  // Check if type is compatible
  if (sipIsErr == NULL) {
    if (!PyList_Check(sipPy)) {
      return 0;
    }
    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
      PyObject *item = PyList_GET_ITEM(sipPy, i);
      if (!PyLong_Check(item)) {
          return 0;
        }
      }
      return 1;
    }

    // Convert Python list of integers to a std::vector<int>
    std::list<uint> *v = new std::list<uint>();
    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
      v->push_back(uint(PyLong_AsUnsignedLong(PyList_GET_ITEM(sipPy, i))));
    }

    *sipCppPtr = v;
    return sipGetState(sipTransferObj);
%End

};

// ****************************************************
// Specialization for std::list<long>
// ****************************************************

%MappedType std::list<long> /TypeHint="List[int]"/ {
%TypeHeaderCode
#include <list>
%End

%ConvertFromTypeCode
  PyObject *l;

  // Create the Python list of the correct length.
  if ((l = PyList_New(sipCpp -> size())) == NULL) {
    return NULL;
  }

  // Go through each element in the C++ instance and convert it to a
  // wrapped object.
  int i = 0;
  for (long l : *sipCpp) {
    // Add the wrapper to the list.
    PyList_SET_ITEM(l, i++, PyLong_FromLong(l));
  }

  // Return the Python list.
  return l;
%End

%ConvertToTypeCode
  // Check if type is compatible
  if (sipIsErr == NULL) {
    if (!PyList_Check(sipPy)) {
      return 0;
    }
    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
      PyObject *item = PyList_GET_ITEM(sipPy, i);
      if (!PyLong_Check(item)) {
          return 0;
        }
      }
      return 1;
    }

    // Convert Python list of integers to a std::vector<int>
    std::list<long> *v = new std::list<long>();
    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
      v->push_back(PyLong_AsLong(PyList_GET_ITEM(sipPy, i)));
    }

    *sipCppPtr = v;
    return sipGetState(sipTransferObj);
%End

};

// ****************************************************
// Specialization for std::list<unsigned long>
// ****************************************************

%MappedType std::list<unsigned long> /TypeHint="List[int]"/ {
%TypeHeaderCode
#include <list>
%End

%ConvertFromTypeCode
  PyObject *l;

  // Create the Python list of the correct length.
  if ((l = PyList_New(sipCpp -> size())) == NULL) {
    return NULL;
  }

  // Go through each element in the C++ instance and convert it to a
  // wrapped object.
  int i = 0;
  for (uint n : *sipCpp) {
    // Add the wrapper to the list.
    PyList_SET_ITEM(l, i++, PyLong_FromUnsignedLong(n));
  }

  // Return the Python list.
  return l;
%End

%ConvertToTypeCode
  // Check if type is compatible
  if (sipIsErr == NULL) {
    if (!PyList_Check(sipPy)) {
      return 0;
    }
    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
      PyObject *item = PyList_GET_ITEM(sipPy, i);
      if (!PyLong_Check(item)) {
          return 0;
        }
      }
      return 1;
    }

    // Convert Python list of integers to a std::vector<int>
    std::list<unsigned long> *v = new std::list<unsigned long>();
    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
      v->push_back(PyLong_AsUnsignedLong(PyList_GET_ITEM(sipPy, i)));
    }

    *sipCppPtr = v;
    return sipGetState(sipTransferObj);
%End

};

// ****************************************************
// Specialization for std::list<bool>
// ****************************************************

%MappedType std::list<bool> /TypeHint="List[int]"/{
%TypeHeaderCode
#include <list>
%End

%ConvertFromTypeCode
  PyObject *l;

  // Create the Python list of the correct length.
  if ((l = PyList_New(sipCpp -> size())) == NULL) {
    return NULL;
  }

  // Go through each element in the C++ instance and convert it to a
  // wrapped object.
  int i = 0;
  for (bool b : *sipCpp) {
    // Add the wrapper to the list.
    PyList_SET_ITEM(l, i, PyBool_FromLong(b));
  }

  // Return the Python list.
  return l;
%End

%ConvertToTypeCode
  // Check if type is compatible
  if (sipIsErr == NULL) {
    if (!PyList_Check(sipPy)) {
      return 0;
    }
    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
      PyObject *item = PyList_GET_ITEM(sipPy, i);
      if (!PyBool_Check(item)) {
        return 0;
      }
    }
    return 1;
  }

  // Convert Python list of booleans to a std::vector<bool>
  std::list<bool> *v = new std::list<bool>();
  for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
    v->push_back(PyList_GET_ITEM(sipPy, i) == Py_True);
  }

  *sipCppPtr = v;
  return sipGetState(sipTransferObj);
%End

};
