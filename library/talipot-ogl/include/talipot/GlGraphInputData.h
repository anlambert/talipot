/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_GL_GRAPH_INPUT_DATA_H
#define TALIPOT_GL_GRAPH_INPUT_DATA_H

#include <talipot/config.h>
#include <talipot/Observable.h>
#include <talipot/LayoutProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/StringProperty.h>
#include <talipot/BooleanProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/IntegerProperty.h>
#include <talipot/ColorProperty.h>

namespace tlp {

class PropertyManager;
class Graph;
class Glyph;
class EdgeExtremityGlyph;
class GlVertexArrayManager;
class GlMetaNodeRenderer;
class GlGraphRenderingParameters;
class GlGlyphRenderer;
class GlyphManager;
class EdgeExtremityGlyphManager;

/**
 * Class use to store inputData of the graph
 */
class TLP_GL_SCOPE GlGraphInputData : public Observable {

public:
  /**
   * GlGraphInputData available properties
   */
  enum PropertyName {
    VIEW_COLOR = 0,        /**< color of nodes/edges */
    VIEW_LABELCOLOR,       /**< color of labels */
    VIEW_LABELBORDERCOLOR, /**< border color of labels */
    VIEW_LABELBORDERWIDTH, /**< border width of labels */
    VIEW_SIZE,             /**< size of nodes/edges */
    VIEW_LABELPOSITION,    /**< position of labels */
    VIEW_SHAPE,            /**< shape of nodes/edges */
    VIEW_ROTATION,         /**< rotation apply on nodes */
    VIEW_SELECTED,         /**< nodes/edges selected */
    VIEW_FONT,             /**< font name of labels */
    VIEW_FONTSIZE,         /**< font size of labels */
    VIEW_LABEL,            /**< text of labels */
    VIEW_LAYOUT,           /**< position of nodes */
    VIEW_TEXTURE,          /**< texture of nodes/edges */
    VIEW_BORDERCOLOR,      /**< border color of nodes/edges */
    VIEW_BORDERWIDTH,      /**< border width of nodes/edges */
    VIEW_SRCANCHORSHAPE,   /**< shape of source arrow edge extremity */
    VIEW_SRCANCHORSIZE,    /**< size of source arrow edge extremity */
    VIEW_TGTANCHORSHAPE,   /**< shape of target arrow edge extremity */
    VIEW_TGTANCHORSIZE,    /**< size of target arrow edge extremity */
    VIEW_ICON,             /**< icon name for the icon glyph*/
    VIEW_LABELROTATION,    /**< rotation apply on nodes labels */
    NB_PROPS               /** must be the last, give the number of enum props */
  };

  /**
   * Create the inputData with Graph : graph and GlGraphRenderingParameters : parameters
   */
  GlGraphInputData(Graph *graph, GlGraphRenderingParameters *parameters,
                   GlMetaNodeRenderer *renderer = nullptr);

  ~GlGraphInputData() override;

  /**
   * Return the graph of this inputData
   */
  Graph *graph() const {
    return _graph;
  }

  void setGraph(Graph *graph) {
    _graph = graph;
  }

  void treatEvent(const Event &ev) override;

  /**
   * Set metaNode renderer
   * If deleteOldMetaNodeRenderer==true : this function delete old meta node renderer
   */
  void setMetaNodeRenderer(GlMetaNodeRenderer *renderer, bool deleteOldMetaNodeRenderer = true);

  /**
   * Return metaNode renderer
   */
  GlMetaNodeRenderer *metaNodeRenderer() const {
    return _metaNodeRenderer;
  }

  /**
   * Return glEdgeDisplayManager
   */
  GlVertexArrayManager *glVertexArrayManager() const {
    return _glVertexArrayManager;
  }

  GlGlyphRenderer *glGlyphRenderer() const {
    return _glGlyphRenderer;
  }

  GlyphManager *glyphManager() const {
    return _glyphManager;
  }

  EdgeExtremityGlyphManager *extremityGlyphManager() const {
    return _extremityGlyphManager;
  }

  /**
   * Set glEdgeDisplayManager
   */
  void setGlVertexArrayManager(GlVertexArrayManager *manager) {
    _glVertexArrayManager = manager;
  }

  /**
   * Function to get the PropertyInterface* corresponding
   * to a given name
   */
  tlp::PropertyInterface *getProperty(const std::string &name) const {
    auto it = _propertiesNameMap.find(name);

    if (it != _propertiesNameMap.end()) {
      return _propertiesMap[it->second];
    }

    return nullptr;
  }

  /**
   * Function to get the typed PropertyInterface* for a given propertyName
   * See PropertyName enum for more details on available properties
   */
  template <typename T>
  T *getProperty(PropertyName propertyName) const {
    return static_cast<T *>(_propertiesMap[propertyName]);
  }

  /**
   * Function to set the PropertyInterface* for a given propertyName
   * See PropertyName enum for more details on available properties
   */
  void setProperty(PropertyName propertyName, PropertyInterface *property) {
    _properties.erase(_propertiesMap[propertyName]);
    _propertiesMap[propertyName] = property;
    _properties.insert(property);
  }

