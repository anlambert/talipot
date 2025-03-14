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

#ifndef TALIPOT_GL_QUANTITATIVE_AXIS_H
#define TALIPOT_GL_QUANTITATIVE_AXIS_H

#include <talipot/GlAxis.h>

namespace tlp {

/**
 * \brief A class to render an axis graduated with numerical values for a given range
 *
 * This class allows to draw a quantitative axis (i.e. an axis axis graduated with numerical values
 * for a given range)
 */
class TLP_GL_SCOPE GlQuantitativeAxis : public GlAxis {

public:
  /**
   * GlQuantitativeAxis constructor. Create an quantitative axis without graduations (need to call
   * setAxisParameters to build them)
   *
   * \param axisName the name of the axis
   * \axisBaseCoord the base coord of the axis (if the axis is horizontal, it is the the left end,
   * if vertical it is the down end)
   * \axisLength the length of the axis
   * \axisOrientation the orientation of the axis, 2 possible values (HORIZONTAL_AXIS or
   * VERTICAL_AXIS)
   * \axisColor the color of the axis
   * \addArrow If true, an arrow will be added to one end of the axis according to the axis order
   * (ascending or descending)
   * \ascendingOrder If true, the min value will be at the bottom end and the max will be at the top
   * end if the axis is vertical (min at the left and max at the right if it is horizontal). If
   * false this positions are switched
   */
  GlQuantitativeAxis(const std::string &axisName, const Coord &axisBaseCoord,
                     const float axisLength, const AxisOrientation &axisOrientation,
                     const Color &axisColor, const bool addArrow = true,
                     const bool ascendingOrder = true);

  /**
   * Method to set the quantitative axis parameters. A call to updateAxis has to be done after
   * calling this method to build or update the axis graduations
   *
   * \param min the min value of the range the axis represents
   * \param max the max value of the range the axis represents
   * \param nbGraduations the number of graduations to build
   * \param axisGradsLabelsPosition the relative position of the axis graduations label. Two
   * possible values : LEFT_OR_BELOW (if the axis is vertical, labels will be on the left of the
   * axis, otherwise below) or RIGHT_OR_ABOVE
   * \param drawFirstLabel If false, the first graduation label will not be drawn (useful when some
   * axis have the same base coord to avoid labels overlapping)
   */
  void setAxisParameters(const double min, const double max, const uint nbGraduations,
                         const LabelPosition &axisGradsLabelsPosition = LEFT_OR_BELOW,
                         const bool drawFirstLabel = true);

  void setAxisParameters(const long long min, const long long max, const ullong incrementStep,
                         const LabelPosition &axisGradsLabelsPosition = LEFT_OR_BELOW,
                         const bool drawFirstLabel = true);

  void setAxisParameters(const int min, const int max, const uint incrementStep,
                         const LabelPosition &axisGradsLabelsPosition = LEFT_OR_BELOW,
                         const bool drawFirstLabel = true) {
    setAxisParameters(static_cast<long long>(min), static_cast<long long>(max),
                      static_cast<ullong>(incrementStep), axisGradsLabelsPosition, drawFirstLabel);
  }

  void setNbGraduations(const uint nbGraduations) {
    this->nbGraduations = nbGraduations;
  }

  /**
   * Method to set a logarithmic scale on the axis. A call to updateAxis has to be done after
   * calling this method to build or update the axis graduations
   *
   * \param logScale If true, activate the logarithmic scale on the axis
   * \param logBase If filled, set the logarithm base
   */
  void setLogScale(const bool logScale, const uint logBase = 10);

  /**
   * Method to set the order of the values on the axis (ascending or descending). A call to
   * updateAxis has to be done after calling this method to build or update the axis graduations
   */
  void setAscendingOrder(const bool ascendingOrder) {
    this->ascendingOrder = ascendingOrder;
  }

  /**
   * Method to update the axis drawing. It has to be called when one (or more) of the setters method
   * above has been used.
   * This method redraw the whole axis and the graduations.
   */
  void updateAxis() override;

  /**
   * Method to get the axis point coordinates for a given value
   *
   * \param value the value we want to retrieve axis point coordinates
   */
  Coord getAxisPointCoordForValue(double value) const;

  /**
   * Method to get the value associated to an axis point
   *
   * \param axisPointCoord the axis point coordinates we want to retrieve associated value
   */
  double getValueForAxisPoint(const Coord &axisPointCoord);

  /**
   * Method to get the order of the values on the axis (ascending or descending)
   */
  bool hasAscendingOrder() const {
    return ascendingOrder;
  }

  double getAxisMinValue() const {
    return min;
  }

  double getAxisMaxValue() const {
    return max;
  }

private:
  void buildAxisGraduations();
  void addArrowDrawing();

  double min, max, scale;
  double minLog, maxLog;
  uint nbGraduations;
  LabelPosition axisGradsLabelsPosition;
  bool drawFistLabel;
  bool ascendingOrder;
  bool addArrow;
  Coord captionCenterCoord;
  bool logScale;
  uint logBase;
  bool integerScale;
  ullong incrementStep;
  bool minMaxSet;
};
}

#endif // TALIPOT_GL_QUANTITATIVE_AXIS_H
