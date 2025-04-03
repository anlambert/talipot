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

#ifndef PARALLEL_COORDS_AXIS_BOX_PLOT_H
#define PARALLEL_COORDS_AXIS_BOX_PLOT_H

#include <string>

#include <talipot/GlEntity.h>
#include <talipot/GLInteractor.h>
#include <talipot/Color.h>

namespace tlp {

class QuantitativeParallelAxis;

class GlAxisBoxPlot : public GlEntity {

  public:
    GlAxisBoxPlot(QuantitativeParallelAxis *axis, const Color &fillColor,
                  const Color &outlineColor);
    ~GlAxisBoxPlot() override = default;

    void draw(float lod, Camera *camera) override;

    void getXML(std::string &) override {}

    void setWithXML(const std::string &, uint &) override {}

    void setHighlightRangeIfAny(Coord sceneCoords);

  private:
    void drawLabel(const Coord &position, const std::string &labelName, Camera *camera);

    QuantitativeParallelAxis *axis;
    Coord bottomOutlierCoord;
    Coord firstQuartileCoord;
    Coord medianCoord;
    Coord thirdQuartileCoord;
    Coord topOutlierCoord;
    float boxWidth;
    Color fillColor, outlineColor;
    Coord *highlightRangeLowBound;
    Coord *highlightRangeHighBound;
};

class ParallelAxis;
class ParallelCoordinatesView;

class ParallelCoordsAxisBoxPlot : public GLInteractorComponent {

  public:
    ParallelCoordsAxisBoxPlot();
    ~ParallelCoordsAxisBoxPlot() override;
    bool eventFilter(QObject *, QEvent *) override;
    bool draw(GlWidget *glWidget) override;
    bool compute(GlWidget *glWidget) override;
    void viewChanged(View *view) override;

  private:
    void buildGlAxisPlot(std::vector<ParallelAxis *> currentAxis);
    void deleteGlAxisPlot();

    void initOrUpdateBoxPlots();

    ParallelCoordinatesView *parallelView;
    Graph *currentGraph;
    std::map<QuantitativeParallelAxis *, GlAxisBoxPlot *> axisBoxPlotMap;
    ParallelAxis *selectedAxis;
    uint lastNbAxis;
};
}

#endif // PARALLEL_COORDS_AXIS_BOX_PLOT_H
