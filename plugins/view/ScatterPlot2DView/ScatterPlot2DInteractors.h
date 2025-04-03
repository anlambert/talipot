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

#ifndef SCATTER_PLOT2D_INTERACTORS_H
#define SCATTER_PLOT2D_INTERACTORS_H

#include <talipot/NodeLinkDiagramViewInteractor.h>
#include "../../utils/PluginNames.h"

namespace tlp {

class ScatterPlotCorrelCoeffSelectorOptionsWidget;

class ScatterPlot2DInteractor : public NodeLinkDiagramViewInteractor {

  public:
    ScatterPlot2DInteractor(const QIcon &icon, const QString &text, const uint priority = 0);

    bool isCompatible(const std::string &viewName) const override;
};

class ScatterPlot2DInteractorNavigation : public ScatterPlot2DInteractor {

  public:
    PLUGININFORMATION(InteractorName::ScatterPlot2DInteractorNavigation, "Tulip Team", "02/04/2009",
                      "Scatter Plot 2D Navigation Interactor", "1.0", "Navigation")

    ScatterPlot2DInteractorNavigation(const tlp::PluginContext *);

    void construct() override;
};

class ScatterPlot2DInteractorTrendLine : public ScatterPlot2DInteractor {

  public:
    PLUGININFORMATION("ScatterPlot2DInteractorTrendLine", "Tulip Team", "02/04/2009",
                      "Scatter Plot 2D Trend Line Interactor", "1.0", "Information")

    ScatterPlot2DInteractorTrendLine(const tlp::PluginContext *);

    void construct() override;
};

class ScatterPlot2DInteractorCorrelCoeffSelector : public ScatterPlot2DInteractor {

  public:
    PLUGININFORMATION("ScatterPlot2DInteractorCorrelCoeffSelector", "Tulip Team", "02/04/2009",
                      "Scatter Plot 2D Correlation Coefficient Interactor", "1.0", "Information")

    ScatterPlot2DInteractorCorrelCoeffSelector(const tlp::PluginContext *);
    ~ScatterPlot2DInteractorCorrelCoeffSelector() override;

    void construct() override;

    QWidget *configurationWidget() const override;

  private:
    ScatterPlotCorrelCoeffSelectorOptionsWidget *optionsWidget;
};

/**
 *  \brief interactor to get information about an element of the graph
 */
class ScatterPlot2DInteractorGetInformation : public NodeLinkDiagramViewInteractor {
  public:
    PLUGININFORMATION("ScatterPlot2DInteractorGetInformation", "Tulip Team", "18/06/2015",
                      "Get Information Interactor", "1.0", "Information")
    /**
     * Default constructor
     */
    ScatterPlot2DInteractorGetInformation(const tlp::PluginContext *);

    /**
     * Construct chain of responsibility
     */
    void construct() override;

    bool isCompatible(const std::string &viewName) const override;
};
}

#endif // SCATTER_PLOT2D_INTERACTORS_H
