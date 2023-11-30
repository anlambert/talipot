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
#include "talipot/PythonInterpreter.h"

#include <talipot/TlpQtTools.h>
#include <talipot/Workspace.h>

#include <QMainWindow>

using namespace tlp;

static PyObject *talipotutils_updateVisualization(PyObject *, PyObject *args) {
  Workspace *workspace = NULL;
  QMainWindow *mainWindow = getMainWindow();
  int i;

  if (mainWindow) {
    workspace = mainWindow->findChild<Workspace *>();
  }

  if (!PyArg_ParseTuple(args, "|i", &i)) {
    Py_RETURN_NONE;
  }

  bool centerViews = i > 0;

  if (workspace) {
    workspace->redrawPanels(centerViews);
  }

  Py_RETURN_NONE;
}

static PyObject *talipotutils_pauseRunningScript(PyObject *, PyObject *) {
  PythonInterpreter::instance().pauseCurrentScript();
  Py_RETURN_NONE;
}

static PyObject *talipotutils_runGraphScript(PyObject *, PyObject *args) {
  char *s = NULL;
  PyObject *o = NULL;

  if (PyArg_ParseTuple(args, "sO", &s, &o)) {
    QString scriptName(s);
    scriptName.replace(".py", "");

    if (PythonInterpreter::instance().runString(QString("import ") + scriptName)) {

      // Getting proper sipWrapperType
      const sipTypeDef *kpTypeDef = sipFindType("tlp::Graph");

      // Checking if the Python object wraps a tlp::Graph instance
      if (sipCanConvertToType(o, kpTypeDef, SIP_NOT_NONE)) {
        int state = 0;
        int err = 0;

        // Unwrapping C++ instance
        auto *graph = static_cast<tlp::Graph *>(
            sipConvertToType(o, kpTypeDef, NULL, SIP_NOT_NONE, &state, &err));

        if (!PythonInterpreter::instance().runGraphScript(scriptName, "main", graph)) {
          PyErr_SetString(PyExc_RuntimeError,
                          (std::string("An exception occurred when executing the ") +
                           std::string(s) + " script")
                              .c_str());
          return NULL;
        }

      } else {
        PyErr_SetString(
            PyExc_TypeError,
            "Second parameter of the runGraphScript function must be of type tlp.Graph");
        return NULL;
      }
    } else {
      PyErr_SetString(PyExc_ValueError,
                      (std::string("The script ") + std::string(s) + " does not exist").c_str());
      return NULL;
    }
  } else {
    PyErr_SetString(PyExc_TypeError,
                    "Parameters provided to the runGraphScript function have invalid types");
    return NULL;
  }

  Py_RETURN_NONE;
}

static PyObject *talipotutils_setProcessQtEvents(PyObject *, PyObject *o) {
  int i;

  if (!PyArg_ParseTuple(o, "i", &i)) {
    return NULL;
  }

  PythonInterpreter::instance().setProcessQtEventsDuringScriptExecution(i > 0);

  Py_RETURN_NONE;
}

static PyMethodDef talipotUtilsMethods[] = {
    {"updateVisualization", talipotutils_updateVisualization, METH_VARARGS,
     "Update views on current graph."},
    {"pauseRunningScript", talipotutils_pauseRunningScript, METH_VARARGS,
     "Pause the execution of the current running script."},
    {"runGraphScript", talipotutils_runGraphScript, METH_VARARGS,
     "Allow to execute a script from a script."},
    {"setProcessQtEvents", talipotutils_setProcessQtEvents, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static struct PyModuleDef TalipotUtilsModuleDef = {
    PyModuleDef_HEAD_INIT,
    "talipotutils",      /* m_name */
    "",                  /* m_doc */
    -1,                  /* m_size */
    talipotUtilsMethods, /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
};

// This is called via the PyImport_AppendInittab mechanism called
// during interpreter initialization, to make the built-in talipotutils
// module known to Python
PyMODINIT_FUNC inittalipotutils(void) {
  return PyModule_Create(&TalipotUtilsModuleDef);
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
