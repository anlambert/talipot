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

#ifndef TALIPOT_VIEW_SETTINGS_H
#define TALIPOT_VIEW_SETTINGS_H

#include <talipot/config.h>
#include <talipot/Color.h>
#include <talipot/Size.h>
#include <talipot/Graph.h>
#include <talipot/Observable.h>
#include <talipot/Singleton.h>

#include <string>
#include <map>

namespace tlp {

class TLP_SCOPE NodeShape {

public:
  enum NodeShapes {
    Billboard = 7,
    BottomShadowedSphere = 21,
    Circle = 14,
    Cone = 3,
    Cross = 8,
    Cube = 0,
    CubeOutlined = 1,
    CubeOutlinedTransparent = 9,
    Cylinder = 6,
    Diamond = 5,
    GlowSphere = 16,
    HalfCylinder = 10,
    Hexagon = 13,
    LeftBottomShadowedSphere = 22,
    Pentagon = 12,
    RightBottomShadowedSphere = 23,
    Ring = 15,
    RoundedBox = 18,
    Sphere = 2,
    Square = 4,
    Triangle = 11,
    Window = 17,
    Star = 19,
    FontAwesomeIcon = 20,
    Icon = 20
  };
};

class TLP_SCOPE EdgeShape {

public:
  enum EdgeShapes { Polyline = 0, BezierCurve = 4, CatmullRomCurve = 8, CubicBSplineCurve = 16 };
};

class TLP_SCOPE EdgeExtremityShape {

public:
  enum EdgeExtremityShapes {
    None = -1,
    Arrow = 50,
    Circle = 14,
    Cone = 3,
    Cross = 8,
    Cube = 0,
    CubeOutlinedTransparent = 9,
    Cylinder = 6,
    Diamond = 5,
    GlowSphere = 16,
    Hexagon = 13,
    Pentagon = 12,
    Ring = 15,
    Sphere = 2,
    Square = 4,
    Star = 19,
    FontAwesomeIcon = 20,
    Icon = 20
  };
};

class TLP_SCOPE LabelPosition {

public:
  enum LabelPositions { Center = 0, Top = 1, Bottom = 2, Left = 3, Right = 4 };
};

class ViewSettings;
DECLARE_DLL_TEMPLATE_INSTANCE(Singleton<ViewSettings>, TLP_TEMPLATE_DECLARE_SCOPE)

class TLP_SCOPE ViewSettings : public Observable, public Singleton<ViewSettings> {

  friend class Singleton<ViewSettings>;

public:
  struct TLP_SCOPE ApplicationDefault {
    static const Color SelectionColor;
    static const Color NodeColor;
    static const Color EdgeColor;
    static const Color NodeBorderColor;
    static const Color EdgeBorderColor;
    static const Color LabelColor;
    static const Color LabelBorderColor;
    static const float NodeBorderWidth;
    static const float EdgeBorderWidth;
    static const float LabelBorderWidth;
    static const int LabelPosition;
    static const Size NodeSize;
    static const Size EdgeSize;
    static const int NodeShape;
    static const int EdgeShape;
    static const int EdgeExtremitySrcShape;
    static const int EdgeExtremityTgtShape;
    static const Size EdgeExtremitySrcSize;
    static const Size EdgeExtremityTgtSize;
    static const std::string Font;
    static const int FontSize;
  };

  typedef std::map<LabelPosition::LabelPositions, std::string> labelmap;

  static labelmap POSITION_LABEL_MAP;

  static Color defaultSelectionColor();
  static void setDefaultSelectionColor(const Color &color);

  static Color defaultColor(ElementType elem);
  static void setDefaultColor(ElementType elem, const Color &color);

  static Color defaultBorderColor(ElementType elem);
  static void setDefaultBorderColor(ElementType elem, const Color &color);

  static float defaultBorderWidth(ElementType elem);
  static void setdefaultBorderWidth(ElementType elem, float borderWidth);

  static Color defaultLabelColor();
  static void setDefaultLabelColor(const Color &color);

  static Color defaultLabelBorderColor();
  static void setDefaultLabelBorderColor(const Color &color);

  static float defaultLabelBorderWidth();
  static void setDefaultLabelBorderWidth(float borderWidth);

  static int defaultLabelPosition();
  static void setDefaultLabelPosition(int position);

  static Size defaultSize(ElementType elem);
  static void setDefaultSize(ElementType elem, const Size &size);

  static int defaultShape(ElementType elem);
  static void setDefaultShape(ElementType elem, int shape);

  static int defaultEdgeExtremitySrcShape();
  static void setDefaultEdgeExtremitySrcShape(int shape);

  static int defaultEdgeExtremityTgtShape();
  static void setDefaultEdgeExtremityTgtShape(int shape);

  static Size defaultEdgeExtremitySrcSize();
  static void setDefaultEdgeExtremitySrcSize(const Size &size);

  static Size defaultEdgeExtremityTgtSize();
  static void setDefaultEdgeExtremityTgtSize(const Size &size);

  static std::string defaultFont();
  static void setDefaultFont(const std::string &fontFile);

  static int defaultFontSize();
  static void setDefaultFontSize(int fontSize);

private:
  ViewSettings() = default;
};

enum class ViewSettingsEventType {
  TLP_DEFAULT_COLOR_MODIFIED,
  TLP_DEFAULT_SHAPE_MODIFIED,
  TLP_DEFAULT_SIZE_MODIFIED,
  TLP_DEFAULT_LABEL_COLOR_MODIFIED,
  TLP_DEFAULT_SELECTION_COLOR_MODIFIED,
};

class TLP_SCOPE ViewSettingsEvent : public tlp::Event {

public:
  ViewSettingsEvent(const Color &color, ViewSettingsEventType type)
      : Event(ViewSettings::instance(), EventType::TLP_MODIFICATION), _type(type), _color(color) {}

  ViewSettingsEvent(ElementType elem, const Color &color)
      : Event(ViewSettings::instance(), EventType::TLP_MODIFICATION),
        _type(ViewSettingsEventType::TLP_DEFAULT_COLOR_MODIFIED), _elem(elem), _color(color) {}

  ViewSettingsEvent(ElementType elem, const Size &size)
      : Event(ViewSettings::instance(), EventType::TLP_MODIFICATION),
        _type(ViewSettingsEventType::TLP_DEFAULT_SIZE_MODIFIED), _elem(elem), _size(size) {}

  ViewSettingsEvent(ElementType elem, int shape)
      : Event(ViewSettings::instance(), EventType::TLP_MODIFICATION),
        _type(ViewSettingsEventType::TLP_DEFAULT_SHAPE_MODIFIED), _elem(elem), _shape(shape) {}

  ViewSettingsEventType getType() const {
    return _type;
  }

  ElementType getElementType() const {
    return _elem;
  }

  Color getColor() const {
    return _color;
  }

  Size getSize() const {
    return _size;
  }

  int getShape() const {
    return _shape;
  }

private:
  ViewSettingsEventType _type;
  ElementType _elem;
  Color _color;
  Size _size;
  int _shape;
};
}

#endif // TALIPOT_VIEW_SETTINGS_H
