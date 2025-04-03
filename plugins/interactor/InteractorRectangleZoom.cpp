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

#include <talipot/MouseInteractors.h>
#include <talipot/NodeLinkDiagramViewInteractor.h>
#include <talipot/MouseBoxZoomer.h>
#include <talipot/NodeLinkDiagramView.h>

#include "../utils/StandardInteractorPriority.h"
#include "../utils/PluginNames.h"
#include "../utils/InteractorIcons.h"

using namespace tlp;

/** \brief Interactor to do a rectangle zoom
 *
 */
class InteractorRectangleZoom : public NodeLinkDiagramViewInteractor {

  public:
    PLUGININFORMATION("InteractorRectangleZoom", "Tulip Team", "01/04/2009",
                      "Rectangle Zoom Interactor", "1.0", "Navigation")
    /**
     * Default constructor
     */
    InteractorRectangleZoom(const tlp::PluginContext *)
        : NodeLinkDiagramViewInteractor(interactorIcon(InteractorType::RectangleZoom),
                                        "Zoom on rectangle",
                                        StandardInteractorPriority::ZoomOnRectangle) {}

    /**
     * Construct chain of responsibility
     */
    void construct() override {
        setConfigurationWidgetText(
            QString("<h3>Zoom on rectangle</h3>") +
            "Zoom on selected rectangle.<br><b>Mouse left</b> down indicates "
            "the first corner.<br> <b>Mouse left</b> up indicates the opposite "
            "corner.<br> <b>Mouse left Doucle click</b> to center the view.");
        push_back(new MousePanNZoomNavigator);
        push_back(new MouseBoxZoomer);
    }

    QCursor cursor() const override {
        return QCursor(Qt::CrossCursor);
    }

    bool isCompatible(const std::string &viewName) const override {
        return ((viewName == NodeLinkDiagramView::viewName) ||
                (viewName == ViewName::HistogramViewName) ||
                (viewName == ViewName::MatrixViewName) ||
                (viewName == ViewName::ParallelCoordinatesViewName) ||
                (viewName == ViewName::PixelOrientedViewName) ||
                (viewName == ViewName::ScatterPlot2DViewName));
    }
};

PLUGIN(InteractorRectangleZoom)
