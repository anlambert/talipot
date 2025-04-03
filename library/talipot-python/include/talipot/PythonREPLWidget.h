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

#ifndef TALIPOT_PYTHON_REPL_WIDGET_H
#define TALIPOT_PYTHON_REPL_WIDGET_H

#include <talipot/config.h>

#include <QWidget>

namespace Ui {
class PythonREPLWidget;
}

namespace tlp {
class GraphHierarchiesModel;
class PythonShellWidget;

class TLP_PYTHON_SCOPE PythonREPLWidget : public QWidget {

    Q_OBJECT
    Ui::PythonREPLWidget *_ui;

  public:
    explicit PythonREPLWidget(QWidget *parent = nullptr);
    ~PythonREPLWidget() override;
    void setModel(tlp::GraphHierarchiesModel *model);
    PythonShellWidget *getEditor();

  protected:
    void dragEnterEvent(QDragEnterEvent *) override;
    void dropEvent(QDropEvent *) override;

  private slots:

    void graphComboIndexChanged();
    void beginCurrentLinesExecution();
    void endCurrentLinesExecution();
};
}
#endif // TALIPOT_PYTHON_REPL_WIDGET_H
