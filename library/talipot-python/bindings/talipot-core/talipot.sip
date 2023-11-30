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

%Module(name=_talipot, keyword_arguments="Optional")

// we define the NO_LIMIT_DOC feature
// to deal with the limit for string constants length of Visual Studio
// and avoid the compiler error (fatal error C1091)
%Feature NO_LIMIT_DOC

%PreInitialisationCode
  // Try to load plugins when importing the module from the Python shell
  if (tlp::TalipotPluginsPath.empty()) {
    tlp::initTalipotLib();
    tlp::PluginLibraryLoader::loadPlugins();
  }
%End

%PostInitialisationCode
  // ensure that 'talipot' is in globals() to avoid exception when calling methods defined
  // in the talipot module (talipot/__init__.py) from a startup script(executed the first time the module is imported)
  PyRun_SimpleString("import sys;globals()['talipot'] = sys.modules['talipot']");
%End

%VirtualErrorHandler pass_through
// pass through virtual error handler used to not clear Python error indicator
// in some Python implementation of C++ virtual methods
%End


%ModuleHeaderCode
#include <talipot/Graph.h>
#include <talipot/PluginsManager.h>
#include <talipot/PropertyAlgorithm.h>
#include <talipot/PythonCppTypesConverter.h>
#include <talipot/Vector.h>
#include <talipot/Color.h>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>

inline tlp::PropertyInterface *copyValue(tlp::PropertyInterface *value) {
  return value;
}

inline tlp::Graph *copyValue(tlp::Graph *value) {
  return value;
}

inline std::string *copyValue(const std::string &value) {
  return new std::string(value);
}

inline const std::string &getValue(const std::string *value) {
  return *value;
}

inline tlp::node *copyValue(const tlp::node &value) {
  return new tlp::node(value);
}

inline tlp::edge *copyValue(const tlp::edge &value) {
  return new tlp::edge(value);
}

inline int copyValue(const int &value) {
  return value;
}

inline int getValue(const int &value) {
  return value;
}

inline double copyValue(double value) {
  return value;
}

inline double getValue(double value) {
  return value;
}

inline tlp::Coord *copyValue(const tlp::Coord &value) {
  return new tlp::Coord(value);
}

inline const tlp::Coord &getValue(const tlp::Coord *value) {
  return *value;
}

inline tlp::Size *copyValue(const tlp::Size &value) {
  return new tlp::Size(value);
}

inline const tlp::Size &getValue(const tlp::Size *value) {
  return *value;
}

inline tlp::Color *copyValue(const tlp::Color &value) {
  return new tlp::Color(value);
}

inline const tlp::Color &getValue(const tlp::Color *value) {
  return *value;
}

inline tlp::ParameterDescription *copyValue(const tlp::ParameterDescription &value) {
  return new tlp::ParameterDescription(value);
}

inline const tlp::ParameterDescription &getValue(const tlp::ParameterDescription *value) {
  return *value;
}

inline std::vector<tlp::Coord> *copyValue(const std::vector<tlp::Coord> &value) {
  return new std::vector<tlp::Coord>(value);
}

inline const std::vector<tlp::Coord> &getValue(const std::vector<tlp::Coord> *value) {
  return *value;
}

inline std::vector<tlp::Size> *copyValue(const std::vector<tlp::Size> &value) {
  return new std::vector<tlp::Size>(value);
}

inline const std::vector<tlp::Size> &getValue(const std::vector<tlp::Size> *value) {
  return *value;
}

template <typename T>
bool pluginExists(std::string pluginName) {
  if (typeid(T).name() == typeid(tlp::Algorithm).name()) {
    std::list<std::string> algoList = tlp::PluginsManager::availablePlugins<tlp::Algorithm>();
    std::list<std::string> propAlgoList =
        tlp::PluginsManager::availablePlugins<tlp::PropertyAlgorithm>();
    std::list<std::string> realAlgoList;
    for (const auto &algo : algoList) {
      if (std::find(propAlgoList.begin(), propAlgoList.end(), algo) == propAlgoList.end()) {
        realAlgoList.push_back(algo);
      }
    }
    return std::find(realAlgoList.begin(), realAlgoList.end(), pluginName) != realAlgoList.end();
  } else {
    return tlp::PluginsManager::pluginExists<T>(pluginName);
  }
}

