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

#ifndef CONVOLUTION_CLUSTERING_SETUP_H
#define CONVOLUTION_CLUSTERING_SETUP_H

#include <QDialog>

namespace Ui {
class ConvolutionClusteringSetup;
}

class QWidget;

namespace tlp {

class ConvolutionClustering;

class ConvolutionClusteringSetup : public QDialog {
    Q_OBJECT

    Ui::ConvolutionClusteringSetup *_ui;

  public:
    ConvolutionClusteringSetup(ConvolutionClustering *convolPlugin, QWidget *parent = nullptr);
    ~ConvolutionClusteringSetup() override;

    ConvolutionClustering *getPlugin() const {
        return convolPlugin;
    }
    bool getLogarithmicScale() const {
        return useLogarithmicScale;
    }
    void abort() {
        reject();
    }

  public slots:
    void update();
    void setlog(bool);

  private:
    QWidget *histogramWidget;
    ConvolutionClustering *convolPlugin;
    bool useLogarithmicScale;
};
}
#endif // CONVOLUTION_CLUSTERING_SETUP_H
