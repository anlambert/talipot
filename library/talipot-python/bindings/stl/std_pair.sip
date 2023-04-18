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

template<T1, T2>
%MappedType std::pair<T1, T2> /TypeHint="Tuple[T1, T2]"/{
%TypeHeaderCode
#include <utility>
%End

%ConvertToTypeCode
PyObject *itm1 = NULL;
PyObject *itm2 = NULL;

const sipTypeDef *kpTypeDef1 = getSipType("T1");

if (!kpTypeDef1) {
  return 0;
}

const sipTypeDef *kpTypeDef2 = getSipType("T2");

if (!kpTypeDef2) {
  return 0;
}

if (sipIsErr == NULL) {
  if (!(PyTuple_Check(sipPy) && PyTuple_Size(sipPy) == 2))
    return 0;

  itm1 = PyTuple_GetItem(sipPy, 0);
  itm2 = PyTuple_GetItem(sipPy, 1);

  bool ok = sipCanConvertToType(itm1, kpTypeDef1, SIP_NOT_NONE);
  ok = ok && sipCanConvertToType(itm2, kpTypeDef2, SIP_NOT_NONE);

  if (!ok) {
    return 0;
  }

  return 1;
}

itm1 = PyTuple_GetItem(sipPy, 0);
itm2 = PyTuple_GetItem(sipPy, 1);

int t1State = 0;
int t2State = 0;

T1 *t1 = static_cast<T1 *>(
    sipConvertToType(itm1, kpTypeDef1, sipTransferObj, SIP_NOT_NONE, &t1State, sipIsErr));
T2 *t2 = static_cast<T2 *>(
    sipConvertToType(itm2, kpTypeDef2, sipTransferObj, SIP_NOT_NONE, &t2State, sipIsErr));

if (*sipIsErr) {
  sipReleaseType(t1, kpTypeDef1, t1State);
  sipReleaseType(t2, kpTypeDef2, t2State);
  return 0;
}

std::pair<T1, T2> *p = new std::pair<T1, T2>(*t1, *t2);

sipReleaseType(t1, kpTypeDef1, t1State);
sipReleaseType(t2, kpTypeDef2, t2State);

*sipCppPtr = p;
return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode

const sipTypeDef *kpTypeDef1 = getSipType("T1");

if (!kpTypeDef1) {
  return NULL;
}

const sipTypeDef *kpTypeDef2 = getSipType("T2");

if (!kpTypeDef2) {
  return NULL;
}

PyObject *pyTuple = PyTuple_New(2);

if (pyTuple == NULL) {
  return NULL;
}

PyObject *pyFirst = NULL;
PyObject *pySecond = NULL;

T1 *first = new T1(sipCpp->first);
T2 *second = new T2(sipCpp->second);

pyFirst = sipConvertFromNewType(first, kpTypeDef1, sipTransferObj);
pySecond = sipConvertFromNewType(second, kpTypeDef2, sipTransferObj);

if (pyFirst == NULL || pySecond == NULL || PyTuple_SetItem(pyTuple, 0, pyFirst)) {
  if (pyFirst)
    Py_XDECREF(pyFirst);
  else
    delete first;
  if (pySecond)
    Py_XDECREF(pySecond);
  else
    delete second;
  Py_XDECREF(pyTuple);

  return NULL;
}

if (PyTuple_SetItem(pyTuple, 1, pySecond)) {
  Py_XDECREF(pySecond);
  Py_XDECREF(pyTuple);

  return NULL;
}

return pyTuple;
%End
};

