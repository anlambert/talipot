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

#ifndef TALIPOT_PYTHON_PIP_WIDGET_H
#define TALIPOT_PYTHON_PIP_WIDGET_H

#include <QVector>

#include <talipot/PythonCodeEditor.h>

class QKeyEvent;

namespace tlp {

class TLP_PYTHON_SCOPE PythonPipWidget : public PythonCodeEditor {

public:
  explicit PythonPipWidget(QWidget *parent = nullptr);

protected:
  void keyPressEvent(QKeyEvent *e) override;

  bool isCursorOnLastLine();

  void executePipCommand(const QString &pipArguments);

  void showEvent(QShowEvent *event) override;

public slots:

  void insert(const QString &txt, const bool atEnd = false);

protected slots:

  void updateAutoCompletionList(bool dotContext = false) override;
};
}

#endif // TALIPOT_PYTHON_PIP_WIDGET_H