template <typename PROP>
bool canGetProperty(const tlp::Graph *graph, const std::string &propName) {
  if (!graph->existProperty(propName)) {
    return true;
  }
  return dynamic_cast<PROP *>(graph->getProperty(propName)) != NULL;
}

extern int throwInvalidNodeException(const tlp::Graph *graph, tlp::node n);

extern int throwInvalidEdgeException(const tlp::Graph *graph, tlp::edge e);

extern int throwInvalidSgException(const tlp::Graph *graph, const tlp::Graph *sg);

extern int throwPropertyNameExistsException(const tlp::Graph *graph,
                                            const std::string &propertyName);

extern void printErrorMessage(const std::string &errMsg);

extern void enableErrorMessages(bool enable);

extern void releaseSIPWrapper(void *wrappedCppObject, const sipTypeDef *td);

extern void replaceAll(std::string &s, const std::string &replaceWhat,
                       const std::string &replaceWithWhat);

extern bool isValidGraphSelection(const tlp::Graph *graph, tlp::BooleanProperty *selection);

extern PyObject *evalPythonStatement(const std::string &pythonStatement);

extern bool isTupleAndCanConvertToVec3fType(PyObject *pyObj);

template <typename VEC_TYPE>
VEC_TYPE *convertToVec3fType(PyObject *pyObj, float zVal = 0) {
  float x = 0, y = 0, z = zVal;
  long xl = 0, yl = 0, zl = 0;
  double xd = 0, yd = 0, zd = 0;

  bool isList = PyList_Check(pyObj);

  PyObject *f = isList ? PyList_GET_ITEM(pyObj, 0) : PyTuple_GET_ITEM(pyObj, 0);
  if (convertPyObjectToLong(f, xl)) {
    x = float(xl);
  } else if (convertPyObjectToDouble(f, xd)) {
    x = float(xd);
  }

  f = isList ? PyList_GET_ITEM(pyObj, 1) : PyTuple_GET_ITEM(pyObj, 1);
  if (convertPyObjectToLong(f, yl)) {
    y = float(yl);
  } else if (convertPyObjectToDouble(f, yd)) {
    y = float(yd);
  }

  if ((isList && PyList_GET_SIZE(pyObj) > 2) || PyTuple_GET_SIZE(pyObj) > 2) {
    f = isList ? PyList_GET_ITEM(pyObj, 2) : PyTuple_GET_ITEM(pyObj, 2);
    if (convertPyObjectToLong(f, zl)) {
      z = float(zl);
    } else if (convertPyObjectToDouble(f, zd)) {
      z = float(zd);
    }
  }

  return new VEC_TYPE(x, y, z);
}
%End


%ModuleCode

int throwInvalidNodeException(const tlp::Graph *graph, tlp::node n) {
  std::ostringstream oss;
  oss << "Node with id " << n.id << " does not belong to graph \"" << graph->getName() << "\" (id "
      << graph->getId() << ")";
  PyErr_SetString(PyExc_IndexError, oss.str().c_str());
  return -1;
}

int throwInvalidEdgeException(const tlp::Graph *graph, tlp::edge e) {
  std::ostringstream oss;
  oss << "Edge with id " << e.id << " does not belong to graph \"" << graph->getName() << "\" (id "
      << graph->getId() << ")";
  PyErr_SetString(PyExc_IndexError, oss.str().c_str());
  return -1;
}

int throwInvalidSgException(const tlp::Graph *graph, const tlp::Graph *sg) {
  std::ostringstream oss;
  oss << "Graph \"" << sg->getName() << "\" (id " << sg->getId()
      << ") is not a subgraph of graph \"" << graph->getName() << "\" (id " << graph->getId()
      << ")";
  PyErr_SetString(PyExc_ValueError, oss.str().c_str());
  return -1;
}

