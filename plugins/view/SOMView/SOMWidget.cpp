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

#include "SOMWidget.h"
#include "ui_SOMWidget.h"

using namespace tlp;

SOMWidget::SOMWidget(QWidget *parent) : QWidget(parent), _ui(new Ui::SOMWidget) {
    _ui->setupUi(this);
}

SOMWidget::~SOMWidget() {
    delete _ui;
}
