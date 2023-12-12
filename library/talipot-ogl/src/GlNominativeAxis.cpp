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

#include <talipot/GlNominativeAxis.h>

using namespace std;

namespace tlp {

GlNominativeAxis::GlNominativeAxis(const string &axisName, const Coord &axisBaseCoord,
                                   const float axisLength, const AxisOrientation &axisOrientation,
                                   const Color &axisColor)
    : GlAxis(axisName, axisBaseCoord, axisLength, axisOrientation, axisColor) {}

void GlNominativeAxis::setAxisGraduationsLabels(const std::vector<std::string> &axisGradsLabels,
                                                const LabelPosition &labelsPos) {
  labelsOrder = axisGradsLabels;
  axisLabelsPosition = labelsPos;
}

void GlNominativeAxis::buildAxisGraduations() {
  setAxisGraduations(labelsOrder, axisLabelsPosition);
  labelsCoord.clear();

  for (uint i = 0; i < labelsOrder.size(); ++i) {
    if (axisOrientation == HORIZONTAL_AXIS) {
      labelsCoord[labelsOrder[i]] =
          Coord(axisBaseCoord.getX() + i * spaceBetweenAxisGrads, axisBaseCoord.getY());
    } else if (axisOrientation == VERTICAL_AXIS) {
      labelsCoord[labelsOrder[i]] =
          Coord(axisBaseCoord.getX(), axisBaseCoord.getY() + i * spaceBetweenAxisGrads);
    }
  }
}

Coord GlNominativeAxis::getAxisPointCoordForValue(const string &value) {
  Coord ret;

  if (labelsCoord.contains(value)) {
    ret = labelsCoord[value];
  }

  return ret;
}

string GlNominativeAxis::getValueAtAxisPoint(const Coord &axisPointCoord) {
  string ret;

  for (const auto &it : labelsCoord) {
    if (it.second == axisPointCoord) {
      ret = it.first;
      break;
    }
  }

  return ret;
}

void GlNominativeAxis::translate(const Coord &c) {
  GlAxis::translate(c);
  for (auto &[label, coord] : labelsCoord) {
    coord += c;
  }
}

void GlNominativeAxis::updateAxis() {
  buildAxisGraduations();
  GlAxis::updateAxis();
}
}
