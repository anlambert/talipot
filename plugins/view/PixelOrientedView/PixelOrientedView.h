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

#ifndef PIXEL_ORIENTED_VIEW_H
#define PIXEL_ORIENTED_VIEW_H

#include <talipot/GlView.h>

#include <QWidget>

#include "POLIB/SpiralLayout.h"
#include "POLIB/SquareLayout.h"
#include "POLIB/ZOrderLayout.h"
#include "POLIB/HilbertLayout.h"
#include "POLIB/NodeColorMapping.h"
#include "POLIB/HSIColorMapping.h"
#include "POLIB/PixelOrientedMediator.h"
#include "POLIB/LinearMappingColor.h"
#include "POLIB/GraphDimension.h"

#include "../../utils/PluginNames.h"

#include "PixelOrientedOverview.h"
#include "PixelOrientedOptionsWidget.h"

namespace tlp {

class ViewGraphPropertiesSelectionWidget;
class PixelOrientedViewQuickAccessBar;

/** \file
 *  \brief Pixel Oriented View

 * This view plugin allows to visualize graph properties using pixel oriented visualization
 techniques.
 * This type of visualization aims to use each pixel of the display to visualize one data value
 * and therefore allow the visualization of the largest amount of data possible.
 * Pixels are mapped so that neighbor pixels in the data are placed close to each other on the
 screen.
 * The technique uses a linear order on data elements (graph nodes), inferred from a selected
 property,
 * which can be seen as a map from the data space onto a line segment. The mapping onto a 2D portion
 * of the plane then uses a “space-filling curve”.
 *
 *
 */
class PixelOrientedView : public GlView {

    static const uint MIN_IMAGE_WIDTH = 512;
    static const uint MIN_IMAGE_HEIGHT = 512;

    Q_OBJECT

    void registerTriggers();

  public:
    PLUGININFORMATION(
        ViewName::PixelOrientedViewName, "Antoine Lambert", "12/2008",
        "<p>The Pixel Oriented view allows to visualize graph properties using pixel "
        "oriented visualization techniques.</p>"
        "<p>This type of visualization aims to use each pixel of the display to "
        "visualize one data value and therefore allows the visualization of the "
        "largest amount of data possible. "
        "Pixels are mapped so that neighbor pixels in the data are placed close to "
        "each other on the screen.</p>"
        "<p>The technique uses a linear order on data elements (graph nodes), inferred "
        "from a selected property, which can be seen as a map from the data space onto "
        "a line segment. The mapping onto a 2D portion of the plane then uses a "
        "\"space-filling curve\"</p>",
        "1.0", "View")

    PixelOrientedView(const PluginContext *);
    ~PixelOrientedView() override;
    std::string icon() const override {
        return ":/pixel_oriented_view.png";
    }
    QuickAccessBar *getQuickAccessBarImpl() override;
    void setState(const DataSet &dataSet) override;
    DataSet state() const override;
    Graph *getPixelOrientedGraph() {
        return pixelOrientedGraph;
    }
    QList<QWidget *> configurationWidgets() const override;
    void interactorsInstalled(const QList<tlp::Interactor *> &) override;

    void toggleInteractors(const bool activate);

    std::vector<PixelOrientedOverview *> getOverviews();
    bool smallMultiplesViewSet() const {
        return smallMultiplesView;
    }
    void switchFromSmallMultiplesToDetailView(PixelOrientedOverview *scatterPlot);
    void switchFromDetailViewToSmallMultiples();
    BoundingBox getSmallMultiplesViewBoundingBox();
    void generatePixelOverview(PixelOrientedOverview *pixelOverview, GlWidget *glWidget = nullptr);

    void draw() override;
    void refresh() override;

  public slots:

    void graphChanged(Graph *graph) override;
    void init();
    void applySettings() override;
    // inherited from GlView
    void centerView(bool = false) override;

  private:
    void initPixelView();
    void destroyData();
    void destroyOverviewsIfNeeded();
    void updateOverviews(const bool updateAll = false);
    void addEmptyViewLabel();
    void removeEmptyViewLabel();
    Color getTextColor() const;

    Graph *pixelOrientedGraph;
    GlGraph *glGraph;
    GlLayer *mainLayer;
    GlComposite *overviewsComposite;

    PixelOrientedOptionsWidget *optionsWidget;
    ViewGraphPropertiesSelectionWidget *propertiesSelectionWidget;

    PixelOrientedMediator *pixelOrientedMediator;

    std::map<std::string, LayoutFunction *> layoutFunctionsMap;

    void initGlWidget();

    void initLayoutFunctions();
    void setColorFunction(ColorFunction *colorFunction);
    void setLayoutFunction(LayoutFunction *layoutFunction);

    uint lastNbNodes;
    std::vector<std::string> selectedGraphProperties;
    std::map<std::string, PixelOrientedOverview *> overviewsMap;
    std::map<std::string, bool> overviewGenMap;
    std::map<std::string, GraphDimension *> dataMap;

    uint overviewWidth;
    uint overviewHeight;
    uint minWidth;
    float refSize;

    HilbertLayout *hilbertLayout;
    SquareLayout *squareLayout;
    SpiralLayout *spiralLayout;
    ZorderLayout *zorderLayout;

    NodeColorMapping *talipotNodeColorMapping;

    bool smallMultiplesView;

    double sceneRadiusBak;
    double zoomFactorBak;
    Coord eyesBak;
    Coord centerBak;
    Coord upBak;

    GlLabel *detailViewLabel;
    PixelOrientedOverview *detailOverview;
    std::string detailOverviewPropertyName;

    bool newGraphSet;
    bool smallMultiplesNeedUpdate;

    int lastViewWindowWidth, lastViewWindowHeight;

    bool center;
    bool isConstruct;
};
}
#endif // PIXEL_ORIENTED_VIEW_H
