/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/InteractorConfigWidget.h>
#include "ui_InteractorConfigWidget.h"

#include <QLabel>

#include <talipot/Interactor.h>

using namespace tlp;

InteractorConfigWidget::InteractorConfigWidget(QWidget *parent)
    : QWidget(parent), _ui(new Ui::InteractorConfigWidget), _interactor(nullptr) {
  _ui->setupUi(this);
}

InteractorConfigWidget::~InteractorConfigWidget() {
  delete _ui;
}

void InteractorConfigWidget::clearWidgets() {
  // Removes widget from the layout to not delete the object and give back parenthood.
  // It is up to the interactor developer to delete its config widget
  if (_interactor) {
    if (_interactor->docWidget()) {
      _ui->scrollAreaDoc->takeWidget()->hide();
    }
    if (_interactor->configurationWidget()) {
      _ui->scrollAreaOptions->takeWidget()->hide();
    }
    _interactor = nullptr;
  }
}

bool InteractorConfigWidget::setWidgets(Interactor *interactor) {

  clearWidgets();
  _interactor = interactor;

  QWidget *docWidget = nullptr;
  QWidget *optionsWidget = nullptr;
  if (interactor) {
    docWidget = interactor->docWidget();
    optionsWidget = interactor->configurationWidget();
  }

  if (!docWidget && !optionsWidget) {
    return false;
  } else {
    if (docWidget) {
      _ui->scrollAreaDoc->setWidget(docWidget);
      _ui->tabWidget->setCurrentIndex(0);
    } else {
      _ui->tabWidget->setCurrentIndex(1);
    }
    _ui->tabWidget->setTabEnabled(0, docWidget != nullptr);

    if (optionsWidget) {
      _ui->scrollAreaOptions->setWidget(optionsWidget);
    }
    _ui->tabWidget->setTabEnabled(1, optionsWidget != nullptr);
    return true;
  }
}
