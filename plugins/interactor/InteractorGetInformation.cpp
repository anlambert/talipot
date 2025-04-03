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
#include <talipot/MouseShowElementInfo.h>
#include <talipot/NodeLinkDiagramViewInteractor.h>
#include <talipot/NodeLinkDiagramView.h>

#include "../utils/StandardInteractorPriority.h"
#include "../utils/PluginNames.h"
#include "../utils/InteractorIcons.h"

using namespace tlp;

/** \brief Interactor to get information about an element of the graph
 *
 */
class InteractorGetInformation : public NodeLinkDiagramViewInteractor {

  public:
    PLUGININFORMATION("InteractorGetInformation", "Tulip Team", "01/04/2009",
                      "Get Information Interactor", "1.0", "Information")
    /**
     * Default constructor
     */
    InteractorGetInformation(const tlp::PluginContext *)
        : NodeLinkDiagramViewInteractor(interactorIcon(InteractorType::GetInformation),
                                        "Display node or edge properties",
                                        StandardInteractorPriority::GetInformation) {}

    /**
     * Construct chain of responsibility
     */
    void construct() override {
        setConfigurationWidgetText(
            QString("<h3>Display node or edge properties</h3>") +
            "<b>Mouse left click</b> on an element to display its "
            "properties.<br/>then <b>Mouse left click</b> on a row to edit the "
            "corresponding value.");
        push_back(new MousePanNZoomNavigator);
        push_back(new MouseShowElementInfo);
    }

    bool isCompatible(const std::string &viewName) const override {
        return ((viewName == NodeLinkDiagramView::viewName) ||
                (viewName == ViewName::PixelOrientedViewName));
    }
};

PLUGIN(InteractorGetInformation)
