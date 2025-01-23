/**
 *
 * Copyright (C) 2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "talipot/PythonInterpreter.h"

#include <talipot/TlpQtTools.h>

#include "talipot/PythonPipWidget.h"
#include "talipot/PythonCodeHighlighter.h"

using namespace tlp;

static QString pipCommandPrefix = "$ pip ";

static QString rtrim(const QString &s) {
  int lastNonSpaceIdx = s.length() - 1;

  while (s.at(lastNonSpaceIdx).isSpace()) {
    --lastNonSpaceIdx;
  }

  return s.mid(0, lastNonSpaceIdx + 1);
}

PythonPipWidget::PythonPipWidget(QWidget *parent) : PythonCodeEditor(parent) {
  setAutoIndentation(false);
  setIndentationGuides(false);
  setHighlightEditedLine(false);
  setFindReplaceActivated(false);
  setCommentShortcutsActivated(false);
  setIndentShortcutsActivated(false);
  setLineNumbersVisible(false);
  executePipCommand("-V");
  _highlighter->setDocument(nullptr);
  setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
}

bool PythonPipWidget::isCursorOnLastLine() {
  int line = textCursor().blockNumber() + 1;
  return line == document()->blockCount();
}

void PythonPipWidget::insert(const QString &txt, const bool atEnd) {
  if (atEnd) {
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
  }

  QTextCharFormat format = textCursor().charFormat();
  format.setForeground(textColor());
  textCursor().insertText(txt, format);
}

void PythonPipWidget::keyPressEvent(QKeyEvent *e) {
  int key = e->key();
  QString txt = e->text();
  QString lineNotTrimmed = textCursor().block().text().mid(pipCommandPrefix.length());
  QString line = rtrim(textCursor().block().text()).mid(pipCommandPrefix.length());
  int col = textCursor().positionInBlock();
  if (key == Qt::Key_Backspace || key == Qt::Key_Left || key == Qt::Key_Right) {
    if (isCursorOnLastLine()) {
      if (key == Qt::Key_Backspace && !textCursor().selectedText().isEmpty()) {
        textCursor().removeSelectedText();
      } else {

        if (col > pipCommandPrefix.length()) {
          PythonCodeEditor::keyPressEvent(e);
        }

        if (key == Qt::Key_Right && col == pipCommandPrefix.length()) {
          PythonCodeEditor::keyPressEvent(e);
        }
      }
    } else {
      setCursorPosition(lines() - 1, lineLength(lines() - 1));
    }
  } else if (key == Qt::Key_Up || key == Qt::Key_Down) {
    setCursorPosition(lines() - 1, lineLength(lines() - 1));
  } else if (key == Qt::Key_Home) {
    if (isCursorOnLastLine()) {

      if (e->modifiers() == Qt::ShiftModifier) {
        setSelection(lines() - 1, pipCommandPrefix.length(), lines() - 1, col);
      } else {
        setCursorPosition(lines() - 1, pipCommandPrefix.length());
      }
    } else {
      setCursorPosition(lines() - 1, lineLength(lines() - 1));
    }
  } else if (key == Qt::Key_End) {
    if (isCursorOnLastLine()) {

      if (e->modifiers() == Qt::ShiftModifier) {
        setSelection(lines() - 1, col, lines() - 1, lineLength(lines() - 1));
      } else {
        setCursorPosition(lines() - 1, lineLength(lines() - 1));
      }
    } else {
      setCursorPosition(lines() - 1, lineLength(lines() - 1));
    }
  } else if (key == Qt::Key_A && e->modifiers() == Qt::ControlModifier) {
    if (isCursorOnLastLine()) {
      setSelection(lines() - 1, pipCommandPrefix.length(), lines() - 1, lineLength(lines() - 1));
    }
  } else if (key == Qt::Key_Enter || key == Qt::Key_Return) {
    QString pipArguments = rtrim(textCursor().block().text()).mid(pipCommandPrefix.length());
    insert("\n");
    executePipCommand(pipArguments);
  } else {
    PythonCodeEditor::keyPressEvent(e);
  }
}

void PythonPipWidget::executePipCommand(const QString &pipArguments) {
#ifdef WIN32
  static const QString pipCommand = PythonInterpreter::talipotVenvDirectory + "/Scripts/pip.exe";
#else
  static const QString pipCommand = PythonInterpreter::talipotVenvDirectory + "/bin/pip";
#endif

  PythonInterpreter::instance().setConsoleWidget(this);
  PythonInterpreter::instance().setProcessQtEventsDuringScriptExecution(true);
  PythonInterpreter::instance().runString(QString(R"(
from subprocess import Popen, PIPE, STDOUT
import sys
pipCommand = '%1' + ' ' + '%2'
p = Popen(pipCommand, stdout=PIPE, stderr=STDOUT, shell=True, text=True, encoding='utf-8')
for line in p.stdout:
    sys.stdout.write(line)
sys.stdout.write('\n')
p.wait()
  )")
                                              .arg(pipCommand, pipArguments));
  PythonInterpreter::instance().setProcessQtEventsDuringScriptExecution(false);
  PythonInterpreter::instance().resetConsoleWidget();
  PythonInterpreter::instance().setDefaultSIGINTHandler();
  insert(pipCommandPrefix, true);
}

void PythonPipWidget::showEvent(QShowEvent *) {
  setFocus();
}

void PythonPipWidget::updateAutoCompletionList(bool) {
  _autoCompletionList->clear();
}
