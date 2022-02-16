/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

namespace tlp {

class ColorScale {
%Docstring
This class represents a color scale to perform color mapping.
The color scale can be either a gradient or defined by colors associated to consecutive intervals.
If the color scale is a gradient, returned colors are interpolated in function of a position between 0.0 and 1.0.
If the color scale is not a gradient returned colors are computed according to the interval the position belongs to.

The sample code below illustrates how to create and manipulate a color scale::

  # Creates an empty color scale (default constructor creates a heatmap from blue to red).
  colorScale = tlp.ColorScale([])

  # Color scale initialization : from blue to red with gradient.
  colorScale.setColorAtPos(0.0, tlp.Color.Blue)
  colorScale.setColorAtPos(1.0, tlp.Color.Red)

  # The following syntaxes could also be used
  # colors = [tlp.Color.Blue, tlp.Color.Red]
  # colorScale.setColorScale(colors)
  #    or
  # colorScale = tlp.ColorScale(colors)
  #
  # colorsMap = {0: tlp.Color.Blue, 1: tlp.Color.Red}
  # colorScale.setColorMap(colors)
  #   or
  # colorScale = tlp.ColorScale(colorsMap)

  # Gets the color for the position 0.5, i.e. tlp.Color(127,0,127).
  color = colorScale.getColorAtPos(0.5);

  # Reinitializes the color scale : from blue to red without gradient.
  newColors = [tlp.Color.Blue, tlp.Color.Red]
  colorScale.setColorScale(newColors,False)

  # Gets the color for the position 0.3, i.e. tlp.Color(0,0,255).
  color = colorScale.getColorAtPos(0.3)

  # Gets the color for the position 0.7, i.e. tlp.Color(255,0,0).
  color = colorScale.getColorAtPos(0.7)

Color scales can also be defined in a more Pythonic way through
the use of a list or a dictionary.

%End

%TypeHeaderCode
#include <talipot/ColorScale.h>
%End

%ConvertToTypeCode

  PyObject *key = NULL;
  PyObject *val = NULL;
  Py_ssize_t pos = 0;

  if (sipIsErr == NULL) {
    if (sipCanConvertToType(sipPy, sipFindType("tlp::ColorScale"), SIP_NOT_NONE|SIP_NO_CONVERTORS)) {
      return 1;
    } else if (PyDict_Check(sipPy)) {
      while (PyDict_Next(sipPy, &pos, &key, &val)) {
        if (!PyNumber_Check(key)) {
          printErrorMessage("Error : when defining a Talipot color scale through a dictionary, its keys must have a number type.");
          return 0;
        }
        double step = PyFloat_AsDouble(key);
        if (step < 0 || step > 1) {
          printErrorMessage("Error : color scale step must be must greater or equal than 0.0 and lesser or equal than 1.0.");
          return 0;
        }
        if (!sipCanConvertToType(val, sipFindType("tlp::Color"), SIP_NOT_NONE)) {
          printErrorMessage("Error : when defining a Talipot color scale through a dictionary, its values must have the tlp.Color type.");
          return 0;
        }
      }
      return 1;
    } else if (PyList_Check(sipPy)) {
        for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
          PyObject *item = PyList_GET_ITEM(sipPy, i);
          if (!sipCanConvertToType(item, sipFindType("tlp::Color"), SIP_NOT_NONE)) {
            printErrorMessage("Error : when defining a Talipot color scale through a list, it must only contain instances of the tlp.Color type.");
            return 0;
          }
        }
        return 1;
    }
    return 0;
  }

  int state=0, err=0;

  if (PyDict_Check(sipPy)) {
    std::map<float, tlp::Color> colorMap;
    while (PyDict_Next(sipPy, &pos, &key, &val)) {
      double step = PyFloat_AsDouble(key);
      const sipTypeDef *sipType  = sipFindType("tlp::Color");
      tlp::Color *color = static_cast<tlp::Color*>(sipConvertToType(val, sipType, NULL, SIP_NOT_NONE, &state, &err));
      colorMap[step] = *color;
      sipReleaseType(color, sipType, state);
    }
    *sipCppPtr = new tlp::ColorScale();
    (*sipCppPtr)->setColorMap(colorMap);
  } else if (PyList_Check(sipPy)) {
    std::vector<tlp::Color> colors;
    for (Py_ssize_t i = 0; i < PyList_GET_SIZE(sipPy); ++i) {
      const sipTypeDef *sipType  = sipFindType("tlp::Color");
      PyObject *item = PyList_GET_ITEM(sipPy, i);
      tlp::Color *color = static_cast<tlp::Color*>(sipConvertToType(item, sipType, NULL, SIP_NOT_NONE, &state, &err));
      colors.push_back(*color);
      sipReleaseType(color, sipType, state);
    }
    *sipCppPtr = new tlp::ColorScale(colors);
  } else {
    const sipTypeDef *sipType  = sipFindType("tlp::ColorScale");
    tlp::ColorScale *cs = static_cast<tlp::ColorScale*>(sipConvertToType(sipPy, sipType, NULL, SIP_NOT_NONE|SIP_NO_CONVERTORS, &state, &err));
    *sipCppPtr = new tlp::ColorScale(*cs);
    sipReleaseType(cs, sipType, state);
  }

