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

#ifndef TALIPOT_PYTHON_IDE_H
#define TALIPOT_PYTHON_IDE_H

#include <QWidget>
#include <QDateTime>
#include <QMap>
#include <QUrl>

#include <talipot/config.h>

class QAction;
class QLabel;
class QProgressBar;
class QTabWidget;
class QToolBar;

namespace Ui {
class PythonIDE;
}

namespace tlp {

class Graph;
class GraphHierarchiesModel;
class Project;
class PythonCodeEditor;
class PythonInterpreter;
class PythonEditorsTabWidget;
class DataSet;
class TreeViewComboBox;

class TLP_PYTHON_SCOPE PythonIDE : public QWidget {

  Q_OBJECT
  Ui::PythonIDE *_ui;
  tlp::PythonInterpreter *_pythonInterpreter;
  bool _dontTreatFocusIn;
  tlp::Project *_project;
  tlp::GraphHierarchiesModel *_graphsModel;
  bool _scriptRunning;
  bool _scriptStopped;
  bool _saveFilesToProject;
  bool _notifyProjectModified;

  QMap<QString, QString> _editedPluginsClassName;
  QMap<QString, QString> _editedPluginsType;
  QMap<QString, QString> _editedPluginsName;

  QWidget *_scriptEditorsWidget, *_scriptControlWidget;
  QWidget *_pluginEditorsWidget, *_pluginControlWidget;
  QWidget *_moduleEditorsWidget, *_moduleControlWidget;
  QProgressBar *_progressBar;

  bool _anchored;

  QByteArray _splitterState;
  QWidget *_outputWidget;

  QToolBar *_scriptsTopToolBar, *_pluginsTopToolBar, *_modulesTopToolBar;
  QToolBar *_scriptsBottomToolBar, *_pluginsBottomToolBar, *_modulesBottomToolBar;
  TreeViewComboBox *_graphComboBox;
  QAction *_runScriptAction, *_pauseScriptAction, *_stopScriptAction, *_useUndoAction;
  QAction *_anchoredAction, *_anchoredAction_2, *_anchoredAction_3;
  QAction *_registerPluginAction, *_removePluginAction;
  QLabel *_pluginStatusLabel;

  bool loadPythonPlugin(const QString &fileName, bool clear = true);
  bool loadPythonPluginFromSrcCode(const QString &moduleName, const QString &pluginSrcCode,
                                   bool clear = true);
  void savePythonPlugin(int tabIdx, bool saveAs = false);
  bool indicateErrors() const;
  void clearErrorIndicators() const;
  bool loadModule(const QString &fileName);
  void saveModule(int tabIdx, bool saveAs = false);

  bool reloadAllModules() const;
  void createProjectPythonPaths();
  void writeScriptsFilesList(int deleted = -1);
  void writePluginsFilesList(int deleted = -1);
  void writeModulesFilesList(int deleted = -1);
  QString readProjectFile(const QString &filePath);
  void writeScriptFileToProject(int idx, const QString &scriptFileName,
                                const QString &scriptContent);
  void writeFileToProject(const QString &projectFile, const QString &fileContent);
  void deleteFilesFromProjectIfRemoved(const QString &projectDir,
                                       const QStringList &existingFilenames);

public:
  explicit PythonIDE(QWidget *parent = nullptr);
  ~PythonIDE() override;

  void setProject(tlp::Project *project);
  void savePythonFilesAndWriteToProject(bool notifyProjectModified = false);
  void setGraphsModel(tlp::GraphHierarchiesModel *model);
  void clearPythonCodeEditors();

  void setScriptEditorsVisible(bool visible);
  void setPluginEditorsVisible(bool visible);
  void setModuleEditorsVisible(bool visible);

  void setAnchoredCheckboxVisible(bool visible);
  void setAnchored(bool anchored);
  bool isAnchored() const;
  bool isScriptRunning();

protected:
  void dragEnterEvent(QDragEnterEvent *) override;
  void dropEvent(QDropEvent *) override;
  bool eventFilter(QObject *obj, QEvent *event) override;

private:
  int addMainScriptEditor(const QString &fileName = "");
  int addModuleEditor(const QString &fileName = "");
  int addPluginEditor(const QString &fileName = "");

  bool loadScript(const QString &fileName, bool clear = true);
  void saveScript(int tabIdx, bool clear = true, bool showFileDialog = false, bool saveAs = false);

  tlp::PythonCodeEditor *getCurrentMainScriptEditor() const;
  tlp::PythonCodeEditor *getMainScriptEditor(int idx) const;
  tlp::PythonCodeEditor *getModuleEditor(int idx) const;
  tlp::PythonCodeEditor *getCurrentModuleEditor() const;
  tlp::PythonCodeEditor *getPluginEditor(int idx) const;
  tlp::PythonCodeEditor *getCurrentPluginEditor() const;

  bool closeEditorTabRequested(PythonEditorsTabWidget *tabWidget, int idx);
  bool loadModuleFromSrcCode(const QString &moduleName, const QString &moduleSrcCode);

  void loadScriptsAndModulesFromPythonScriptViewDataSet(const DataSet &dataSet);

  template <typename Slot>
  QAction *addToolBarAction(QToolBar *toolBar, const QString &iconName,
                            const QKeySequence &shortcut, const QString &toolTip, Slot slot,
                            const QSize &iconSize = QSize(), const QColor &iconColor = Qt::white);

  QAction *addCommonBottomToolBarActions(QToolBar *toolBar);

signals:

  void anchoredRequest(bool anchored);

public slots:

  void executeCurrentScript();
  void stopCurrentScript();
  void pauseCurrentScript();

private slots:

  void newPythonPlugin();
  void currentTabChanged(int index);
  void loadPythonPlugin();
  void savePythonPlugin();
  void savePythonPluginAs();
  void saveAllPlugins();
  void registerPythonPlugin(bool clear = true);
  void removePythonPlugin();
  void newFileModule();
  void newStringModule();
  void loadModule();
  void saveModule();
  void saveModuleAs();
  void saveAllModules();
  void scrollToEditorLine(const QUrl &);
  void increaseFontSize();
  void decreaseFontSize();
  void scriptSaved(int);
  void pluginSaved(int);
  void moduleSaved(int);
  void graphComboBoxIndexChanged();

  void newScript();
  void loadScript();
  void saveScript();
  void saveScriptAs();
  void saveAllScripts();
  void currentScriptPaused();

  void closeModuleTabRequested(int index);
  void closeScriptTabRequested(int index);
  void closePluginTabRequested(int index);

  void anchored(bool anchored);

  tlp::Graph *getSelectedGraph() const;

  void useUndoToggled(bool useUndo);
};
}

#endif // TALIPOT_PYTHON_IDE_H
