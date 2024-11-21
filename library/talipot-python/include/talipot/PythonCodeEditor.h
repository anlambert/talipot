/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_PYTHON_CODE_EDITOR_H
#define TALIPOT_PYTHON_CODE_EDITOR_H

#include <talipot/AutoCompletionDataBase.h>

#include <QDateTime>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QDialog>

#include <memory>
#include <tuple>

namespace Ui {
class FindReplaceDialog;
}

class PythonCodeHighlighter;
class ParenMatcherHighlighter;

class QMainWindow;

namespace tlp {

class PythonCodeEditor;
class LineNumberArea;

class TLP_PYTHON_SCOPE AutoCompletionList : public QListWidget {

  tlp::PythonCodeEditor *_codeEditor;
  bool _activated;
  bool _wasActivated;

public:
  explicit AutoCompletionList();

  void setCodeEditor(tlp::PythonCodeEditor *parent);

protected:
  void insertSelectedItem();
  void keyPressEvent(QKeyEvent *e) override;
  void showEvent(QShowEvent *event) override;
  void hideEvent(QHideEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  bool eventFilter(QObject *obj, QEvent *event) override;
};

class TLP_PYTHON_SCOPE FindReplaceDialog : public QDialog {

  Q_OBJECT

  Ui::FindReplaceDialog *_ui;
  QPlainTextEdit *_editor;
  QString _lastSearch;
  bool _resetSearch;

  void setSearchResult(const bool result);

public:
  FindReplaceDialog(QPlainTextEdit *editor);
  ~FindReplaceDialog() override;

  void setFindMode(const bool findMode);

  void setTextToFind(const QString &text);

public slots:

  void textToFindChanged();
  bool doFind();
  bool doReplace();
  void doReplaceFind();
  void doReplaceAll();
  void setResetSearch() {
    _resetSearch = true;
  }
  void regexpToggled(bool toggled);

protected:
  bool eventFilter(QObject *obj, QEvent *evt) override;
  void hideEvent(QHideEvent *event) override;
};

class TLP_PYTHON_SCOPE PythonCodeEditor : public QPlainTextEdit {

  Q_OBJECT

  friend class LineNumberArea;
  friend class AutoCompletionList;

public:
  explicit PythonCodeEditor(QWidget *parent = nullptr);
  ~PythonCodeEditor() override;

  QString getCleanCode() const;

  int lineNumberAreaWidth() const;

  qreal tabWidth() const;
  void setTabWidth(qreal width);

  int charWidth(char c) const;
  int textWidth(const QString &text) const;

  void indicateScriptCurrentError(int lineNumber);
  void clearErrorIndicator();

  void zoomIn();
  void zoomOut();

  std::tuple<int, int> getCursorPosition() const;
  void setCursorPosition(int line, int col);
  void scrollToLine(int line);

  std::tuple<int, int, int, int> getSelection() const;
  void setSelection(int startLine, int startCol, int endLine, int endCol);
  void removeSelectedText();
  bool hasSelectedText() const;
  QString selectedText() const;

  int lines() const;
  int lineLength(int lineNumber) const;

  void insertAt(QString text, int line, int col);

  void commentSelectedCode();
  void uncommentSelectedCode();
  bool selectedLinesCommented() const;

  void indentSelectedCode();
  void unindentSelectedCode();

  void setAutoIndentation(const bool autoIndent) {
    this->_autoIndent = autoIndent;
  }

  bool autoIndentation() const {
    return _autoIndent;
  }

  void setIndentationGuides(const bool indentGuides) {
    this->_indentGuides = indentGuides;
  }

  bool indentationGuides() const {
    return _indentGuides;
  }

  void setHighlightEditedLine(const bool highlightCurLine) {
    this->_highlightCurLine = highlightCurLine;
  }

  bool highlightEditedLine() const {
    return _highlightCurLine;
  }

  void setFindReplaceActivated(const bool activateFindReplace) {
    _findReplaceActivate = activateFindReplace;
  }

  bool findReplaceActivated() const {
    return _findReplaceActivate;
  }

  void setCommentShortcutsActivated(const bool activateCommentShortcuts) {
    _commentShortcutsActivate = activateCommentShortcuts;
  }

  bool commentShortcutsActivated() const {
    return _commentShortcutsActivate;
  }

  void setIndentShortcutsActivated(const bool activateIndentShortcuts) {
    _indentShortcutsActivate = activateIndentShortcuts;
  }

  bool indentShortcutsActivated() const {
    return _indentShortcutsActivate;
  }

  void setFileName(const QString &fileName) {
    _pythonFileName = fileName;
  }

  const QString &getFileName() const {
    return _pythonFileName;
  }

  bool loadCodeFromFile(const QString &filePath);

  bool saveCodeToFile();

  QDateTime getLastSavedTime() const {
    return _lastSavedTime;
  }

  void setModuleEditor(const bool moduleEditor) {
    this->_moduleEditor = moduleEditor;
  }

  void analyseScriptCode(const bool wholeText = false);

  AutoCompletionDataBase *getAutoCompletionDb() const {
    return _autoCompletionDb.get();
  }

  void setPlainText(const QString &text);

  QMainWindow *mainWindow() const {
    return _mainWindow;
  }

  void resetFindReplaceDialog();

  void guiThemeChanged();

protected:
  void resizeEvent(QResizeEvent *event) override;
  void showEvent(QShowEvent *) override;
  void paintEvent(QPaintEvent *event) override;
  void keyPressEvent(QKeyEvent *e) override;
  void wheelEvent(QWheelEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void lineNumberAreaPaintEvent(QPaintEvent *event);
  void insertFromMimeData(const QMimeData *source) override;

protected slots:

  void updateLineNumberAreaWidth();
  void updateLineNumberArea(const QRect &, int);
  void resetExtraSelections();
  void matchParens();
  virtual void highlightCurrentLine();
  void highlightErrors();
  virtual void showAutoCompletionList(bool dotContext = false);
  virtual void updateAutoCompletionList(bool dotContext = false);
  void highlightSelection();

protected:
  virtual void updateAutoCompletionListPosition();

  void createParenSelection(int pos);
  void updateTabWidth();
  void findIndentPattern(const QString &pythonCode);

  QString getEditedFunctionName() const;

  void showTooltip(int line, int col, const QString &text);
  void hideTooltip();
  bool isTooltipActive() const;

  QFontMetrics fontMetrics() const;

  QWidget *_lineNumberArea;
  PythonCodeHighlighter *_highlighter;
  ParenMatcherHighlighter *_parenHighlighter;
  QFont _currentFont;
  QVector<int> _currentErrorLines;

  static std::unique_ptr<AutoCompletionList> _autoCompletionList;
  static std::unique_ptr<AutoCompletionDataBase> _autoCompletionDb;

  FindReplaceDialog *_findReplaceDialog;

  bool _autoIndent;
  bool _indentGuides;
  bool _highlightCurLine;
  bool _tooltipActive;
  bool _findReplaceActivate;
  bool _commentShortcutsActivate;
  bool _indentShortcutsActivate;

  QPoint _toolTipPos;
  QString _toolTipText;
  QString _toolTipFunc;

  QString _pythonFileName;
  QDateTime _lastSavedTime;

  bool _shellWidget;
  bool _moduleEditor;

  QMainWindow *_mainWindow;

  QString _indentPattern;
};
}

#endif // TALIPOT_PYTHON_CODE_EDITOR_H
