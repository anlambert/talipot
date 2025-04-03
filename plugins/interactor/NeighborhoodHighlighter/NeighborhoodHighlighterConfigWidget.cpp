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

#include "NeighborhoodHighlighterConfigWidget.h"
#include "ui_NeighborhoodHighlighterConfigWidget.h"

#include <talipot/TlpQtTools.h>

using namespace tlp;

NeighborhoodHighlighterConfigWidget::NeighborhoodHighlighterConfigWidget(QWidget *parent)
    : QWidget(parent), _ui(new Ui::NeighborhoodHighlighterConfigWidget) {
    _ui->setupUi(this);
}

NeighborhoodHighlighterConfigWidget::~NeighborhoodHighlighterConfigWidget() {
    delete _ui;
}

NodeNeighborhoodView::NeighborNodesType
NeighborhoodHighlighterConfigWidget::getNeighborsType() const {
    if (_ui->outputEdgesRB->isChecked()) {
        return NodeNeighborhoodView::OUT_NEIGHBORS;
    } else if (_ui->inputEdgesRB->isChecked()) {
        return NodeNeighborhoodView::IN_NEIGHBORS;
    } else {
        return NodeNeighborhoodView::IN_OUT_NEIGHBORS;
    }
}

bool NeighborhoodHighlighterConfigWidget::computeReachableSubGraph() const {
    return _ui->computeReachableSGCB->isChecked();
}

void NeighborhoodHighlighterConfigWidget::setCurrentMaxDistanceForReachableNodes(uint distance) {
    _ui->distanceLineEdit->setText(QString::number(distance));
}

bool NeighborhoodHighlighterConfigWidget::bringAndGoAnimation1() const {
    return _ui->bagAnimation1->isChecked();
}

int NeighborhoodHighlighterConfigWidget::numberOfNodesToBring() const {
    return _ui->nodesNumber->value();
}

std::string NeighborhoodHighlighterConfigWidget::propertyToUse() const {
    return tlp::QStringToTlpString(_ui->propertyName->currentText());
}

void NeighborhoodHighlighterConfigWidget::setNumberOfNodes(int nodesNb) {
    _ui->nodesNumber->setValue(nodesNb);
}

void NeighborhoodHighlighterConfigWidget::setPropertyToUse(const std::string &propertyName) {
    _ui->propertyName->addItem(tlpStringToQString(propertyName));
    _ui->propertyName->setCurrentIndex(
        _ui->propertyName->findText(tlpStringToQString(propertyName)));
}

bool NeighborhoodHighlighterConfigWidget::isdisplayEdgesCBChecked() const {
    return _ui->displayEdgesCB->isChecked();
}
