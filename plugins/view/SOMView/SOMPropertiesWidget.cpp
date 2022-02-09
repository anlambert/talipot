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

#include "SOMPropertiesWidget.h"
#include "SOMView.h"
#include "ui_SOMPropertiesWidget.h"

#include <QRadioButton>
#include <QPushButton>
#include <QButtonGroup>

#include <talipot/GraphPropertiesSelectionWidget.h>
#include <talipot/ColorScalesManager.h>
#include <talipot/TlpQtTools.h>

using namespace std;
using namespace tlp;

SOMPropertiesWidget::SOMPropertiesWidget(SOMView *view, QWidget *parent)
    : QWidget(parent), _ui(new Ui::SOMPropertiesWidget), view(view) {
  _ui->setupUi(this);
  dimensionConfigurationWidget = new tlp::ComputeSOMWidget(parent);

  defaultScale = new ColorScale(ColorScalesManager::getLatestColorScale());

  defaultScale->addObserver(this);

  auto *sizeMappingLayout = new QVBoxLayout(_ui->nodeSizeMappingGroupBox);
  sizeMappingLayout->setContentsMargins(0, 0, 0, 0);
  sizeMappingLayout->setSpacing(0);
  sizeMappingLayout->setContentsMargins(0, 5, 0, 0);
  sizeMappingButtonGroup = new QButtonGroup();
  noNodeSizeMappingRadioButton = new QRadioButton("No size mapping");
  sizeMappingButtonGroup->addButton(noNodeSizeMappingRadioButton);
  sizeMappingLayout->addWidget(noNodeSizeMappingRadioButton);
  realNodeSizeMappingRadioButton = new QRadioButton("Map node size on real node size");
  sizeMappingButtonGroup->addButton(realNodeSizeMappingRadioButton);
  sizeMappingLayout->layout()->addWidget(realNodeSizeMappingRadioButton);
  realNodeSizeMappingRadioButton->setChecked(true);

  // Display multiple properties at same time
  multiplePropertiesRepresentation = false;

  dimensionConfigurationWidget->setWindowTitle("Dimensions");
  setWindowTitle("Options");
}

QList<QWidget *> SOMPropertiesWidget::configurationWidgets() const {

  QList<QWidget *> widgets;

  widgets << dimensionConfigurationWidget
          << const_cast<QWidget *>(static_cast<const QWidget *>(this));

  return widgets;
}

uint SOMPropertiesWidget::getGridWidth() const {
  return _ui->gridWidthSpinBox->value();
}
uint SOMPropertiesWidget::getGridHeight() const {
  return _ui->gridHeightSpinBox->value();
}

QString SOMPropertiesWidget::getConnectivityLabel() const {
  return _ui->nodeConnectivityComboBox->currentText();
}

unsigned SOMPropertiesWidget::getConnectivityIndex() const {
  return _ui->nodeConnectivityComboBox->currentIndex();
}

bool SOMPropertiesWidget::getOppositeConnected() const {
  return _ui->opposedConnectedCheckBox->checkState() == Qt::Checked;
}

double SOMPropertiesWidget::getLearningRateValue() const {
  return _ui->baseLearningRateSpinBox->value();
}

QString SOMPropertiesWidget::getDiffusionRateMethodLabel() const {
  return _ui->diffusionRateComputationMethodComboBox->currentText();
}

uint SOMPropertiesWidget::getMaxDistanceValue() const {
  return _ui->maxDistanceSpinBox->value();
}

double SOMPropertiesWidget::getDiffusionRateValue() const {
  return _ui->baseDiffusionRateSpinBox->value();
}

bool SOMPropertiesWidget::getAutoMapping() const {
  return _ui->autoMappingCheckBox->isChecked();
}

bool SOMPropertiesWidget::getLinkColor() const {
  return _ui->colorLinkCheckBox->checkState() == Qt::Checked;
}

bool SOMPropertiesWidget::useAnimation() const {
  return _ui->animationCheckBox->isChecked();
}

unsigned SOMPropertiesWidget::getIterationNumber() const {
  return dimensionConfigurationWidget->number();
}

void SOMPropertiesWidget::clearpropertiesConfigurationWidget() {
  dimensionConfigurationWidget->clearLists();
}

void SOMPropertiesWidget::addfilter(Graph *g, vector<string> &propertyFilterType) {
  dimensionConfigurationWidget->setWidgetParameters(g, propertyFilterType);
}

vector<string> SOMPropertiesWidget::getSelectedProperties() const {
  return dimensionConfigurationWidget->getSelectedProperties();
}

SOMPropertiesWidget::~SOMPropertiesWidget() {
  delete defaultScale;
  delete _ui;
}

void SOMPropertiesWidget::diffusionMethodChange() {}

void SOMPropertiesWidget::scalingMethodChange(QAbstractButton *button) const {
  if (button == singleColorScale) {
    editGradients->setEnabled(false);
  } else {
    editGradients->setEnabled(true);
  }
}

void SOMPropertiesWidget::graphChanged(tlp::Graph *graph) {

  vector<string> types;
  types.push_back("double");
  tlp::GraphPropertiesSelectionWidget s;
  s.setWidgetParameters(graph, types);
  // Init the set of colors with all the possible properties.
  gradientManager.init(s.getCompleteStringsList());
}

tlp::ColorScale *SOMPropertiesWidget::getPropertyColorScale(const std::string &) {
  return defaultScale;
}

