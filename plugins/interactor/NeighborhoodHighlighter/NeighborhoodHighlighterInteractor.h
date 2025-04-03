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

#ifndef NEIGHBORHOOD_HIGHLIGHTER_INTERACTOR_H
#define NEIGHBORHOOD_HIGHLIGHTER_INTERACTOR_H

#include "NeighborhoodHighlighterConfigWidget.h"
#include "NodeNeighborhoodView.h"
#include "../../utils/StandardInteractorPriority.h"

#include <QObject>

#include <talipot/GLInteractor.h>
#include <talipot/GlGraph.h>
#include <talipot/GlLayer.h>
#include <talipot/GlScene.h>

namespace tlp {

class AdditionalGlSceneAnimation;

/** \file
 *  \brief Node Neighbourhood Highlighter

 * This interactor plugin allow to get information regarding the neighbourhood of a node by
 highlighting
 * the nodes connected to it. A "Bring and Go" feature is also implemented allowing to navigate
 */
class NeighborhoodHighlighterInteractor : public GLInteractorComposite {

  public:
    PLUGININFORMATION("NeighborhoodHighlighterInteractor", "Antoine Lambert", "19/05/2009",
                      "Node neighborhood highlighter", "1.0", "Navigation")

    NeighborhoodHighlighterInteractor(const PluginContext *);

    ~NeighborhoodHighlighterInteractor() override;

    void construct() override;

    QWidget *configurationWidget() const override {
        return configWidget;
    }

    uint priority() const override {
        return StandardInteractorPriority::NeighborhoodHighlighter;
    }

    bool isCompatible(const std::string &viewName) const override;

  private:
    NeighborhoodHighlighterConfigWidget *configWidget;
};

class NeighborhoodHighlighter : public GLInteractorComponent {

    Q_OBJECT

  public:
    NeighborhoodHighlighter();

    NeighborhoodHighlighter(const NeighborhoodHighlighter &neighborhoodHighlighter);

    ~NeighborhoodHighlighter() override;

    bool eventFilter(QObject *widget, QEvent *e) override;

    bool draw(GlWidget *glWidget) override;

    void viewChanged(View *view) override;

    void setConfigWidget(NeighborhoodHighlighterConfigWidget *configWidget) {
        this->configWidget = configWidget;
    }

  public slots:

    void updateNeighborhoodGraph();

    void morphCircleAlphaAnimStep(int animStep);

  private:
    node selectNodeInOriginalGraph(GlWidget *glWidget, int x, int y);

    void buildNeighborhoodGraph(node n, Graph *g);

    void computeNeighborhoodGraphCircleLayout();

    float computeNeighborhoodGraphRadius(LayoutProperty *neighborhoodGraphLayoutProp);

    void cleanupNeighborhoodGraph();

    bool selectInAugmentedDisplayGraph(const int x, const int y, SelectedEntity &);

    void updateNeighborhoodGraphLayoutAndColors();

    void updateGlNeighborhoodGraph();

    void computeNeighborhoodGraphBoundingBoxes();

    void performZoomAndPan(const BoundingBox &destBB,
                           AdditionalGlSceneAnimation *additionalAnimation = nullptr);

    void morphCircleAlpha(unsigned char startAlpha, unsigned endAlpha, int nbAnimationSteps = 40);

    void checkIfGraphHasChanged();

    Graph *originalGraph;
    GlGraph *originalGlGraph;
    node selectedNode;
    NodeNeighborhoodView *neighborhoodGraph;
    GlGraph *glNeighborhoodGraph;
    Coord circleCenter;

    node neighborhoodGraphCentralNode;
    LayoutProperty *neighborhoodGraphLayout;
    LayoutProperty *neighborhoodGraphCircleLayout;
    LayoutProperty *neighborhoodGraphOriginalLayout;
    ColorProperty *neighborhoodGraphColors;
    ColorProperty *neighborhoodGraphBackupColors;

    bool centralNodeLocked;
    bool circleLayoutSet;
    GlWidget *glWidget;
    node selectedNeighborNode;
    uint neighborhoodDist;

    NeighborhoodHighlighterConfigWidget *configWidget;

    BoundingBox neighborhoodGraphCircleLayoutBB, neighborhoodGraphOriginalLayoutBB;

    unsigned char circleAlphaValue;
    unsigned char startAlpha, endAlpha;
    int nbAnimSteps;
};
}

#endif // NEIGHBORHOOD_HIGHLIGHTER_INTERACTOR_H
