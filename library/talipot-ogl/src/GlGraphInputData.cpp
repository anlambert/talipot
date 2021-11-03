/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/GlGraphInputData.h>
#include <talipot/GlyphManager.h>
#include <talipot/EdgeExtremityGlyphManager.h>
#include <talipot/GlVertexArrayManager.h>
#include <talipot/GlMetaNodeRenderer.h>
#include <talipot/GlGlyphRenderer.h>

namespace tlp {
GlGraphInputData::GlGraphInputData(Graph *graph, GlGraphRenderingParameters *parameters,
                                   GlMetaNodeRenderer *renderer)
    : _graph(graph), _parameters(parameters) {

  reloadGraphProperties();

  if (_graph) {
    _graph->addListener(this);
  }

  _glyphManager = new GlyphManager(this);
  _extremityGlyphManager = new EdgeExtremityGlyphManager(this);

  if (renderer) {
    _metaNodeRenderer = renderer;
  } else {
    _metaNodeRenderer = new GlMetaNodeRenderer(this);
  }

  _glVertexArrayManager = new GlVertexArrayManager(this);
  _glGlyphRenderer = new GlGlyphRenderer(this);
}

void GlGraphInputData::setMetaNodeRenderer(GlMetaNodeRenderer *renderer,
                                           bool deleteOldMetaNodeRenderer) {
  if (deleteOldMetaNodeRenderer) {
    delete _metaNodeRenderer;
  }

  _metaNodeRenderer = renderer;
}

GlGraphInputData::~GlGraphInputData() {
  delete _glVertexArrayManager;
  delete _metaNodeRenderer;
  delete _glGlyphRenderer;
  delete _glyphManager;
  delete _extremityGlyphManager;
}

std::unordered_map<std::string, GlGraphInputData::PropertyName>
    GlGraphInputData::_propertiesNameMap;

void GlGraphInputData::reloadGraphProperties() {
  if (_propertiesNameMap.empty()) {
    _propertiesNameMap["viewColor"] = VIEW_COLOR;
    _propertiesNameMap["viewLabelColor"] = VIEW_LABELCOLOR;
    _propertiesNameMap["viewLabelBorderColor"] = VIEW_LABELBORDERCOLOR;
    _propertiesNameMap["viewLabelBorderWidth"] = VIEW_LABELBORDERWIDTH;
    _propertiesNameMap["viewSize"] = VIEW_SIZE;
    _propertiesNameMap["viewLabel"] = VIEW_LABEL;
    _propertiesNameMap["viewLabelPosition"] = VIEW_LABELPOSITION;
    _propertiesNameMap["viewShape"] = VIEW_SHAPE;
    _propertiesNameMap["viewRotation"] = VIEW_ROTATION;
    _propertiesNameMap["viewSelection"] = VIEW_SELECTED;
    _propertiesNameMap["viewFont"] = VIEW_FONT;
    _propertiesNameMap["viewFontSize"] = VIEW_FONTSIZE;
    _propertiesNameMap["viewTexture"] = VIEW_TEXTURE;
    _propertiesNameMap["viewBorderColor"] = VIEW_BORDERCOLOR;
    _propertiesNameMap["viewBorderWidth"] = VIEW_BORDERWIDTH;
    _propertiesNameMap["viewLayout"] = VIEW_LAYOUT;
    _propertiesNameMap["viewSrcAnchorShape"] = VIEW_SRCANCHORSHAPE;
    _propertiesNameMap["viewSrcAnchorSize"] = VIEW_SRCANCHORSIZE;
    _propertiesNameMap["viewTgtAnchorShape"] = VIEW_TGTANCHORSHAPE;
    _propertiesNameMap["viewTgtAnchorSize"] = VIEW_TGTANCHORSIZE;
    _propertiesNameMap["viewIcon"] = VIEW_ICON;
    _propertiesNameMap["viewLabelRotation"] = VIEW_LABELROTATION;
  }

  if (_graph) {
    _properties.clear();
    _propertiesMap[VIEW_COLOR] = _graph->getColorProperty("viewColor");
    _properties.insert(_propertiesMap[VIEW_COLOR]);
    _propertiesMap[VIEW_LABELCOLOR] = _graph->getColorProperty("viewLabelColor");
    _properties.insert(_propertiesMap[VIEW_LABELCOLOR]);
    _propertiesMap[VIEW_LABELBORDERCOLOR] = _graph->getColorProperty("viewLabelBorderColor");
    _properties.insert(_propertiesMap[VIEW_LABELBORDERCOLOR]);
    _propertiesMap[VIEW_LABELBORDERWIDTH] = _graph->getDoubleProperty("viewLabelBorderWidth");
    _properties.insert(_propertiesMap[VIEW_LABELBORDERWIDTH]);
    _propertiesMap[VIEW_SIZE] = _graph->getSizeProperty("viewSize");
    _properties.insert(_propertiesMap[VIEW_SIZE]);
    _propertiesMap[VIEW_LABEL] = _graph->getStringProperty("viewLabel");
    _properties.insert(_propertiesMap[VIEW_LABEL]);
    _propertiesMap[VIEW_LABELPOSITION] = _graph->getIntegerProperty("viewLabelPosition");
    _properties.insert(_propertiesMap[VIEW_LABELPOSITION]);
    _propertiesMap[VIEW_SHAPE] = _graph->getIntegerProperty("viewShape");
    _properties.insert(_propertiesMap[VIEW_SHAPE]);
    _propertiesMap[VIEW_ROTATION] = _graph->getDoubleProperty("viewRotation");
    _properties.insert(_propertiesMap[VIEW_ROTATION]);
    _propertiesMap[VIEW_SELECTED] = _graph->getBooleanProperty("viewSelection");
    _properties.insert(_propertiesMap[VIEW_SELECTED]);
    _propertiesMap[VIEW_FONT] = _graph->getStringProperty("viewFont");
    _properties.insert(_propertiesMap[VIEW_FONT]);
    _propertiesMap[VIEW_FONTSIZE] = _graph->getIntegerProperty("viewFontSize");
    _properties.insert(_propertiesMap[VIEW_FONTSIZE]);
    _propertiesMap[VIEW_TEXTURE] = _graph->getStringProperty("viewTexture");
    _properties.insert(_propertiesMap[VIEW_TEXTURE]);
    _propertiesMap[VIEW_BORDERCOLOR] = _graph->getColorProperty("viewBorderColor");
    _properties.insert(_propertiesMap[VIEW_BORDERCOLOR]);
    _propertiesMap[VIEW_BORDERWIDTH] = _graph->getDoubleProperty("viewBorderWidth");
    _properties.insert(_propertiesMap[VIEW_BORDERWIDTH]);
    _propertiesMap[VIEW_LAYOUT] = _graph->getLayoutProperty("viewLayout");
    _properties.insert(_propertiesMap[VIEW_LAYOUT]);
    _propertiesMap[VIEW_SRCANCHORSHAPE] = _graph->getIntegerProperty("viewSrcAnchorShape");
    _properties.insert(_propertiesMap[VIEW_SRCANCHORSHAPE]);
    _propertiesMap[VIEW_SRCANCHORSIZE] = _graph->getSizeProperty("viewSrcAnchorSize");
    _properties.insert(_propertiesMap[VIEW_SRCANCHORSIZE]);
    _propertiesMap[VIEW_TGTANCHORSHAPE] = _graph->getIntegerProperty("viewTgtAnchorShape");
    _properties.insert(_propertiesMap[VIEW_TGTANCHORSHAPE]);
    _propertiesMap[VIEW_TGTANCHORSIZE] = _graph->getSizeProperty("viewTgtAnchorSize");
    _properties.insert(_propertiesMap[VIEW_TGTANCHORSIZE]);
    _propertiesMap[VIEW_ICON] = _graph->getStringProperty("viewIcon");
    _properties.insert(_propertiesMap[VIEW_ICON]);
    _propertiesMap[VIEW_LABELROTATION] = _graph->getDoubleProperty("viewLabelRotation");
    _properties.insert(_propertiesMap[VIEW_ICON]);
  }
}

bool GlGraphInputData::setProperty(const std::string &name, PropertyInterface *property) {
  auto it = _propertiesNameMap.find(name);
  bool result = it != _propertiesNameMap.end();

  if (result) {
    setProperty(it->second, property);
  }

  return result;
}

bool GlGraphInputData::installProperties(
    const std::map<std::string, tlp::PropertyInterface *> &propsMap) {
  bool result = false;

  for (const auto &pmIt : propsMap) {
    if (setProperty(pmIt.first, pmIt.second)) {
      result = true;
    }
  }

  if (result) {
    glVertexArrayManager()->setHaveToComputeAll(true);
  }

  return result;
}

void GlGraphInputData::treatEvent(const Event &ev) {
  if (dynamic_cast<const GraphEvent *>(&ev) != nullptr) {
    const auto *graphEv = static_cast<const GraphEvent *>(&ev);

    if (graphEv->getType() == GraphEvent::TLP_ADD_LOCAL_PROPERTY ||
        graphEv->getType() == GraphEvent::TLP_AFTER_DEL_LOCAL_PROPERTY ||
        graphEv->getType() == GraphEvent::TLP_ADD_INHERITED_PROPERTY ||
        graphEv->getType() == GraphEvent::TLP_AFTER_DEL_INHERITED_PROPERTY) {
      if (_propertiesNameMap.count(graphEv->getPropertyName()) != 0) {
        PropertyInterface *oldProperty =
            _propertiesMap[_propertiesNameMap[graphEv->getPropertyName()]];
        _properties.erase(oldProperty);
        _propertiesMap[_propertiesNameMap[graphEv->getPropertyName()]] =
            _graph->getProperty(graphEv->getPropertyName());
        _properties.insert(_propertiesMap[_propertiesNameMap[graphEv->getPropertyName()]]);
      }
    }
  }
}
}
