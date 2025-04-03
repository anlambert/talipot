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

#ifndef PARALLEL_COORDINATES_DRAWING_H
#define PARALLEL_COORDINATES_DRAWING_H

#include <talipot/GlComposite.h>
#include <talipot/Observable.h>
#include <talipot/Size.h>
#include <talipot/Color.h>
#include <talipot/Graph.h>

namespace tlp {

class GlWidget;
class GlProgressBar;
class ParallelCoordinatesGraphProxy;
class ParallelAxis;
class LayoutProperty;
class SizeProperty;
class IntegerProperty;
class StringProperty;
class BooleanProperty;
class ColorProperty;

class ParallelCoordinatesDrawing : public GlComposite, public Observable {

  public:
    enum LayoutType { PARALLEL = 0, CIRCULAR };

    enum LinesType { STRAIGHT = 0, CATMULL_ROM_SPLINE, CUBIC_BSPLINE_INTERPOLATION };

    enum LinesThickness { THICK = 0, THIN };

    enum HighlightedEltsSetOp { NONE = 0, INTERSECTION, UNION };

    ParallelCoordinatesDrawing(ParallelCoordinatesGraphProxy *graphProxy, Graph *axisPointsGraph);

    ~ParallelCoordinatesDrawing() override;

    bool getDataIdFromGlEntity(GlEntity *glEntity, uint &dataId);
    bool getDataIdFromAxisPoint(node axisPoint, uint &dataId);

    uint nbParallelAxis() const;
    const std::vector<std::string> &getAxisNames() const;
    void swapAxis(ParallelAxis *axis1, ParallelAxis *axis2);
    void removeAxis(ParallelAxis *axis);
    void addAxis(ParallelAxis *axis);

    void setAxisHeight(const uint axisHeight) {
        this->height = axisHeight;
    }
    void setSpaceBetweenAxis(const uint spaceBetweenAxis) {
        this->spaceBetweenAxis = spaceBetweenAxis;
    }
    void setAxisPointMinSize(const Size &axisPointMinSize) {
        this->axisPointMinSize = axisPointMinSize;
    }
    void setAxisPointMaxSize(const Size &axisPointMaxSize) {
        this->axisPointMaxSize = axisPointMaxSize;
    }
    void setDrawPointsOnAxis(const bool drawPointsOnAxis) {
        this->drawPointsOnAxis = drawPointsOnAxis;
    }
    void setLinesColorAlphaValue(const uint linesColorAlphaValue) {
        this->linesColorAlphaValue = linesColorAlphaValue;
    }
    void setLineTextureFilename(const std::string &lineTextureFilename) {
        this->lineTextureFilename = lineTextureFilename;
    }
    void setBackgroundColor(const Color &backgroundColor) {
        this->backgroundColor = backgroundColor;
    }
    void setLayoutType(const LayoutType layoutType) {
        this->layoutType = layoutType;
    }
    void setLinesType(const LinesType linesType) {
        this->linesType = linesType;
    }
    void setLinesThickness(const LinesThickness linesThickness) {
        this->linesThickness = linesThickness;
    }
    std::vector<ParallelAxis *> getAllAxis();

    void resetAxisLayoutNextUpdate() {
        resetAxisLayout = true;
    }
    void update(GlWidget *glWidget, bool updateWithoutProgressBar = false);
    void updateWithAxisSlidersRange(ParallelAxis *axis,
                                    HighlightedEltsSetOp highlightedEltsSetOp = NONE);

    void resetAxisSlidersPosition();

    void delNode(Graph *, const node);
    void delEdge(Graph *, const edge);
    void treatEvent(const tlp::Event &) override;

  private:
    void computeResizeFactor();
    void createAxis(GlWidget *glWidget, GlProgressBar *progressBar);
    void destroyAxisIfNeeded();
    void plotAllData(GlWidget *glWidget, GlProgressBar *progressBar);
    void plotData(const uint dataIdx, const Color &color);

    void erase();
    void eraseDataPlot();
    void eraseAxisPlot();
    void removeHighlightedElt(const uint dataId);

    uint nbAxis;
    Coord firstAxisPos;
    uint width, height;
    uint spaceBetweenAxis;
    uint linesColorAlphaValue;
    bool drawPointsOnAxis;

    std::vector<std::string> axisOrder;
    std::map<std::string, ParallelAxis *> parallelAxis;

    std::map<GlEntity *, uint> glEntitiesDataMap;
    std::map<node, uint> axisPointsDataMap;

    ParallelCoordinatesGraphProxy *graphProxy;

    Color backgroundColor;
    std::string lineTextureFilename;
    Size axisPointMinSize;
    Size axisPointMaxSize;
    Size resizeFactor;

    GlComposite *dataPlotComposite;
    GlComposite *axisPlotComposite;

    bool createAxisFlag;
    std::set<uint> lastHighlightedElements;

    Graph *axisPointsGraph;
    LayoutProperty *axisPointsGraphLayout;
    SizeProperty *axisPointsGraphSize;
    IntegerProperty *axisPointsGraphShape;
    StringProperty *axisPointsGraphLabels;
    ColorProperty *axisPointsGraphColors;
    BooleanProperty *axisPointsGraphSelection;

    LayoutType layoutType;
    LinesType linesType;
    LinesThickness linesThickness;

    bool resetAxisLayout;
};
}

#endif // PARALLEL_COORDINATES_DRAWING_H
