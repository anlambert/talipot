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

#ifndef TALIPOT_PYTHON_SHELL_WIDGET_H
#define TALIPOT_PYTHON_SHELL_WIDGET_H

#include <QVector>

#include <talipot/PythonCodeEditor.h>

class QKeyEvent;

namespace tlp {

class TLP_PYTHON_SCOPE PythonShellWidget : public PythonCodeEditor {

    Q_OBJECT

    QString _currentPs;
    QString _currentCodeLines;

    QVector<QString> _history;
    int _currentHistoryPos;

  public:
    explicit PythonShellWidget(QWidget *parent = nullptr);

  signals:

    void beginCurrentLinesExecution();
    void endCurrentLinesExecution();

  protected:
    void keyPressEvent(QKeyEvent *e) override;

    bool isCursorOnLastLine();

    void executeCurrentLines();

    void showEvent(QShowEvent *event) override;

  public slots:

    void insert(const QString &txt, const bool atEnd = false);

  protected slots:

    void updateAutoCompletionList(bool dotContext = false) override;
};
}

#endif // TALIPOT_PYTHON_SHELL_WIDGET_H