  if (!*sipCppPtr) {
    *sipIsErr = 1;
  }

  return sipGetState(sipTransferObj);

%End

public:

//==========================================================================================

  ColorScale();
  ColorScale(const std::vector<tlp::Color> &colors, const bool gradient = true);
  ColorScale(const std::map<float, tlp::Color> &colors, const bool gradient = true);
  ColorScale(const tlp::ColorScale& scale);
  virtual ~ColorScale();

//==========================================================================================

  void clear();
%Docstring
tlp.ColorScale.clear()

Clears the color scale.
%End

//==========================================================================================

  virtual void setColorScale(const std::vector<tlp::Color> &colors, const bool gradient = true);
%Docstring
tlp.ColorScale.setColorScale(colors, gradient=True)

This method configures the color scale from a list of colors and
associates regular stop points to them.

.. warning:: If the scale was previously configured the old configuration is lost.

:param colors: The colors to use in the color scale.
:type colors: list of :class:`tlp.Color`
:param gradient: If set to :const:`True`, the color scale is a gradient
:type gradient: boolean
%End

//==========================================================================================

  virtual void setColorAtPos(const float pos, const tlp::Color &color);
%Docstring
tlp.ColorScale.setColorAtPos(pos, color)

This method adds a color to the color scale at a specific position.

:param pos: the position in the color scale (0.0 <= pos <= 1.0)
:type pos: float
:param color: the color to add at the specified position
:type color: :class:`tlp.Color`
%End

//==========================================================================================

  virtual tlp::Color getColorAtPos(const float pos) const;
%Docstring
tlp.ColorScale.getColorAtPos(pos)

This method computes the color associated to a specific position in the color scale and returns it.

:param pos: This value defines the position of the color in the scale and must be between 0.0 and 1.0 (it will be clamped otherwise).
:type pos: float
:rtype: :class:`tlp.Color`
%End

//==========================================================================================

  bool isGradient() const;
%Docstring
tlp.ColorScale.isGradient()

Returns :const:`True` if the color scale is a gradient.

:rtype: boolean
%End

//==========================================================================================

  void setGradient(const bool gradient);
%Docstring
tlp.ColorScale.setGradient(gradient)

Specify whether the color scale must be considered as a gradient.

:param gradient: the flag value
:type gradient: boolean
%End

//==========================================================================================

  const std::map<float, tlp::Color> &getColorMap() const /NoCopy/;
%Docstring
tlp.ColorScale.getColorMap()

Returns a dictionary corresponding to the color scale.

:rtype: a dictionary whose indexes are of type float and the values of type :class:`tlp.Color`
%End

//==========================================================================================

  bool hasRegularStops() const;
%Docstring
tlp.ColorScale.hasRegularStops()

Returns :const:`True` if the color scale has regular stop points, meaning the distance between each consecutive stop is constant.

:rtype: boolean
%End

//==========================================================================================

  bool colorScaleInitialized() const;
%Docstring
tlp.ColorScale.colorScaleInitialized()

Returns :const;`True` if the color scale is initialized, i.e. the underlying color map is not empty.

:rtype: boolean
%End

//==========================================================================================

  void setColorMap(const std::map<float, tlp::Color>& colorMap);
%Docstring
tlp.ColorScale.setColorMap(colorMap)

Sets the map of stop points and colors used to perform color mapping.

.. warning:: The keys of the map must be between 0.0 and 1.0, other values will be ignored.

:param colorMap: the color map that defines the color scale
:type colorMap: a dictionary with float keys and :class:`tlp.Color` values

%End

//==========================================================================================

  void setColorMapTransparency(unsigned char transparency);
%Docstring
tlp.ColorScale.setColorMapTransparency(alpha)

Sets the transparency of all the colors in the underlying map.

:param alpha: the alpha value of each colors (0 <= alpha <= 255)
:type alpha: integer
%End

//==========================================================================================

  bool operator==(const tlp::ColorScale &) const;

  std::string __repr__() const;
%MethodCode
  std::ostringstream oss;
  std::vector<float> v;

  auto colorMap = sipCpp->getColorMap();

  for(const auto &it : colorMap) {
    v.push_back(it.first);
  }

  std::sort(v.begin(), v.end());
  oss << "{";
  for (size_t i = 0 ; i < v.size() ; ++i) {
    oss << v[i] << ": " << colorMap[v[i]];
    if (i != v.size() - 1) {
      oss << ", ";
    }
  }
  oss << "}";
  sipRes = new std::string(oss.str());
%End

};
};