  /**
   * Function to set the PropertyInterface* for a given name
   */
  bool setProperty(const std::string &name, PropertyInterface *property);

  /**
   * Function to set a bunch of named PropertyInterface*
   */
  bool installProperties(const std::map<std::string, tlp::PropertyInterface *> &propsMap);

  /**
   * Return a pointer on the property used to elementColor
   */
  ColorProperty *colors() const {
    return getProperty<ColorProperty>(VIEW_COLOR);
  }
  /**
   * Set the pointer on the property used to elementColor
   */
  void setColors(ColorProperty *property) {
    setProperty(VIEW_COLOR, property);
  }
  /**
   * Return a pointer on the property used to elementLabelColor
   */
  ColorProperty *labelColors() const {
    return getProperty<ColorProperty>(VIEW_LABELCOLOR);
  }
  /**
   * Set the pointer on the property used to elementLabelColor
   */
  void setLabelColors(ColorProperty *property) {
    setProperty(VIEW_LABELCOLOR, property);
  }
  /**
   * Return a pointer on the property used to elementLabelBorderColor
   */
  ColorProperty *labelBorderColors() const {
    return getProperty<ColorProperty>(VIEW_LABELBORDERCOLOR);
  }
  /**
   * Set the pointer on the property used to elementLabelBorderColor
   */
  void setLabelBorderColors(ColorProperty *property) {
    setProperty(VIEW_LABELBORDERCOLOR, property);
  }
  /**
   * Return a pointer on the property used to elementLabelBorderWidth
   */
  DoubleProperty *labelBorderWidths() const {
    return getProperty<DoubleProperty>(VIEW_LABELBORDERWIDTH);
  }
  /**
   * Set the pointer on the property used to elementLabelBorderColor
   */
  void setLabelBorderWidths(DoubleProperty *property) {
    setProperty(VIEW_LABELBORDERWIDTH, property);
  }
  /**
   * Return a pointer on the property used to elementSize
   */
  SizeProperty *sizes() const {
    return getProperty<SizeProperty>(VIEW_SIZE);
  }
  /**
   * Set the pointer on the property used to elementSize
   */
  void setSizes(SizeProperty *property) {
    setProperty(VIEW_SIZE, property);
  }
  /**
   * Return a pointer on the property used to elementLabelPosition
   */
  IntegerProperty *labelPositions() const {
    return getProperty<IntegerProperty>(VIEW_LABELPOSITION);
  }
  /**
   * Set the pointer on the property used to elementLabelPosition
   */
  void setLabelPositions(IntegerProperty *property) {
    setProperty(VIEW_LABELPOSITION, property);
  }
  /**
   * Return a pointer on the property used to elementShape
   */
  IntegerProperty *shapes() const {
    return getProperty<IntegerProperty>(VIEW_SHAPE);
  }
  /**
   * Set the pointer on the property used to elementShape
   */
  void setShapes(IntegerProperty *property) {
    setProperty(VIEW_SHAPE, property);
  }
  /**
   * Return a pointer on the property used to elementRotation
   */
  DoubleProperty *rotations() const {
    return getProperty<DoubleProperty>(VIEW_ROTATION);
  }
  /**
   * Set the pointer on the property used to elementRotation
   */
  void setRotations(DoubleProperty *property) {
    setProperty(VIEW_ROTATION, property);
  }
  /**
   * Return a pointer on the property used to elementLabelRotation
   */
  DoubleProperty *labelRotations() const {
    return getProperty<DoubleProperty>(VIEW_LABELROTATION);
  }
  /**
   * Set the pointer on the property used to elementLabelRotation
   */
  void setLabelRotations(DoubleProperty *property) {
    setProperty(VIEW_LABELROTATION, property);
  }
  /**
   * Return a pointer on the property used to elementSelected
   */
  BooleanProperty *selection() const {
    return getProperty<BooleanProperty>(VIEW_SELECTED);
  }
  /**
   * Set the pointer on the property used to elementSelected
   */
  void setSelection(BooleanProperty *property) {
    setProperty(VIEW_SELECTED, property);
  }
  /**
   * Return a pointer on the property used to elementFont
   */
  StringProperty *fonts() const {
    return getProperty<StringProperty>(VIEW_FONT);
  }
  /**
   * Set the pointer on the property used to elementFont
   */
  void setFonts(StringProperty *property) {
    setProperty(VIEW_FONT, property);
  }
  /**
   * Return a pointer on the property used to elementFontSize
   */
  IntegerProperty *fontSizes() const {
    return getProperty<IntegerProperty>(VIEW_FONTSIZE);
  }
  /**
   * Set the pointer on the property used to elementFontSize
   */
  void setFontSizes(IntegerProperty *property) {
    setProperty(VIEW_FONTSIZE, property);
  }
  /**
   * Return a pointer on the property used to elementLabel
   */
  StringProperty *labels() const {
    return getProperty<StringProperty>(VIEW_LABEL);
  }
  /**
   * Set the pointer on the property used to elementLabel
   */
  void setLabels(StringProperty *property) {
    setProperty(VIEW_LABEL, property);
  }
  /**
   * Return a pointer on the property used to elementLayout
   */
  LayoutProperty *layout() const {
    return getProperty<LayoutProperty>(VIEW_LAYOUT);
  }
  /**
   * Set the pointer on the property used to elementLayout
   */
  void setLayout(LayoutProperty *property) {
    setProperty(VIEW_LAYOUT, property);
  }
  /**
   * Return a pointer on the property used to elementTexture
   */
  StringProperty *textures() const {
    return getProperty<StringProperty>(VIEW_TEXTURE);
  }
  /**
   * Set the pointer on the property used to elementTexture
   */
  void setTextures(StringProperty *property) {
    setProperty(VIEW_TEXTURE, property);
  }
  /**
   * Return a pointer on the property used to elementBorderColor
   */
  ColorProperty *borderColors() const {
    return getProperty<ColorProperty>(VIEW_BORDERCOLOR);
  }
  /**
   * Set the pointer on the property used to elementBorderColor
   */
  void setBorderColors(ColorProperty *property) {
    setProperty(VIEW_BORDERCOLOR, property);
  }
  /**
   * Return a pointer on the property used to elementBorderWidth
   */
  DoubleProperty *borderWidths() const {
    return getProperty<DoubleProperty>(VIEW_BORDERWIDTH);
  }
  /**
   * Set the pointer on the property used to elementBorderWidth
   */
  void setBorderWidths(DoubleProperty *property) {
    setProperty(VIEW_BORDERWIDTH, property);
  }
  /**
   * Return a pointer on the property used to elementSrcAnchorShape
   */
  IntegerProperty *srcAnchorShapes() const {
    return getProperty<IntegerProperty>(VIEW_SRCANCHORSHAPE);
  }
  /**
   * Set the pointer on the property used to elementSrcAnchorShape
   */
  void setSrcAnchorShapes(IntegerProperty *property) {
    setProperty(VIEW_SRCANCHORSHAPE, property);
  }
  /**
   * Return a pointer on the property used to elementSrcAnchorSize
   */
  SizeProperty *srcAnchorSizes() const {
    return getProperty<SizeProperty>(VIEW_SRCANCHORSIZE);
  }
  /**
   * Set the pointer on the property used to elementSrcAnchorSize
   */
  void setSrcAnchorSizes(SizeProperty *property) {
    setProperty(VIEW_SRCANCHORSIZE, property);
  }
  /**
   * Return a pointer on the property used to elementTgtAnchorShape
   */
  IntegerProperty *tgtAnchorShapes() const {
    return getProperty<IntegerProperty>(VIEW_TGTANCHORSHAPE);
  }
  /**
   * Set the pointer on the property used to elementTgtAnchorShape
   */
  void setTgtAnchorShapes(IntegerProperty *property) {
    setProperty(VIEW_TGTANCHORSHAPE, property);
  }
  /**
   * Return a pointer on the property used to elementTgtAnchorSize
   */
  SizeProperty *tgtAnchorSizes() const {
    return getProperty<SizeProperty>(VIEW_TGTANCHORSIZE);
  }
  /**
   * Set the property name for elementSourceAnchorSize
   */
  void setTgtAnchorSizes(SizeProperty *property) {
    setProperty(VIEW_TGTANCHORSIZE, property);
  }

