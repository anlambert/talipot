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
#include <talipot/PluginLibraryLoader.h>
#include <talipot/PluginsManager.h>
#include <talipot/PluginLoaderTxt.h>
#include <talipot/TlpTools.h>

namespace tlp {
void setGlobalVar(const std::string &varName, const std::string &value);
std::string getGlobalVar(const std::string &varName);
}

extern tlp::DataSet getDefaultPluginParameters(const std::string &pluginName, tlp::Graph *graph = 0,
                                               bool transformFilesParameters = false);
%End

%ModuleCode
std::map<std::string, std::string *> talipotGlobalVars;

void initGlobalVars() {
  if (talipotGlobalVars.empty()) {
    talipotGlobalVars["TalipotLibDir"] = &tlp::TalipotLibDir;
    talipotGlobalVars["TalipotPluginsPath"] = &tlp::TalipotPluginsPath;
    talipotGlobalVars["TalipotBitmapDir"] = &tlp::TalipotBitmapDir;
    talipotGlobalVars["TalipotShareDir"] = &tlp::TalipotShareDir;
  }
}

namespace tlp {

void setGlobalVar(const std::string &varName, const std::string &value) {
  initGlobalVars();
  if (talipotGlobalVars.find(varName) != talipotGlobalVars.end()) {
    *talipotGlobalVars[varName] = value;
  }
}

std::string getGlobalVar(const std::string &varName) {
  initGlobalVars();
  if (talipotGlobalVars.find(varName) != talipotGlobalVars.end()) {
    return *talipotGlobalVars[varName];
  } else {
    return "";
  }
}

}

tlp::DataSet getDefaultPluginParameters(const std::string &pluginName, tlp::Graph *graph,
                                        bool transformFilesParameters) {
  tlp::DataSet result;
  const tlp::ParameterDescriptionList &parameters =
      tlp::PluginsManager::getPluginParameters(pluginName);
  parameters.buildDefaultDataSet(result, graph);
  if (transformFilesParameters) {
    std::vector<std::string> filesPrefixes;
    filesPrefixes.push_back("file::");
    filesPrefixes.push_back("anyfile::");
    filesPrefixes.push_back("dir::");
    for (const std::pair<std::string, tlp::DataType *> &entry :
         tlp::stableIterator(result.getValues())) {
      for (size_t i = 0; i < filesPrefixes.size(); ++i) {
        if (entry.first.length() > filesPrefixes[i].length() &&
            entry.first.substr(0, filesPrefixes[i].length()) == filesPrefixes[i]) {
          std::string paramName = entry.first.substr(filesPrefixes[i].length());
          result.set(paramName, *(static_cast<std::string *>(entry.second->value)));
          result.remove(entry.first);
        }
      }
    }
  }
  return result;
}
%End

namespace tlp {

std::string TalipotLibDir;
std::string TalipotPluginsPath;
std::string TalipotBitmapDir;
std::string TalipotShareDir;

void setGlobalVar(const std::string &varName, const std::string &value);
std::string getGlobalVar(const std::string &varName);

//=====================================================================

std::string getRelease();
%Docstring
tlp.getRelease()

Returns the current version number of Talipot in the form X.Y.Z, X being the major version, Y the
minor version, and Z the patch version.

:rtype:
   string
%End

%MethodCode
sipRes = new std::string(TALIPOT_VERSION);
%End

//=====================================================================

void initTalipotLib(const std::string &appDirPath = "");
%Docstring
tlp.initTalipotLib()

Looks for the Talipot plugins directory and fills the Talipot path variables : tlp.TalipotLibDir,
tlp.TalipotPluginsPath, tlp.TalipotBitmapDir. The plug-ins directory can be defined in different
ways, given by order of prevalence:

   -  the TLP_DIR environment variable, if it has a value

   -  the appDirPath parameter, if it is not NULL

   -  at that point, the Talipot paths will be retrieved from the path of the loaded Talipot shared
      library (you must dispose of a standard Talipot installation for that feature to work, meaning
      the relative path for the Talipot plugins folder to the Talipot libraries one must be
      ../lib/talipot/).

