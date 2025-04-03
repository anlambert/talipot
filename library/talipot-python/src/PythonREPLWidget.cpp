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

#include <talipot/PythonInterpreter.h>
#include <talipot/PythonREPLWidget.h>
#include <talipot/GraphHierarchiesModel.h>
#include <talipot/Mimes.h>
#include <talipot/MetaTypes.h>

#include "ui_PythonREPLWidget.h"

using namespace tlp;

static const QString setCurrentGraphFunction = "graph = None\n"
                                               "def setCurrentGraph(g):\n"
                                               "	  global graph\n"
                                               "	  graph = g\n";

PythonREPLWidget::PythonREPLWidget(QWidget *parent)
    : QWidget(parent), _ui(new Ui::PythonREPLWidget) {
    _ui->setupUi(this);
    connect(_ui->graphCombo, &TreeViewComboBox::currentItemChanged, this,
            &PythonREPLWidget::graphComboIndexChanged);
    tlp::PythonInterpreter::instance().runString(setCurrentGraphFunction);
    connect(_ui->pythonShellWidget, &tlp::PythonShellWidget::beginCurrentLinesExecution, this,
            &PythonREPLWidget::beginCurrentLinesExecution);
    connect(_ui->pythonShellWidget, &tlp::PythonShellWidget::endCurrentLinesExecution, this,
            &PythonREPLWidget::endCurrentLinesExecution);
}

PythonREPLWidget::~PythonREPLWidget() {
    delete _ui;
}

void PythonREPLWidget::setModel(tlp::GraphHierarchiesModel *model) {
    _ui->graphCombo->setModel(model);
}

void PythonREPLWidget::graphComboIndexChanged() {
    auto *g = _ui->graphCombo->model()
                  ->data(_ui->graphCombo->selectedIndex(), tlp::Model::GraphRole)
                  .value<tlp::Graph *>();
    tlp::PythonInterpreter::instance().runGraphScript("__main__", "setCurrentGraph", g);
    _ui->pythonShellWidget->getAutoCompletionDb()->setGraph(g);
}

void PythonREPLWidget::dragEnterEvent(QDragEnterEvent *dragEv) {
    const auto *mimeType = dynamic_cast<const tlp::GraphMimeType *>(dragEv->mimeData());

    if (mimeType != nullptr) {
        dragEv->accept();
    }
}

void PythonREPLWidget::dropEvent(QDropEvent *dropEv) {
    const auto *mimeType = dynamic_cast<const tlp::GraphMimeType *>(dropEv->mimeData());

    if (mimeType != nullptr) {
        auto *model = static_cast<tlp::GraphHierarchiesModel *>(_ui->graphCombo->model());
        QModelIndex graphIndex = model->indexOf(mimeType->graph());

        if (graphIndex == _ui->graphCombo->selectedIndex()) {
            return;
        }

        _ui->graphCombo->selectIndex(graphIndex);
        dropEv->accept();
    }
}

void PythonREPLWidget::beginCurrentLinesExecution() {
    auto *g = _ui->graphCombo->model()
                  ->data(_ui->graphCombo->selectedIndex(), tlp::Model::GraphRole)
                  .value<tlp::Graph *>();

    // undo/redo management
    if (g) {
        g->push();
    }
}

void PythonREPLWidget::endCurrentLinesExecution() {
    auto *g = _ui->graphCombo->model()
                  ->data(_ui->graphCombo->selectedIndex(), tlp::Model::GraphRole)
                  .value<tlp::Graph *>();
    // undo/redo management
    g->popIfNoUpdates();
}

PythonShellWidget *PythonREPLWidget::getEditor() {
    return _ui->pythonShellWidget;
}