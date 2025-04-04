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

#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/NumericProperty.h>
#include <talipot/ViewSettings.h>

using namespace tlp;

GlGraphRenderingParameters::GlGraphRenderingParameters()
    : _antialiased(true), _viewArrow(true), _viewNodeLabel(true), _viewEdgeLabel(false),
      _viewMetaLabel(false), _viewOutScreenLabel(false), _elementOrdered(false),
      _elementOrderedDescending(true), _edgeColorInterpolate(true), _edge3D(false),
      _edgeSizeInterpolate(true), _edgeFrontDisplay(true), _displayEdges(true), _displayNodes(true),
      _displayMetaNodes(true), _elementZOrdered(false), _selectedNodesStencil(0x0002),
      _selectedMetaNodesStencil(0x0002), _selectedEdgesStencil(0x0002), _nodesStencil(0xFFFF),
      _metaNodesStencil(0xFFFF), _edgesStencil(0xFFFF), _nodesLabelStencil(0xFFFF),
      _metaNodesLabelStencil(0xFFFF), _edgesLabelStencil(0xFFFF), _labelScaled(false),
      _labelFixedFontSize(false), _labelMinSize(5), _labelMaxSize(30), _labelsDensity(0),
      _labelsAreBillboarded(false), _fontsPath(tlp::TalipotBitmapDir),
      _edgesMaxSizeToNodesSize(true), _selectionColor(ViewSettings::defaultSelectionColor()),
      _displayFilteringProperty(nullptr), _elementOrderingProperty(nullptr) {}
// This function should rewritten completely
DataSet GlGraphRenderingParameters::getParameters() const {
  DataSet data;
  // bool
  data.set("antialiased", _antialiased);
  data.set("arrow", _viewArrow);
  data.set("displayNodes", _displayNodes);
  data.set("displayEdges", _displayEdges);
  data.set("displayMetaNodes", _displayMetaNodes);
  data.set("nodeLabel", _viewNodeLabel);
  data.set("edgeLabel", _viewEdgeLabel);
  data.set("metaLabel", _viewMetaLabel);
  data.set("outScreenLabel", _viewOutScreenLabel);
  data.set("elementOrdered", _elementOrdered);
  data.set("elementZOrdered", _elementZOrdered);
  data.set("edgeColorInterpolation", _edgeColorInterpolate);
  data.set("edgeSizeInterpolation", _edgeSizeInterpolate);
  data.set("edgeFrontDisplay", _edgeFrontDisplay);
  data.set("edge3D", _edge3D);
  data.set("labelScaled", _labelScaled);
  // int
  data.set("labelMinSize", _labelMinSize);
  data.set("labelMaxSize", _labelMaxSize);
  data.set("selectedNodesStencil", _selectedNodesStencil);
  data.set("selectedMetaNodesStencil", _selectedMetaNodesStencil);
  data.set("selectedEdgesStencil", _selectedEdgesStencil);
  data.set("nodesStencil", _nodesStencil);
  data.set("metaNodesStencil", _metaNodesStencil);
  data.set("edgesStencil", _edgesStencil);
  data.set("nodesLabelStencil", _nodesLabelStencil);
  data.set("metaNodesLabelStencil", _metaNodesLabelStencil);
  data.set("edgesLabelStencil", _edgesLabelStencil);
  data.set("edgesMaxSizeToNodesSize", _edgesMaxSizeToNodesSize);
  data.set("selectionColor", _selectionColor);
  data.set("labelsDensity", _labelsDensity);
  data.set("labelsAreBillboarded", _labelsAreBillboarded);
  // ordering
  data.set("elementsOrdered", _elementOrdered);
  data.set("elementsOrderedDescending", _elementOrderedDescending);

  if (_elementOrderingProperty) {
    data.set("elementsOrderingPropertyName", _elementOrderingProperty->getName());
  }

  return data;
}