SOMPropertiesWidget::SizeMappingType SOMPropertiesWidget::getSizeMapping() const {
  if (noNodeSizeMappingRadioButton->isChecked()) {
    return NoSizeMapping;
  } else {
    return RealNodeSizeMapping;
  }
}

void SOMPropertiesWidget::treatEvents(const std::vector<Event> &) {
  view->updateDefaultColorProperty();
}

DataSet SOMPropertiesWidget::getData() const {
  DataSet data;

  // Save grid state.
  data.set("gridWidth", getGridWidth());
  data.set("gridHeight", getGridHeight());
  data.set("oppositeConnected", getOppositeConnected());
  data.set("connectivity", _ui->nodeConnectivityComboBox->currentIndex());
  // Learning rate properties.
  data.set("learningRate", getLearningRateValue());

  // Diffusion rate properties.
  data.set("diffusionMethod", _ui->diffusionRateComputationMethodComboBox->currentIndex());
  data.set("maxDistance", getMaxDistanceValue());
  data.set("diffusionRate", getDiffusionRateValue());

  // Representation properties
  data.set("performMapping", getAutoMapping());
  data.set("linkColors", getLinkColor());

  // SizeMapping
  data.set("useSizeMapping", getSizeMapping() == SOMPropertiesWidget::RealNodeSizeMapping);

  // Animation
  data.set("withAnimation", useAnimation());

  // Save current properties.
  vector<string> properties = dimensionConfigurationWidget->getSelectedProperties();

  if (!properties.empty()) {
    // Use QStringList class to store a list in a string
    QStringList stringlist;

    for (const auto &p : properties) {
      stringlist.push_back(tlpStringToQString(p));
    }

    data.set("properties", QStringToTlpString(stringlist.join(";")));
  }

  // Save iteration number
  data.set("iterationNumber", dimensionConfigurationWidget->number());

  // Save color scale
  DataSet colorScaleDataSet;

  QStringList colorsList;

  for (const auto &it : defaultScale->getColorMap()) {
    colorsList.push_back(tlpStringToQString(ColorType::toString(it.second)));
  }

  colorScaleDataSet.set("colorList", QStringToTlpString(colorsList.join(";")));
  colorScaleDataSet.set("gradient", defaultScale->isGradient());
  data.set("defaultScale", colorScaleDataSet);

  return data;
}

void SOMPropertiesWidget::setData(const DataSet &data) {

  double doubleValue = 0;
  uint uintValue = 0;
  bool boolValue = false;
  int intValue = 0;
  // Restore grid state.
  data.get("gridWidth", uintValue);
  _ui->gridWidthSpinBox->setValue(uintValue);
  data.get("gridHeight", uintValue);
  _ui->gridHeightSpinBox->setValue(uintValue);
  data.get("connectivity", intValue);
  _ui->nodeConnectivityComboBox->setCurrentIndex(intValue);
  data.get("oppositeConnected", boolValue);
  _ui->opposedConnectedCheckBox->setChecked(boolValue);

  // Learning rate properties.
  data.get("learningRate", doubleValue);
  _ui->baseLearningRateSpinBox->setValue(doubleValue);

  // Diffusion rate properties.
  data.get("diffusionMethod", intValue);
  _ui->diffusionRateComputationMethodComboBox->setCurrentIndex(intValue);

  data.get("maxDistance", uintValue);
  _ui->maxDistanceSpinBox->setValue(uintValue);
  data.get("diffusionRate", doubleValue);
  _ui->baseDiffusionRateSpinBox->setValue(doubleValue);

  // Representation properties
  data.get("performMapping", boolValue);
  _ui->autoMappingCheckBox->setChecked(boolValue);
  data.get("linkColors", boolValue);
  _ui->colorLinkCheckBox->setChecked(boolValue);

  // SizeMapping
  data.get("useSizeMapping", boolValue);

  if (boolValue) {
    realNodeSizeMappingRadioButton->setChecked(true);
  } else {
    noNodeSizeMappingRadioButton->setChecked(true);
  }

  // Animation
  data.get("withAnimation", boolValue);
  _ui->animationCheckBox->setChecked(boolValue);

  // If there is saved properties reload them.
  if (data.exists("properties")) {
    string propertiesString;
    data.get("properties", propertiesString);
    // Use QString split function to parse string.
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    QStringList list = tlpStringToQString(propertiesString).split(";", Qt::SkipEmptyParts);
#else
    QStringList list = tlpStringToQString(propertiesString).split(";", QString::SkipEmptyParts);
#endif
    vector<string> properties;

    for (const QString &s : list) {
      properties.push_back(QStringToTlpString(s));
    }

    dimensionConfigurationWidget->setOutputPropertiesList(properties);
  }

  data.get("iterationNumber", uintValue);
  dimensionConfigurationWidget->setNumber(uintValue);

  // Reload color scale

  DataSet colorScaleDataSet;
  data.get("defaultScale", colorScaleDataSet);
  string colorsList;
  colorScaleDataSet.get("colorList", colorsList);
  QString colorListQString = tlpStringToQString(colorsList);
  QStringList colorStringList = colorListQString.split(";");
  vector<Color> colors;

  for (const auto &cstr : colorStringList) {
    Color c;
    if (ColorType::fromString(c, QStringToTlpString(cstr))) {
      colors.push_back(c);
    }
  }

  colorScaleDataSet.get("gradient", boolValue);
  // Avoid calling update while data initialization causing segfault
  defaultScale->removeObserver(this);
  defaultScale->setColorScale(colors, boolValue);
  defaultScale->addObserver(this);
}
