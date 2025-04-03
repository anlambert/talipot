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

#include "PixelOrientedInteractors.h"
#include "PixelOrientedViewNavigator.h"

#include "../../utils/StandardInteractorPriority.h"
#include "../../utils/InteractorIcons.h"

using namespace std;

namespace tlp {

PixelOrientedInteractor::PixelOrientedInteractor(const QIcon &icon, const QString &text,
                                                 const uint priority)
    : NodeLinkDiagramViewInteractor(icon, text, priority) {}

bool PixelOrientedInteractor::isCompatible(const std::string &viewName) const {
    return (viewName == ViewName::PixelOrientedViewName);
}

PLUGIN(PixelOrientedInteractorNavigation)

PixelOrientedInteractorNavigation::PixelOrientedInteractorNavigation(const PluginContext *)
    : PixelOrientedInteractor(interactorIcon(InteractorType::Navigation), "Navigate in view",
                              StandardInteractorPriority::Navigation) {}

void PixelOrientedInteractorNavigation::construct() {
    setConfigurationWidgetText(
        QString("<html><head>") + "<title></title>" + "</head>" + "<body>" +
        "<h3>View navigation interactor</h3>" +
        "<p>This interactor allows to navigate in the pixel oriented view.</p>" +
        "<p>When there is more than one graph properties selected, the corresponding pixel "
        "oriented "
        "previews are generated and displayed in a matrix form. By <b>double clicking on a pixel "
        "oriented preview, " +
        "this one is displayed in fullscreen </b> in a more detailed way and the others "
        "interactors "
        "become available. To go back to the pixel oriented previews matrix, double click anywhere "
        "in the view.</p>" +
        "<p>Otherwise, this interactor offers the same functionalities as the one in the \"Node "
        "Link Diagram view\". The commands are described below :</p>" +
        "<b>Ctrl + Mouse up/down</b> : zoom<br>" +
        "<b>Ctrl + Mouse left/right</b> : z rotation<br>" + "<b>Shift + Mouse</b> : rotation<br>" +
        "<b>Key up/down</b> : up/down<br>" + "<b>Key left/right</b> : left/right<br>" +
        "<b>Key page up/down</b> : zoom<br>" + "<b>Key insert</b> : rotate<br>" + "</body>" +
        "</html>");
    push_back(new PixelOrientedViewNavigator);
    push_back(new MouseNKeysNavigator);
}
}
