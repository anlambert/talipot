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

#include "HistoStatsConfigWidget.h"
#include "ui_HistoStatsConfigWidget.h"

using namespace std;

namespace tlp {

HistoStatsConfigWidget::HistoStatsConfigWidget(QWidget *parent)
    : QWidget(parent), _ui(new Ui::HistoStatsConfigWidget) {
    _ui->setupUi(this);
    connect(_ui->applyButton, &QAbstractButton::clicked, this,
            &HistoStatsConfigWidget::computeAndDrawInteractor);
}

HistoStatsConfigWidget::~HistoStatsConfigWidget() {
    delete _ui;
}

double HistoStatsConfigWidget::getSampleStep() const {
    return _ui->sampleStepSB->value();
}

double HistoStatsConfigWidget::getBandwidth() const {
    return _ui->windowWidthSP->value();
}

QString HistoStatsConfigWidget::getKernelFunctionName() const {
    return _ui->kernelFunctionsList->currentText();
}

bool HistoStatsConfigWidget::densityEstimation() const {
    return _ui->densityEstimationGB->isChecked();
}

bool HistoStatsConfigWidget::displayMeanAndStandardDeviation() const {
    return _ui->displayMeanAndSdGB->isChecked();
}

bool HistoStatsConfigWidget::nodesSelection() const {
    return _ui->nodesSelectionGB->isChecked();
}

double HistoStatsConfigWidget::getSelectionLowerBound() const {
    return getBoundFromString(_ui->lowerBoundCB->currentText());
}

double HistoStatsConfigWidget::getSelectionUpperBound() const {
    return getBoundFromString(_ui->upperBoundCB->currentText());
}

void HistoStatsConfigWidget::setMinMaxMeanAndSd(double min, double max, double mean,
                                                double standardDeviation) {
    bool changed = false;

    if (this->min != min) {
        this->min = min;
        changed = true;
    }

    if (this->max != max) {
        this->max = max;
        changed = true;
    }

    if (this->mean != mean) {
        this->mean = mean;
        changed = true;
    }

    if (this->standardDeviation != standardDeviation) {
        this->standardDeviation = standardDeviation;
        changed = true;
    }

    if (changed) {
        _ui->meanValue->setText(QString::number(mean));
        _ui->standardDeviationValue->setText(QString::number(standardDeviation));
        _ui->lowerBoundCB->clear();
        _ui->upperBoundCB->clear();
        _ui->lowerBoundCB->addItem("min");
        _ui->upperBoundCB->addItem("min");
        bool m3sdOk = false;
        bool m2sdOk = false;

        if ((mean - (3 * standardDeviation)) > min) {
            _ui->lowerBoundCB->addItem("m - 3sd");
            _ui->upperBoundCB->addItem("m - 3sd");
            m3sdOk = true;
        }

        if ((mean - (2 * standardDeviation)) > min) {
            _ui->lowerBoundCB->addItem("m - 2sd");
            _ui->upperBoundCB->addItem("m - 2sd");
            m2sdOk = true;
        }

        _ui->lowerBoundCB->addItem("m - sd");
        _ui->upperBoundCB->addItem("m - sd");
        _ui->lowerBoundCB->addItem("m");
        _ui->upperBoundCB->addItem("m");
        _ui->lowerBoundCB->addItem("m + sd");
        _ui->upperBoundCB->addItem("m + sd");

        if (m2sdOk) {
            _ui->lowerBoundCB->addItem("m + 2sd");
            _ui->upperBoundCB->addItem("m + 2sd");
        }

        if (m3sdOk) {
            _ui->lowerBoundCB->addItem("m + 3sd");
            _ui->upperBoundCB->addItem("m + 3sd");
        }

        _ui->lowerBoundCB->addItem("max");
        _ui->upperBoundCB->addItem("max");

        _ui->lowerBoundCB->setCurrentIndex(_ui->lowerBoundCB->findText("m - sd"));
        _ui->upperBoundCB->setCurrentIndex(_ui->upperBoundCB->findText("m + sd"));
    }
}

double HistoStatsConfigWidget::getBoundFromString(const QString &bound) const {
    if (bound == "min") {
        return min;
    } else if (bound == "m - 3sd") {
        return (mean - 3 * standardDeviation);
    } else if (bound == "m - 2sd") {
        return (mean - 2 * standardDeviation);
    } else if (bound == "m - sd") {
        return (mean - standardDeviation);
    } else if (bound == "m") {
        return mean;
    } else if (bound == "m + sd") {
        return (mean + standardDeviation);
    } else if (bound == "m + 2sd") {
        return (mean + 2 * standardDeviation);
    } else if (bound == "m + 3sd") {
        return (mean + 3 * standardDeviation);
    } else {
        return max;
    }
}
}