   -  a fallback value of 'C:/Talipot/lib/' on windows, or '/usr/local/lib/' on Unix.
%End

%MethodCode
const char *appDirPathStr = NULL;
if ((*a0) != "") {
  appDirPathStr = (*a0).c_str();
}
tlp::initTalipotLib(appDirPathStr);
%End

//=====================================================================

void loadPlugins(tlp::PluginLoader *plug=0);
%Docstring
tlp.loadPlugins()

Loads C++ plugins installed in the directories listed in the tlp.TalipotPluginsPath variable (in the
form "<path1>;<path2>" on windows and "<path1>:<path2>" on unix). The tlp.TalipotPluginsPath can be
initialized with standard Talipot directories by calling the :func:`tlp.initTalipotLib` function.
%End

%MethodCode
tlp::PluginLibraryLoader::loadPlugins(a0);
%End

//=====================================================================

void loadPluginsFromDir(const std::string &pluginsDir, tlp::PluginLoader *plug=0, bool loadPythonPlugins=true);
%Docstring
tlp.loadPluginsFromDir(pluginsDir)

Loads C++ and Python plugins installed in a specific directory.

The loading of plugins is done recursively, each subdirectory will be visited to search for plugins
to load.

:param pluginsDir:
   the root directory path to recursively search for python files (file extension: .py) or shared
   libraries containing the Talipot plugin (file extension : .so on linux, .dylib on mac, .dll on
   windows)

:type filename:
   string
%End

%MethodCode
tlp::PluginLibraryLoader::loadPluginsFromDir(*a0, a1);
if (a2) {
  // TODO : wrap the plugin loader C++ instance and pass it over to Python
  std::string importPythonPlugins = "talipot.tlp.loadTalipotPluginsFromDir(\"";
  importPythonPlugins += (*a0 + "\", False)");
  PyObject *pyRet = evalPythonStatement(importPythonPlugins);
  Py_XDECREF(pyRet);
}
%End

//=====================================================================

bool loadPlugin(const std::string &filename, tlp::PluginLoader *plug=0);
%Docstring
tlp.loadPlugin(filename)

Loads a C++ plugin or a Python plugin from a specific file (shared library or Python source file).
Returns :const:`True` if the plugin was correctly loaded.

:param filename:
   the path to the the python file (file extension: .py) or shared library containing the Talipot
   plugin (file extension : .so on linux, .dylib on mac, .dll on windows)

:type filename:
   string

:rtype:
   boolean
%End

%MethodCode
std::string ext = a0->substr(a0->length() - 3);
if (ext != ".py") {
  sipRes = tlp::PluginLibraryLoader::loadPluginLibrary(*a0, a1);
} else {
  // TODO : wrap the plugin loader C++ instance and pass it over to Python
  std::string importPythonPlugin = "talipot.tlp.loadTalipotPythonPlugin(\"";
  importPythonPlugin += (*a0 + "\")");
  PyObject *pyRet = evalPythonStatement(importPythonPlugin);
  sipRes = (pyRet == Py_True);
  Py_XDECREF(pyRet);
}
%End

//=====================================================================

void loadPluginsCheckDependencies(tlp::PluginLoader* loader=0);
%Docstring
tlp.loadPluginsCheckDependencies()

Checks the dependencies of all C++ plugins loaded so far. If a plugin does not have all its
dependencies fulfilled, it is unloaded and removed from the current plugins database.
%End

%MethodCode
tlp::PluginsManager::checkLoadedPluginsDependencies(a0);
%End

//=====================================================================

SIP_PYOBJECT getDefaultPluginParameters(const std::string &pluginName, tlp::Graph *graph = 0);
%Docstring
tlp.getDefaultPluginParameters(pluginName, graph=None)

Returns a Python dictionary with string keys filled with default parameters (if any) for a given
plugin (algorithm, import, property algorithm, ...).

:param pluginName:
   the name of the plugin on which to retrieve its default parameters

:type pluginName:
   string

:param graph:
   if provided, property parameters will be retrieved from the graph if they exist

:type graph:
   :class:`tlp.Graph`

:rtype:
   dictionary with string keys (parameters names) and parameters values
%End

%MethodCode
if (tlp::PluginsManager::pluginExists(*a0)) {
  tlp::DataSet params = ::getDefaultPluginParameters(*a0, a1, true);
  sipRes = convertTlpDataSetToPyDict(params);
} else {
  sipIsErr = 1;
  std::string msg = "No plugin named  ";
  msg += *a0;
  msg += ".";
  PyErr_SetString(PyExc_ValueError, msg.c_str());
}
%End

//=====================================================================

std::list<std::string> getAlgorithmPluginsList() /TypeHint="List[str]"/;
%Docstring
tlp.getAlgorithmPluginsList()

Returns a list containing the names of the algorithm plugins (written in C++ or Python) loaded so
far. Algorithm plugins are objects implementing the tlp::Algorithm interface in C++ or the
:class:`tlp.Algorithm` interface in Python.

:rtype:
   list of string
%End

%MethodCode
std::list<std::string> algoList = tlp::PluginsManager::availablePlugins<tlp::Algorithm>();
std::list<std::string> propAlgoList =
    tlp::PluginsManager::availablePlugins<tlp::PropertyAlgorithm>();
std::list<std::string> ret;
for (const auto &algo : algoList) {
  if (std::find(propAlgoList.begin(), propAlgoList.end(), algo) == propAlgoList.end()) {
    ret.push_back(algo);
  }
}
sipRes = new std::list<std::string>(ret);
%End

//=====================================================================

std::list<std::string> getImportPluginsList() /TypeHint="List[str]"/;
%Docstring
tlp.getImportPluginsList()

Returns a list containing the names of the import plugins (written in C++ or Python) loaded so far.
Import plugins are objects implementing the tlp::ImportModule interface in C++ or the
:class:`tlp.ImportModule` interface in Python.

:rtype:
   list of string
%End

%MethodCode
sipRes = new std::list<std::string>(tlp::PluginsManager::availablePlugins<tlp::ImportModule>());
%End

//=====================================================================

std::list<std::string> getExportPluginsList() /TypeHint="List[str]"/;
%Docstring
tlp.getExportPluginsList()

Returns a list containing the names of the export plugins (written in C++ or Python) loaded so far.
Export plugins are objects implementing the tlp::ExportModule interface in C++ or the
:class:`tlp.ExportModule` interface in Python.

:rtype:
   list of string
%End

%MethodCode
sipRes = new std::list<std::string>(tlp::PluginsManager::availablePlugins<tlp::ExportModule>());
%End

//=====================================================================

std::list<std::string> getBooleanAlgorithmPluginsList() /TypeHint="List[str]"/;
%Docstring
tlp.getBooleanAlgorithmPluginsList()

Returns a list containing the names of the boolean algorithm plugins (written in C++ or Python)
loaded so far. Boolean algorithm plugins are objects implementing the tlp::BooleanAlgorithm
interface in C++ or the :class:`tlp.BooleanAlgorithm` interface in Python.

:rtype:
   list of string
%End

%MethodCode
sipRes = new std::list<std::string>(tlp::PluginsManager::availablePlugins<tlp::BooleanAlgorithm>());
%End

//=====================================================================

std::list<std::string> getIntegerAlgorithmPluginsList() /TypeHint="List[str]"/;
%Docstring
tlp.getIntegerAlgorithmPluginsList()

Returns a list containing the names of the integer algorithm plugins (written in C++ or Python)
loaded so far. Integer algorithm plugins are objects implementing the tlp::IntegerAlgorithm
interface in C++ or the :class:`tlp.IntegerAlgorithm` interface in Python.

:rtype:
   list of string
%End

%MethodCode
sipRes = new std::list<std::string>(tlp::PluginsManager::availablePlugins<tlp::IntegerAlgorithm>());
%End

//=====================================================================

std::list<std::string> getDoubleAlgorithmPluginsList() /TypeHint="List[str]"/;
%Docstring
tlp.getDoubleAlgorithmPluginsList()

Returns a list containing the names of the double algorithm plugins (written in C++ or Python)
loaded so far. Double algorithm plugins are objects implementing the tlp::DoubleAlgorithm interface
in C++ or the :class:`tlp.DoubleAlgorithm` interface in Python.

:rtype:
   list of string
%End

%MethodCode
sipRes = new std::list<std::string>(tlp::PluginsManager::availablePlugins<tlp::DoubleAlgorithm>());
%End
//=====================================================================

std::list<std::string> getColorAlgorithmPluginsList() /TypeHint="List[str]"/;
%Docstring
tlp.getColorAlgorithmPluginsList()

Returns a list containing the names of the color algorithm plugins (written in C++ or Python) loaded
so far. Color algorithm plugins are objects implementing the tlp::ColorAlgorithm interface in C++ or
the :class:`tlp.ColorAlgorithm` interface in Python.

:rtype:
   list of string
%End

%MethodCode
sipRes = new std::list<std::string>(tlp::PluginsManager::availablePlugins<tlp::ColorAlgorithm>());
%End

//=====================================================================

std::list<std::string> getLayoutAlgorithmPluginsList() /TypeHint="List[str]"/;
%Docstring
tlp.getLayoutAlgorithmPluginsList()

Returns a list containing the names of the layout algorithm plugins (written in C++ or Python)
loaded so far. Layout algorithm plugins are objects implementing the tlp::LayoutAlgorithm interface
in C++ or the :class:`tlp.LayoutAlgorithm` interface in Python.

:rtype:
   list of string
%End

%MethodCode
sipRes = new std::list<std::string>(tlp::PluginsManager::availablePlugins<tlp::LayoutAlgorithm>());
%End

//=====================================================================

std::list<std::string> getSizeAlgorithmPluginsList() /TypeHint="List[str]"/;
%Docstring
tlp.getSizeAlgorithmPluginsList()

Returns a list containing the names of the size algorithm plugins (written in C++ or Python) loaded
so far. Size algorithm plugins are objects implementing the tlp::SizeAlgorithm interface in C++ or
the :class:`tlp.SizeAlgorithm` interface in Python.

:rtype:
   list of string
%End

%MethodCode
sipRes = new std::list<std::string>(tlp::PluginsManager::availablePlugins<tlp::SizeAlgorithm>());
%End

//=====================================================================

std::list<std::string> getStringAlgorithmPluginsList() /TypeHint="List[str]"/;
%Docstring
tlp.getStringAlgorithmPluginsList()

Returns a list containing the names of the string algorithm plugins (written in C++ or Python)
loaded so far. String algorithm plugins are objects implementing the tlp::StringAlgorithm interface
in C++ or the :class:`tlp.StringAlgorithm` interface in Python.

:rtype:
   list of string
%End

%MethodCode
sipRes = new std::list<std::string>(tlp::PluginsManager::availablePlugins<tlp::StringAlgorithm>());
%End

//=====================================================================

uint getSeedOfRandomSequence();
%Docstring
tlp.getSeedOfRandomSequence()

Returns the value of the seed used for further initialization of a random sequence

:rtype:
   integer
%End

//=====================================================================

void setSeedOfRandomSequence(uint seed);
%Docstring
tlp.setSeedOfRandomSequence(seed)

Gives the value of the seed used for further initialization of a random sequence (with further calls
to rand() or rand_r(...)). Set seed to UINT_MAX if you need a random choice of the seed.

:param seed:
   the value of the seed

:type seed:
   integer
%End

//=====================================================================

void initRandomSequence();
%Docstring
tlp.initRandomSequence()

Initializes a random sequence with the seed previously set. Further calls to rand() or rand_r(...)
will return the elements of that sequence
%End

};
