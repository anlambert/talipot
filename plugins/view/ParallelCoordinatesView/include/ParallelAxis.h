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

#ifndef PARALLEL_AXIS_H
#define PARALLEL_AXIS_H

#include <talipot/GlEntity.h>
#include <talipot/GlAxis.h>

#include <set>

namespace tlp {

class GlRect;

class ParallelAxis : public GlEntity {

  public:
    ~ParallelAxis() override;

    BoundingBox getBoundingBox() override;
    void setStencil(int stencil) override {
        glAxis->setStencil(stencil);
    }
    void draw(float lod, Camera *camera) override;
    void translate(const Coord &c) override;
    void getXML(std::string &) override {}
    void setWithXML(const std::string &, uint &) override {}

    virtual void redraw();
    virtual Coord getPointCoordOnAxisForData(const uint dataIdx) = 0;
    virtual void showConfigDialog() = 0;

    void setBaseCoord(const Coord &baseCoord);
    Coord getBaseCoord() const;
    Coord getTopCoord() const;
    std::string getAxisName() const {
        return glAxis->getAxisName();
    }
    float getAxisHeight() const {
        return glAxis->getAxisLength();
    }
    float getAxisGradsWidth() {
        return glAxis->getAxisGradsWidth();
    }
    float getLabelHeight() {
        return glAxis->getLabelHeight();
    }
    void setAxisHeight(const float axisHeight);
    Color getAxisColor() const {
        return glAxis->getAxisColor();
    }
    void setAxisColor(const Color &axisColor) {
        glAxis->setAxisColor(axisColor);
    }
    void setMaxCaptionWidth(const float maxCaptionWidth) {
        glAxis->setMaxCaptionWidth(maxCaptionWidth / 2);
    }
    void setCaptionPosition(const GlAxis::CaptionLabelPosition captionPosition);

    void setSlidersActivated(const bool slidersActivated) {
        this->slidersActivated = slidersActivated;
    }
    bool isSlidersActivated() const {
        return slidersActivated;
    }
    Coord getTopSliderCoord() {
        return topSliderCoord;
    }
    void setTopSliderCoord(const Coord &topSliderCoord) {
        this->topSliderCoord = topSliderCoord;
    }
    Coord getBottomSliderCoord() {
        return bottomSliderCoord;
    }
    void setBottomSliderCoord(const Coord &bottomSliderCoord) {
        this->bottomSliderCoord = bottomSliderCoord;
    }

    virtual std::string getTopSliderTextValue() = 0;
    virtual std::string getBottomSliderTextValue() = 0;
    void resetSlidersPosition();

    void setHidden(const bool hidden) {
        this->hidden = hidden;
    }
    bool isHidden() const {
        return hidden;
    }

    virtual const std::set<uint> &getDataInSlidersRange() = 0;
    virtual void updateSlidersWithDataSubset(const std::set<uint> &dataSubset) = 0;

    void enableTrickForSelection();
    void disableTrickForSelection();

    void setRotationAngle(const float rotationAngle);
    float getRotationAngle() const {
        return rotationAngle;
    }
    Array<Coord, 4> getBoundingPolygonCoords() const;

  protected:
    ParallelAxis(GlAxis *glAxis, const float axisAreaWidth, const float rotationAngle,
                 const GlAxis::CaptionLabelPosition captionPosition = GlAxis::BELOW);

    GlAxis *glAxis;
    GlRect *emptyRect;

    float axisAreaWidth;
    bool slidersActivated;
    Coord topSliderCoord;
    Coord bottomSliderCoord;
    float rotationAngle;
    bool hidden;
    std::set<uint> dataSubset;
};
}

#endif // PARALLEL_AXIS_H
