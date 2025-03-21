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

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#include "talipot/PythonIncludes.h"
#include "talipot/PythonInterpreter.h"

using namespace tlp;

QString consoleOuputString = "";
QString consoleErrorOuputString = "";
QString mainScriptFileName = "";

QString currentConcatOutput = "";

typedef struct {
  PyObject_HEAD bool stderrflag;
  bool writeToConsole;
} consoleutils_ConsoleOutput;

static void consoleutils_ConsoleOutput_dealloc(consoleutils_ConsoleOutput *self) {
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject *>(self));
}

static PyObject *consoleutils_ConsoleOutput_new(PyTypeObject *type, PyObject *, PyObject *) {
  consoleutils_ConsoleOutput *self;
  self = reinterpret_cast<consoleutils_ConsoleOutput *>(type->tp_alloc(type, 0));
  self->stderrflag = false;
  self->writeToConsole = true;
  return reinterpret_cast<PyObject *>(self);
}

static int consoleutils_ConsoleOutput_init(consoleutils_ConsoleOutput *self, PyObject *args,
                                           PyObject *) {
  int i;

  if (!PyArg_ParseTuple(args, "|i", &i)) {
    return -1;
  }

  self->stderrflag = i > 0;
  self->writeToConsole = true;
  return 0;
}

/* This redirects stdout from the calling script. */
static PyObject *consoleutils_ConsoleOutput_write(PyObject *self, PyObject *o) {

  char *buf;

  if (!PyArg_ParseTuple(o, "s", &buf)) {
    return NULL;
  }

  QString output(QString::fromUtf8(buf));

  bool stdErr = reinterpret_cast<consoleutils_ConsoleOutput *>(self)->stderrflag;

  if (stdErr) {
    if (!mainScriptFileName.isEmpty()) {
      output.replace("<string>", mainScriptFileName);
    }

    consoleErrorOuputString += output;
  } else {
    consoleOuputString += output;
  }

  if ((PythonInterpreter::instance().outputEnabled() && !stdErr) ||
      (PythonInterpreter::instance().errorOutputEnabled() && stdErr)) {

    if (buf != NULL && reinterpret_cast<consoleutils_ConsoleOutput *>(self)->writeToConsole) {

      currentConcatOutput += output;

      QStringList lines = currentConcatOutput.split('\n');

      for (int i = 0; i < lines.count() - 1; ++i) {
        PythonInterpreter::instance().sendOutputToConsole(lines[i], stdErr);
      }

      currentConcatOutput = lines[lines.size() - 1];
    }
  }

  Py_RETURN_NONE;
}

static PyObject *consoleutils_ConsoleOutput_enableConsoleOutput(PyObject *self, PyObject *o) {
  int i;

  if (!PyArg_ParseTuple(o, "i", &i)) {
    return NULL;
  }

  reinterpret_cast<consoleutils_ConsoleOutput *>(self)->writeToConsole = i > 0;

  Py_RETURN_NONE;
}

static PyObject *consoleutils_ConsoleOutput_flush(PyObject *self, PyObject *) {
  if (!currentConcatOutput.isEmpty()) {
    PythonInterpreter::instance().sendOutputToConsole(
        currentConcatOutput, reinterpret_cast<consoleutils_ConsoleOutput *>(self)->stderrflag);
    currentConcatOutput = "";
  }

  Py_RETURN_NONE;
}

static PyObject *consoleutils_ConsoleOutput_close(PyObject *, PyObject *) {
  Py_RETURN_NONE;
}

// T_BOOL is not defined for older versions of Python (2.5 for instance)
// define it as T_INT in that case
#ifndef T_BOOL
#define T_BOOL T_INT
#endif

static PyMemberDef consoleutils_ConsoleOutput_members[] = {
    {const_cast<char *>("stderrflag"), T_BOOL, offsetof(consoleutils_ConsoleOutput, stderrflag), 0,
     const_cast<char *>("flag for stderr")},
    {const_cast<char *>("writeToConsole"), T_BOOL,
     offsetof(consoleutils_ConsoleOutput, writeToConsole), 0,
     const_cast<char *>("flag for enabling/disabling console output")},
    {NULL, 0, 0, 0, NULL} /* Sentinel */
};

