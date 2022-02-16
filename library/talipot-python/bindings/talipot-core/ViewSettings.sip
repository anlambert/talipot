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

%ModuleHeaderCode
#include <talipot/ViewSettings.h>

namespace tlp {

class NodeShapeConstants {

public:

  static const int Billboard = 7;
  static const int Circle = 14;
  static const int Cone = 3;
  static const int Cross = 8;
  static const int Cube = 0;
  static const int CubeOutlined = 1;
  static const int CubeOutlinedTransparent = 9;
  static const int Cylinder = 6;
  static const int Diamond = 5;
  static const int GlowSphere = 16;
  static const int HalfCylinder = 10;
  static const int Hexagon = 13;
  static const int Pentagon = 12;
  static const int Ring = 15;
  static const int RoundedBox = 18;
  static const int Sphere = 2;
  static const int Square = 4;
  static const int Triangle = 11;
  static const int Window = 17;
  static const int Star = 19;
  static const int FontAwesomeIcon = 20;
  static const int Icon = 20;

};

class EdgeShapeConstants {

public:

  static const int Polyline = 0;
  static const int BezierCurve = 4;
  static const int CatmullRomCurve = 8;
  static const int CubicBSplineCurve = 16;
};

class EdgeExtremityShapeConstants {

public:

  static const int None = -1;
  static const int Arrow = 50;
  static const int Circle = 14;
  static const int Cone = 3;
  static const int Cross = 8;
  static const int Cube = 0;
  static const int CubeOutlinedTransparent = 9;
  static const int Cylinder = 6;
  static const int Diamond = 5;
  static const int GlowSphere = 16;
  static const int Hexagon = 13;
  static const int Pentagon = 12;
  static const int Ring = 15;
  static const int Sphere = 2;
  static const int Square = 4;
  static const int Star = 19;
  static const int FontAwesomeIcon = 20;
  static const int Icon = 20;

};

class LabelPositionConstants {

public:

  static const int Center = 0;
  static const int Top = 1;
  static const int Bottom = 2;
  static const int Left = 3;
  static const int Right = 4;
};


}

%End

namespace tlp {

class NodeShapeConstants /NoDefaultCtors, PyName=NodeShape/ {

public:

  static const int Billboard;
  static const int Circle;
  static const int Cone;
  static const int Cross;
  static const int Cube;
  static const int CubeOutlined;
  static const int CubeOutlinedTransparent;
  static const int Cylinder;
  static const int Diamond;
  static const int GlowSphere;
  static const int HalfCylinder;
  static const int Hexagon;
  static const int Pentagon;
  static const int Ring;
  static const int RoundedBox;
  static const int Sphere;
  static const int Square;
  static const int Triangle;
  static const int Window;
  static const int Star;
  static const int FontAwesomeIcon;
  static const int Icon;

};

class EdgeShapeConstants /NoDefaultCtors, PyName=EdgeShape/ {

public:

  static const int Polyline;
  static const int BezierCurve;
  static const int CatmullRomCurve;
  static const int CubicBSplineCurve;
};

class EdgeExtremityShapeConstants /NoDefaultCtors, PyName=EdgeExtremityShape/ {

public:

  static const int None;
  static const int Arrow;
  static const int Circle;
  static const int Cone;
  static const int Cross;
  static const int Cube;
  static const int CubeOutlinedTransparent;
  static const int Cylinder;
  static const int Diamond;
  static const int GlowSphere;
  static const int Hexagon;
  static const int Pentagon;
  static const int Ring;
  static const int Sphere;
  static const int Square;
  static const int Star;
  static const int FontAwesomeIcon;
  static const int Icon;

};

class LabelPositionConstants /NoDefaultCtors, PyName=LabelPosition/ {

public:

  static const int Center;
  static const int Top;
  static const int Bottom;
  static const int Left;
  static const int Right;
};

class ViewSettings : tlp::Observable /NoDefaultCtors/ {
%TypeHeaderCode
#include <talipot/ViewSettings.h>
%End

public:

  struct ApplicationDefault /NoDefaultCtors/ {
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

  static tlp::Color defaultSelectionColor();

  static void setDefaultSelectionColor(const tlp::Color& color);

  static tlp::Color defaultColor(tlp::ElementType elem);

  static void setDefaultColor(tlp::ElementType elem, const tlp::Color& color);

  static tlp::Color defaultBorderColor(tlp::ElementType elem);

  static void setDefaultBorderColor(tlp::ElementType elem, const tlp::Color& color);

  static float defaultBorderWidth(tlp::ElementType elem);

  static void setdefaultBorderWidth(tlp::ElementType elem, float borderWidth);

  static tlp::Color defaultLabelColor();

  static void setDefaultLabelColor(const tlp::Color& color);

  static tlp::Color defaultLabelBorderColor();

  static void setDefaultLabelBorderColor(const tlp::Color& color);

  static float defaultLabelBorderWidth();

  static void setDefaultLabelBorderWidth(float borderWidth);

  static int defaultLabelPosition();

  static void setDefaultLabelPosition(int position);

  static tlp::Size defaultSize(tlp::ElementType elem);

  static void setDefaultSize(tlp::ElementType elem, const tlp::Size& size);

  static int defaultShape(tlp::ElementType elem);

  static void setDefaultShape(tlp::ElementType elem, int shape);

  static int defaultEdgeExtremitySrcShape();

  static void setDefaultEdgeExtremitySrcShape(int shape);

  static int defaultEdgeExtremityTgtShape();

  static void setDefaultEdgeExtremityTgtShape(int shape);

  static tlp::Size defaultEdgeExtremitySrcSize();

  static void setDefaultEdgeExtremitySrcSize(const tlp::Size &size);

  static tlp::Size defaultEdgeExtremityTgtSize();

  static void setDefaultEdgeExtremityTgtSize(const tlp::Size &size);

  static std::string defaultFont();

  static void setDefaultFont(const std::string &fontFile);

  static int defaultFontSize();

  static void setDefaultFontSize(int fontSize);

private:

  ViewSettings();

};

enum class ViewSettingsEventType {
  TLP_DEFAULT_COLOR_MODIFIED,
  TLP_DEFAULT_SHAPE_MODIFIED,
  TLP_DEFAULT_SIZE_MODIFIED,
  TLP_DEFAULT_LABEL_COLOR_MODIFIED
};

class ViewSettingsEvent : public tlp::Event /NoDefaultCtors/ {
%TypeHeaderCode
#include <talipot/ViewSettings.h>
%End

%ConvertToSubClassCode
  if (dynamic_cast<tlp::ViewSettingsEvent*>(sipCpp)) {
    sipType = sipFindType("tlp::ViewSettingsEvent");
  } else {
    sipType = NULL;
  }
%End

public:

  ViewSettingsEventType getType();

  tlp::ElementType getElementType();

  tlp::Color getColor();

  tlp::Size getSize();

  int getShape();

};
};
