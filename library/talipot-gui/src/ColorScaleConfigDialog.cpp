/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
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
#include <QPainter>
#include <QMessageBox>
#include <QInputDialog>

#include <talipot/ColorScaleConfigDialog.h>
#include <talipot/ColorScalesManager.h>
#include <talipot/Settings.h>
#include <talipot/FontIcon.h>
#include <talipot/MaterialDesignIcons.h>

#include "ui_ColorScaleConfigDialog.h"

using namespace std;

namespace tlp {

map<QString, vector<Color>> ColorScaleConfigDialog::talipotImageColorScales;

ColorScaleConfigDialog::ColorScaleConfigDialog(const ColorScale &colorScale, QWidget *parent)
    : QDialog(parent), _ui(new Ui::ColorScaleDialog), colorScale(colorScale) {
  _ui->setupUi(this);
  _ui->invertColorScaleButton->setIcon(FontIcon::icon(MaterialDesignIcons::ArrowUpDownBold));
  _ui->colorsTable->setColumnWidth(0, _ui->colorsTable->width());
  _ui->colorsTable->horizontalHeader()->setHidden(true);
  QPalette palette;
  palette.setColor(QPalette::Window, Qt::white);
  _ui->savedGradientPreview->setPalette(palette);
  _ui->userGradientPreview->setPalette(palette);
  _ui->savedGradientPreview->setAutoFillBackground(true);
  _ui->userGradientPreview->setAutoFillBackground(true);
  connect(_ui->savedColorScalesList, &QListWidget::currentItemChanged, this,
          &ColorScaleConfigDialog::displaySavedGradientPreview);
  connect(_ui->savedColorScalesList, &QListWidget::itemDoubleClicked, this,
          &ColorScaleConfigDialog::reeditSaveColorScale);
  connect(_ui->nbColors, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &ColorScaleConfigDialog::nbColorsValueChanged);
  connect(_ui->colorsTable, &QTableWidget::itemDoubleClicked, this,
          &ColorScaleConfigDialog::colorTableItemDoubleClicked);
  connect(_ui->tabWidget, &QTabWidget::currentChanged, this,
          &ColorScaleConfigDialog::displaySavedGradientPreview);
  connect(_ui->tabWidget, &QTabWidget::currentChanged, this,
          &ColorScaleConfigDialog::displayUserGradientPreview);
  connect(_ui->gradientCB, &QAbstractButton::clicked, this,
          &ColorScaleConfigDialog::displayUserGradientPreview);
  connect(_ui->saveColorScaleButton, &QAbstractButton::clicked, this,
          &ColorScaleConfigDialog::saveCurrentColorScale);
  connect(_ui->deleteColorScaleButton, &QAbstractButton::clicked, this,
          &ColorScaleConfigDialog::deleteSavedColorScale);
  connect(_ui->importFromImgButton, &QAbstractButton::clicked, this,
          &ColorScaleConfigDialog::importColorScaleFromImageFile);
  connect(_ui->invertColorScaleButton, &QAbstractButton::clicked, this,
          &ColorScaleConfigDialog::invertEditedColorScale);
  connect(_ui->globalAlphaCB, &QAbstractButton::toggled, _ui->globalAlphaSB, &QWidget::setEnabled);
  connect(_ui->globalAlphaCB, &QAbstractButton::toggled, this,
          &ColorScaleConfigDialog::applyGlobalAlphaToColorScale);
  connect(_ui->globalAlphaSB, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &ColorScaleConfigDialog::applyGlobalAlphaToColorScale);

  if (talipotImageColorScales.empty()) {
    loadImageColorScales();
  }

  loadUserSavedColorScales();
  setColorScale(colorScale);
}

ColorScaleConfigDialog::~ColorScaleConfigDialog() {
  delete _ui;
}

void ColorScaleConfigDialog::accept() {
  vector<Color> colors;
  bool gradient = true;

  if (_ui->tabWidget->currentIndex() == 1) {
    if (_ui->savedColorScalesList->count() > 0 && _ui->savedColorScalesList->currentItem()) {
      QString savedColorScaleId = _ui->savedColorScalesList->currentItem()->text();

      if (talipotImageColorScales.find(savedColorScaleId) != talipotImageColorScales.end()) {
        colors = talipotImageColorScales[savedColorScaleId];
      } else {
        Settings::instance().beginGroup("ColorScales");
        QList<QVariant> colorsVector = Settings::instance().value(savedColorScaleId).toList();
        QString gradientScaleId = savedColorScaleId + "_gradient?";
        gradient = Settings::instance().value(gradientScaleId).toBool();
        Settings::instance().endGroup();

        for (const auto &colorv : colorsVector) {
          colors.push_back(QColorToColor(colorv.value<QColor>()));
        }

        std::reverse(colors.begin(), colors.end());
      }
    }
  } else {
    for (int i = 0; i < _ui->colorsTable->rowCount(); ++i) {
      QColor itemColor = _ui->colorsTable->item(i, 0)->background().color();
      colors.push_back(QColorToColor(itemColor));
    }

    std::reverse(colors.begin(), colors.end());
    gradient = _ui->gradientCB->isChecked();
  }

  if (!colors.empty()) {
    colorScale.setColorScale(colors, gradient);
  }

  ColorScalesManager::setLatestColorScale(colorScale);

  QDialog::accept();
}

vector<Color> ColorScaleConfigDialog::getColorScaleFromImageFile(const QString &imageFilePath) {
  QImage gradientImage(imageFilePath);
  uint imageHeight = gradientImage.height();

  uint step = 1;

  if (imageHeight > 50) {
    step = 10;
  }

  vector<Color> colors;

  for (uint i = 0; i < imageHeight; i += step) {
    QColor pixelColor = gradientImage.pixelColor(0, i);
    colors.push_back(QColorToColor(pixelColor));
  }

  if (imageHeight % step != 0) {
    QColor pixelColor = gradientImage.pixelColor(0, imageHeight - 1);
    colors.push_back(QColorToColor(pixelColor));
  }

  std::reverse(colors.begin(), colors.end());
  return colors;
}

ColorScale ColorScaleConfigDialog::getColorScaleFromImageFile(const std::string &imageFilePath,
                                                              bool gradient) {
  return ColorScale(getColorScaleFromImageFile(tlpStringToQString(imageFilePath)), gradient);
}

void ColorScaleConfigDialog::loadImageColorScalesFromDir(const QString &colorScalesDir) {
  QFileInfo colorscaleDirectory(colorScalesDir);

  if (colorscaleDirectory.exists() && colorscaleDirectory.isDir()) {
    QDir dir(colorscaleDirectory.absoluteFilePath());
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList();

    for (const auto &fileInfo : list) {
      if (fileInfo.isDir()) {
        loadImageColorScalesFromDir(fileInfo.absoluteFilePath());
      } else if (fileInfo.suffix() == "png") {
        talipotImageColorScales[fileInfo.fileName()] =
            getColorScaleFromImageFile(fileInfo.absoluteFilePath());
      }
    }
  }
}

void ColorScaleConfigDialog::loadImageColorScales() {
  loadImageColorScalesFromDir(tlpStringToQString(TalipotBitmapDir) + "colorscales");
}

void ColorScaleConfigDialog::importColorScaleFromFile(const QString &currentDir) {
  QString imageFilePath = QFileDialog::getOpenFileName(this, tr("Open Image File"), currentDir,
                                                       tr("Image Files (*.png *.jpg *.bmp)"));

  if (imageFilePath.isEmpty()) {
    return;
  }

  vector<Color> colorsList = getColorScaleFromImageFile(imageFilePath);

  if (!colorsList.empty()) {
    ColorScale scaleTmp(colorsList, true);
    setColorScale(scaleTmp);
    displayUserGradientPreview();
  }
}

void ColorScaleConfigDialog::importColorScaleFromImageFile() {
  importColorScaleFromFile(QString("./"));
}

void ColorScaleConfigDialog::pressButtonBrowse() {

  displaySavedGradientPreview();
}

void ColorScaleConfigDialog::displaySavedGradientPreview() {
  if (_ui->savedColorScalesList->count() > 0 && _ui->savedColorScalesList->currentItem()) {
    QList<QColor> colorsList;
    QString savedColorScaleId = _ui->savedColorScalesList->currentItem()->text();
    bool gradient = true;

    if (talipotImageColorScales.find(savedColorScaleId) != talipotImageColorScales.end()) {
      vector<Color> colors = talipotImageColorScales[savedColorScaleId];
      std::reverse(colors.begin(), colors.end());

      for (const auto &color : colors) {
        colorsList.push_back(QColor(color[0], color[1], color[2], color[3]));
      }
    } else {
      Settings::instance().beginGroup("ColorScales");
      QList<QVariant> colorsListv = Settings::instance().value(savedColorScaleId).toList();
      QString gradientScaleId = savedColorScaleId + "_gradient?";
      gradient = Settings::instance().value(gradientScaleId).toBool();
      Settings::instance().endGroup();

      for (const auto &c : colorsListv) {
        colorsList.push_back(c.value<QColor>());
      }
    }

    displayGradientPreview(colorsList, gradient, _ui->savedGradientPreview);
  }
}

void ColorScaleConfigDialog::displayUserGradientPreview() {

  QList<QColor> colorsVector;

  for (int i = 0; i < _ui->colorsTable->rowCount(); ++i) {
    colorsVector.push_back(_ui->colorsTable->item(i, 0)->background().color());
  }

  displayGradientPreview(colorsVector, _ui->gradientCB->isChecked(), _ui->userGradientPreview);
}

void ColorScaleConfigDialog::invertEditedColorScale() {
  QList<QTableWidgetItem *> itemsList;
  int nbItems = _ui->colorsTable->rowCount();

  for (int i = 0; i < _ui->colorsTable->rowCount(); ++i) {
    itemsList.push_front(_ui->colorsTable->takeItem(i, 0));
  }

  for (int i = 0; i < nbItems; ++i) {
    _ui->colorsTable->setItem(i, 0, itemsList.at(i));
  }

  displayUserGradientPreview();
}

static qreal clamp(qreal f, qreal minVal, qreal maxVal) {
  return min(max(f, minVal), maxVal);
}

void ColorScaleConfigDialog::displayGradientPreview(const QList<QColor> &colorsVector,
                                                    bool gradient, QLabel *displayLabel) {
  QPixmap pixmap(displayLabel->width(), displayLabel->height());
  pixmap.fill(Qt::transparent);
  QPainter painter;
  painter.begin(&pixmap);

  if (gradient) {
    QLinearGradient qLinearGradient(displayLabel->width() / 2, 0, displayLabel->width() / 2,
                                    displayLabel->height() - 1);
    qreal increment = 1.0 / (colorsVector.size() - 1);
    qreal relPos = 0;

    for (const auto &c : colorsVector) {
      qLinearGradient.setColorAt(clamp(relPos, 0.0, 1.0), c);
      relPos += increment;
    }

    painter.fillRect(0, 0, displayLabel->width(), displayLabel->height(), qLinearGradient);
  } else {
    float rectHeight = displayLabel->height() / colorsVector.size();

    for (int i = 0; i < colorsVector.size(); ++i) {
      painter.fillRect(0, i * rectHeight, displayLabel->width(), (i + 1) * rectHeight,
                       QBrush(colorsVector.at(i)));
    }
  }

  painter.end();
  displayLabel->setPixmap(pixmap.scaled(displayLabel->width(), displayLabel->height()));
}

void ColorScaleConfigDialog::nbColorsValueChanged(int value) {
  int lastCount = _ui->colorsTable->rowCount();
  _ui->colorsTable->setRowCount(value);

  if (lastCount < value) {
    for (int j = 0; j < value - lastCount; ++j) {
      auto *item = new QTableWidgetItem();
      QColor color(255, 255, 255, 255);

      if (_ui->globalAlphaCB->isChecked()) {
        color.setAlpha(_ui->globalAlphaSB->value());
      }

      item->setBackground(QBrush(color));
      item->setFlags(Qt::ItemIsEnabled);
      _ui->colorsTable->setItem(0, lastCount + j, item);
    }
  }

  displayUserGradientPreview();
}

void ColorScaleConfigDialog::colorTableItemDoubleClicked(QTableWidgetItem *item) {
  QColor itemBgColor = item->background().color();
  QColor newColor;

  if (getColorDialog(itemBgColor, this, "Select Color", newColor)) {
    if (_ui->globalAlphaCB->isChecked()) {
      newColor.setAlpha(_ui->globalAlphaSB->value());
    }

    item->setBackground(QBrush(newColor));
    displayUserGradientPreview();
  }
}

void ColorScaleConfigDialog::saveCurrentColorScale() {
  Settings::instance().beginGroup("ColorScales");
  QStringList savedColorScalesList = Settings::instance().childKeys();
  bool ok;
  QString text = QInputDialog::getText(this, tr("Color scale saving"),
                                       tr("Enter a name for this color scale : "),
                                       QLineEdit::Normal, "unnamed", &ok);

  if (ok && !text.isEmpty()) {
    if (savedColorScalesList.contains(text)) {
      QString question = "There is already a color scale saved under the name " + text +
                         ". Do you want to overwrite it ?";

      if (QMessageBox::question(this, "Color scale saving", question,
                                QMessageBox::Yes | QMessageBox::No,
                                QMessageBox::Yes) != QMessageBox::Yes) {
        return;
      }
    }

    QList<QVariant> colorsVector;

    for (int i = 0; i < _ui->colorsTable->rowCount(); ++i) {
      colorsVector.push_back(QVariant(_ui->colorsTable->item(i, 0)->background().color()));
    }

    Settings::instance().setValue(text, colorsVector);
    QString gradientId = text + "_gradient?";
    Settings::instance().setValue(gradientId, _ui->gradientCB->isChecked());
  }

  Settings::instance().endGroup();
  loadUserSavedColorScales();
}

void ColorScaleConfigDialog::deleteSavedColorScale() {
  if (_ui->savedColorScalesList->count() > 0 && _ui->savedColorScalesList->currentItem()) {
    QString savedColorScaleId = _ui->savedColorScalesList->currentItem()->text();

    if (QMessageBox::question(
            this, "Color scale deleting", "Delete saved color scale " + savedColorScaleId + " ?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) != QMessageBox::Yes) {
      return;
    }

    Settings::instance().beginGroup("ColorScales");
    Settings::instance().remove(savedColorScaleId);
    Settings::instance().remove(savedColorScaleId + "_gradient?");
    Settings::instance().endGroup();
    loadUserSavedColorScales();
  }
}

void ColorScaleConfigDialog::loadUserSavedColorScales() {
  _ui->savedColorScalesList->clear();

  for (const auto &it : talipotImageColorScales) {
    _ui->savedColorScalesList->addItem(it.first);
  }

  Settings::instance().beginGroup("ColorScales");
  QStringList savedColorScalesIdList = Settings::instance().childKeys();

  for (int i = 0; i < savedColorScalesIdList.size(); ++i) {
    if (!savedColorScalesIdList.at(i).contains("_gradient?")) {
      _ui->savedColorScalesList->addItem(savedColorScalesIdList.at(i));
    }
  }

  Settings::instance().endGroup();
}

void ColorScaleConfigDialog::resizeEvent(QResizeEvent *) {
  displaySavedGradientPreview();
  displayUserGradientPreview();
  _ui->colorsTable->setColumnWidth(0, _ui->colorsTable->width());
}

void ColorScaleConfigDialog::showEvent(QShowEvent *) {
  displaySavedGradientPreview();
  displayUserGradientPreview();
  _ui->colorsTable->setColumnWidth(0, _ui->colorsTable->width());
}

void ColorScaleConfigDialog::reeditSaveColorScale(QListWidgetItem *savedColorScaleItem) {
  QString savedColorScaleId = savedColorScaleItem->text();
  vector<Color> colorsList;
  bool gradient = true;

  if (talipotImageColorScales.find(savedColorScaleId) != talipotImageColorScales.end()) {
    colorsList = talipotImageColorScales[savedColorScaleId];
  } else {
    Settings::instance().beginGroup("ColorScales");
    QList<QVariant> colorsListv = Settings::instance().value(savedColorScaleId).toList();
    QString gradientScaleId = savedColorScaleId + "_gradient?";
    gradient = Settings::instance().value(gradientScaleId).toBool();
    Settings::instance().endGroup();

    for (const auto &colorv : colorsListv) {
      colorsList.push_back(QColorToColor(colorv.value<QColor>()));
    }

    reverse(colorsList.begin(), colorsList.end());
  }

  ColorScale scaleTmp(colorsList, gradient);
  setColorScale(scaleTmp);
}

void ColorScaleConfigDialog::setColorScale(const ColorScale &colorScale) {
  if (colorScale.colorScaleInitialized()) {
    for (int row = 0; row < _ui->savedColorScalesList->count(); ++row) {
      QListWidgetItem *item = _ui->savedColorScalesList->item(row);

      if (talipotImageColorScales.find(item->text()) != talipotImageColorScales.end() &&
          colorScale == talipotImageColorScales[item->text()]) {
        // colorScale is a predefined one
        // so select it in the list view
        _ui->savedColorScalesList->setCurrentItem(item);
      }
    }

    disconnect(_ui->nbColors, QOverload<int>::of(&QSpinBox::valueChanged), this,
               &ColorScaleConfigDialog::nbColorsValueChanged);

    _ui->colorsTable->clear();
    _ui->colorsTable->setRowCount(0);

    // init dialog with colors in the color Scale
    const auto &colorMap = colorScale.getColorMap();
    uint row = 0;

    if (colorScale.isGradient()) {
      _ui->colorsTable->setRowCount(colorMap.size());
      _ui->nbColors->setValue(colorMap.size());
      _ui->gradientCB->setChecked(true);
      row = colorMap.size() - 1;
    } else {
      _ui->colorsTable->setRowCount(colorMap.size() / 2);
      _ui->nbColors->setValue(colorMap.size() / 2);
      _ui->gradientCB->setChecked(false);
      row = (colorMap.size() / 2) - 1;
    }

    for (auto it = colorMap.begin(); it != colorMap.end();) {
      auto *item = new QTableWidgetItem();
      item->setFlags(Qt::ItemIsEnabled);
      item->setBackground(QBrush(
          QColor(it->second.getR(), it->second.getG(), it->second.getB(), it->second.getA())));
      _ui->colorsTable->setItem(row, 0, item);
      --row;

      if (colorScale.isGradient()) {
        ++it;
      } else {
        ++it;
        ++it;
      }
    }

    connect(_ui->nbColors, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &ColorScaleConfigDialog::nbColorsValueChanged);
    _ui->tabWidget->setCurrentIndex(0);
    applyGlobalAlphaToColorScale();
  } else {
    // use latest ColorScale
    setColorScale(ColorScalesManager::getLatestColorScale());
  }
}

const ColorScale &ColorScaleConfigDialog::getColorScale() const {
  return colorScale;
}

void ColorScaleConfigDialog::applyGlobalAlphaToColorScale() {
  if (_ui->globalAlphaCB->isChecked()) {
    for (int i = 0; i < _ui->colorsTable->rowCount(); ++i) {
      QColor itemColor = _ui->colorsTable->item(i, 0)->background().color();
      itemColor.setAlpha(_ui->globalAlphaSB->value());
      _ui->colorsTable->item(i, 0)->setBackground(QBrush(itemColor));
    }

    displayUserGradientPreview();
  }
}
}
