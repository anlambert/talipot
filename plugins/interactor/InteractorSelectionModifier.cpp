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
#include <talipot/MouseSelectionEditor.h>
#include <talipot/NodeLinkDiagramView.h>

#include "../utils/StandardInteractorPriority.h"
#include "../utils/InteractorIcons.h"

using namespace tlp;

/** \brief Interactor to move/reshape
 *
 */
class InteractorSelectionModifier : public NodeLinkDiagramViewInteractor {

  public:
    PLUGININFORMATION("InteractorSelectionModifier", "Tulip Team", "01/04/2009",
                      "Selection Modifier Interactor", "1.0", "Modification")
    /**
     * Default constructor
     */
    InteractorSelectionModifier(const tlp::PluginContext *)
        : NodeLinkDiagramViewInteractor(interactorIcon(InteractorType::SelectionModifier),
                                        "Move/Reshape rectangle selection",
                                        StandardInteractorPriority::RectangleSelectionModifier) {}

    /**
     * Construct chain of responsibility
     */
    void construct() override {
        setConfigurationWidgetText(
            QString("<h3>Move/Reshape rectangle selection</h3>") + "Modify selection<br/><br/>" +
            "Resize : <ul><li><b>Mouse left</b> down on triangle + moves</li></ul>" +
            "<ul><li><b>Mouse left</b> down on square + moves</li></ul>" +
            "Only change node size : <ul><li><b>Ctrl + Mouse left</b> down on triangle + "
            "moves</li></ul>" +
            "Only change selection size : <ul><li><b>Shift + Mouse left</b> down on triangle + "
            "moves</li></ul>" +
            "Rotate : <ul><li><b>Mouse left</b> down on circle + moves</li></ul>" +
            "Only rotate nodes : <ul><li><b>Ctrl + Mouse left</b> down on circle + "
            "moves</li></ul>" +
            "Only rotate selection : <ul><li><b>Shift + Mouse left</b> down on circle + "
            "moves</li></ul>" +
            "Translate : <ul><li><b>Mouse left</b> down inside rectangle + moves</li></ul>" +
            "Align vertically/horizontally : <ul><li><b>Mouse left</b> click on two arrows icon in "
            "top "
            "right zone</li></ul>" +
            "Align left/right/top/bottom : <ul><li><b>Mouse left</b> click on simple arrow icon in "
            "top "
            "right zone</li></ul>");
        push_back(new MousePanNZoomNavigator);
        push_back(new MouseSelector);
        push_back(new MouseSelectionEditor);
    }

    QCursor cursor() const override {
        return QCursor(Qt::CrossCursor);
    }

    bool isCompatible(const std::string &viewName) const override {
        return (viewName == NodeLinkDiagramView::viewName);
    }
};

PLUGIN(InteractorSelectionModifier)
