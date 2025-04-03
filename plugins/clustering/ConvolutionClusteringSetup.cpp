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

#include "ConvolutionClusteringSetup.h"
#include "ui_ConvolutionClusteringSetup.h"
#include "ConvolutionClustering.h"

#include <QPainter>

using namespace std;

namespace tlp {

class HistogramWidget : public QWidget {

  public:
    HistogramWidget(ConvolutionClusteringSetup *dialog, QWidget *parent)
        : QWidget(parent), setupDialog(dialog) {}

    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        vector<double> &histogram = *(setupDialog->getPlugin()->getHistogram());

        if (histogram.size() < 1) {
            setupDialog->abort();
            return;
        }

        double theMax, theMin;
        theMax = histogram[0];
        theMin = histogram[0];

        for (uint i = 1; i < histogram.size(); ++i) {
            if (theMax < histogram[i]) {
                theMax = histogram[i];
            }

            if (theMin > histogram[i]) {
                theMin = histogram[i];
            }
        }

        if (setupDialog->getLogarithmicScale()) {
            theMax = log10(theMax + 1.0);
            theMin = log10(theMin + 1.0);
        }

        // compute axis position
        QFont f("times", 12, QFont::Bold);
        painter.setFont(f);
        painter.setPen(Qt::black);

        // les 20 de plus permetront de placer des l�gendes sur les axes.
        double scale = double(histogram.size()) / 64.0;
        int legendWidth = int(20.0 * scale);
        int borderWidth = int(10.0 * scale);
        int axisWidth = int(15.0 * scale);
        painter.setWindow(0, 0, 2 * histogram.size() + legendWidth,
                          histogram.size() + legendWidth); // defines coordinate system
        painter.fillRect(0, 0, 2 * histogram.size() + legendWidth, histogram.size() + legendWidth,
                         QBrush(QColor(255, 255, 255)));
        // draw bars
        QColor c;
        double histoScale = histogram.size() / theMax;

        for (uint i = 0; i < histogram.size(); i++) {
            c.setHsv(int(i * 360.0 / histogram.size()), 255, 255);
            painter.setBrush(c);
            int height;

            if (setupDialog->getLogarithmicScale()) {
                height = int(log10(1.0 + histogram[i]) * histoScale);
            } else {
                height = int(histogram[i] * histoScale);
            }

            if (height < 1) {
                height = 1;
            }

            painter.drawRect(borderWidth + i * 2, borderWidth + 1 + histogram.size() - height, 2,
                             height);
        }

        // draw axis
        painter.drawLine(borderWidth, borderWidth, borderWidth, histogram.size() + borderWidth);
        painter.drawLine(borderWidth, histogram.size() + borderWidth,
                         2 * histogram.size() + axisWidth, histogram.size() + borderWidth);
        c.setHsv(359, 255, 255);

        // draw local minimum
        list<int> localMinimum = setupDialog->getPlugin()->getLocalMinimum();

        while (!localMinimum.empty()) {
            int i = localMinimum.front();
            localMinimum.pop_front();
            painter.drawLine(borderWidth + i * 2, borderWidth, borderWidth + i * 2,
                             histogram.size() + borderWidth);
        }
    }

  private:
    ConvolutionClusteringSetup *setupDialog;
};

ConvolutionClusteringSetup::ConvolutionClusteringSetup(ConvolutionClustering *convolPlugin,
                                                       QWidget *parent)
    : QDialog(parent), _ui(new Ui::ConvolutionClusteringSetup), convolPlugin(convolPlugin),
      useLogarithmicScale(false) {
    _ui->setupUi(this);
    histogramWidget = new HistogramWidget(this, _ui->Frame3);
    auto *flayout = new QGridLayout(_ui->Frame3);
    flayout->setContentsMargins(1, 1, 1, 1);
    flayout->addWidget(histogramWidget, 0, 0);

    int a, b, c;
    convolPlugin->getParameters(a, b, c);
    _ui->widthSlider->setMinimum(1);
    _ui->widthSlider->setMaximum(a);
    _ui->widthSlider->setValue(c);
    _ui->discretizationSlider->setMinimum(1);
    _ui->discretizationSlider->setMaximum(2 * a);
    _ui->discretizationSlider->setValue(a);

    connect(_ui->widthSlider, &QSlider::valueChanged, this, QOverload<>::of(&QDialog::update));
    connect(_ui->okButton, &QPushButton::pressed, this, QOverload<>::of(&QDialog::accept));
    connect(_ui->cancelButton, &QPushButton::pressed, this, QOverload<>::of(&QDialog::reject));
    connect(_ui->discretizationSlider, &QSlider::valueChanged, this,
            QOverload<>::of(&QDialog::update));
    connect(_ui->discretizationSlider, &QSlider::valueChanged, _ui->LCDNumber1_3,
            QOverload<int>::of(&QLCDNumber::display));
    connect(_ui->CheckBox1, &QCheckBox::toggled, this, &ConvolutionClusteringSetup::setlog);
    connect(_ui->widthSlider, &QSlider::valueChanged, _ui->LCDNumber1,
            QOverload<int>::of(&QLCDNumber::display));
}

ConvolutionClusteringSetup::~ConvolutionClusteringSetup() {
    delete _ui;
}

/*
 * public slot
 */
void ConvolutionClusteringSetup::setlog(bool b) {
    useLogarithmicScale = b;
    update();
}

void ConvolutionClusteringSetup::update() {
    _ui->widthSlider->setMaximum(std::max(_ui->discretizationSlider->value() / 2, 1));
    convolPlugin->setParameters(_ui->discretizationSlider->value(), 0, _ui->widthSlider->value());

    if (histogramWidget) {
        histogramWidget->update();
    }

    QDialog::update();
}
}
