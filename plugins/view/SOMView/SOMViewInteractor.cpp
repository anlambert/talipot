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

#include "SOMViewInteractor.h"
#include <talipot/MouseInteractors.h>
#include <talipot/MouseSelector.h>
#include <talipot/MouseShowElementInfo.h>
#include "ThresholdInteractor.h"

#include "../../utils/StandardInteractorPriority.h"
#include "../../utils/InteractorIcons.h"

using namespace std;
using namespace tlp;

PLUGIN(SOMViewNavigation)
PLUGIN(SOMViewSelection)
PLUGIN(SOMViewProperties)
PLUGIN(SOMViewThreshold)

SOMViewInteractor::SOMViewInteractor(const QIcon &icon, const QString &text, const uint priority)
    : NodeLinkDiagramViewInteractor(icon, text, priority) {}

bool SOMViewInteractor::isCompatible(const std::string &viewName) const {
    return (viewName == ViewName::SOMViewName);
}

SOMViewNavigation::SOMViewNavigation(PluginContext *)
    : SOMViewInteractor(interactorIcon(InteractorType::Navigation), "Navigate",
                        StandardInteractorPriority::Navigation) {}

void SOMViewNavigation::construct() {
    push_back(new MouseNKeysNavigator());
    push_back(new EditColorScaleInteractor());
}

SOMViewSelection::SOMViewSelection(PluginContext *)
    : SOMViewInteractor(interactorIcon(InteractorType::Selection), "Select",
                        StandardInteractorPriority::RectangleSelection) {}

void SOMViewSelection::construct() {
    push_back(new MouseSelector());
    push_back(new MousePanNZoomNavigator());
    push_back(new EditColorScaleInteractor());
}

SOMViewProperties::SOMViewProperties(PluginContext *)
    : SOMViewInteractor(interactorIcon(InteractorType::GetInformation), "Properties",
                        StandardInteractorPriority::GetInformation) {}

void SOMViewProperties::construct() {
    push_back(new MouseShowElementInfo());
    push_back(new MousePanNZoomNavigator());
    push_back(new EditColorScaleInteractor());
}

SOMViewThreshold::SOMViewThreshold(PluginContext *)
    : SOMViewInteractor(QIcon(":/i_slider.png"), "Threshold Selection",
                        StandardInteractorPriority::ViewInteractor1) {}

void SOMViewThreshold::construct() {
    setConfigurationWidgetText(QString(
        "<H1>Threshold Interactor</H1><p>This interactor is used to select nodes with a value "
        "between those indicated by the two sliders</p><p>Move the each slider to change the "
        "bound.</p><p>Press the Ctrl button to add the new threshold selection to the current "
        "selection. If Ctrl is not pressed the old selection will be replaced</p>"));
    push_back(new MouseNKeysNavigator());
    push_back(new ThresholdInteractor());
}
