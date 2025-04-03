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

#ifndef PARALLEL_COORDS_AXIS_SLIDERS_H
#define PARALLEL_COORDS_AXIS_SLIDERS_H

#include <talipot/GlEntity.h>
#include <talipot/GLInteractor.h>
#include <talipot/GlLabel.h>
#include <talipot/GlPolygon.h>

#include "ParallelCoordinatesDrawing.h"

namespace tlp {

class GlQuad;
class ParallelCoordinatesView;

enum sliderType { TOP_SLIDER = 0, BOTTOM_SLIDER = 1 };

class AxisSlider : public GlEntity {

  public:
    AxisSlider(const sliderType type, const Coord &sliderCoord, const float halfWidth,
               const float halfHeight, const Color &sliderColor, const Color &labelColor,
               const float rotationAngle = 0);
    ~AxisSlider() override;

    void setSliderFillColor(const Color &color);
    void setSliderOutlineColor(const Color &color);
    void setSliderLabel(const std::string &label) {
        sliderLabel->setText(label);
    }
    void setRotationAngle(const float rotationAngle) {
        this->rotationAngle = rotationAngle;
    }

    void draw(float lod, Camera *camera) override;
    BoundingBox getBoundingBox() override;
    Coord getSliderCoord() const {
        return sliderCoord;
    }
    void translate(const Coord &move) override;
    void moveToCoord(const Coord &coord) {
        translate(coord - sliderCoord);
    }
    sliderType getSliderType() const {
        return type;
    }
    Color getColor() {
        return arrowPolygon->getFillColor(0);
    }

    void getXML(std::string &) override {}
    void setWithXML(const std::string &, uint &) override {}

  private:
    sliderType type;

    GlComposite *sliderComposite;
    GlQuad *sliderQuad;
    GlPolygon *sliderPolygon;
    GlPolygon *arrowPolygon;
    GlLabel *sliderLabel;
    Coord sliderCoord;
    float rotationAngle;
};

class ParallelCoordsAxisSliders : public GLInteractorComponent {

  public:
    ParallelCoordsAxisSliders();
    ~ParallelCoordsAxisSliders() override;
    bool eventFilter(QObject *, QEvent *) override;
    bool draw(GlWidget *glWidget) override;
    bool compute(GlWidget *glWidget) override;
    void viewChanged(View *view) override;

  private:
    void initOrUpdateSliders();
    AxisSlider *getSliderUnderPointer(GlWidget *glWidget, ParallelAxis *axis, int x, int y);
    void updateOtherAxisSliders();
    void buildGlSliders(std::vector<ParallelAxis *> axis);
    void deleteGlSliders();
    void setSlidersColor(const Color &color);
    void updateSlidersYBoundaries();

    ParallelCoordinatesView *parallelView;
    Graph *currentGraph;
    std::map<ParallelAxis *, std::vector<AxisSlider *>> axisSlidersMap;
    ParallelAxis *selectedAxis;
    std::vector<ParallelAxis *> lastSelectedAxis;
    AxisSlider *selectedSlider;
    bool axisSliderDragStarted;
    bool pointerBetweenSliders;
    bool slidersRangeDragStarted;
    int slidersRangeLength;
    int xClick, yClick;
    float lastAxisHeight;
    uint lastNbAxis;
    ParallelCoordinatesDrawing::HighlightedEltsSetOp highlightedEltsSetOperation;
    std::map<ParallelAxis *, std::pair<float, float>> slidersYBoundaries;
    GlLayer *selectionLayer;
};
}

#endif // PARALLEL_COORDS_AXIS_SLIDERS_H
