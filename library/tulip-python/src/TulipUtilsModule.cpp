/**
 *
 * Copyright (C) 2019  The Talipot developers
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

#include "tulip/PythonIncludes.h"
#include "tulip/PythonInterpreter.h"

#include <tulip/PluginLister.h>
#include <tulip/Perspective.h>

using namespace tlp;

static PyObject *tuliputils_updateVisualization(PyObject *, PyObject *args) {
  tlp::Perspective *persp = tlp::Perspective::instance();
  int i;

  if (!PyArg_ParseTuple(args, "|i", &i))
    Py_RETURN_NONE;

  bool centerViews = i > 0;

  if (persp != nullptr) {
    persp->redrawPanels(centerViews);
  }

  Py_RETURN_NONE;
}

static PyObject *tuliputils_pauseRunningScript(PyObject *, PyObject *) {
  PythonInterpreter::getInstance()->pauseCurrentScript();
  Py_RETURN_NONE;
}

static PyObject *tuliputils_runGraphScript(PyObject *, PyObject *args) {
  char *s = nullptr;
  PyObject *o = nullptr;

  if (PyArg_ParseTuple(args, "sO", &s, &o)) {
    QString scriptName(s);
    scriptName.replace(".py", "");

    if (PythonInterpreter::getInstance()->runString(QString("import ") + scriptName)) {

      // Getting proper sipWrapperType
      const sipTypeDef *kpTypeDef = sipFindType("tlp::Graph");

      // Checking if the Python object wraps a tlp::Graph instance
      if (sipCanConvertToType(o, kpTypeDef, SIP_NOT_NONE)) {
        int state = 0;
        int err = 0;

        // Unwrapping C++ instance
        tlp::Graph *graph = static_cast<tlp::Graph *>(
            sipConvertToType(o, kpTypeDef, nullptr, SIP_NOT_NONE, &state, &err));

        if (!PythonInterpreter::getInstance()->runGraphScript(scriptName, "main", graph)) {
          PyErr_SetString(PyExc_Exception,
                          (std::string("An exception occurred when executing the ") +
                           std::string(s) + " script")
                              .c_str());
          return nullptr;
        }

      } else {
        PyErr_SetString(
            PyExc_TypeError,
            "Second parameter of the runGraphScript function must be of type tlp.Graph");
        return nullptr;
      }
    } else {
      PyErr_SetString(PyExc_Exception,
                      (std::string("The script ") + std::string(s) + " does not exist").c_str());
      return nullptr;
    }
  } else {
    PyErr_SetString(PyExc_TypeError,
                    "Parameters provided to the runGraphScript function have invalid types");
    return nullptr;
  }

  Py_RETURN_NONE;
}

static PyObject *tuliputils_removePlugin(PyObject *, PyObject *args) {

  char *buf;

  if (!PyArg_ParseTuple(args, "s", &buf))
    Py_RETURN_NONE;

  if (tlp::PluginLister::pluginExists(buf)) {
    tlp::PluginLister::removePlugin(buf);
  }

  Py_RETURN_NONE;
}

static PyObject *tuliputils_setProcessQtEvents(PyObject *, PyObject *o) {
  int i;

  if (!PyArg_ParseTuple(o, "i", &i))
    return nullptr;

  PythonInterpreter::getInstance()->setProcessQtEventsDuringScriptExecution(i > 0);

  Py_RETURN_NONE;
}

static PyMethodDef tulipUtilsMethods[] = {
    {"updateVisualization", tuliputils_updateVisualization, METH_VARARGS,
     "Update views on current graph."},
    {"pauseRunningScript", tuliputils_pauseRunningScript, METH_VARARGS,
     "Pause the execution of the current running script."},
    {"removePlugin", tuliputils_removePlugin, METH_VARARGS, ""},
    {"runGraphScript", tuliputils_runGraphScript, METH_VARARGS,
     "Allow to execute a script from a script."},
    {"setProcessQtEvents", tuliputils_setProcessQtEvents, METH_VARARGS, ""},
    {nullptr, nullptr, 0, nullptr} /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef tulipUtilsModuleDef = {
    PyModuleDef_HEAD_INIT,
    "tuliputils",      /* m_name */
    "",                /* m_doc */
    -1,                /* m_size */
    tulipUtilsMethods, /* m_methods */
    nullptr,           /* m_reload */
    nullptr,           /* m_traverse */
    nullptr,           /* m_clear */
    nullptr,           /* m_free */
};
#endif

// This is called via the PyImport_AppendInittab mechanism called
// during interpreter initialization, to make the built-in tuliputils
// module known to Python
PyMODINIT_FUNC inittuliputils(void) {
#if PY_MAJOR_VERSION >= 3
  return PyModule_Create(&tulipUtilsModuleDef);
#else
  Py_InitModule("tuliputils", tulipUtilsMethods);
#endif
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
