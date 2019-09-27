/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "talipot/SceneLayersConfigWidget.h"
#include "ui_SceneLayersConfigWidget.h"

#include <talipot/GlMainView.h>
#include <talipot/GlMainWidget.h>
#include <talipot/SceneLayersModel.h>

using namespace tlp;

SceneLayersConfigWidget::SceneLayersConfigWidget(QWidget *parent)
    : QWidget(parent), _ui(new Ui::SceneLayersConfigWidget), _glMainWidget(nullptr) {
  _ui->setupUi(this);
}

SceneLayersConfigWidget::~SceneLayersConfigWidget() {
  delete _ui;
}

void SceneLayersConfigWidget::setGlMainWidget(GlMainWidget *glMainWidget) {
  _glMainWidget = glMainWidget;
  SceneLayersModel *model = new SceneLayersModel(_glMainWidget->getScene(), _ui->treeView);
  _ui->treeView->setModel(model);
  connect(model, SIGNAL(drawNeeded(tlp::GlScene *)), this, SIGNAL(drawNeeded()));
  connect(_ui->treeView, SIGNAL(collapsed(const QModelIndex &)), this, SLOT(resizeFirstColumn()));
  connect(_ui->treeView, SIGNAL(expanded(const QModelIndex &)), this, SLOT(resizeFirstColumn()));
  _ui->treeView->setColumnWidth(0, 110);
}

void SceneLayersConfigWidget::resizeFirstColumn() {
  _ui->treeView->resizeColumnToContents(0);
  _ui->treeView->resizeColumnToContents(1);
}
