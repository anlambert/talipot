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
#include <talipot/MouseSelector.h>
#include <talipot/NodeLinkDiagramView.h>

#include "../utils/StandardInteractorPriority.h"
#include "../utils/PluginNames.h"
#include "../utils/InteractorIcons.h"

using namespace tlp;

/** \brief Interactor to select par of the graph
 *
 */
class InteractorSelection : public NodeLinkDiagramViewInteractor {

  public:
    PLUGININFORMATION("InteractorSelection", "Tulip Team", "01/04/2009", "Selection Interactor",
                      "1.0", "Modification")
    /**
     * Default constructor
     */
    InteractorSelection(const tlp::PluginContext *)
        : NodeLinkDiagramViewInteractor(interactorIcon(InteractorType::Selection),
                                        "Select nodes/edges in a rectangle",
                                        StandardInteractorPriority::RectangleSelection) {}

    /**
     * Construct chain of responsibility
     */
    void construct() override {
        setConfigurationWidgetText(
            QString("<h3>Selection nodes/edges in a rectangle</h3>") +
            "Draw selection rectangle.<br/><b>Mouse left</b> down indicates the first corner, "
            "<b>Mouse "
            "left</b> up indicates the opposite corner,<br/>all graph elements instersecting the "
            "rectangle are selected<br/><br/>" +
#if !defined(__APPLE__)
            "Add to selection: <ul><li><b>Ctrl + Mouse left</b> click on an element</li></ul>" +
#else
            "Add/Remove from selection: <ul><li><b>Alt + Mouse left</b> click</li></ul>" +
#endif
            "Remove from selection: <ul><li><b>Shift + Mouse</b> click</li></ul>");
        push_back(new MousePanNZoomNavigator);
        push_back(new MouseSelector);
    }

    QCursor cursor() const override {
        return Qt::CrossCursor;
    }

    bool isCompatible(const std::string &viewName) const override {
        return ((viewName == NodeLinkDiagramView::viewName) ||
                (viewName == ViewName::HistogramViewName) ||
                (viewName == ViewName::MatrixViewName) ||
                (viewName == ViewName::PixelOrientedViewName) ||
                (viewName == ViewName::ScatterPlot2DViewName));
    }
};

PLUGIN(InteractorSelection)
