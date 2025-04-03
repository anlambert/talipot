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

#include "ComputeSOMWidget.h"
#include "ui_ComputeSOMWidget.h"

using namespace tlp;
using namespace std;

ComputeSOMWidget::ComputeSOMWidget(QWidget *parent)
    : QWidget(parent), _ui(new Ui::SOMComputeWidget) {
    _ui->setupUi(this);
}

ComputeSOMWidget::~ComputeSOMWidget() {
    delete _ui;
}

unsigned ComputeSOMWidget::number() const {
    return _ui->iterationNumberSpinBox->value();
}

void ComputeSOMWidget::setNumber(unsigned uintValue) {
    return _ui->iterationNumberSpinBox->setValue(uintValue);
}

void ComputeSOMWidget::clearLists() {
    _ui->propertiesConfigurationWidget->clearLists();
}

vector<string> ComputeSOMWidget::getSelectedProperties() {
    return _ui->propertiesConfigurationWidget->getSelectedProperties();
}

void ComputeSOMWidget::setWidgetParameters(Graph *g, vector<string> &propertyFilterType) {
    _ui->propertiesConfigurationWidget->setWidgetParameters(g, propertyFilterType);
}

void ComputeSOMWidget::setOutputPropertiesList(std::vector<std::string> &properties) {
    _ui->propertiesConfigurationWidget->setOutputPropertiesList(properties);
}