static PyMethodDef consoleutils_ConsoleOutput_methods[] = {
    {"write", static_cast<PyCFunction>(consoleutils_ConsoleOutput_write), METH_VARARGS,
     "Post output to the scripting engine"},
    {"enableConsoleOutput",
     static_cast<PyCFunction>(consoleutils_ConsoleOutput_enableConsoleOutput), METH_VARARGS,
     "enable / disable console output"},
    {"flush", static_cast<PyCFunction>(consoleutils_ConsoleOutput_flush), METH_VARARGS, ""},
    {"close", static_cast<PyCFunction>(consoleutils_ConsoleOutput_close), METH_VARARGS, ""},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static PyTypeObject consoleutils_ConsoleOutputType = {
    PyVarObject_HEAD_INIT(NULL, 0)

        "consoleutils.ConsoleOutput",                                 /*tp_name*/
    sizeof(consoleutils_ConsoleOutput),                               /*tp_basicsize*/
    0,                                                                /*tp_itemsize*/
    reinterpret_cast<destructor>(consoleutils_ConsoleOutput_dealloc), /*tp_dealloc*/
    0,                                                                /*tp_print*/
    0,                                                                /*tp_getattr*/
    0,                                                                /*tp_setattr*/
    0,                                                                /*tp_compare*/
    0,                                                                /*tp_repr*/
    0,                                                                /*tp_as_number*/
    0,                                                                /*tp_as_sequence*/
    0,                                                                /*tp_as_mapping*/
    0,                                                                /*tp_hash */
    0,                                                                /*tp_call*/
    0,                                                                /*tp_str*/
    0,                                                                /*tp_getattro*/
    0,                                                                /*tp_setattro*/
    0,                                                                /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                         /*tp_flags*/
    "",                                                               /* tp_doc */
    0,                                                                /* tp_traverse */
    0,                                                                /* tp_clear */
    0,                                                                /* tp_richcompare */
    0,                                                                /* tp_weaklistoffset */
    0,                                                                /* tp_iter */
    0,                                                                /* tp_iternext */
    consoleutils_ConsoleOutput_methods,                               /* tp_methods */
    consoleutils_ConsoleOutput_members,                               /* tp_members */
    0,                                                                /* tp_getset */
    0,                                                                /* tp_base */
    0,                                                                /* tp_dict */
    0,                                                                /* tp_descr_get */
    0,                                                                /* tp_descr_set */
    0,                                                                /* tp_dictoffset */
    reinterpret_cast<initproc>(consoleutils_ConsoleOutput_init),      /* tp_init */
    0,                                                                /* tp_alloc */
    consoleutils_ConsoleOutput_new,                                   /* tp_new */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
#if PY_VERSION_HEX >= 0x03040000
    ,
    0
#endif
#if PY_VERSION_HEX >= 0x03080000
    ,
    0
#endif
#if PY_VERSION_HEX >= 0x03080000 && PY_VERSION_HEX < 0x03090000
    ,
    0
#endif
#if PY_VERSION_HEX >= 0x030C00A1
    ,
    0
#endif
#if PY_VERSION_HEX >= 0x030D0000
    ,
    0
#endif

};

typedef struct {
  PyObject_HEAD
} consoleutils_ConsoleInput;

static void consoleutils_ConsoleInput_dealloc(consoleutils_ConsoleInput *self) {
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject *>(self));
}

static PyObject *consoleutils_ConsoleInput_new(PyTypeObject *type, PyObject *, PyObject *) {
  return type->tp_alloc(type, 0);
}

static int consoleutils_ConsoleInput_init(consoleutils_ConsoleInput *, PyObject *, PyObject *) {
  return 0;
}

/* This redirects stdin from the calling script. */
static PyObject *consoleutils_ConsoleInput_readline(PyObject *, PyObject *) {
  QString line = PythonInterpreter::instance().readLineFromConsole();
  PyObject *obj = PyUnicode_FromString(line.toLatin1().data());
  return obj;
}

static PyMemberDef consoleutils_ConsoleInput_members[] = {
    {NULL, 0, 0, 0, NULL} /* Sentinel */
};

