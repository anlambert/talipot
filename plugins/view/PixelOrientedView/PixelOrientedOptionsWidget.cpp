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

#include <talipot/TlpQtTools.h>

#include "PixelOrientedOptionsWidget.h"
#include "ui_PixelOrientedOptionsWidget.h"

using namespace std;
using namespace tlp;

PixelOrientedOptionsWidget::PixelOrientedOptionsWidget(QWidget *parent)
    : QWidget(parent), _ui(new Ui::PixelOrientedOptionsWidget), oldValuesInitialized(false) {
    _ui->setupUi(this);
    _ui->backColorButton->setDialogTitle("Choose the background color");
    setBackgroundColor(Color(255, 255, 255));
}

PixelOrientedOptionsWidget::~PixelOrientedOptionsWidget() {
    delete _ui;
}

Color PixelOrientedOptionsWidget::getBackgroundColor() const {
    return _ui->backColorButton->talipotColor();
}

void PixelOrientedOptionsWidget::setBackgroundColor(const Color &color) {
    _ui->backColorButton->setColor(color);
}

string PixelOrientedOptionsWidget::getLayoutType() const {
    return QStringToTlpString(_ui->layoutTypeCB->currentText());
}

void PixelOrientedOptionsWidget::setLayoutType(const string &layoutType) {
    int idx = _ui->layoutTypeCB->findText(QString(layoutType.c_str()));

    if (idx != -1) {
        _ui->layoutTypeCB->setCurrentIndex(idx);
    }
}

bool PixelOrientedOptionsWidget::configurationChanged() {
    bool confChanged = false;

    if (oldValuesInitialized) {
        if (oldBackgroundColor != getBackgroundColor() || oldLayoutType != getLayoutType()) {
            confChanged = true;
        }
    } else {
        confChanged = true;
        oldValuesInitialized = true;
    }

    if (confChanged) {
        oldBackgroundColor = getBackgroundColor();
        oldLayoutType = getLayoutType();
    }

    return confChanged;
}
