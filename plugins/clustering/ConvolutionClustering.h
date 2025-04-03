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

#ifndef CONVOLUTION_CLUSTERING_H
#define CONVOLUTION_CLUSTERING_H

#include <vector>

#include <talipot/PluginHeaders.h>

/** This plugin allow the discretization and the filtering of the distribution of
 * a node metric using convolution.
 *
 * A detailed usage of this procedure is detailed in :
 *
 * D. Auber, M. Delest and Y. Chiricota \n
 * "Strahler based graph clustering using convolution",\n
 * Published by the IEEE Computer Society, \n
 * 2004.
 *
 */

namespace tlp {

class ConvolutionClustering : public tlp::DoubleAlgorithm {
  public:
    PLUGININFORMATION(
        "Convolution", "David Auber", "14/08/2001",
        "Discretization and filtering of the distribution of a node metric using a convolution.",
        "2.1", "Clustering")
    ConvolutionClustering(tlp::PluginContext *context);
    bool run() override;
    bool check(std::string &) override;
    std::vector<double> *getHistogram();
    void setParameters(int histosize, int threshold, int width);
    void getParameters(int &histosize, int &threshold, int &width) const;
    void autoSetParameter();
    std::list<int> getLocalMinimum();

  private:
    void getClusters(const std::vector<int> &ranges);
    std::vector<double> smoothHistogram;
    std::map<int, int> histogramOfValues;
    int histosize, threshold, width;
    tlp::NumericProperty *metric;
};
}

#endif // CONVOLUTION_CLUSTERING_H