// This function should rewritten completely
void GlGraphRenderingParameters::setParameters(const DataSet &data) {
  bool b = false;
  Color c;

  if (data.get<bool>("antialiased", b)) {
    setAntialiasing(b);
  }

  if (data.get<bool>("arrow", b)) {
    setViewArrow(b);
  }

  if (data.get<bool>("displayNodes", b)) {
    setDisplayNodes(b);
  }

  if (data.get<bool>("displayMetaNodes", b)) {
    setDisplayMetaNodes(b);
  }

  if (data.get<bool>("displayEdges", b)) {
    setDisplayEdges(b);
  }

  if (data.get<bool>("nodeLabel", b)) {
    setViewNodeLabel(b);
  }

  if (data.get<bool>("edgeLabel", b)) {
    setViewEdgeLabel(b);
  }

  if (data.get<bool>("metaLabel", b)) {
    setViewMetaLabel(b);
  }

  if (data.get<bool>("outScreenLabel", b)) {
    setViewOutScreenLabel(b);
  }

  if (data.get<bool>("elementOrdered", b)) {
    setElementOrdered(b);
  }

  if (data.get<bool>("elementZOrdered", b)) {
    setElementZOrdered(b);
  }

  if (data.get<bool>("edgeColorInterpolation", b)) {
    setEdgeColorInterpolate(b);
  }

  if (data.get<bool>("edgeSizeInterpolation", b)) {
    setEdgeSizeInterpolate(b);
  }

  if (data.get<bool>("edgeFrontDisplay", b)) {
    setEdgeFrontDisplay(b);
  }

  if (data.get<bool>("edge3D", b)) {
    setEdge3D(b);
  }

  if (data.get<bool>("autoScale", b)) {
    setLabelScaled(b);
  }

  if (data.get<bool>("labelScaled", b)) {
    setLabelScaled(b);
  }

  if (data.get<bool>("labelOverlaped", b)) {
    setLabelsDensity(b ? 100 : 0);
  }

  if (data.get<bool>("labelsAreBillboarded", b)) {
    setLabelsAreBillboarded(b);
  }

  uint ui = 0;

  if (data.get<uint>("fontType", ui)) {
    setFontsType(ui);
  }

  int i = 0;

  if (data.get<int>("labelMinSize", i)) {
    setMinSizeOfLabel(i);
  }

  if (data.get<int>("labelMaxSize", i)) {
    setMaxSizeOfLabel(i);
  }

  if (data.get<int>("selectedNodesStencil", i)) {
    setSelectedNodesStencil(i);
  }

  if (data.get<int>("selectedMetaNodesStencil", i)) {
    setSelectedMetaNodesStencil(i);
  }

  if (data.get<int>("selectedEdgesStencil", i)) {
    setSelectedEdgesStencil(i);
  }

  if (data.get<int>("nodesStencil", i)) {
    setNodesStencil(i);
  }

  if (data.get<int>("metaNodesStencil", i)) {
    setMetaNodesStencil(i);
  }

  if (data.get<int>("edgesStencil", i)) {
    setEdgesStencil(i);
  }

  if (data.get<int>("nodesLabelStencil", i)) {
    setNodesLabelStencil(i);
  }

  if (data.get<int>("metaNodesLabelStencil", i)) {
    setMetaNodesLabelStencil(i);
  }

  if (data.get<int>("edgesLabelStencil", i)) {
    setEdgesLabelStencil(i);
  }

  if (data.get<int>("labelsDensity", i)) {
    setLabelsDensity(i);
  }

  if (data.get<bool>("edgesMaxSizeToNodesSize", b)) {
    setEdgesMaxSizeToNodesSize(b);
  }

  if (data.get<Color>("selectionColor", c)) {
    setSelectionColor(c);
  }

  if (data.get<bool>("elementsOrdered", b)) {
    setElementOrdered(b);
  }

  if (data.get<bool>("elementsOrderedDescending", b)) {
    setElementOrderedDescending(b);
  }
}
//====================================================
bool GlGraphRenderingParameters::isViewMetaLabel() const {
  return (_viewMetaLabel);
}
void GlGraphRenderingParameters::setViewMetaLabel(bool b) {
  _viewMetaLabel = b;
}
//====================================================
uint GlGraphRenderingParameters::getFontsType() const {
  if (_labelScaled) {
    return 0;
  } else {
    return 1;
  }
}
void GlGraphRenderingParameters::setFontsType(uint i) {
  if (i == 1) {
    _labelsDensity = 100;
  } else {
    _labelScaled = true;
  }
}
//====================================================
void GlGraphRenderingParameters::setFontsPath(const std::string &path) {
  _fontsPath = path;
}
std::string GlGraphRenderingParameters::getFontsPath() const {
  return _fontsPath;
}
//====================================================
void GlGraphRenderingParameters::setTexturePath(const std::string &path) {
  _texturePath = path;
}
std::string GlGraphRenderingParameters::getTexturePath() const {
  return _texturePath;
}
//====================================================
bool GlGraphRenderingParameters::isDisplayEdges() const {
  return _displayEdges;
}
void GlGraphRenderingParameters::setDisplayEdges(const bool b) {
  _displayEdges = b;
}
//====================================================
bool GlGraphRenderingParameters::isDisplayNodes() const {
  return _displayNodes;
}
void GlGraphRenderingParameters::setDisplayNodes(const bool b) {
  _displayNodes = b;
}
//====================================================
bool GlGraphRenderingParameters::isDisplayMetaNodes() const {
  return _displayMetaNodes;
}
void GlGraphRenderingParameters::setDisplayMetaNodes(const bool b) {
  _displayMetaNodes = b;
}
//====================================================
bool GlGraphRenderingParameters::isAntialiased() const {
  return (_antialiased);
}
void GlGraphRenderingParameters::setAntialiasing(const bool b) {
  _antialiased = b;
}
//====================================================
bool GlGraphRenderingParameters::isViewArrow() const {
  return (_viewArrow);
}
void GlGraphRenderingParameters::setViewArrow(const bool b) {
  _viewArrow = b;
}
//====================================================
bool GlGraphRenderingParameters::isElementOrdered() const {
  return (_elementOrdered);
}
void GlGraphRenderingParameters::setElementOrdered(const bool b) {
  _elementOrdered = b;
}
//====================================================
bool GlGraphRenderingParameters::isElementOrderedDescending() const {
  return (_elementOrderedDescending);
}
void GlGraphRenderingParameters::setElementOrderedDescending(const bool b) {
  _elementOrderedDescending = b;
}
//====================================================
bool GlGraphRenderingParameters::isElementZOrdered() const {
  return (_elementZOrdered);
}
void GlGraphRenderingParameters::setElementZOrdered(const bool b) {
  _elementZOrdered = b;
}
//====================================================
bool GlGraphRenderingParameters::isEdge3D() const {
  return _edge3D;
}
void GlGraphRenderingParameters::setEdge3D(const bool b) {
  _edge3D = b;
}
//====================================================
bool GlGraphRenderingParameters::isViewNodeLabel() const {
  return (_viewNodeLabel);
}
void GlGraphRenderingParameters::setViewNodeLabel(const bool b) {
  _viewNodeLabel = b;
}
//====================================================
bool GlGraphRenderingParameters::isViewEdgeLabel() const {
  return (_viewEdgeLabel);
}
void GlGraphRenderingParameters::setViewEdgeLabel(const bool b) {
  _viewEdgeLabel = b;
}
//====================================================
void GlGraphRenderingParameters::setViewOutScreenLabel(const bool state) {
  _viewOutScreenLabel = state;
}
bool GlGraphRenderingParameters::isViewOutScreenLabel() const {
  return _viewOutScreenLabel;
}
//====================================================
void GlGraphRenderingParameters::setSelectedNodesStencil(const int stencil) {
  _selectedNodesStencil = stencil;
}
int GlGraphRenderingParameters::getSelectedNodesStencil() const {
  return _selectedNodesStencil;
}
//====================================================
void GlGraphRenderingParameters::setSelectedMetaNodesStencil(const int stencil) {
  _selectedMetaNodesStencil = stencil;
}
int GlGraphRenderingParameters::getSelectedMetaNodesStencil() const {
  return _selectedMetaNodesStencil;
}
//====================================================
void GlGraphRenderingParameters::setSelectedEdgesStencil(const int stencil) {
  _selectedEdgesStencil = stencil;
}
int GlGraphRenderingParameters::getSelectedEdgesStencil() const {
  return _selectedEdgesStencil;
}
//====================================================
void GlGraphRenderingParameters::setNodesStencil(const int stencil) {
  _nodesStencil = stencil;
}
int GlGraphRenderingParameters::getNodesStencil() const {
  return _nodesStencil;
}
//====================================================
void GlGraphRenderingParameters::setMetaNodesStencil(const int stencil) {
  _metaNodesStencil = stencil;
}
int GlGraphRenderingParameters::getMetaNodesStencil() const {
  return _metaNodesStencil;
}
//====================================================
void GlGraphRenderingParameters::setEdgesStencil(const int stencil) {
  _edgesStencil = stencil;
}
int GlGraphRenderingParameters::getEdgesStencil() const {
  return _edgesStencil;
}
//====================================================
void GlGraphRenderingParameters::setNodesLabelStencil(const int stencil) {
  _nodesLabelStencil = stencil;
}
int GlGraphRenderingParameters::getNodesLabelStencil() const {
  return _nodesLabelStencil;
}
//====================================================
void GlGraphRenderingParameters::setMetaNodesLabelStencil(const int stencil) {
  _metaNodesLabelStencil = stencil;
}
int GlGraphRenderingParameters::getMetaNodesLabelStencil() const {
  return _metaNodesLabelStencil;
}
//====================================================
void GlGraphRenderingParameters::setEdgesLabelStencil(const int stencil) {
  _edgesLabelStencil = stencil;
}
int GlGraphRenderingParameters::getEdgesLabelStencil() const {
  return _edgesLabelStencil;
}
//====================================================
bool GlGraphRenderingParameters::isEdgeColorInterpolate() const {
  return (_edgeColorInterpolate);
}
void GlGraphRenderingParameters::setEdgeColorInterpolate(const bool b) {
  _edgeColorInterpolate = b;
}
//====================================================
bool GlGraphRenderingParameters::isEdgeSizeInterpolate() const {
  return (_edgeSizeInterpolate);
}
void GlGraphRenderingParameters::setEdgeSizeInterpolate(const bool b) {
  _edgeSizeInterpolate = b;
}
//====================================================
bool GlGraphRenderingParameters::isEdgeFrontDisplay() const {
  return (_edgeFrontDisplay);
}
void GlGraphRenderingParameters::setEdgeFrontDisplay(const bool b) {
  _edgeFrontDisplay = b;
}
//====================================================
bool GlGraphRenderingParameters::getEdgesMaxSizeToNodesSize() const {
  return (_edgesMaxSizeToNodesSize);
}
void GlGraphRenderingParameters::setEdgesMaxSizeToNodesSize(const bool b) {
  _edgesMaxSizeToNodesSize = b;
}
//====================================================
void GlGraphRenderingParameters::setSelectionColor(const Color &color) {
  _selectionColor = color;
}
Color GlGraphRenderingParameters::getSelectionColor() const {
  return _selectionColor;
}
//====================================================
bool GlGraphRenderingParameters::isLabelScaled() const {
  return _labelScaled;
}
void GlGraphRenderingParameters::setLabelScaled(bool state) {
  _labelScaled = state;
}
//====================================================
bool GlGraphRenderingParameters::isLabelFixedFontSize() const {
  return _labelFixedFontSize;
}
void GlGraphRenderingParameters::setLabelFixedFontSize(bool state) {
  _labelFixedFontSize = state;
}
//====================================================
int GlGraphRenderingParameters::getLabelsDensity() const {
  return _labelsDensity;
}
void GlGraphRenderingParameters::setLabelsDensity(int density) {
  _labelsDensity = density;
}
//====================================================
int GlGraphRenderingParameters::getMinSizeOfLabel() const {
  return _labelMinSize;
}
void GlGraphRenderingParameters::setMinSizeOfLabel(int size) {
  _labelMinSize = size;
}
//====================================================
int GlGraphRenderingParameters::getMaxSizeOfLabel() const {
  return _labelMaxSize;
}
void GlGraphRenderingParameters::setMaxSizeOfLabel(int size) {
  _labelMaxSize = size;
}
//====================================================
void GlGraphRenderingParameters::setElementOrderingProperty(tlp::NumericProperty *property) {
  _elementOrderingProperty = property;
}
tlp::NumericProperty *GlGraphRenderingParameters::getElementOrderingProperty() const {
  return _elementOrderingProperty;
}
