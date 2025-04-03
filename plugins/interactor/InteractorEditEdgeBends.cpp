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
#include <talipot/MouseSelector.h>
#include <talipot/MouseEdgeBendEditor.h>
#include <talipot/NodeLinkDiagramViewInteractor.h>
#include <talipot/NodeLinkDiagramView.h>

#include "../utils/StandardInteractorPriority.h"
#include "../utils/InteractorIcons.h"

using namespace tlp;

/** \brief Interactor to edit edge bends
 *
 */
class InteractorEditEdgeBends : public NodeLinkDiagramViewInteractor {

  public:
    PLUGININFORMATION("InteractorEditEdgeBends", "Tulip Team", "01/04/2009",
                      "Edit Edge Bends Interactor", "1.0", "Modification")
    /**
     * Default constructor
     */
    InteractorEditEdgeBends(const tlp::PluginContext *)
        : NodeLinkDiagramViewInteractor(interactorIcon(InteractorType::EditEdgeBends),
                                        "Edit edge bends",
                                        StandardInteractorPriority::EditEdgeBends) {}

    /**
     * Construct chain of responsibility
     */
    void construct() override {
        setConfigurationWidgetText(
            QString("<h3>Edit edge bends</h3>") + "Modify edge bends<br/><br/>" +
            "Select edge: <ul><li>use rectangle selection</li></ul>" +
            "Translate bend: <ul><li><b>Mouse left</b> down on a selected bend + moves</li></ul>" +
            "Change source node: <ul><li><b>Drag and drop circle on source node</li></ul>" +
            "Change target node: <ul><li><b>Drag and drop triangle on target node</li></ul>" +
            "Add bend: <ul><li><b>Double click with mouse left</b> click on the selected "
            "edge</li></ul>" +
#if !defined(__APPLE__)
            "Delete bend: <ul><li><b>Ctrl + Mouse left</b> click on a selected bend</li></ul>"
#else
            "Delete bend: <ul><li><b>Alt + Mouse left</b> click on a selected bend</li></ul>"
#endif
        );
        push_back(new MousePanNZoomNavigator);
        push_back(new MouseSelector(Qt::LeftButton, Qt::NoModifier, MouseSelector::EdgesOnly));
        push_back(new MouseEdgeBendEditor);
    }

    bool isCompatible(const std::string &viewName) const override {
        return (viewName == NodeLinkDiagramView::viewName);
    }
};

PLUGIN(InteractorEditEdgeBends)
