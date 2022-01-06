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

#include "AxisConfigDialogs.h"
#include "QuantitativeParallelAxis.h"
#include "NominalParallelAxis.h"

#include <talipot/IntegerProperty.h>
#include <talipot/FontIcon.h>
#include <talipot/MaterialDesignIcons.h>

#include <QGridLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>

using namespace std;

namespace tlp {

QuantitativeAxisConfigDialog::QuantitativeAxisConfigDialog(QuantitativeParallelAxis *axis)
    : QDialog(), axis(axis) {
  setWindowTitle("Quantitative Axis Configuration");

  auto *dialogLayout = new QVBoxLayout();
  auto *nbGradsLayout = new QHBoxLayout();
  auto *axisMinLayout = new QHBoxLayout();
  auto *axisMaxLayout = new QHBoxLayout();
  auto *axisOrderLayout = new QHBoxLayout();
  auto *okButtonLayout = new QHBoxLayout();

  nbGrads = new QSpinBox();
  okButton = new QPushButton("OK");
  axisOrder = new QComboBox();
  log10Scale = new QCheckBox("use log scale (base 10)");
  log10Scale->setChecked(axis->hasLog10Scale());

  nbGrads->setRange(10, axis->getAxisHeight());
  nbGrads->setValue(axis->getNbAxisGrad());
  nbGradsLayout->addWidget(new QLabel("Number of ticks"));
  nbGradsLayout->addWidget(nbGrads);

  axisMinLayout->addWidget(new QLabel("Axis min value"));

  if (axis->getAxisDataTypeName() == "int") {
    intAxisMinValue = new QSpinBox();
    intAxisMinValue->setMaximum(int(axis->getAssociatedPropertyMinValue()));
    intAxisMinValue->setMinimum(INT_MIN);
    intAxisMinValue->setValue(int(axis->getAxisMinValue()));
    axisMinLayout->addWidget(intAxisMinValue);
  } else {
    doubleAxisMinValue = new QDoubleSpinBox();
    doubleAxisMinValue->setMaximum(axis->getAssociatedPropertyMinValue());
    doubleAxisMinValue->setMinimum(-DBL_MAX);
    doubleAxisMinValue->setValue(axis->getAxisMinValue());
    axisMinLayout->addWidget(doubleAxisMinValue);
  }

  axisMaxLayout->addWidget(new QLabel("Axis max value"));

  if (axis->getAxisDataTypeName() == "int") {
    intAxisMaxValue = new QSpinBox();
    intAxisMaxValue->setMinimum(int(axis->getAssociatedPropertyMaxValue()));
    intAxisMaxValue->setMaximum(INT_MAX);
    intAxisMaxValue->setValue(int(axis->getAxisMaxValue()));
    axisMaxLayout->addWidget(intAxisMaxValue);
  } else {
    doubleAxisMaxValue = new QDoubleSpinBox();
    doubleAxisMaxValue->setMinimum(axis->getAssociatedPropertyMaxValue());
    doubleAxisMaxValue->setMaximum(DBL_MAX);
    doubleAxisMaxValue->setValue(axis->getAxisMaxValue());
    axisMaxLayout->addWidget(doubleAxisMaxValue);
  }

  axisOrder->addItem("ascending");
  axisOrder->addItem("descending");

  if (axis->hasAscendingOrder()) {
    axisOrder->setCurrentIndex(0);
  } else {
    axisOrder->setCurrentIndex(1);
  }

  axisOrderLayout->addWidget(new QLabel("Axis order"));
  axisOrderLayout->addWidget(axisOrder);

  dialogLayout->addLayout(nbGradsLayout);
  dialogLayout->addLayout(axisMinLayout);
  dialogLayout->addLayout(axisMaxLayout);
  dialogLayout->addLayout(axisOrderLayout);
  dialogLayout->addWidget(log10Scale);
  okButtonLayout->addStretch(1);
  okButtonLayout->addWidget(okButton);
  okButtonLayout->addStretch(1);
  dialogLayout->addLayout(okButtonLayout);

  setLayout(dialogLayout);
  connect(okButton, &QAbstractButton::clicked, this, &QWidget::close);

  resize(300, 100);
}

void QuantitativeAxisConfigDialog::closeEvent(QCloseEvent *) {
  axis->setNbAxisGrad(nbGrads->value());
  axis->setAscendingOrder(axisOrder->currentText() == "ascending");
  axis->setLog10Scale(log10Scale->isChecked());

  if (axis->getAxisDataTypeName() == "int") {
    axis->setAxisMinMaxValues(intAxisMinValue->value(), intAxisMaxValue->value());
  } else {
    axis->setAxisMinMaxValues(doubleAxisMinValue->value(), doubleAxisMaxValue->value());
  }

  axis->redraw();
}

NominalAxisConfigDialog::NominalAxisConfigDialog(NominalParallelAxis *axis)
    : QDialog(), axis(axis) {

  setWindowTitle("Nominal Axis Configuration");

  auto *dialogLayout = new QVBoxLayout();
  auto *buttonsUpDownLayout = new QVBoxLayout();
  auto *labelsOrderLayout = new QHBoxLayout();
  auto *okButtonLayout = new QHBoxLayout();

  okButton = new QPushButton("OK");
  axisLabelsOrder = new ItemsListWidget(this);
  auto *up = new QPushButton(FontIcon::icon(MaterialDesignIcons::ArrowUpBold), "");
  auto *lexOrder = new QPushButton(QString("Lexicographic") + QChar(QChar::LineSeparator) +
                                   QString("     order"));
  auto *down = new QPushButton(FontIcon::icon(MaterialDesignIcons::ArrowDownBold), "");

  buttonsUpDownLayout->addWidget(up);
  buttonsUpDownLayout->addWidget(lexOrder);
  buttonsUpDownLayout->addWidget(down);

  labelsOrderLayout->addWidget(axisLabelsOrder);
  labelsOrderLayout->addLayout(buttonsUpDownLayout);

  dialogLayout->addWidget(new QLabel("Axis Labels Order"));
  dialogLayout->addLayout(labelsOrderLayout);
  okButtonLayout->addStretch(1);
  okButtonLayout->addWidget(okButton);
  okButtonLayout->addStretch(1);
  dialogLayout->addLayout(okButtonLayout);

  vector<string> labelsOrder = axis->getLabelsOrder();

  for (const auto &l : reversed(labelsOrder)) {
    axisLabelsOrder->addItem(tlpStringToQString(l));
  }

  setLayout(dialogLayout);

  connect(okButton, &QAbstractButton::clicked, this, &QWidget::close);
  connect(up, &QAbstractButton::clicked, this, &NominalAxisConfigDialog::pressButtonUp);
  connect(down, &QAbstractButton::clicked, this, &NominalAxisConfigDialog::pressButtonDown);
  connect(lexOrder, &QAbstractButton::clicked, this, &NominalAxisConfigDialog::pressButtonLexOrder);
}

void NominalAxisConfigDialog::closeEvent(QCloseEvent *) {
  vector<string> labelsOrder;

  for (int i = 0; i < axisLabelsOrder->count(); ++i) {
    labelsOrder.push_back(QStringToTlpString(axisLabelsOrder->item(i)->text()));
  }

  reverse(labelsOrder.begin(), labelsOrder.end());
  axis->setLabelsOrder(labelsOrder);
  axis->redraw();
}

void NominalAxisConfigDialog::pressButtonUp() {
  if (axisLabelsOrder->count() > 0) {
    int row = axisLabelsOrder->currentRow();

    if (row > 0) {
      QString s = axisLabelsOrder->currentItem()->text();
      QString s2 = axisLabelsOrder->item(row - 1)->text();
      axisLabelsOrder->deleteItemList(axisLabelsOrder->item(row - 1));
      axisLabelsOrder->deleteItemList(axisLabelsOrder->item(row - 1));
      axisLabelsOrder->insertItem(row - 1, s2);
      axisLabelsOrder->insertItem(row - 1, s);
      axisLabelsOrder->setCurrentRow(row - 1);
    }
  }
}

void NominalAxisConfigDialog::pressButtonDown() {
  if (axisLabelsOrder->count() > 0) {
    int row = axisLabelsOrder->currentRow();

    if (row != -1 && row < (axisLabelsOrder->count() - 1)) {
      QString s = axisLabelsOrder->currentItem()->text();
      QString s2 = axisLabelsOrder->item(row + 1)->text();
      axisLabelsOrder->deleteItemList(axisLabelsOrder->item(row));
      axisLabelsOrder->deleteItemList(axisLabelsOrder->item(row));
      axisLabelsOrder->insertItem(row, s);
      axisLabelsOrder->insertItem(row, s2);
      axisLabelsOrder->setCurrentRow(row + 1);
    }
  }
}

void NominalAxisConfigDialog::pressButtonLexOrder() {
  axisLabelsOrder->clear();
  static uint clickCount = 0;
  vector<string> labelsOrder = axis->getLabelsOrder();
  std::sort(labelsOrder.begin(), labelsOrder.end());

  if (++clickCount % 2 == 1) {
    for (const auto &l : labelsOrder) {
      axisLabelsOrder->addItem(tlpStringToQString(l));
    }
  } else {
    for (const auto &l : reversed(labelsOrder)) {
      axisLabelsOrder->addItem(tlpStringToQString(l));
    }
  }
}
}
