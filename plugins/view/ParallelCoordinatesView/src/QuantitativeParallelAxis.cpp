/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "QuantitativeParallelAxis.h"
#include "AxisConfigDialogs.h"
#include "ParallelCoordinatesGraphProxy.h"
#include "ParallelTools.h"

#include <talipot/IntegerProperty.h>
#include <talipot/DoubleProperty.h>

using namespace std;

namespace tlp {

QuantitativeParallelAxis::QuantitativeParallelAxis(
    const Coord &baseCoord, const float height, const float axisAreaWidth,
    ParallelCoordinatesGraphProxy *graphProxy, const string &graphPropertyName,
    const bool ascendingOrder, const Color &axisColor, const float rotationAngle,
    const GlAxis::CaptionLabelPosition captionPosition)
    : ParallelAxis(new GlQuantitativeAxis(graphPropertyName, baseCoord, height,
                                          GlAxis::VERTICAL_AXIS, axisColor, true, ascendingOrder),
                   axisAreaWidth, rotationAngle, captionPosition),
      nbAxisGrad(DEFAULT_NB_AXIS_GRAD), axisMinValue(DBL_MAX), axisMaxValue(-DBL_MAX),
      graphProxy(graphProxy), log10Scale(false), integerScale(false) {
  glQuantitativeAxis = dynamic_cast<GlQuantitativeAxis *>(glAxis);
  boxPlotValuesCoord.resize(5);
  boxPlotStringValues.resize(5);
  redraw();
}

void QuantitativeParallelAxis::setAxisLabels() {
  double propertyMin = getAssociatedPropertyMinValue();
  double propertyMax = getAssociatedPropertyMaxValue();
  bool realScale = false;

  if (propertyMin >= INT_MIN && propertyMax <= INT_MAX) {
    if (getAxisDataTypeName() == "double") {
      for (uint n : graphProxy->getDataIterator()) {
        double val =
            graphProxy->getPropertyValueForData<DoubleProperty, DoubleType>(getAxisName(), n);
        double intpart, fracpart;
        fracpart = modf(val, &intpart);
        realScale = realScale || (fracpart != 0);
      }
    }
  } else {
    realScale = true;
  }

  if (axisMinValue > propertyMin || propertyMin == propertyMax) {
    axisMinValue = propertyMin;
  }

  if (axisMaxValue < propertyMax || propertyMin == propertyMax) {
    axisMaxValue = propertyMax;
  }

  if (getAxisDataTypeName() == "double" && realScale) {
    glQuantitativeAxis->setAxisParameters(axisMinValue, axisMaxValue, nbAxisGrad,
                                          GlAxis::RIGHT_OR_ABOVE, true);
    integerScale = false;
  } else {
    auto min = static_cast<long long>(axisMinValue);
    auto max = static_cast<long long>(axisMaxValue);
    ullong incrementStep = (max - min) / DEFAULT_NB_AXIS_GRAD;

    if (incrementStep < 1) {
      incrementStep = 1;
    }

    glQuantitativeAxis->setAxisParameters(min, max, incrementStep, GlAxis::RIGHT_OR_ABOVE, true);
    integerScale = true;
  }

  glQuantitativeAxis->setLogScale(log10Scale);
}

void QuantitativeParallelAxis::computeBoxPlotCoords() {

  set<double> propertyValuesSet;

  for (uint dataId : graphProxy->getDataIterator()) {
    double value;

    if (getAxisDataTypeName() == "double") {
      value =
          graphProxy->getPropertyValueForData<DoubleProperty, DoubleType>(getAxisName(), dataId);
    } else {
      value = double(
          graphProxy->getPropertyValueForData<IntegerProperty, IntegerType>(getAxisName(), dataId));
    }

    propertyValuesSet.insert(value);
  }

  vector<double> propertyValuesVector(propertyValuesSet.begin(), propertyValuesSet.end());
  uint vectorSize = propertyValuesVector.size();

  if (vectorSize < 4) {

    boxPlotValuesCoord[BOTTOM_OUTLIER] = Coord(-1.0f, -1.0f, -1.0f);
    boxPlotValuesCoord[FIRST_QUARTILE] = Coord(-1.0f, -1.0f, -1.0f);
    boxPlotValuesCoord[MEDIAN] = Coord(-1.0f, -1.0f, -1.0f);
    boxPlotValuesCoord[THIRD_QUARTILE] = Coord(-1.0f, -1.0f, -1.0f);
    boxPlotValuesCoord[TOP_OUTLIER] = Coord(-1.0f, -1.0f, -1.0f);

    boxPlotStringValues[BOTTOM_OUTLIER] = "KO";
    boxPlotStringValues[FIRST_QUARTILE] = "KO";
    boxPlotStringValues[MEDIAN] = "KO";
    boxPlotStringValues[THIRD_QUARTILE] = "KO";
    boxPlotStringValues[TOP_OUTLIER] = "KO";

  } else {

    double median;

    if (vectorSize % 2 == 1) {
      median = propertyValuesVector[vectorSize / 2.];
    } else {
      median =
          (propertyValuesVector[(vectorSize / 2.) - 1] + propertyValuesVector[vectorSize / 2.]) /
          2.;
    }

    double firstQuartile;

    if (vectorSize % 2 == 1) {
      firstQuartile = propertyValuesVector[vectorSize / 4.];
    } else {
      firstQuartile =
          (propertyValuesVector[(vectorSize / 4.) - 1] + propertyValuesVector[vectorSize / 4.]) / 2;
    }

    double thirdQuartile;

    if (vectorSize % 2 == 1) {
      thirdQuartile = propertyValuesVector[3 * (vectorSize / 4.)];
    } else {
      thirdQuartile = (propertyValuesVector[3 * (vectorSize / 4.) - 1] +
                       propertyValuesVector[3 * (vectorSize / 4.)]) /
                      2.;
    }

    double lowBorder = (firstQuartile - (1.5 * (thirdQuartile - firstQuartile)));
    double bottomOutlier = 0.0;

    for (auto v : propertyValuesVector) {
      if (v > lowBorder) {
        bottomOutlier = v;
        break;
      }
    }

    double highBorder = (thirdQuartile + (1.5 * (thirdQuartile - firstQuartile)));
    double topOutlier = 0;

    for (auto v : reversed(propertyValuesVector)) {
      if (v < highBorder) {
        topOutlier = v;
        break;
      }
    }

    boxPlotValuesCoord[BOTTOM_OUTLIER] = getAxisCoordForValue(bottomOutlier);
    boxPlotValuesCoord[FIRST_QUARTILE] = getAxisCoordForValue(firstQuartile);
    boxPlotValuesCoord[MEDIAN] = getAxisCoordForValue(median);
    boxPlotValuesCoord[THIRD_QUARTILE] = getAxisCoordForValue(thirdQuartile);
    boxPlotValuesCoord[TOP_OUTLIER] = getAxisCoordForValue(topOutlier);

    boxPlotStringValues[BOTTOM_OUTLIER] = getStringFromNumber(bottomOutlier);
    boxPlotStringValues[FIRST_QUARTILE] = getStringFromNumber(firstQuartile);
    boxPlotStringValues[MEDIAN] = getStringFromNumber(median);
    boxPlotStringValues[THIRD_QUARTILE] = getStringFromNumber(thirdQuartile);
    boxPlotStringValues[TOP_OUTLIER] = getStringFromNumber(topOutlier);
  }
}

double QuantitativeParallelAxis::getAssociatedPropertyMinValue() {
  if (graphProxy->getGraph()->getRoot() == graphProxy->getGraph()) {
    if (getAxisDataTypeName() == "double") {
      return graphProxy->getPropertyMinValue<DoubleProperty, DoubleType>(getAxisName());
    } else {
      return graphProxy->getPropertyMinValue<IntegerProperty, IntegerType>(getAxisName());
    }
  } else {

    double min = DBL_MAX;

    for (uint dataId : graphProxy->getDataIterator()) {
      double propertyValue;

      if (getAxisDataTypeName() == "double") {
        propertyValue =
            graphProxy->getPropertyValueForData<DoubleProperty, DoubleType>(getAxisName(), dataId);
      } else {
        propertyValue = graphProxy->getPropertyValueForData<IntegerProperty, IntegerType>(
            getAxisName(), dataId);
      }
      min = std::min(propertyValue, min);
    }

    return min;
  }
}

double QuantitativeParallelAxis::getAssociatedPropertyMaxValue() {
  if (getAxisDataTypeName() == "double") {
    return graphProxy->getPropertyMaxValue<DoubleProperty, DoubleType>(getAxisName());
  } else {
    return graphProxy->getPropertyMaxValue<IntegerProperty, IntegerType>(getAxisName());
  }
}

void QuantitativeParallelAxis::translate(const Coord &c) {
  ParallelAxis::translate(c);
  boxPlotValuesCoord[BOTTOM_OUTLIER] += c;
  boxPlotValuesCoord[FIRST_QUARTILE] += c;
  boxPlotValuesCoord[MEDIAN] += c;
  boxPlotValuesCoord[THIRD_QUARTILE] += c;
  boxPlotValuesCoord[TOP_OUTLIER] += c;
}

Coord QuantitativeParallelAxis::getPointCoordOnAxisForData(const uint dataIdx) {
  double value = 0;

  if (getAxisDataTypeName() == "double") {
    value = graphProxy->getPropertyValueForData<DoubleProperty, DoubleType>(getAxisName(), dataIdx);
  } else if (getAxisDataTypeName() == "int") {
    value = double(
        graphProxy->getPropertyValueForData<IntegerProperty, IntegerType>(getAxisName(), dataIdx));
  }

  Coord axisPointCoord = glQuantitativeAxis->getAxisPointCoordForValue(value);

  if (rotationAngle != 0.0f) {
    rotateVector(axisPointCoord, rotationAngle, Z_ROT);
  }

  return axisPointCoord;
}

Coord QuantitativeParallelAxis::getAxisCoordForValue(double value) {
  return glQuantitativeAxis->getAxisPointCoordForValue(value);
}

void QuantitativeParallelAxis::redraw() {
  setAxisLabels();
  ParallelAxis::redraw();
  computeBoxPlotCoords();
}

void QuantitativeParallelAxis::showConfigDialog() {
  QuantitativeAxisConfigDialog dialog(this);
  dialog.exec();
  updateSlidersWithDataSubset(graphProxy->getHighlightedElts());
}

string QuantitativeParallelAxis::getAxisDataTypeName() const {
  return graphProxy->getProperty(getAxisName())->getTypename();
}

double QuantitativeParallelAxis::getValueForAxisCoord(const Coord &axisCoord) {
  return glQuantitativeAxis->getValueForAxisPoint(axisCoord);
}

std::string QuantitativeParallelAxis::getTopSliderTextValue() {
  if (getAxisDataTypeName() == "int" || integerScale) {
    int val = int(getValueForAxisCoord(topSliderCoord));

    if (glQuantitativeAxis->hasAscendingOrder()) {
      return getStringFromNumber(val == glQuantitativeAxis->getAxisMaxValue() ? val : val - 1);
    } else {
      return getStringFromNumber(val);
    }
  } else if (getAxisDataTypeName() == "double") {
    return getStringFromNumber(getValueForAxisCoord(topSliderCoord));
  } else {
    return "";
  }
}

std::string QuantitativeParallelAxis::getBottomSliderTextValue() {
  if (getAxisDataTypeName() == "int" || integerScale) {
    int val = int(getValueForAxisCoord(bottomSliderCoord));

    if (!glQuantitativeAxis->hasAscendingOrder()) {
      return getStringFromNumber(val == glQuantitativeAxis->getAxisMaxValue() ? val : val - 1);
    } else {
      return getStringFromNumber(val);
    }

    return getStringFromNumber(getValueForAxisCoord(bottomSliderCoord));
  } else if (getAxisDataTypeName() == "double") {
    return getStringFromNumber(getValueForAxisCoord(bottomSliderCoord));
  } else {
    return "";
  }
}

const set<uint> &QuantitativeParallelAxis::getDataInRange(float yLowBound, float yHighBound) {
  dataSubset.clear();
  float rotAngleBak = rotationAngle;
  rotationAngle = 0.0f;

  for (uint dataId : graphProxy->getDataIterator()) {
    Coord dataCoord = getPointCoordOnAxisForData(dataId);

    if (dataCoord.getY() <= yHighBound && dataCoord.getY() >= yLowBound) {
      dataSubset.insert(dataId);
    }
  }

  rotationAngle = rotAngleBak;
  return dataSubset;
}

const set<uint> &QuantitativeParallelAxis::getDataInSlidersRange() {
  return getDataInRange(bottomSliderCoord.getY(), topSliderCoord.getY());
}

const set<uint> &QuantitativeParallelAxis::getDataBetweenBoxPlotBounds() {
  if (boxPlotLowBound != NO_VALUE && boxPlotHighBound != NO_VALUE) {
    return getDataInRange(boxPlotValuesCoord[boxPlotLowBound].getY(),
                          boxPlotValuesCoord[boxPlotHighBound].getY());
  } else {
    dataSubset.clear();
    return dataSubset;
  }
}

void QuantitativeParallelAxis::updateSlidersWithDataSubset(const set<uint> &dataSubset) {
  float rotAngleBak = rotationAngle;
  rotationAngle = 0.0f;
  Coord max = getBaseCoord();
  Coord min = getBaseCoord() + Coord(0.0f, getAxisHeight());

  for (auto d : dataSubset) {
    Coord dataCoord = getPointCoordOnAxisForData(d);

    if (dataCoord.getY() < min.getY()) {
      min = dataCoord;
    }

    if (dataCoord.getY() > max.getY()) {
      max = dataCoord;
    }
  }

  bottomSliderCoord = min;
  topSliderCoord = max;
  rotationAngle = rotAngleBak;
}

void QuantitativeParallelAxis::setAscendingOrder(const bool ascendingOrder) {

  Coord baseCoord = getBaseCoord();
  float axisHeight = getAxisHeight();

  if (ascendingOrder != hasAscendingOrder()) {

    float spaceBetweenSlider = topSliderCoord.getY() - bottomSliderCoord.getY();

    float axisCenterTopSliderDist = topSliderCoord.getY() - (baseCoord.getY() + axisHeight / 2.0f);
    topSliderCoord.setY((baseCoord.getY() + axisHeight / 2.0f) - axisCenterTopSliderDist +
                        spaceBetweenSlider);

    float axisCenterBottomSliderDist =
        bottomSliderCoord.getY() - (baseCoord.getY() + axisHeight / 2.0f);
    bottomSliderCoord.setY((baseCoord.getY() + axisHeight / 2.0f) - axisCenterBottomSliderDist -
                           spaceBetweenSlider);
  }

  glQuantitativeAxis->setAscendingOrder(ascendingOrder);
}
}
