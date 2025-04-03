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
#include "../utils/InteractorIcons.h"

using namespace tlp;

/** \brief Interactor to delete an element
 *
 */
class InteractorDeleteElement : public NodeLinkDiagramViewInteractor {

  public:
    PLUGININFORMATION("InteractorDeleteElement", "Tulip Team", "01/04/2009",
                      "Delete Element Interactor", "1.0", "Modification")
    /**
     * Default constructor
     */
    InteractorDeleteElement(const tlp::PluginContext *)
        : NodeLinkDiagramViewInteractor(interactorIcon(InteractorType::DeleteElement),
                                        "Delete nodes or edges",
                                        StandardInteractorPriority::DeleteElement) {}

    /**
     * Construct chain of responsibility
     */
    void construct() override {
        setConfigurationWidgetText(QString("<h3>Delete nodes or edges</h3>") +
                                   "<b>Mouse left</b> click on an element to delete it.<br/>No "
                                   "deletion confirmation will be asked.");
        push_back(new MousePanNZoomNavigator);
        push_back(new MouseElementDeleter);
    }

    bool isCompatible(const std::string &viewName) const override {
        return (viewName == NodeLinkDiagramView::viewName);
    }
};

PLUGIN(InteractorDeleteElement)
