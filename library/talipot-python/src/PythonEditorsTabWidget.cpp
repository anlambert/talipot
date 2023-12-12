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

#include <QFileInfo>

#include "talipot/PythonEditorsTabWidget.h"
#include "talipot/PythonCodeEditor.h"

using namespace tlp;

PythonEditorsTabWidget::PythonEditorsTabWidget(QWidget *parent)
    : QTabWidget(parent), _fontZoom(0), _dontTreatFocusIn(false) {
  connect(this, &QTabWidget::tabCloseRequested, this, &PythonEditorsTabWidget::closeTabRequested);
}

int PythonEditorsTabWidget::addEditor(const QString &fileName) {
  auto *codeEditor = new PythonCodeEditor();

  QFileInfo fileInfo(fileName);

  if (fileInfo.exists()) {
    codeEditor->loadCodeFromFile(fileName);
  }

  codeEditor->analyseScriptCode(true);
  codeEditor->setFocus(Qt::ActiveWindowFocusReason);
  codeEditor->installEventFilter(this);
  connect(codeEditor, &QPlainTextEdit::textChanged, this,
          &PythonEditorsTabWidget::scriptTextChanged);
  int idx = addTab(codeEditor, fileInfo.fileName());
  setTabToolTip(idx, fileInfo.exists() ? fileInfo.absoluteFilePath() : fileInfo.fileName());
  setCurrentIndex(idx);

  if (_fontZoom < 0) {
    for (int i = _fontZoom; i < 0; ++i) {
      codeEditor->zoomOut();
    }
  }

  if (_fontZoom > 0) {
    for (int i = _fontZoom; i > 0; --i) {
      codeEditor->zoomIn();
    }
  }

  return idx;
}

PythonCodeEditor *PythonEditorsTabWidget::getCurrentEditor() const {
  return getEditor(currentIndex());
}

PythonCodeEditor *PythonEditorsTabWidget::getEditor(int editorIdx) const {
  if (editorIdx >= 0 && editorIdx < count()) {
    return static_cast<PythonCodeEditor *>(widget(editorIdx));
  } else {
    return nullptr;
  }
}

void PythonEditorsTabWidget::scriptTextChanged() {

  if (QObject::sender() != widget(currentIndex())) {
    return;
  }

  QString curTabText = tabText(currentIndex());

  // workaround a Qt5 bug on linux
  curTabText = curTabText.replace("&", "");

  if (curTabText.isEmpty()) {
    return;
  }

  if (curTabText[curTabText.size() - 1] != '*') {
    curTabText += "*";
    setTabText(currentIndex(), curTabText);
  }
}

void PythonEditorsTabWidget::indicateErrors(const QMap<QString, QVector<int>> &errorLines) {
  for (int i = 0; i < count(); ++i) {
    QString moduleFile = getEditor(i)->getFileName();

    if (errorLines.contains(moduleFile)) {
      const QVector<int> &linesErrorNumbers = errorLines[moduleFile];
      PythonCodeEditor *codeEditor = getEditor(i);

      for (int linesErrorNumber : linesErrorNumbers) {
        codeEditor->indicateScriptCurrentError(linesErrorNumber - 1);
      }
    }
  }
}

void PythonEditorsTabWidget::clearErrorIndicators() {
  for (int i = 0; i < count(); ++i) {
    getEditor(i)->clearErrorIndicator();
  }
}

bool PythonEditorsTabWidget::eventFilter(QObject *obj, QEvent *event) {
#ifdef __APPLE__
  Qt::KeyboardModifiers modifier = Qt::MetaModifier;
#else
  Qt::KeyboardModifiers modifier = Qt::ControlModifier;
#endif

  if (event->type() == QEvent::KeyPress) {
    auto *keyEvt = static_cast<QKeyEvent *>(event);

    if (keyEvt->modifiers() == modifier && keyEvt->key() == Qt::Key_S) {
      if (obj == getCurrentEditor()) {
        QString moduleFile = tabText(currentIndex());

        // workaround a Qt5 bug on linux
        moduleFile = moduleFile.replace("&", "");

        if (!moduleFile.contains("no file")) {
          saveCurrentEditorContentToFile();
          return true;
        } else {
          // when there is no file associated to the Python module, its content will then be saved
          // in the project file (.tlpx) currently loaded in Talipot
          if (moduleFile[moduleFile.size() - 1] == '*') {
            moduleFile = moduleFile.mid(0, moduleFile.size() - 1);
          }

          setTabText(currentIndex(), moduleFile);
          emit fileSaved(currentIndex());
          return false;
        }
      }
    }
  } else if (event->type() == QEvent::FocusIn && !_dontTreatFocusIn) {
    _dontTreatFocusIn = true;
    reloadCodeInEditorsIfNeeded();
    _dontTreatFocusIn = false;
  }

  return false;
}

void PythonEditorsTabWidget::reloadCodeInEditorsIfNeeded() {
  bool emitSignal = false;

  for (int i = 0; i < count(); ++i) {
    emitSignal = reloadCodeInEditorIfNeeded(i) || emitSignal;
  }

  if (emitSignal) {
    emit filesReloaded();
  }
}

bool PythonEditorsTabWidget::reloadCodeInEditorIfNeeded(int index) {
  PythonCodeEditor *codeEditor = getEditor(index);
  QString fileName = codeEditor->getFileName();

  if (!fileName.isEmpty()) {
    QFileInfo fileInfo(fileName);

    if (fileInfo.exists() && fileInfo.lastModified() != codeEditor->getLastSavedTime()) {
      if (codeEditor->loadCodeFromFile(fileName)) {
        QString filename = tabText(index);
        if (filename.endsWith("*")) {
          setTabText(index, filename.mid(0, filename.size() - 1));
        }
      }

      return true;
    }
  }

  return false;
}

void PythonEditorsTabWidget::saveCurrentEditorContentToFile() {
  saveEditorContentToFile(currentIndex());
}

void PythonEditorsTabWidget::saveEditorContentToFile(int editorIdx) {
  if (editorIdx >= 0 && editorIdx < count()) {
    QString moduleNameExt = tabText(editorIdx);
    QString moduleName;

    if (!moduleNameExt.contains("no file")) {

      moduleName = moduleNameExt.mid(0, moduleNameExt.indexOf("."));

      // workaround a Qt5 bug on linux
      moduleName = moduleName.replace("&", "");

      setTabText(editorIdx, moduleName + ".py");
      QFile file(getEditor(editorIdx)->getFileName());
      QFileInfo fileInfo(file);

      if (getEditor(editorIdx)->saveCodeToFile()) {
        setTabToolTip(editorIdx, fileInfo.absoluteFilePath());
      }

      emit fileSaved(editorIdx);
    }
  }
}

void PythonEditorsTabWidget::closeTabRequested(int tab) {
  emit tabAboutToBeDeleted(tab);
}

void PythonEditorsTabWidget::closeTab(int tab) {
  QWidget *editorWidget = widget(tab);
  _dontTreatFocusIn = true;
  delete editorWidget;
  _dontTreatFocusIn = false;
}

void PythonEditorsTabWidget::decreaseFontSize() {
  for (int i = 0; i < count(); ++i) {
    getEditor(i)->zoomOut();
  }

  --_fontZoom;
}

void PythonEditorsTabWidget::increaseFontSize() {
  for (int i = 0; i < count(); ++i) {
    getEditor(i)->zoomIn();
  }

  ++_fontZoom;
}

QTabBar *PythonEditorsTabWidget::tabBar() const {
  return QTabWidget::tabBar();
}
