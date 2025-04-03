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
#include <talipot/NodeLinkDiagramView.h>

#include "../utils/StandardInteractorPriority.h"
#include "../utils/PluginNames.h"
#include "../utils/InteractorIcons.h"

using namespace tlp;

/** \brief Interactor to navigate on the graph
 *
 */
class InteractorNavigation : public NodeLinkDiagramViewInteractor {

  public:
    PLUGININFORMATION(InteractorName::InteractorNavigation, "Tulip Team", "01/04/2009",
                      "Navigation Interactor", "1.0", "Navigation")
    /**
     * Default constructor
     */
    InteractorNavigation(const tlp::PluginContext *)
        : NodeLinkDiagramViewInteractor(interactorIcon(InteractorType::Navigation),
                                        "Navigate in graph",
                                        StandardInteractorPriority::Navigation) {}

    /**
     * Construct chain of responsibility
     */
    void construct() override {
        setConfigurationWidgetText(
            QString("<h3>Navigate in graph</h3>") + "3D Navigation in the graph<br/><br/>" +
            "Translation: <ul><li><b>Mouse left</b> down + moves</li><li>or <b>Arrow</b> keys "
            "down</li></ul>" +
            "X or Y rotation: <ul><li><b>Shift + Mouse left</b> down + up/down or left/right "
            "moves</li></ul>" +
#if !defined(__APPLE__)
            "Z rotation: <ul><li><b>Ctrl + Mouse left</b> down + left/right moves</li><li> or "
            "<b>Insert</b> key</li></ul>" +
            "Zoom/Unzoom: <ul><li><b>Ctrl + Mouse left</b> down + up/down moves</li><li> or <b>Pg "
            "up/Pg down</b> keys</li></ul>"
#else
            "Z rotation: <ul><li><b>Alt + Mouse left</b> down + left/right moves</li><li> or "
            "<b>Insert</b> key</li></ul>" +
            "Zoom/Unzoom: <ul><li><b>Alt + Mouse left</b> down + up/down moves</li><li> or <b>Pg "
            "up/Pg "
            "down</b> keys</li></ul>"
#endif
            + "Meta node navigation: <ul><li><b>double Mouse left click</b> go inside the "
              "metanode</li>" +
            "<li><b>Ctrl + double Mouse left click</b> go outside the metanode</li></ul>");
        push_back(new MouseNKeysNavigator);
    }

    QCursor cursor() const override {
        return QCursor(Qt::OpenHandCursor);
    }

    bool isCompatible(const std::string &viewName) const override {
        return ((viewName == NodeLinkDiagramView::viewName) ||
                (viewName == ViewName::MatrixViewName) ||
                (viewName == ViewName::ParallelCoordinatesViewName));
    }
};

PLUGIN(InteractorNavigation)
