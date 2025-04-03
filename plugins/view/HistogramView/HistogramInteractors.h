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

#ifndef HISTOGRAM_INTERACTORS_H
#define HISTOGRAM_INTERACTORS_H

#include <talipot/NodeLinkDiagramViewInteractor.h>
#include "../../utils/PluginNames.h"

using namespace std;

namespace tlp {

class HistogramStatistics;
class HistoStatsConfigWidget;

class HistogramInteractor : public NodeLinkDiagramViewInteractor {

  public:
    HistogramInteractor(const QIcon &icon, const QString &text, const uint priority = 0);

    bool isCompatible(const std::string &viewName) const override;
};

class HistogramInteractorNavigation : public HistogramInteractor {

  public:
    PLUGININFORMATION(InteractorName::HistogramInteractorNavigation, "Tulip Team", "02/04/2009",
                      "Histogram Navigation Interactor", "1.0", "Navigation")

    HistogramInteractorNavigation(const PluginContext *);

    void construct() override;
};

class HistogramInteractorMetricMapping : public HistogramInteractor {

  public:
    PLUGININFORMATION("HistogramInteractorColorMapping", "Tulip Team", "02/04/2009",
                      "Histogram Color Mapping Interactor", "1.0", "Information")

    HistogramInteractorMetricMapping(const PluginContext *);

    void construct() override;
};

class HistogramInteractorStatistics : public HistogramInteractor {

  public:
    PLUGININFORMATION("HistogramInteractorStatistics", "Tulip Team", "02/04/2009",
                      "Histogram Statistics Interactor", "1.0", "Information")

    HistogramInteractorStatistics(const PluginContext *);
    ~HistogramInteractorStatistics() override;

    void construct() override;
    QWidget *configurationWidget() const override;
    void install(QObject *target) override;

  private:
    HistoStatsConfigWidget *histoStatsConfigWidget;
    HistogramStatistics *histoStatistics;
};

/**
 *  \brief interactor to get information about an element of the graph
 */
class HistogramInteractorGetInformation : public NodeLinkDiagramViewInteractor {
  public:
    PLUGININFORMATION("HistogramInteractorGetInformation", "Tulip Team", "18/06/2015",
                      "Get Information Interactor", "1.0", "Information")
    /**
     * Default constructor
     */
    HistogramInteractorGetInformation(const tlp::PluginContext *);

    /**
     * Construct chain of responsibility
     */
    void construct() override;

    bool isCompatible(const std::string &viewName) const override;
};
}

#endif // HISTOGRAM_INTERACTORS_H
