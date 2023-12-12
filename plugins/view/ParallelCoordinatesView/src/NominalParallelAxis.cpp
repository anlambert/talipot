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

#include "NominalParallelAxis.h"
#include "AxisConfigDialogs.h"
#include "ParallelCoordinatesGraphProxy.h"
#include "ParallelTools.h"

#include <talipot/GlNominativeAxis.h>
#include <talipot/StringProperty.h>

using namespace std;

namespace tlp {

NominalParallelAxis::NominalParallelAxis(const Coord &base_coord, const float height,
                                         const float axisAreaWidth,
                                         ParallelCoordinatesGraphProxy *graph,
                                         const std::string &propertyName, const Color &axisColor,
                                         const float rotationAngle,
                                         const GlAxis::CaptionLabelPosition captionPosition)
    : ParallelAxis(
          new GlNominativeAxis(propertyName, base_coord, height, GlAxis::VERTICAL_AXIS, axisColor),
          axisAreaWidth, rotationAngle, captionPosition),
      graphProxy(graph) {
  glNominativeAxis = static_cast<GlNominativeAxis *>(glAxis);
  setLabels();
  ParallelAxis::redraw();
}

void NominalParallelAxis::setLabels() {

  vector<string> labels;

  for (uint dataId : graphProxy->getDataIterator()) {
    string labelName =
        graphProxy->getPropertyValueForData<StringProperty, StringType>(getAxisName(), dataId);

    if (std::find(labels.begin(), labels.end(), labelName) == labels.end()) {
      labels.push_back(labelName);
    }
  }

  if (labelsOrder.empty() || (labelsOrder.size() != labels.size())) {
    labelsOrder = labels;
  }

  glNominativeAxis->setAxisGraduationsLabels(labelsOrder, GlAxis::RIGHT_OR_ABOVE);
}

Coord NominalParallelAxis::getPointCoordOnAxisForData(const uint dataIdx) {
  string propertyValue =
      graphProxy->getPropertyValueForData<StringProperty, StringType>(getAxisName(), dataIdx);
  Coord axisPointCoord = glNominativeAxis->getAxisPointCoordForValue(propertyValue);

  if (rotationAngle != 0.0f) {
    rotateVector(axisPointCoord, rotationAngle, Z_ROT);
  }

  return axisPointCoord;
}

void NominalParallelAxis::showConfigDialog() {
  NominalAxisConfigDialog dialog(this);
  dialog.exec();
}

const set<uint> &NominalParallelAxis::getDataInSlidersRange() {

  dataSubset.clear();
  map<string, uint> labelsInRange;

  for (const auto &l : labelsOrder) {
    Coord labelCoord = glNominativeAxis->getAxisPointCoordForValue(l);

    if (labelCoord.getY() >= bottomSliderCoord.getY() &&
        labelCoord.getY() <= topSliderCoord.getY()) {
      labelsInRange[l] = 1;
    }
  }

  for (uint dataId : graphProxy->getDataIterator()) {
    string labelName =
        graphProxy->getPropertyValueForData<StringProperty, StringType>(getAxisName(), dataId);

    if (labelsInRange.contains(labelName)) {
      dataSubset.insert(dataId);
    }
  }

  return dataSubset;
}

void NominalParallelAxis::updateSlidersWithDataSubset(const set<uint> &dataSubset) {
  float rotAngleBak = rotationAngle;
  rotationAngle = 0.0f;
  Coord max = getBaseCoord();
  Coord min = getBaseCoord() + Coord(0.0f, getAxisHeight());

  for (auto d : dataSubset) {
    Coord labelCoord = getPointCoordOnAxisForData(d);

    if (labelCoord.getY() < min.getY()) {
      min = labelCoord;
    }

    if (labelCoord.getY() > max.getY()) {
      max = labelCoord;
    }
  }

  bottomSliderCoord = min;
  topSliderCoord = max;
  rotationAngle = rotAngleBak;
}

void NominalParallelAxis::redraw() {
  setLabels();
  ParallelAxis::redraw();
}
}