int throwPropertyNameExistsException(const tlp::Graph *graph, const std::string &propertyName) {
  tlp::PropertyInterface *prop = graph->getProperty(propertyName);
  std::ostringstream oss;
  oss << "A property named \"" << propertyName << "\" of type " << prop->getTypename()
      << " already exists in graph \"" << graph->getName() << "\" (id " << graph->getId() << ")";
  PyErr_SetString(PyExc_ValueError, oss.str().c_str());
  return -1;
}

static bool errorMessagesEnabled = true;

void enableErrorMessages(bool enable) {
  errorMessagesEnabled = enable;
}

void printErrorMessage(const std::string &errMsg) {
  if (errorMessagesEnabled) {
    std::string pythonCode = "import sys\n"
                             "sys.stderr.write(\"";
    pythonCode += errMsg;
    pythonCode += "\\n\")";
    PyRun_SimpleString(pythonCode.c_str());
  }
}

void releaseSIPWrapper(void *wrappedCppObject, const sipTypeDef *td) {
  PyObject *pyObj = sipGetPyObject(wrappedCppObject, td);
  if (pyObj) {
    sipTransferTo(pyObj, pyObj);
    sipSimpleWrapper *wrapper = reinterpret_cast<sipSimpleWrapper *>(pyObj);
    sipInstanceDestroyed(wrapper);
  }
}

void replaceAll(std::string &s, const std::string &replaceWhat,
                const std::string &replaceWithWhat) {
  while (1) {
    const size_t pos = s.find(replaceWhat);
    if (pos == std::string::npos)
      break;
    s.replace(pos, replaceWhat.size(), replaceWithWhat);
  }
}

bool isValidGraphSelection(const tlp::Graph *graph, tlp::BooleanProperty *selection) {
  for (const tlp::edge &e : graph->edges()) {
    if (selection->getEdgeValue(e)) {
      if (!selection->getNodeValue(graph->source(e)) ||
          !selection->getNodeValue(graph->target(e))) {
        return false;
      }
    }
  }
  return true;
}

PyObject *evalPythonStatement(const std::string &pythonStatement) {
  PyObject *pName = PyUnicode_FromString("__main__");

  PyObject *pMainModule = PyImport_Import(pName);
  Py_XDECREF(pName);
  PyObject *pMainDict = PyModule_GetDict(pMainModule);

  PyObject *ret = PyRun_String(pythonStatement.c_str(), Py_eval_input, pMainDict, pMainDict);

  if (PyErr_Occurred()) {
    PyErr_Print();
    PyErr_Clear();
  }

  return ret;
}

bool isTupleAndCanConvertToVec3fType(PyObject *pyObj) {
  if (!PyTuple_Check(pyObj) || PyTuple_GET_SIZE(pyObj) < 2 || PyTuple_GET_SIZE(pyObj) > 3) {
    return false;
  }

  for (Py_ssize_t i = 0; i < PyTuple_GET_SIZE(pyObj); ++i) {
    PyObject *item = PyTuple_GET_ITEM(pyObj, i);
    bool canConvert = false;
    if (!PyBool_Check(item) && PyLong_Check(item)) {
      canConvert = canConvert || true;
    }
    if (!PyBool_Check(item) && PyFloat_Check(item)) {
      canConvert = canConvert || true;
    }
    if (!canConvert) {
      return false;
    }
  }

  return true;
}
%End

typedef unsigned int uint;

%Include stl.sip

%Include Iterator.sip
%Include MapIterator.sip
%Include Node.sip
%Include Edge.sip
%Include Vector.sip
%Include Color.sip
%Include Coord.sip
%Include Size.sip
%Include Matrix.sip
%Include BoundingBox.sip
%Include StringCollection.sip
%Include ColorScale.sip
%Include DataSet.sip

%Include PluginContext.sip
%Include Plugin.sip

%Include Algorithm.sip
%Include PropertyAlgorithm.sip

%Include ImportModule.sip
%Include ExportModule.sip

%Include PropertyInterface.sip

