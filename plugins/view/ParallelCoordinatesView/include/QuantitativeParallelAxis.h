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

#ifndef QUANTITATIVE_PARALLEL_AXIS_H
#define QUANTITATIVE_PARALLEL_AXIS_H

#include "ParallelAxis.h"

#include <talipot/GlQuantitativeAxis.h>

const uint DEFAULT_NB_AXIS_GRAD = 20;

namespace tlp {

class ParallelCoordinatesGraphProxy;

enum BoxPlotValue {
    BOTTOM_OUTLIER = 0,
    FIRST_QUARTILE = 1,
    MEDIAN = 2,
    THIRD_QUARTILE = 3,
    TOP_OUTLIER = 4,
    NO_VALUE = 5
};

// Class which allows to render a quantitative axis
// Associated datatypes can be real or integer
class QuantitativeParallelAxis : public ParallelAxis {

  public:
    QuantitativeParallelAxis(const Coord &baseCoord, const float height, const float axisAreaWidth,
                             ParallelCoordinatesGraphProxy *graphProxy,
                             const std::string &graphPropertyName, const bool ascendingOrder = true,
                             const Color &axisColor = Color(0, 0, 0), const float rotationAngle = 0,
                             const GlAxis::CaptionLabelPosition captionPosition = GlAxis::BELOW);

    void setNbAxisGrad(const uint nbAxisGrad) {
        this->nbAxisGrad = nbAxisGrad;
    }
    uint getNbAxisGrad() const {
        return nbAxisGrad;
    }

    Coord getPointCoordOnAxisForData(const uint dataIdx) override;

    void translate(const Coord &c) override;
    void redraw() override;

    void showConfigDialog() override;

    std::string getAxisDataTypeName() const;

    void setLog10Scale(const bool log10Scale) {
        this->log10Scale = log10Scale;
    }
    bool hasLog10Scale() const {
        return log10Scale;
    }

    double getAssociatedPropertyMinValue();
    double getAssociatedPropertyMaxValue();

    void setAxisMinMaxValues(const double min, const double max) {
        axisMinValue = min;
        axisMaxValue = max;
    }

    double getAxisMinValue() const {
        return glQuantitativeAxis->getAxisMinValue();
    }
    double getAxisMaxValue() const {
        return glQuantitativeAxis->getAxisMaxValue();
    }

    double getValueForAxisCoord(const Coord &axisCoord);
    Coord getAxisCoordForValue(double value);

    std::string getTopSliderTextValue() override;
    std::string getBottomSliderTextValue() override;

    const std::set<uint> &getDataInSlidersRange() override;
    void updateSlidersWithDataSubset(const std::set<uint> &dataSubset) override;

    void setBoxPlotHighlightBounds(BoxPlotValue lowBound, BoxPlotValue highBound) {
        boxPlotLowBound = lowBound;
        boxPlotHighBound = highBound;
    }
    const std::set<uint> &getDataBetweenBoxPlotBounds();

    bool hasAscendingOrder() const {
        return glQuantitativeAxis->hasAscendingOrder();
    }
    void setAscendingOrder(const bool ascendingOrder);

    // Axis BoxPlot methods
    Coord getBottomOutlierCoord() const {
        return boxPlotValuesCoord[BOTTOM_OUTLIER];
    }
    Coord getFirstQuartileCoord() const {
        return boxPlotValuesCoord[FIRST_QUARTILE];
    }
    Coord getMedianCoord() const {
        return boxPlotValuesCoord[MEDIAN];
    }
    Coord getThirdQuartileCoord() const {
        return boxPlotValuesCoord[THIRD_QUARTILE];
    }
    Coord getTopOutlierCoord() const {
        return boxPlotValuesCoord[TOP_OUTLIER];
    }
    std::string getBottomOutlierStringValue() const {
        return boxPlotStringValues[BOTTOM_OUTLIER];
    }
    std::string getFirstQuartileStringValue() const {
        return boxPlotStringValues[FIRST_QUARTILE];
    }
    std::string getMedianStringValue() const {
        return boxPlotStringValues[MEDIAN];
    }
    std::string getThirdQuartileStringValue() const {
        return boxPlotStringValues[THIRD_QUARTILE];
    }
    std::string getTopOutlierStringValue() const {
        return boxPlotStringValues[TOP_OUTLIER];
    }

  private:
    const std::set<uint> &getDataInRange(float yLowBound, float yHighBound);

    void computeBoxPlotCoords();

    void showAxisConfigDialog();

    void setAxisLabels();

    GlQuantitativeAxis *glQuantitativeAxis;

    uint nbAxisGrad;
    double axisMinValue, axisMaxValue;

    ParallelCoordinatesGraphProxy *graphProxy;
    bool log10Scale;

    std::vector<Coord> boxPlotValuesCoord;
    std::vector<std::string> boxPlotStringValues;
    BoxPlotValue boxPlotLowBound, boxPlotHighBound;
    bool integerScale;
};
}

#endif // QUANTITATIVE_PARALLEL_AXIS_H
