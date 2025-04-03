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

#ifndef AXIS_CONFIG_DIALOGS_H
#define AXIS_CONFIG_DIALOGS_H

#include <QDialog>

#include <talipot/ItemsListWidget.h>

class QSpinBox;
class QDoubleSpinBox;
class QComboBox;
class QCheckBox;
class QCloseEvent;

namespace tlp {

class QuantitativeParallelAxis;
class NominalParallelAxis;

// A really simple dialog which allows user to set number of graduations
// and data ordering (ascendent or not) for a particular quantitative axis
class QuantitativeAxisConfigDialog : public QDialog {

    Q_OBJECT

  public:
    QuantitativeAxisConfigDialog(QuantitativeParallelAxis *axis);
    void closeEvent(QCloseEvent *event) override;

  private:
    void initAxisPartitionsTable();

    QuantitativeParallelAxis *axis;
    QSpinBox *nbGrads;
    QDoubleSpinBox *doubleAxisMinValue, *doubleAxisMaxValue;
    QSpinBox *intAxisMinValue, *intAxisMaxValue;
    QComboBox *axisOrder;
    QPushButton *okButton;
    QCheckBox *log10Scale;
};

// A really simple dialog which allows user to set labels order
// on a particular nominative axis
class NominalAxisConfigDialog : public QDialog {

    Q_OBJECT

  public:
    NominalAxisConfigDialog(NominalParallelAxis *axis);
    void closeEvent(QCloseEvent *event) override;

  public slots:

    void pressButtonUp();
    void pressButtonDown();
    void pressButtonLexOrder();

  private:
    NominalParallelAxis *axis;
    QPushButton *okButton;
    QHash<QString, itemInfo> hashDataBase;
    ItemsListWidget *axisLabelsOrder;
};
}

#endif // AXIS_CONFIG_DIALOGS_H
