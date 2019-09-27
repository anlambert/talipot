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

#include <QFileDialog>
#include <QColorDialog>
#include <QMainWindow>

#include <talipot/TlpTools.h>
#include <talipot/Perspective.h>
#include <talipot/TlpQtTools.h>

#include "ParallelTools.h"
#include "ParallelCoordsDrawConfigWidget.h"
#include "ui_ParallelCoordsDrawConfigWidget.h"

using namespace std;

namespace tlp {

ParallelCoordsDrawConfigWidget::ParallelCoordsDrawConfigWidget(QWidget *parent)
    : QWidget(parent), oldValuesInitialized(false),
      _ui(new Ui::ParallelCoordsDrawConfigWidgetData) {
  _ui->setupUi(this);
  setBackgroundColor(Color(255, 255, 255));
  connect(_ui->browseButton, SIGNAL(clicked()), this, SLOT(pressButtonBrowse()));
  connect(_ui->userTexture, SIGNAL(toggled(bool)), this, SLOT(userTextureRbToggled(bool)));
  connect(_ui->minAxisPointSize, SIGNAL(valueChanged(int)), this,
          SLOT(minAxisPointSizeValueChanged(int)));
  connect(_ui->maxAxisPointSize, SIGNAL(valueChanged(int)), this,
          SLOT(maxAxisPointSizeValueChanged(int)));

  if (Perspective::instance() != nullptr && Perspective::instance()->mainWindow() != nullptr) {
    _ui->bgColorButton->setDialogParent(Perspective::instance()->mainWindow());
  }
}

ParallelCoordsDrawConfigWidget::~ParallelCoordsDrawConfigWidget() {
  delete _ui;
}

void ParallelCoordsDrawConfigWidget::pressButtonBrowse() {
  QString fileName(QFileDialog::getOpenFileName(this, tr("Open Texture File"), "./",
                                                tr("Image Files (*.png *.jpg *.bmp)")));
  _ui->userTextureFile->setText(fileName);
}

unsigned int ParallelCoordsDrawConfigWidget::getAxisHeight() const {
  return _ui->axisHeight->value();
}

void ParallelCoordsDrawConfigWidget::setAxisHeight(const unsigned int aHeight) {
  _ui->axisHeight->setValue(aHeight);
}

bool ParallelCoordsDrawConfigWidget::drawPointOnAxis() const {
  return _ui->gBoxAxisPoints->isChecked();
}

string ParallelCoordsDrawConfigWidget::getLinesTextureFilename() const {
  if (_ui->gBoxLineTexture->isChecked()) {
    if (_ui->defaultTexture->isChecked()) {
      return DEFAULT_TEXTURE_FILE;
    } else {
      return QStringToTlpString(_ui->userTextureFile->text());
    }
  } else {
    return "";
  }
}

void ParallelCoordsDrawConfigWidget::setLinesTextureFilename(
    const std::string &linesTextureFileName) {
  if (!linesTextureFileName.empty()) {
    _ui->gBoxLineTexture->setChecked(true);

    if (linesTextureFileName == string(TalipotBitmapDir + DEFAULT_TEXTURE_FILE)) {
      _ui->defaultTexture->setChecked(true);
    } else {
      _ui->userTexture->setChecked(true);
      _ui->userTextureFile->setText(tlpStringToQString(linesTextureFileName));
    }
  } else {
    _ui->gBoxLineTexture->setChecked(false);
  }
}

Size ParallelCoordsDrawConfigWidget::getAxisPointMinSize() const {
  float pointSize = _ui->minAxisPointSize->text().toFloat();
  return Size(pointSize, pointSize, pointSize);
}

Size ParallelCoordsDrawConfigWidget::getAxisPointMaxSize() const {
  float pointSize = _ui->maxAxisPointSize->text().toFloat();
  return Size(pointSize, pointSize, pointSize);
}

void ParallelCoordsDrawConfigWidget::setAxisPointMinSize(const unsigned int axisPointMinSize) {
  _ui->minAxisPointSize->setValue(axisPointMinSize);
}

void ParallelCoordsDrawConfigWidget::setAxisPointMaxSize(const unsigned int axisPointMaxSize) {
  _ui->maxAxisPointSize->setValue(axisPointMaxSize);
}

bool ParallelCoordsDrawConfigWidget::displayNodeLabels() const {
  return _ui->displayLabelsCB->isChecked();
}

void ParallelCoordsDrawConfigWidget::setDisplayNodeLabels(const bool set) {
  return _ui->displayLabelsCB->setChecked(set);
}

void ParallelCoordsDrawConfigWidget::userTextureRbToggled(const bool checked) {
  _ui->userTextureFile->setEnabled(checked);
  _ui->browseButton->setEnabled(checked);
}

void ParallelCoordsDrawConfigWidget::minAxisPointSizeValueChanged(const int newValue) {
  if (_ui->maxAxisPointSize->value() < newValue) {
    _ui->maxAxisPointSize->setValue(newValue + 1);
  }
}

void ParallelCoordsDrawConfigWidget::maxAxisPointSizeValueChanged(const int newValue) {
  if (_ui->minAxisPointSize->value() > newValue) {
    _ui->minAxisPointSize->setValue(newValue - 1);
  }
}

void ParallelCoordsDrawConfigWidget::setLinesColorAlphaValue(unsigned int value) {
  if (value > 255) {
    _ui->viewColorAlphaRb->setChecked(true);
    _ui->userAlphaRb->setChecked(false);
  } else {
    _ui->viewColorAlphaRb->setChecked(false);
    _ui->userAlphaRb->setChecked(true);
    _ui->viewColorAlphaValue->setValue(value);
  }
}

unsigned int ParallelCoordsDrawConfigWidget::getLinesColorAlphaValue() const {
  if (_ui->viewColorAlphaRb->isChecked()) {
    return 300;
  } else {
    return _ui->viewColorAlphaValue->value();
  }
}

void ParallelCoordsDrawConfigWidget::setBackgroundColor(const Color &color) {
  _ui->bgColorButton->setColor(color);
}

Color ParallelCoordsDrawConfigWidget::getBackgroundColor() const {
  return _ui->bgColorButton->talipotColor();
}

void ParallelCoordsDrawConfigWidget::setDrawPointOnAxis(const bool drawPointOnAxis) {
  _ui->gBoxAxisPoints->setChecked(drawPointOnAxis);
}

unsigned int ParallelCoordsDrawConfigWidget::getUnhighlightedEltsColorsAlphaValue() const {
  return _ui->nonHighlightedEltsAlphaValue->value();
}

void ParallelCoordsDrawConfigWidget::setUnhighlightedEltsColorsAlphaValue(
    const unsigned int alphaValue) {
  _ui->nonHighlightedEltsAlphaValue->setValue(alphaValue);
}

bool ParallelCoordsDrawConfigWidget::configurationChanged() {
  bool confChanged = false;

  if (oldValuesInitialized) {
    if (oldAxisHeight != getAxisHeight() || oldDrawPointOnAxis != drawPointOnAxis() ||
        oldAxisPointMinSize != getAxisPointMinSize() ||
        oldAxisPointMaxSize != getAxisPointMaxSize() ||
        oldDisplayNodesLabels != displayNodeLabels() ||
        oldLinesColorAlphaValue != getLinesColorAlphaValue() ||
        oldBackgroundColor != getBackgroundColor() ||
        oldUnhighlightedEltsColorsAlphaValue != getUnhighlightedEltsColorsAlphaValue() ||
        oldLinesTextureFilename != getLinesTextureFilename()) {
      confChanged = true;
    }
  } else {
    confChanged = true;
    oldValuesInitialized = true;
  }

  if (confChanged) {
    oldAxisHeight = getAxisHeight();
    oldDrawPointOnAxis = drawPointOnAxis();
    oldAxisPointMinSize = getAxisPointMinSize();
    oldAxisPointMaxSize = getAxisPointMaxSize();
    oldDisplayNodesLabels = displayNodeLabels();
    oldLinesColorAlphaValue = getLinesColorAlphaValue();
    oldBackgroundColor = getBackgroundColor();
    oldUnhighlightedEltsColorsAlphaValue = getUnhighlightedEltsColorsAlphaValue();
    oldLinesTextureFilename = getLinesTextureFilename();
  }

  return confChanged;
}
} // namespace tlp