  /**
   * Return a pointer on the property used to elementIcon
   *
   */
  StringProperty *icons() const {
    return getProperty<StringProperty>(VIEW_ICON);
  }

  /**
   * Set the pointer on the property used to elementIcon
   *
   */
  void setIcons(StringProperty *property) {
    setProperty(VIEW_ICON, property);
  }

  std::set<tlp::PropertyInterface *> properties() const {
    return _properties;
  }

  /**
   * @brief reloadGraphProperties restore the properties of the GlGraphInputData from the the graph.
   */
  void reloadGraphProperties();

  /**
   * @brief renderingParameters return a pointer on the rendering parameters.
   * @return
   */
  GlGraphRenderingParameters *renderingParameters() const {
    return _parameters;
  }

  /**
   * @brief setRenderingParameters set the pointer on the rendering parameters.
   * @param newParameters
   */
  void setRenderingParameters(GlGraphRenderingParameters *newParameters) {
    _parameters = newParameters;
  }

protected:
  Graph *_graph;

  GlGraphRenderingParameters *_parameters;

  GlyphManager *_glyphManager;
  EdgeExtremityGlyphManager *_extremityGlyphManager;

  std::set<PropertyInterface *> _properties;

  PropertyInterface *_propertiesMap[NB_PROPS];
  static flat_hash_map<std::string, PropertyName> _propertiesNameMap;

  GlMetaNodeRenderer *_metaNodeRenderer;
  GlVertexArrayManager *_glVertexArrayManager;
  GlGlyphRenderer *_glGlyphRenderer;
};
}

#endif // TALIPOT_GL_GRAPH_INPUT_DATA_H
