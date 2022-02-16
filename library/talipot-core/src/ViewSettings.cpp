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

#include <talipot/ViewSettings.h>

namespace tlp {

INSTANTIATE_DLL_TEMPLATE(Singleton<ViewSettings>, TLP_TEMPLATE_DEFINE_SCOPE)

const Color ViewSettings::ApplicationDefault::SelectionColor = {23, 81, 228};
const Color ViewSettings::ApplicationDefault::NodeColor = {255, 95, 95};
const Color ViewSettings::ApplicationDefault::EdgeColor = {180, 180, 180};
const Color ViewSettings::ApplicationDefault::NodeBorderColor = Color::Black;
const Color ViewSettings::ApplicationDefault::EdgeBorderColor = Color::Black;
const Color ViewSettings::ApplicationDefault::LabelColor = Color::Black;
const Color ViewSettings::ApplicationDefault::LabelBorderColor = Color::Black;
const float ViewSettings::ApplicationDefault::NodeBorderWidth = 0;
const float ViewSettings::ApplicationDefault::EdgeBorderWidth = 0;
const float ViewSettings::ApplicationDefault::LabelBorderWidth = 1;
const int ViewSettings::ApplicationDefault::LabelPosition = LabelPosition::Center;
const Size ViewSettings::ApplicationDefault::NodeSize = 1;
const Size ViewSettings::ApplicationDefault::EdgeSize = {0.125, 0.125, 0.5};
const int ViewSettings::ApplicationDefault::NodeShape = NodeShape::Circle;
const int ViewSettings::ApplicationDefault::EdgeShape = EdgeShape::Polyline;
const int ViewSettings::ApplicationDefault::EdgeExtremitySrcShape = EdgeExtremityShape::None;
const int ViewSettings::ApplicationDefault::EdgeExtremityTgtShape = EdgeExtremityShape::Arrow;
const Size ViewSettings::ApplicationDefault::EdgeExtremitySrcSize = {1, 1, 0};
const Size ViewSettings::ApplicationDefault::EdgeExtremityTgtSize = {1, 1, 0};
const std::string ViewSettings::ApplicationDefault::Font = "DejaVu Sans-Book";
const int ViewSettings::ApplicationDefault::FontSize = 18;

static Color _defaultSelectionColor = ViewSettings::ApplicationDefault::SelectionColor;
static Color _defaultNodeColor = ViewSettings::ApplicationDefault::NodeColor;
static Color _defaultEdgeColor = ViewSettings::ApplicationDefault::EdgeColor;
static Color _defaultNodeBorderColor = ViewSettings::ApplicationDefault::NodeBorderColor;
static Color _defaultEdgeBorderColor = ViewSettings::ApplicationDefault::EdgeBorderColor;
static Color _defaultLabelColor = ViewSettings::ApplicationDefault::LabelColor;
static Color _defaultLabelBorderColor = ViewSettings::ApplicationDefault::LabelBorderColor;
static float _defaultNodeBorderWidth = ViewSettings::ApplicationDefault::NodeBorderWidth;
static float _defaultEdgeBorderWidth = ViewSettings::ApplicationDefault::EdgeBorderWidth;
static float _defaultLabelBorderWidth = ViewSettings::ApplicationDefault::LabelBorderWidth;
static int _defaultLabelPosition = ViewSettings::ApplicationDefault::LabelPosition;
static Size _defaultNodeSize = ViewSettings::ApplicationDefault::NodeSize;
static Size _defaultEdgeSize = ViewSettings::ApplicationDefault::EdgeSize;
static int _defaultNodeShape = ViewSettings::ApplicationDefault::NodeShape;
static int _defaultEdgeShape = ViewSettings::ApplicationDefault::EdgeShape;
static int _defaultEdgeExtremitySrcShape = ViewSettings::ApplicationDefault::EdgeExtremitySrcShape;
static int _defaultEdgeExtremityTgtShape = ViewSettings::ApplicationDefault::EdgeExtremityTgtShape;
static Size _defaultEdgeExtremitySrcSize = ViewSettings::ApplicationDefault::EdgeExtremitySrcSize;
static Size _defaultEdgeExtremityTgtSize = ViewSettings::ApplicationDefault::EdgeExtremityTgtSize;
static std::string _defaultFont = ViewSettings::ApplicationDefault::Font;
static int _defaultFontSize = ViewSettings::ApplicationDefault::FontSize;

Color ViewSettings::defaultSelectionColor() {
  return _defaultSelectionColor;
}

void ViewSettings::setDefaultSelectionColor(const Color &color) {
  if (color == defaultSelectionColor()) {
    return;
  }
  _defaultSelectionColor = color;
  instance().sendEvent(
      ViewSettingsEvent(color, ViewSettingsEventType::TLP_DEFAULT_SELECTION_COLOR_MODIFIED));
}

Color ViewSettings::defaultColor(ElementType elem) {
  if (elem == ElementType::NODE) {
    return _defaultNodeColor;
  } else {
    return _defaultEdgeColor;
  }
}

void ViewSettings::setDefaultColor(ElementType elem, const Color &color) {
  if (color == defaultColor(elem)) {
    return;
  }

  if (elem == ElementType::NODE) {
    _defaultNodeColor = color;
  } else {
    _defaultEdgeColor = color;
  }

  instance().sendEvent(ViewSettingsEvent(elem, color));
}

Color ViewSettings::defaultBorderColor(ElementType elem) {
  if (elem == ElementType::NODE) {
    return _defaultNodeBorderColor;
  } else {
    return _defaultEdgeBorderColor;
  }
}

void ViewSettings::setDefaultBorderColor(ElementType elem, const Color &color) {
  if (elem == ElementType::NODE) {
    _defaultNodeBorderColor = color;
  } else {
    _defaultEdgeBorderColor = color;
  }
}

float ViewSettings::defaultBorderWidth(ElementType elem) {
  if (elem == ElementType::NODE) {
    return _defaultNodeBorderWidth;
  } else {
    return _defaultEdgeBorderWidth;
  }
}

void ViewSettings::setdefaultBorderWidth(ElementType elem, float borderWidth) {
  if (elem == ElementType::NODE) {
    _defaultNodeBorderWidth = borderWidth;
  } else {
    _defaultEdgeBorderWidth = borderWidth;
  }
}

Color ViewSettings::defaultLabelColor() {
  return _defaultLabelColor;
}

void ViewSettings::setDefaultLabelColor(const Color &color) {
  if (color == _defaultLabelColor) {
    return;
  }

  _defaultLabelColor = color;
  instance().sendEvent(
      ViewSettingsEvent(color, ViewSettingsEventType::TLP_DEFAULT_LABEL_COLOR_MODIFIED));
}

Color ViewSettings::defaultLabelBorderColor() {
  return _defaultLabelBorderColor;
}

void ViewSettings::setDefaultLabelBorderColor(const Color &color) {
  _defaultLabelBorderColor = color;
}

float ViewSettings::defaultLabelBorderWidth() {
  return _defaultLabelBorderWidth;
}

void ViewSettings::setDefaultLabelBorderWidth(float borderWidth) {
  _defaultLabelBorderWidth = borderWidth;
}

ViewSettings::labelmap ViewSettings::POSITION_LABEL_MAP = {{LabelPosition::Center, "Center"},
                                                           {LabelPosition::Top, "Top"},
                                                           {LabelPosition::Bottom, "Bottom"},
                                                           {LabelPosition::Left, "Left"},
                                                           {LabelPosition::Right, "Right"}};

int ViewSettings::defaultLabelPosition() {
  return _defaultLabelPosition;
}

void ViewSettings::setDefaultLabelPosition(int position) {
  _defaultLabelPosition = position;
}

Size ViewSettings::defaultSize(ElementType elem) {
  if (elem == ElementType::NODE) {
    return _defaultNodeSize;
  } else {
    return _defaultEdgeSize;
  }
}

void ViewSettings::setDefaultSize(ElementType elem, const Size &size) {
  if (size == defaultSize(elem)) {
    return;
  }

  if (elem == ElementType::NODE) {
    _defaultNodeSize = size;
  } else {
    _defaultEdgeSize = size;
  }

  instance().sendEvent(ViewSettingsEvent(elem, size));
}

int ViewSettings::defaultShape(ElementType elem) {
  if (elem == ElementType::NODE) {
    return _defaultNodeShape;
  } else {
    return _defaultEdgeShape;
  }
}

void ViewSettings::setDefaultShape(ElementType elem, int shape) {
  if (shape == defaultShape(elem)) {
    return;
  }

  if (elem == ElementType::NODE) {
    _defaultNodeShape = shape;
  } else {
    _defaultEdgeShape = shape;
  }

  instance().sendEvent(ViewSettingsEvent(elem, shape));
}

int ViewSettings::defaultEdgeExtremitySrcShape() {
  return _defaultEdgeExtremitySrcShape;
}

void ViewSettings::setDefaultEdgeExtremitySrcShape(int shape) {
  _defaultEdgeExtremitySrcShape = shape;
}

int ViewSettings::defaultEdgeExtremityTgtShape() {
  return _defaultEdgeExtremityTgtShape;
}

void ViewSettings::setDefaultEdgeExtremityTgtShape(int shape) {
  _defaultEdgeExtremityTgtShape = shape;
}

Size ViewSettings::defaultEdgeExtremitySrcSize() {
  return _defaultEdgeExtremitySrcSize;
}

void ViewSettings::setDefaultEdgeExtremitySrcSize(const Size &size) {
  _defaultEdgeExtremitySrcSize = size;
}

Size ViewSettings::defaultEdgeExtremityTgtSize() {
  return _defaultEdgeExtremityTgtSize;
}

void ViewSettings::setDefaultEdgeExtremityTgtSize(const Size &size) {
  _defaultEdgeExtremityTgtSize = size;
}

std::string ViewSettings::defaultFont() {
  return _defaultFont;
}

void ViewSettings::setDefaultFont(const std::string &fontFile) {
  _defaultFont = fontFile;
}

int ViewSettings::defaultFontSize() {
  return _defaultFontSize;
}

void ViewSettings::setDefaultFontSize(int fontSize) {
  _defaultFontSize = fontSize;
}
}
