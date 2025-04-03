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

#ifndef PARALLEL_COORDINATES_INTERACTORS_H
#define PARALLEL_COORDINATES_INTERACTORS_H

#include <talipot/NodeLinkDiagramViewInteractor.h>

#include <QWidget>

namespace Ui {
class AxisSlidersOptions;
}

namespace tlp {

/** \brief Interactor abstract class for ParallelCoordinates
 *
 */
class ParallelCoordinatesInteractor : public NodeLinkDiagramViewInteractor {

  public:
    ParallelCoordinatesInteractor(const QIcon &icon, const QString &text, const uint priority = 0);
    /**
     * return if this interactor is compatible with given View
     */
    bool isCompatible(const std::string &viewName) const override;
};

/** \brief Parallel coordinates interactor to select elements
 *
 */
class InteractorParallelCoordsSelection : public ParallelCoordinatesInteractor {

  public:
    PLUGININFORMATION("InteractorParallelCoordsSelection", "Tulip Team", "02/04/2009",
                      "Selection Interactor", "1.0", "Selection")

    /**
     * Default constructor
     */
    InteractorParallelCoordsSelection(const tlp::PluginContext *);

    /**
     * Construct chain of responsibility
     */
    void construct() override;
};

/** \brief Parallel coordinates interactor to highlite elements
 *
 */
class InteractorHighLighter : public ParallelCoordinatesInteractor {

  public:
    PLUGININFORMATION("InteractorHighLiter", "Tulip Team", "02/04/2009",
                      "Element Highliter Interactor", "1.0", "Information")

    /**
     * Default constructor
     */
    InteractorHighLighter(const tlp::PluginContext *);

    /**
     * Construct chain of responsibility
     */
    void construct() override;
};

/** \brief Parallel coordinates interactor to swap axis
 *
 */
class InteractorAxisSwapper : public ParallelCoordinatesInteractor {

  public:
    PLUGININFORMATION("InteractorAxisSwapper", "Tulip Team", "02/04/2009", "Axis Swap Interactor",
                      "1.0", "Modification")

    /**
     * Default constructor
     */
    InteractorAxisSwapper(const tlp::PluginContext *);

    /**
     * Construct chain of responsibility
     */
    void construct() override;
};

/** \brief Parallel coordinates interactor to have sliders on axis
 *
 */

class AxisSliderOptions : public QWidget {

    Q_OBJECT

    Ui::AxisSlidersOptions *_ui;

  public:
    AxisSliderOptions(QWidget *parent = nullptr);
    ~AxisSliderOptions();

  signals:
    void resetSliders();
};

class InteractorAxisSliders : public ParallelCoordinatesInteractor {

    AxisSliderOptions *configwidget;

  public:
    PLUGININFORMATION("InteractorAxisSliders", "Tulip Team", "02/04/2009",
                      "Axis Sliders Interactor", "1.0", "Modification")

    /**
     * Default constructor
     */
    InteractorAxisSliders(const tlp::PluginContext *);
    ~InteractorAxisSliders();

    /**
     * Construct chain of responsibility
     */
    void construct() override;
    virtual QWidget *configurationWidget() const override;
};

/** \brief Parallel coordinates interactor to have boxplot
 *
 */
class InteractorBoxPlot : public ParallelCoordinatesInteractor {

  public:
    PLUGININFORMATION("InteractorBoxPlot", "Tulip Team", "02/04/2009", "Box Plot Interactor", "1.0",
                      "Information")

    /**
     * Default constructor
     */
    InteractorBoxPlot(const tlp::PluginContext *);

    /**
     * Construct chain of responsibility
     */
    void construct() override;
};

/** \brief Parallel coordinates interactor to show element properties when clicking on it
 *
 */
class InteractorShowElementInfo : public ParallelCoordinatesInteractor {

  public:
    PLUGININFORMATION("InteractorShowElementInfo", "Tulip Team", "02/04/2009",
                      "Show Element Info Interactor", "1.0", "Information")

    /**
     * Default constructor
     */
    InteractorShowElementInfo(const tlp::PluginContext *);

    /**
     * Construct chain of responsibility
     */
    void construct() override;
};

/** \brief Parallel coordinates interactor to modify space between two consecutive axis by using the
 * mouse wheel
 *
 */
class InteractorAxisSpacer : public ParallelCoordinatesInteractor {

  public:
    PLUGININFORMATION("InteractorAxisSpacer", "Tulip Team", "02/04/2009", "Axis Spacer Interactor",
                      "1.0", "Modification")

    /**
     * Default constructor
     */
    InteractorAxisSpacer(const tlp::PluginContext *);

    /**
     * Construct chain of responsibility
     */
    void construct() override;
};
}

#endif // PARALLEL_COORDINATES_INTERACTORS_H