static PyMethodDef consoleutils_ConsoleInput_methods[] = {
    {"readline", static_cast<PyCFunction>(consoleutils_ConsoleInput_readline), METH_VARARGS,
     "read an input line from the console"},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static PyTypeObject consoleutils_ConsoleInputType = {
    PyVarObject_HEAD_INIT(NULL, 0)

        "consoleutils.ConsoleInput",                                 /*tp_name*/
    sizeof(consoleutils_ConsoleInput),                               /*tp_basicsize*/
    0,                                                               /*tp_itemsize*/
    reinterpret_cast<destructor>(consoleutils_ConsoleInput_dealloc), /*tp_dealloc*/
    0,                                                               /*tp_print*/
    0,                                                               /*tp_getattr*/
    0,                                                               /*tp_setattr*/
    0,                                                               /*tp_compare*/
    0,                                                               /*tp_repr*/
    0,                                                               /*tp_as_number*/
    0,                                                               /*tp_as_sequence*/
    0,                                                               /*tp_as_mapping*/
    0,                                                               /*tp_hash */
    0,                                                               /*tp_call*/
    0,                                                               /*tp_str*/
    0,                                                               /*tp_getattro*/
    0,                                                               /*tp_setattro*/
    0,                                                               /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                        /*tp_flags*/
    "",                                                              /* tp_doc */
    0,                                                               /* tp_traverse */
    0,                                                               /* tp_clear */
    0,                                                               /* tp_richcompare */
    0,                                                               /* tp_weaklistoffset */
    0,                                                               /* tp_iter */
    0,                                                               /* tp_iternext */
    consoleutils_ConsoleInput_methods,                               /* tp_methods */
    consoleutils_ConsoleInput_members,                               /* tp_members */
    0,                                                               /* tp_getset */
    0,                                                               /* tp_base */
    0,                                                               /* tp_dict */
    0,                                                               /* tp_descr_get */
    0,                                                               /* tp_descr_set */
    0,                                                               /* tp_dictoffset */
    reinterpret_cast<initproc>(consoleutils_ConsoleInput_init),      /* tp_init */
    0,                                                               /* tp_alloc */
    consoleutils_ConsoleInput_new,                                   /* tp_new */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
#if PY_VERSION_HEX >= 0x03040000
    ,
    0
#endif
#if PY_VERSION_HEX >= 0x03080000
    ,
    0
#endif
#if PY_VERSION_HEX >= 0x03080000 && PY_VERSION_HEX < 0x03090000
    ,
    0
#endif
#if PY_VERSION_HEX >= 0x030C00A1
    ,
    0
#endif
#if PY_VERSION_HEX >= 0x030D0000
    ,
    0
#endif
};

static struct PyModuleDef consoleutilsModuleDef = {
    PyModuleDef_HEAD_INIT,
    "consoleutils", /* m_name */
    "",             /* m_doc */
    -1,             /* m_size */
    NULL,           /* m_methods */
    NULL,           /* m_reload */
    NULL,           /* m_traverse */
    NULL,           /* m_clear */
    NULL,           /* m_free */
};

// This is called via the PyImport_AppendInittab mechanism called
// during interpreter initialization, to make the built-in consoleutils
// module known to Python
PyMODINIT_FUNC initconsoleutils(void) {
  PyObject *m;

  consoleutils_ConsoleOutputType.tp_new = PyType_GenericNew;
  consoleutils_ConsoleInputType.tp_new = PyType_GenericNew;

  PyType_Ready(&consoleutils_ConsoleOutputType);
  PyType_Ready(&consoleutils_ConsoleInputType);

  m = PyModule_Create(&consoleutilsModuleDef);

  auto *cot = reinterpret_cast<PyObject *>(&consoleutils_ConsoleOutputType);
  Py_INCREF(cot);
  PyModule_AddObject(m, "ConsoleOutput", cot);
  auto *cit = reinterpret_cast<PyObject *>(&consoleutils_ConsoleInputType);
  Py_INCREF(cit);
  PyModule_AddObject(m, "ConsoleInput", cit);

  return m;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
