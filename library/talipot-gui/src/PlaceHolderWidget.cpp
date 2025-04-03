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

#include "talipot/PlaceHolderWidget.h"

#include <QVBoxLayout>

PlaceHolderWidget::PlaceHolderWidget(QWidget *parent) : QWidget(parent), _widget(nullptr) {
    setLayout(new QVBoxLayout);
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->setSpacing(0);
}

void PlaceHolderWidget::setWidget(QWidget *widget) {
    if (_widget != nullptr) {
        _widget->hide();
        layout()->removeWidget(_widget);
        _widget->setParent(nullptr);
        _widget = nullptr;
    }

    _widget = widget;

    if (_widget != nullptr) {
        _widget->installEventFilter(this);
        layout()->addWidget(_widget);
        _widget->show();
    }
}

QWidget *PlaceHolderWidget::widget() const {
    return _widget;
}

void PlaceHolderWidget::resetWidget() {
    _widget = nullptr;
}
