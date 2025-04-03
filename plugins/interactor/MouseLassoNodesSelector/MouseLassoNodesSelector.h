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

#ifndef MOUSE_LASSO_NODES_SELECTOR_H
#define MOUSE_LASSO_NODES_SELECTOR_H

#include <talipot/NodeLinkDiagramViewInteractor.h>
#include <talipot/GlWidget.h>

#include <vector>

namespace tlp {

/** \file
 *  \brief Lasso Selector Interactor

 * This interactor plugin allow to select graph nodes with a lasso.
 * By defining a closed polygon with the mouse above the visualization,
 * all nodes under that polygon and edges connecting these nodes will be selected.
 */
class MouseLassoNodesSelectorInteractor
    : public NodeLinkDiagramViewInteractor { /* GLInteractorComposite*/

  public:
    PLUGININFORMATION("MouseLassoNodesSelectorInteractor", "Antoine Lambert", "19/06/2009",
                      "Mouse Lasso Nodes Selector Interactor", "1.0", "Modification")

    MouseLassoNodesSelectorInteractor(const tlp::PluginContext *);

    void construct() override;

    bool isCompatible(const std::string &viewName) const override;
};

class MouseLassoNodesSelectorInteractorComponent : public GLInteractorComponent {

  public:
    MouseLassoNodesSelectorInteractorComponent();
    ~MouseLassoNodesSelectorInteractorComponent() override;

    bool draw(GlWidget *glWidget) override;
    bool eventFilter(QObject *obj, QEvent *e) override;

  private:
    void selectGraphElementsUnderPolygon(GlWidget *glWidget);

    std::vector<Coord> polygon;
    Coord currentPointerScreenCoord;
    bool drawInteractor;
    Camera *camera;
    Graph *graph;
    BooleanProperty *viewSelection;
    bool dragStarted;
};
}

#endif // MOUSE_LASSO_NODES_SELECTOR_H
