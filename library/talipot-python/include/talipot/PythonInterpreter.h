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

#ifndef TALIPOT_PYTHON_INTERPRETER_H
#define TALIPOT_PYTHON_INTERPRETER_H

#ifndef PyObject_HEAD
struct _object;
typedef _object PyObject;
#endif

#include <talipot/PythonCppTypesConverter.h>
#include <talipot/Graph.h>
#include <talipot/Singleton.h>
#include <talipot/TlpTools.h>

#include <QObject>
#include <QVector>
#include <QSet>
#include <QString>

class QAbstractScrollArea;
class QPlainTextEdit;
class QTextBrowser;

namespace tlp {

class PythonInterpreter;
DECLARE_DLL_TEMPLATE_INSTANCE(Singleton<PythonInterpreter>, TLP_PYTHON_TEMPLATE_DECLARE_SCOPE)

class TLP_PYTHON_SCOPE PythonInterpreter : public QObject, public Singleton<PythonInterpreter> {

  friend class Singleton<PythonInterpreter>;

  Q_OBJECT

  PythonInterpreter();

  void holdGIL() const;
  void releaseGIL() const;

  void setDefaultConsoleWidget(QAbstractScrollArea *consoleWidget);
  void setConsoleWidget(QAbstractScrollArea *consoleWidget);
  void setupVirtualEnv();

  bool _wasInit;
  bool _runningScript;
  QSet<QString> _currentImportPaths;
  QAbstractScrollArea *_defaultConsoleWidget;
  QString _pythonVersion;
  bool _outputEnabled;
  bool _errorOutputEnabled;

  template <typename T, typename... Param>
  void buildParamDataSet(DataSet *ds, T a, Param... param);
  template <typename T>
  void buildParamDataSet(DataSet *ds, T a);
  template <typename T>
  void addParameter(DataSet *ds, T a);

public:
  static const QString pythonPluginsPath;
  static const QString pythonPluginsPathHome;
  static const QString talipotVenvDirectory;
  static const char pythonReservedCharacters[];
  static const std::vector<QString> pythonAccentuatedCharacters;
  static const std::vector<QString> pythonAccentuatedCharactersReplace;
  static const char *pythonKeywords[];

  ~PythonInterpreter() override;

  bool interpreterInit();

  bool importModule(const QString &moduleName);

  bool registerNewModuleFromString(const QString &moduleName, const QString &moduleSrcCode);

  bool runString(const QString &pyhtonCode, const QString &scriptFilePath = "");

  bool runGraphScript(const QString &module, const QString &function, tlp::Graph *graph,
                      const QString &scriptFilePath = "");

  template <typename T>
  bool evalSingleStatementAndGetValue(const QString &pythonStatement, T &value);

  bool callFunction(const QString &module, const QString &function, const tlp::DataSet &parameters);

  template <typename RETURN_TYPE, typename... Param>
  bool callFunctionWithParamsAndGetReturnValue(const QString &module, const QString &function,
                                               RETURN_TYPE &returnValue, Param... param);
  template <typename... Param>
  bool callFunctionWithParams(const QString &module, const QString &function, Param... param);

  template <typename RETURN_TYPE>
  bool callFunctionAndGetReturnValue(const QString &module, const QString &function,
                                     const tlp::DataSet &parameters, RETURN_TYPE &returnValue);

  bool functionExists(const QString &moduleName, const QString &functionName);

  void addModuleSearchPath(const QString &path, const bool beforeOtherPaths = false);

  void deleteModule(const QString &moduleName);

  bool reloadModule(const QString &moduleName);

  void stopCurrentScript();

  void pauseCurrentScript(const bool pause = true);

  bool isScriptPaused() const;

  void setProcessQtEventsDuringScriptExecution(bool processQtEvents);

  bool isRunningScript() const {
    return _runningScript;
  }

  QString getPythonVersionStr() const {
    return _pythonVersion;
  }

  double getPythonVersion() const;

  QString getPythonShellBanner();

  void setDefaultSIGINTHandler();

  QVector<QString> getImportedModulesList();

  QVector<QString> getBaseTypesForType(const QString &type);

  QVector<QString> getGlobalDictEntries(const QString &prefixFilter = "");

  QVector<QString> getObjectDictEntries(const QString &objectName,
                                        const QString &prefixFilter = "");

  QString getVariableType(const QString &varName);

  void setDefaultConsoleWidget(QPlainTextEdit *consoleWidget);
  void setDefaultConsoleWidget(QTextBrowser *consoleWidget);

  void setConsoleWidget(QPlainTextEdit *consoleWidget);
  void setConsoleWidget(QTextBrowser *consoleWidget);

  void resetConsoleWidget();

  void initConsoleOutput();

  void loadTalipotPythonPluginsFromDir();

  QString getStandardOutput() const;

  QString getStandardErrorOutput() const;

  void clearOutputBuffers();

  void setOutputEnabled(const bool enableOutput);

  bool outputEnabled() const;

  void setErrorOutputEnabled(const bool enableOutput);

  bool errorOutputEnabled() const;

  void sendOutputToConsole(const QString &output, bool stdErr) const;

  QString readLineFromConsole();

  PyObject *callPythonFunction(const QString &module, const QString &function,
                               const tlp::DataSet &parameters);

  PyObject *evalPythonStatement(const QString &pythonStatement, bool singleInput = false);

  void clearTracebacks();

signals:

  void scriptExecutionPaused();
};

#include "PythonInterpreter.cxx"
}

#endif // TALIPOT_PYTHON_INTERPRETER_H