template<T1*, T2*>
%MappedType std::pair<T1*, T2*> {
%TypeHeaderCode
#include <utility>
%End

%ConvertToTypeCode
PyObject *itm1 = NULL;
PyObject *itm2 = NULL;

const sipTypeDef *kpTypeDef1 = getSipType("T1");

if (!kpTypeDef1) {
  return 0;
}

const sipTypeDef *kpTypeDef2 = getSipType("T2");

if (!kpTypeDef2) {
  return 0;
}

if (sipIsErr == NULL) {
  if (!(PyTuple_Check(sipPy) && PyTuple_Size(sipPy) == 2))
    return 0;

  itm1 = PyTuple_GetItem(sipPy, 0);
  itm2 = PyTuple_GetItem(sipPy, 1);

  bool ok = sipCanConvertToType(itm1, kpTypeDef1, SIP_NOT_NONE);
  ok = ok && sipCanConvertToType(itm2, kpTypeDef2, SIP_NOT_NONE);

  if (!ok) {
    return 0;
  }

  return 1;
}

itm1 = PyTuple_GetItem(sipPy, 0);
itm2 = PyTuple_GetItem(sipPy, 1);

int t1State = 0;
int t2State = 0;

T1 *t1 =
    static_cast<T1 *>(sipConvertToType(itm1, kpTypeDef1, NULL, SIP_NOT_NONE, &t1State, sipIsErr));
T2 *t2 =
    static_cast<T2 *>(sipConvertToType(itm2, kpTypeDef2, NULL, SIP_NOT_NONE, &t2State, sipIsErr));

if (*sipIsErr) {
  sipReleaseType(t1, kpTypeDef1, t1State);
  sipReleaseType(t2, kpTypeDef2, t2State);
  return 0;
}

sipTransferTo(itm1, itm1);
sipTransferTo(itm2, itm2);

std::pair<T1 *, T2 *> *p = new std::pair<T1 *, T2 *>(t1, t2);

sipReleaseType(t1, kpTypeDef1, t1State);
sipReleaseType(t2, kpTypeDef2, t2State);

*sipCppPtr = p;
return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject *pyTuple = PyTuple_New(2);

const sipTypeDef *kpTypeDef1 = getSipType("T1");

if (!kpTypeDef1) {
  return NULL;
}

const sipTypeDef *kpTypeDef2 = getSipType("T2");

if (!kpTypeDef2) {
  return NULL;
}

if (pyTuple == NULL) {
  return NULL;
}

PyObject *pyFirst = NULL;
PyObject *pySecond = NULL;

T1 *first = sipCpp->first;
T2 *second = sipCpp->second;

pyFirst = sipConvertFromType(first, kpTypeDef1, sipTransferObj);
pySecond = sipConvertFromType(second, kpTypeDef2, sipTransferObj);

if (pyFirst == NULL || pySecond == NULL || PyTuple_SetItem(pyTuple, 0, pyFirst)) {
  if (pyFirst)
    Py_XDECREF(pyFirst);
  if (pySecond)
    Py_XDECREF(pySecond);
  Py_XDECREF(pyTuple);

  return NULL;
}

if (PyTuple_SetItem(pyTuple, 1, pySecond)) {
  Py_XDECREF(pySecond);
  Py_XDECREF(pyTuple);

  return NULL;
}

return pyTuple;
%End
};