%Include BooleanProperty.sip
%Include DoubleProperty.sip
%Include ColorProperty.sip
%Include StringProperty.sip
%Include SizeProperty.sip
%Include IntegerProperty.sip
%Include LayoutProperty.sip
%Include NumericProperty.sip
%Include PropertyProxy.sip

%Include GraphProperty.sip

%Include WithParameter.sip
%Include WithDependency.sip
%Include PluginProgress.sip
%Include Graph.sip
%Include GraphMeasure.sip
%Include GraphTools.sip
%Include PluginLoader.sip
%Include PluginLoaderTxt.sip

%Include TlpTools.sip
%Include AcyclicTest.sip
%Include ConnectedTest.sip
%Include SimpleTest.sip
%Include TreeTest.sip
%Include OuterPlanarTest.sip
%Include BiconnectedTest.sip
%Include TriconnectedTest.sip
%Include PlanarityTest.sip

%Include DrawingTools.sip
%Include Observable.sip
%Include GraphEvent.sip
%Include PropertyEvent.sip
%Include ViewSettings.sip
%Include Delaunay.sip
%Include ConvexHull.sip
%Include PluginsManager.sip
%Include ParametricCurves.sip

%Include FontAwesome.sip
%Include MaterialDesignIcons.sip

// force generation of wrappers for specialized template classes
typedef std::vector<tlp::Graph*> vectorGraphType;
typedef std::list<tlp::Graph*> listGraphType;
typedef std::set<tlp::Graph*> setGraphType;

typedef std::vector<tlp::node> vectorNodeType;
typedef std::list<tlp::node> listNodeType;
typedef std::set<tlp::node> setNodeType;

typedef std::vector<tlp::edge> vectorEdgeType;
typedef std::list<tlp::edge> listEdgeType;
typedef std::set<tlp::edge> setEdgeType;

typedef std::vector<tlp::Color> vectorColorType;
typedef std::list<tlp::Color> listColorType;
typedef std::set<tlp::Color> setColorType;

typedef std::vector<tlp::Coord> vectorCoordType;
typedef std::list<tlp::Coord> listCoordType;
typedef std::set<tlp::Coord> setCoordType;

typedef std::vector<tlp::Size> vectorSizeType;
typedef std::list<tlp::Size> listSizeType;
typedef std::set<tlp::Size> setSizeType;

typedef std::vector<tlp::DataSet> vectorDataSetType;
typedef std::list<tlp::DataSet> listDataSetType;

typedef std::vector<tlp::ColorScale> vectorColorScaleType;
typedef std::list<tlp::ColorScale> listColorScaleType;

typedef std::vector<tlp::StringCollection> vectorStringCollectionType;
typedef std::list<tlp::StringCollection> listStringCollectionType;

typedef std::vector<tlp::BooleanProperty*> vectorBooleanPropertyType;
typedef std::list<tlp::BooleanProperty*> listBooleanPropertyType;

typedef std::vector<tlp::ColorProperty*> vectorColorPropertyType;
typedef std::list<tlp::ColorProperty*> listColorPropertyType;

typedef std::vector<tlp::DoubleProperty*> vectorDoublePropertyType;
typedef std::list<tlp::DoubleProperty*> listDoublePropertyType;

typedef std::vector<tlp::IntegerProperty*> vectorIntegerPropertyType;
typedef std::list<tlp::IntegerProperty*> listIntegerPropertyType;

typedef std::vector<tlp::LayoutProperty*> vectorLayoutPropertyType;
typedef std::list<tlp::LayoutProperty*> listLayoutPropertyType;

typedef std::vector<tlp::SizeProperty*> vectorSizePropertyType;
typedef std::list<tlp::SizeProperty*> listSizePropertyType;

typedef std::vector<tlp::StringProperty*> vectorStringPropertyType;
typedef std::list<tlp::StringProperty*> listStringPropertyType;

typedef std::vector<tlp::PropertyInterface*> vectorPropertyInterfaceType;
typedef std::list<tlp::PropertyInterface*> listPropertyInterfaceType;