template<T1*, T2>
%MappedType std::pair<T1*, T2> {
%TypeHeaderCode
#include <utility>
%End

%ConvertToTypeCode
PyObject *itm1 = NULL;
PyObject *itm2 = NULL;

const sipTypeDef *kpTypeDef1 = getSipType("T1");

if (!kpTypeDef1) {
  return 0;
}

const sipTypeDef *kpTypeDef2 = getSipType("T2");

if (!kpTypeDef2) {
  return 0;
}

if (sipIsErr == NULL) {
  if (!(PyTuple_Check(sipPy) && PyTuple_Size(sipPy) == 2))
    return 0;

  itm1 = PyTuple_GetItem(sipPy, 0);
  itm2 = PyTuple_GetItem(sipPy, 1);

  bool ok = sipCanConvertToType(itm1, kpTypeDef1, SIP_NOT_NONE);
  ok = ok && sipCanConvertToType(itm2, kpTypeDef2, SIP_NOT_NONE);

  if (!ok) {
    return 0;
  }

  return 1;
}

itm1 = PyTuple_GetItem(sipPy, 0);
itm2 = PyTuple_GetItem(sipPy, 1);

int t1State = 0;
int t2State = 0;

T1 *t1 =
    static_cast<T1 *>(sipConvertToType(itm1, kpTypeDef1, NULL, SIP_NOT_NONE, &t1State, sipIsErr));
T2 *t2 = static_cast<T2 *>(
    sipConvertToType(itm2, kpTypeDef2, sipTransferObj, SIP_NOT_NONE, &t2State, sipIsErr));

if (*sipIsErr) {
  sipReleaseType(t1, kpTypeDef1, t1State);
  sipReleaseType(t2, kpTypeDef2, t2State);
  return 0;
}

sipTransferTo(itm1, itm1);

std::pair<T1 *, T2> *p = new std::pair<T1 *, T2>(t1, *t2);

sipReleaseType(t1, kpTypeDef1, t1State);
sipReleaseType(t2, kpTypeDef2, t2State);

*sipCppPtr = p;
return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject *pyTuple = PyTuple_New(2);

const sipTypeDef *kpTypeDef1 = getSipType("T1");

if (!kpTypeDef1) {
  return NULL;
}

const sipTypeDef *kpTypeDef2 = getSipType("T2");

if (!kpTypeDef2) {
  return NULL;
}

if (pyTuple == NULL) {
  return NULL;
}

PyObject *pyFirst = NULL;
PyObject *pySecond = NULL;

T1 *first = sipCpp->first;
T2 *second = new T2(sipCpp->second);

pyFirst = sipConvertFromType(first, kpTypeDef1, sipTransferObj);
pySecond = sipConvertFromNewType(second, kpTypeDef2, sipTransferObj);

if (pyFirst == NULL || pySecond == NULL || PyTuple_SetItem(pyTuple, 0, pyFirst)) {
  if (pyFirst)
    Py_XDECREF(pyFirst);
  if (pySecond)
    Py_XDECREF(pySecond);
  else
    delete second;
  Py_XDECREF(pyTuple);

  return NULL;
}

if (PyTuple_SetItem(pyTuple, 1, pySecond)) {
  Py_XDECREF(pySecond);
  Py_XDECREF(pyTuple);
  return NULL;
}

return pyTuple;
%End
};

template<T1, T2*>
%MappedType std::pair<T1, T2*> {
%TypeHeaderCode
#include <utility>
%End

%ConvertToTypeCode
PyObject *itm1 = NULL;
PyObject *itm2 = NULL;

const sipTypeDef *kpTypeDef1 = getSipType("T1");

if (!kpTypeDef1) {
  return 0;
}

const sipTypeDef *kpTypeDef2 = getSipType("T2");

if (!kpTypeDef2) {
  return 0;
}

if (sipIsErr == NULL) {
  if (!(PyTuple_Check(sipPy) && PyTuple_Size(sipPy) == 2))
    return 0;

  itm1 = PyTuple_GetItem(sipPy, 0);
  itm2 = PyTuple_GetItem(sipPy, 1);

  bool ok = sipCanConvertToType(itm1, kpTypeDef1, SIP_NOT_NONE);
  ok = ok && sipCanConvertToType(itm2, kpTypeDef2, SIP_NOT_NONE);

  if (!ok) {
    return 0;
  }

  return 1;
}

itm1 = PyTuple_GetItem(sipPy, 0);
itm2 = PyTuple_GetItem(sipPy, 1);

int t1State = 0;
int t2State = 0;

T1 *t1 = static_cast<T1 *>(
    sipConvertToType(itm1, kpTypeDef1, sipTransferObj, SIP_NOT_NONE, &t1State, sipIsErr));
T2 *t2 =
    static_cast<T2 *>(sipConvertToType(itm2, kpTypeDef2, NULL, SIP_NOT_NONE, &t2State, sipIsErr));

if (*sipIsErr) {
  sipReleaseType(t1, kpTypeDef1, t1State);
  sipReleaseType(t2, kpTypeDef2, t2State);
  return 0;
}

sipTransferTo(itm2, itm2);

std::pair<T1, T2 *> *p = new std::pair<T1, T2 *>(*t1, t2);

sipReleaseType(t1, kpTypeDef1, t1State);
sipReleaseType(t2, kpTypeDef2, t2State);

*sipCppPtr = p;
return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject *pyTuple = PyTuple_New(2);

const sipTypeDef *kpTypeDef1 = getSipType("T1");

if (!kpTypeDef1) {
  return NULL;
}

const sipTypeDef *kpTypeDef2 = getSipType("T2");

if (!kpTypeDef2) {
  return NULL;
}

if (pyTuple == NULL) {
  return NULL;
}

PyObject *pyFirst = NULL;
PyObject *pySecond = NULL;

T1 *first = new T1(sipCpp->first);
T2 *second = sipCpp->second;

pyFirst = sipConvertFromNewType(first, kpTypeDef1, sipTransferObj);
pySecond = sipConvertFromType(second, kpTypeDef2, sipTransferObj);

if (pyFirst == NULL || pySecond == NULL || PyTuple_SetItem(pyTuple, 0, pyFirst)) {
  if (pyFirst)
    Py_XDECREF(pyFirst);
  else
    delete first;
  if (pySecond)
    Py_XDECREF(pySecond);
  Py_XDECREF(pyTuple);

  return NULL;
}

if (PyTuple_SetItem(pyTuple, 1, pySecond)) {
  Py_XDECREF(pySecond);
  Py_XDECREF(pyTuple);

  return NULL;
}

return pyTuple;
%End
};

%MappedType std::pair<uint, uint> /TypeHint="Tuple[int, int]"/{
%TypeHeaderCode
#include <utility>
%End

%ConvertToTypeCode
PyObject *itm1 = NULL;
PyObject *itm2 = NULL;

if (sipIsErr == NULL) {
  if (!(PyTuple_Check(sipPy) && PyTuple_Size(sipPy) == 2))
    return 0;

  itm1 = PyTuple_GetItem(sipPy, 0);
  itm2 = PyTuple_GetItem(sipPy, 1);

  bool ok = PyLong_Check(itm1);
  ok = ok && PyLong_Check(itm2);

  if (!ok) {
    return 0;
  }

  return 1;
}

itm1 = PyTuple_GetItem(sipPy, 0);
itm2 = PyTuple_GetItem(sipPy, 1);

int t1State = 0;
int t2State = 0;

uint ui1 = 0;
uint ui2 = 0;

ui1 = PyLong_AsUnsignedLong(itm1);
ui2 = PyLong_AsUnsignedLong(itm2);

std::pair<uint, uint> *p = new std::pair<uint, uint>(ui1, ui2);

*sipCppPtr = p;
return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject *pyTuple = PyTuple_New(2);

if (pyTuple == NULL) {
  return 0;
}

PyObject *pyFirst = NULL;
PyObject *pySecond = NULL;

pyFirst = PyLong_FromUnsignedLong(sipCpp->first);
pySecond = PyLong_FromUnsignedLong(sipCpp->second);

if (pyFirst == NULL || pySecond == NULL || PyTuple_SetItem(pyTuple, 0, pyFirst)) {
  if (pyFirst)
    Py_XDECREF(pyFirst);

  if (pySecond)
    Py_XDECREF(pySecond);

  Py_XDECREF(pyTuple);

  return 0;
}

if (PyTuple_SetItem(pyTuple, 1, pySecond)) {
  Py_XDECREF(pySecond);
  Py_XDECREF(pyTuple);

  return 0;
}

return pyTuple;
%End
};

