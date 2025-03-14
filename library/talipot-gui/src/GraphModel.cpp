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

#include "talipot/GraphModel.h"

#include <talipot/MetaTypes.h>
#include <talipot/FontIcon.h>
#include <talipot/MaterialDesignIcons.h>
#include <talipot/Settings.h>

using namespace tlp;

// Abstract Graph model
GraphModel::GraphModel(QObject *parent) : Model(parent), _graph(nullptr) {}

uint GraphModel::elementAt(int row) const {
  return _elements[row];
}

void GraphModel::setGraph(Graph *g) {
  if (_graph != nullptr) {
    _graph->removeListener(this);
    _graph->removeObserver(this);
    for (PropertyInterface *pi : _graph->getObjectProperties()) {
      pi->removeListener(this);
    }
  }

  _graph = g;
  _elements.clear();
  _properties.clear();

  if (_graph != nullptr) {
    _graph->addListener(this);
    _graph->addObserver(this);
    for (PropertyInterface *pi : _graph->getObjectProperties()) {
#ifdef NDEBUG

      if (pi->getName() == "viewMetaGraph") {
        continue;
      }

#endif
      _properties.push_back(pi);
      pi->addListener(this);
      pi->addObserver(this);
    }
  }
}

Graph *GraphModel::graph() const {
  return _graph;
}

int GraphModel::rowCount(const QModelIndex &parent) const {
  if (_graph == nullptr || parent.isValid()) {
    return 0;
  }

  return _elements.size();
}

int GraphModel::columnCount(const QModelIndex &parent) const {
  if (_graph == nullptr || parent.isValid()) {
    return 0;
  }

  return _properties.size();
}

QModelIndex GraphModel::parent(const QModelIndex & /*child*/) const {
  return QModelIndex();
}

Qt::ItemFlags GraphModel::flags(const QModelIndex &index) const {
  Qt::ItemFlags iflags = QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled;
#ifdef NDEBUG
  return iflags | Qt::ItemIsEditable;
#else

  if (static_cast<PropertyInterface *>(index.internalPointer())->getName() == "viewMetaGraph")
    return iflags;

  return iflags | Qt::ItemIsEditable;
#endif
}

QVariant GraphModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (_graph == nullptr) {
    return QVariant();
  }

  if (orientation == Qt::Vertical) {
    if (section > _elements.size() || section < 0) {
      return QVariant();
    }

    QModelIndex index = this->index(_elements[section], 0);

    if (role == Qt::DisplayRole) {
      return _elements[section];
    } else if (role == Qt::BackgroundRole) {
      bool selected = data(index, ElementSelectedRole).toBool();
      if (selected) {
        return colorToQColor(Settings::defaultSelectionColor());
      }
    } else if (role == Qt::ForegroundRole) {
      bool selected = data(index, ElementSelectedRole).toBool();
      if (selected) {
        QColor background = data(index, Qt::BackgroundRole).value<QColor>();
        if (background.lightnessF() < 0.5) {
          return QColor(Qt::white);
        } else {
          return QColor(Qt::black);
        }
      }
    }
  } else {
    if (section > _properties.size() || section < 0) {
      return QVariant();
    }

    PropertyInterface *prop = _properties[section];

    if (role == Qt::DisplayRole) {
      return QString(prop->getName().c_str());
    } else if (role == Qt::DecorationRole && !_graph->existLocalProperty(prop->getName())) {
      return FontIcon::icon(MaterialDesignIcons::TransferUp);
      ;
    } else if (role == Model::PropertyRole) {
      return QVariant::fromValue<PropertyInterface *>(prop);
    } else if (role == Qt::ToolTipRole) {
      return QString(_graph->existLocalProperty(prop->getName()) ? "local " : "inherited ")
          .append("property \"")
          .append(tlpStringToQString(prop->getName()))
          .append("\" of type ")
          .append(tlpStringToQString(prop->getTypename()))
          .append("\ndefault ")
          .append(isNode() ? "node value: " : "edge value: ")
          .append(isNode()
                      ? tlpStringToQString(
                            _graph->getProperty(prop->getName())->getNodeDefaultStringValue())
                      : tlpStringToQString(
                            _graph->getProperty(prop->getName())->getEdgeDefaultStringValue()));
    }
  }

  return Model::headerData(section, orientation, role);
}

QModelIndex GraphModel::index(int row, int column, const QModelIndex &parent) const {
  if (parent.isValid() || _graph == nullptr || column < 0 || column >= _properties.size()) {
    return QModelIndex();
  }

  PropertyInterface *prop = _properties[column];
  return createIndex(row, column, prop);
}

QVariant GraphModel::data(const QModelIndex &index, int role) const {
  if (role == Qt::DisplayRole) {
    return value(_elements[index.row()], static_cast<PropertyInterface *>(index.internalPointer()));
  } else if (role == PropertyRole) {
    return QVariant::fromValue<PropertyInterface *>(
        static_cast<PropertyInterface *>(index.internalPointer()));
  } else if (role == GraphRole) {
    return QVariant::fromValue<Graph *>(_graph);
  } else if (role == IsNodeRole) {
    return isNode();
  } else if (role == StringRole) {
    return stringValue(_elements[index.row()],
                       static_cast<PropertyInterface *>(index.internalPointer()));
  } else if (role == ElementIdRole) {
    return _elements[index.row()];
  } else if (role == ElementSelectedRole) {
    if (isNode()) {
      return static_cast<bool>((*_graph)["viewSelection"][node(_elements[index.row()])]);
    } else {
      return static_cast<bool>((*_graph)["viewSelection"][edge(_elements[index.row()])]);
    }
  } else if (role == Qt::BackgroundRole) {
    bool selected = data(index, ElementSelectedRole).toBool();
    if (selected) {
      return colorToQColor(Settings::defaultSelectionColor());
    }
  } else if (role == Qt::ForegroundRole) {
    bool selected = data(index, ElementSelectedRole).toBool();
    if (selected) {
      QColor background = data(index, Qt::BackgroundRole).value<QColor>();
      if (background.lightnessF() < 0.5) {
        return QColor(Qt::white);
      } else {
        return QColor(Qt::black);
      }
    }
  }

  return QVariant();
}

bool GraphModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (role == Qt::EditRole) {
    bool ok = setValue(_elements[index.row()],
                       static_cast<PropertyInterface *>(index.internalPointer()), value);

    if (ok) {
      emit dataChanged(index, index);
    }

    return ok;
  }

  return QAbstractItemModel::setData(index, value, role);
}

void GraphModel::treatEvent(const Event &ev) {
  const auto *graphEv = dynamic_cast<const GraphEvent *>(&ev);
  if (graphEv != nullptr) {

    if (graphEv->getType() == GraphEventType::TLP_ADD_INHERITED_PROPERTY ||
        graphEv->getType() == GraphEventType::TLP_ADD_LOCAL_PROPERTY) {
#ifdef NDEBUG

      if (graphEv->getPropertyName() == "viewMetaGraph") {
        return;
      }

#endif
      // insert in respect with alphabetic order
      std::string propName = graphEv->getPropertyName();
      int pos = 0;

      for (; pos < _properties.size(); ++pos) {
        if (propName < _properties[pos]->getName()) {
          break;
        }
      }

      beginInsertColumns(QModelIndex(), pos, pos);
      PropertyInterface *prop = _graph->getProperty(propName);
      _properties.insert(pos, prop);
      prop->addListener(this);
      prop->addObserver(this);
      endInsertColumns();
    } else if (graphEv->getType() == GraphEventType::TLP_BEFORE_DEL_INHERITED_PROPERTY ||
               graphEv->getType() == GraphEventType::TLP_BEFORE_DEL_LOCAL_PROPERTY) {
#ifdef NDEBUG

      if (graphEv->getPropertyName() == "viewMetaGraph") {
        return;
      }

#endif
      PropertyInterface *prop = _graph->getProperty(graphEv->getPropertyName());
      int col = _properties.indexOf(prop);
      beginRemoveColumns(QModelIndex(), col, col);
      _properties.remove(col);
      endRemoveColumns();
    } else if (graphEv->getType() == GraphEventType::TLP_BEFORE_RENAME_LOCAL_PROPERTY) {
      PropertyInterface *prop = graphEv->getProperty();
      // remove from old place
      int col = _properties.indexOf(prop);
      // insert according to new name
      std::string propName = graphEv->getPropertyNewName();
      int insertCol = 0;

      for (insertCol = 0; insertCol < _properties.size(); ++insertCol) {
        if ((prop != _properties[insertCol]) && (propName < _properties[insertCol]->getName())) {
          break;
        }
      }

      if (insertCol == col + 1) {
        return;
      }

      beginMoveColumns(QModelIndex(), col, col, QModelIndex(), insertCol);
      _properties.remove(col);

      if (col < insertCol) {
        --insertCol;
      }

      _properties.insert(insertCol, prop);
      endMoveColumns();
    }
  }
}

#define STANDARD_CHECKS(MACRO)                          \
  MACRO(DoubleProperty, double)                         \
  MACRO(DoubleVectorProperty, std::vector<double>)      \
  MACRO(ColorProperty, tlp::Color)                      \
  MACRO(ColorVectorProperty, std::vector<tlp::Color>)   \
  MACRO(SizeProperty, tlp::Size)                        \
  MACRO(SizeVectorProperty, std::vector<tlp::Size>)     \
  MACRO(StringVectorProperty, std::vector<std::string>) \
  MACRO(CoordVectorProperty, std::vector<tlp::Coord>)   \
  MACRO(IntegerVectorProperty, std::vector<int>)        \
  MACRO(BooleanProperty, bool)

#define STANDARD_NODE_CHECKS(MACRO) \
  STANDARD_CHECKS(MACRO)            \
  MACRO(LayoutProperty, tlp::Coord) \
  MACRO(GraphProperty, tlp::Graph *)

#define STANDARD_EDGE_CHECKS(MACRO)              \
  STANDARD_CHECKS(MACRO)                         \
  MACRO(LayoutProperty, std::vector<tlp::Coord>) \
  MACRO(GraphProperty, std::set<tlp::edge>)

#define GET_NODE_VALUE(PROP, TYPE)                                                \
  else if (dynamic_cast<PROP *>(prop) != nullptr) {                               \
    return QVariant::fromValue<TYPE>(static_cast<PROP *>(prop)->getNodeValue(n)); \
  }

QVariant GraphModel::nodeValue(uint id, PropertyInterface *prop) {
  node n(id);

  if (dynamic_cast<IntegerProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewShape") {
      return QVariant::fromValue<NodeShape::NodeShapes>(static_cast<NodeShape::NodeShapes>(
          static_cast<IntegerProperty *>(prop)->getNodeValue(n)));
    }

    if (prop->getName() == "viewLabelPosition") {
      return QVariant::fromValue<LabelPosition::LabelPositions>(
          static_cast<LabelPosition::LabelPositions>(
              static_cast<IntegerProperty *>(prop)->getNodeValue(n)));
    }

    return QVariant::fromValue<int>(static_cast<IntegerProperty *>(prop)->getNodeValue(n));
  } else if (dynamic_cast<StringProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewFont") {
      return QVariant::fromValue<Font>(
          Font::fromName(static_cast<StringProperty *>(prop)->getNodeValue(n)));
    }

    if (prop->getName() == "viewIcon") {
      return QVariant::fromValue<FontIconName>(
          FontIconName(tlpStringToQString(static_cast<StringProperty *>(prop)->getNodeValue(n))));
    }

    if (prop->getName() == "viewTexture") {
      return QVariant::fromValue<TextureFile>(
          TextureFile(tlpStringToQString(static_cast<StringProperty *>(prop)->getNodeValue(n))));
    }

    return QVariant::fromValue<QString>(
        tlpStringToQString(static_cast<StringProperty *>(prop)->getNodeValue(n)));
  } else if (dynamic_cast<BooleanVectorProperty *>(prop) != nullptr) {
    const std::vector<bool> &vb = static_cast<BooleanVectorProperty *>(prop)->getNodeValue(n);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    return QVariant::fromValue<QVector<bool>>(QVector<bool>(vb.begin(), vb.end()));
#else
    return QVariant::fromValue<QVector<bool>>(QVector<bool>::fromStdVector(vb));
#endif
  }

  STANDARD_NODE_CHECKS(GET_NODE_VALUE)
  return QVariant();
}

#define GET_NODE_DEFAULT_VALUE(PROP, TYPE)                                              \
  else if (dynamic_cast<PROP *>(prop) != nullptr) {                                     \
    return QVariant::fromValue<TYPE>(static_cast<PROP *>(prop)->getNodeDefaultValue()); \
  }

QVariant GraphModel::nodeDefaultValue(PropertyInterface *prop) {
  if (dynamic_cast<IntegerProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewShape") {
      return QVariant::fromValue<NodeShape::NodeShapes>(static_cast<NodeShape::NodeShapes>(
          static_cast<IntegerProperty *>(prop)->getNodeDefaultValue()));
    }

    if (prop->getName() == "viewLabelPosition") {
      return QVariant::fromValue<LabelPosition::LabelPositions>(
          static_cast<LabelPosition::LabelPositions>(
              static_cast<IntegerProperty *>(prop)->getNodeDefaultValue()));
    }

    return QVariant::fromValue<int>(static_cast<IntegerProperty *>(prop)->getNodeDefaultValue());
  } else if (dynamic_cast<StringProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewFont") {
      return QVariant::fromValue<Font>(
          Font::fromName(static_cast<StringProperty *>(prop)->getNodeDefaultValue()));
    }

    if (prop->getName() == "viewIcon") {
      return QVariant::fromValue<FontIconName>(FontIconName(
          tlpStringToQString(static_cast<StringProperty *>(prop)->getNodeDefaultValue())));
    }

    if (prop->getName() == "viewTexture") {
      return QVariant::fromValue<TextureFile>(TextureFile(
          tlpStringToQString(static_cast<StringProperty *>(prop)->getNodeDefaultValue())));
    }

    return QVariant::fromValue<QString>(
        tlpStringToQString(static_cast<StringProperty *>(prop)->getNodeDefaultValue()));
  } else if (dynamic_cast<BooleanVectorProperty *>(prop) != nullptr) {
    const std::vector<bool> &vb = static_cast<BooleanVectorProperty *>(prop)->getNodeDefaultValue();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    return QVariant::fromValue<QVector<bool>>(QVector<bool>(vb.begin(), vb.end()));
#else
    return QVariant::fromValue<QVector<bool>>(QVector<bool>::fromStdVector(vb));
#endif
  }

  STANDARD_NODE_CHECKS(GET_NODE_DEFAULT_VALUE)
  return QVariant();
}

#define SET_ALL_NODE_VALUE(PROP, TYPE)                                  \
  else if (dynamic_cast<PROP *>(prop) != nullptr) {                     \
    static_cast<PROP *>(prop)->setAllNodeValue(v.value<TYPE>(), graph); \
  }

bool GraphModel::setAllNodeValue(PropertyInterface *prop, QVariant v, const Graph *graph) {
  if (dynamic_cast<IntegerProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewShape") {
      static_cast<IntegerProperty *>(prop)->setAllNodeValue(v.value<NodeShape::NodeShapes>(),
                                                            graph);
    } else if (prop->getName() == "viewLabelPosition") {
      static_cast<IntegerProperty *>(prop)->setAllNodeValue(
          v.value<LabelPosition::LabelPositions>(), graph);
    } else {
      static_cast<IntegerProperty *>(prop)->setAllNodeValue(v.value<int>(), graph);
    }
  } else if (dynamic_cast<StringProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewFont") {
      static_cast<StringProperty *>(prop)->setAllNodeValue(v.value<Font>().fontName(), graph);
    } else if (prop->getName() == "viewIcon") {
      static_cast<StringProperty *>(prop)->setAllNodeValue(
          QStringToTlpString(v.value<FontIconName>().iconName), graph);
    } else if (prop->getName() == "viewTexture") {
      static_cast<StringProperty *>(prop)->setAllNodeValue(
          QStringToTlpString(v.value<TextureFile>().texturePath), graph);
    } else {
      static_cast<StringProperty *>(prop)->setAllNodeValue(QStringToTlpString(v.value<QString>()),
                                                           graph);
    }
  } else if (dynamic_cast<BooleanVectorProperty *>(prop) != nullptr) {
    auto vb = v.value<QVector<bool>>();
    static_cast<BooleanVectorProperty *>(prop)->setAllNodeValue(
        std::vector<bool>(vb.begin(), vb.end()), graph);
  }

  STANDARD_NODE_CHECKS(SET_ALL_NODE_VALUE)
  else return false;

  return true;
}

#define SET_NODE_VALUE(PROP, TYPE)                               \
  else if (dynamic_cast<PROP *>(prop) != nullptr) {              \
    static_cast<PROP *>(prop)->setNodeValue(n, v.value<TYPE>()); \
  }

bool GraphModel::setNodeValue(uint id, PropertyInterface *prop, QVariant v) {
  node n(id);

  if (dynamic_cast<IntegerProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewShape") {
      static_cast<IntegerProperty *>(prop)->setNodeValue(n, v.value<NodeShape::NodeShapes>());
    } else if (prop->getName() == "viewLabelPosition") {
      static_cast<IntegerProperty *>(prop)->setNodeValue(n,
                                                         v.value<LabelPosition::LabelPositions>());
    } else {
      static_cast<IntegerProperty *>(prop)->setNodeValue(n, v.value<int>());
    }
  } else if (dynamic_cast<StringProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewFont") {
      static_cast<StringProperty *>(prop)->setNodeValue(n, v.value<Font>().fontName());
    } else if (prop->getName() == "viewIcon") {
      static_cast<StringProperty *>(prop)->setNodeValue(
          n, QStringToTlpString(v.value<FontIconName>().iconName));
    } else if (prop->getName() == "viewTexture") {
      static_cast<StringProperty *>(prop)->setNodeValue(
          n, QStringToTlpString(v.value<TextureFile>().texturePath));
    } else {
      static_cast<StringProperty *>(prop)->setNodeValue(n, QStringToTlpString(v.value<QString>()));
    }
  } else if (dynamic_cast<BooleanVectorProperty *>(prop) != nullptr) {
    auto vb = v.value<QVector<bool>>();
    static_cast<BooleanVectorProperty *>(prop)->setNodeValue(
        n, std::vector<bool>(vb.begin(), vb.end()));
  }

  STANDARD_NODE_CHECKS(SET_NODE_VALUE)
  else return false;

  return true;
}

#define SET_NODE_DEFAULT_VALUE(PROP, TYPE)                           \
  else if (dynamic_cast<PROP *>(prop) != nullptr) {                  \
    static_cast<PROP *>(prop)->setNodeDefaultValue(v.value<TYPE>()); \
  }

bool GraphModel::setNodeDefaultValue(PropertyInterface *prop, QVariant v) {

  if (dynamic_cast<IntegerProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewShape") {
      static_cast<IntegerProperty *>(prop)->setNodeDefaultValue(v.value<NodeShape::NodeShapes>());
    } else if (prop->getName() == "viewLabelPosition") {
      static_cast<IntegerProperty *>(prop)->setNodeDefaultValue(
          v.value<LabelPosition::LabelPositions>());
    } else {
      static_cast<IntegerProperty *>(prop)->setNodeDefaultValue(v.value<int>());
    }
  } else if (dynamic_cast<StringProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewFont") {
      static_cast<StringProperty *>(prop)->setNodeDefaultValue(v.value<Font>().fontName());
    } else if (prop->getName() == "viewIcon") {
      static_cast<StringProperty *>(prop)->setNodeDefaultValue(
          QStringToTlpString(v.value<FontIconName>().iconName));
    } else if (prop->getName() == "viewTexture") {
      static_cast<StringProperty *>(prop)->setNodeDefaultValue(
          QStringToTlpString(v.value<TextureFile>().texturePath));
    } else {
      static_cast<StringProperty *>(prop)->setNodeDefaultValue(
          QStringToTlpString(v.value<QString>()));
    }
  } else if (dynamic_cast<BooleanVectorProperty *>(prop) != nullptr) {
    auto vb = v.value<QVector<bool>>();
    static_cast<BooleanVectorProperty *>(prop)->setNodeDefaultValue(
        std::vector<bool>(vb.begin(), vb.end()));
  }

  STANDARD_NODE_CHECKS(SET_NODE_DEFAULT_VALUE)
  else return false;

  return true;
}

#define GET_EDGE_VALUE(PROP, TYPE)                                                \
  else if (dynamic_cast<PROP *>(prop) != nullptr) {                               \
    return QVariant::fromValue<TYPE>(static_cast<PROP *>(prop)->getEdgeValue(e)); \
  }

QVariant GraphModel::edgeValue(uint id, PropertyInterface *prop) {
  edge e(id);

  if (dynamic_cast<IntegerProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewShape") {
      return QVariant::fromValue<EdgeShape::EdgeShapes>(static_cast<EdgeShape::EdgeShapes>(
          static_cast<IntegerProperty *>(prop)->getEdgeValue(e)));
    }

    if (prop->getName() == "viewTgtAnchorShape") {
      return QVariant::fromValue<EdgeExtremityShape::EdgeExtremityShapes>(
          static_cast<EdgeExtremityShape::EdgeExtremityShapes>(
              static_cast<IntegerProperty *>(prop)->getEdgeValue(e)));
    }

    if (prop->getName() == "viewSrcAnchorShape") {
      return QVariant::fromValue<EdgeExtremityShape::EdgeExtremityShapes>(
          static_cast<EdgeExtremityShape::EdgeExtremityShapes>(
              static_cast<IntegerProperty *>(prop)->getEdgeValue(e)));
    }

    if (prop->getName() == "viewLabelPosition") {
      return QVariant::fromValue<LabelPosition::LabelPositions>(
          static_cast<LabelPosition::LabelPositions>(
              static_cast<IntegerProperty *>(prop)->getEdgeValue(e)));
    }

    return QVariant::fromValue<int>(static_cast<IntegerProperty *>(prop)->getEdgeValue(e));
  } else if (dynamic_cast<StringProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewFont") {
      return QVariant::fromValue<Font>(
          Font::fromName(static_cast<StringProperty *>(prop)->getEdgeValue(e)));
    }

    if (prop->getName() == "viewIcon") {
      return QVariant::fromValue<FontIconName>(
          FontIconName(tlpStringToQString(static_cast<StringProperty *>(prop)->getEdgeValue(e))));
    }

    if (prop->getName() == "viewTexture") {
      return QVariant::fromValue<TextureFile>(
          TextureFile(tlpStringToQString(static_cast<StringProperty *>(prop)->getEdgeValue(e))));
    }

    return QVariant::fromValue<QString>(
        tlpStringToQString(static_cast<StringProperty *>(prop)->getEdgeValue(e)));
  } else if (dynamic_cast<BooleanVectorProperty *>(prop) != nullptr) {
    const std::vector<bool> &vb = static_cast<BooleanVectorProperty *>(prop)->getEdgeValue(e);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    return QVariant::fromValue<QVector<bool>>(QVector<bool>(vb.begin(), vb.end()));
#else
    return QVariant::fromValue<QVector<bool>>(QVector<bool>::fromStdVector(vb));
#endif
  }

  STANDARD_EDGE_CHECKS(GET_EDGE_VALUE)
  return QVariant();
}

#define GET_EDGE_DEFAULT_VALUE(PROP, TYPE)                                              \
  else if (dynamic_cast<PROP *>(prop) != nullptr) {                                     \
    return QVariant::fromValue<TYPE>(static_cast<PROP *>(prop)->getEdgeDefaultValue()); \
  }

QVariant GraphModel::edgeDefaultValue(PropertyInterface *prop) {
  if (dynamic_cast<IntegerProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewShape") {
      return QVariant::fromValue<EdgeShape::EdgeShapes>(static_cast<EdgeShape::EdgeShapes>(
          static_cast<IntegerProperty *>(prop)->getEdgeDefaultValue()));
    }

    if (prop->getName() == "viewTgtAnchorShape") {
      return QVariant::fromValue<EdgeExtremityShape::EdgeExtremityShapes>(
          static_cast<EdgeExtremityShape::EdgeExtremityShapes>(
              static_cast<IntegerProperty *>(prop)->getEdgeDefaultValue()));
    }

    if (prop->getName() == "viewSrcAnchorShape") {
      return QVariant::fromValue<EdgeExtremityShape::EdgeExtremityShapes>(
          static_cast<EdgeExtremityShape::EdgeExtremityShapes>(
              static_cast<IntegerProperty *>(prop)->getEdgeDefaultValue()));
    }

    if (prop->getName() == "viewLabelPosition") {
      return QVariant::fromValue<LabelPosition::LabelPositions>(
          static_cast<LabelPosition::LabelPositions>(
              static_cast<IntegerProperty *>(prop)->getEdgeDefaultValue()));
    }

    return QVariant::fromValue<int>(static_cast<IntegerProperty *>(prop)->getEdgeDefaultValue());
  } else if (dynamic_cast<StringProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewFont") {
      return QVariant::fromValue<Font>(
          Font::fromName(static_cast<StringProperty *>(prop)->getEdgeDefaultValue()));
    }

    if (prop->getName() == "viewIcon") {
      return QVariant::fromValue<FontIconName>(FontIconName(
          tlpStringToQString(static_cast<StringProperty *>(prop)->getEdgeDefaultValue())));
    }

    if (prop->getName() == "viewTexture") {
      return QVariant::fromValue<TextureFile>(TextureFile(
          tlpStringToQString(static_cast<StringProperty *>(prop)->getEdgeDefaultValue())));
    }

    return QVariant::fromValue<QString>(
        tlpStringToQString(static_cast<StringProperty *>(prop)->getEdgeDefaultValue()));
  } else if (dynamic_cast<BooleanVectorProperty *>(prop) != nullptr) {
    const std::vector<bool> &vb = static_cast<BooleanVectorProperty *>(prop)->getEdgeDefaultValue();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    return QVariant::fromValue<QVector<bool>>(QVector<bool>(vb.begin(), vb.end()));
#else
    return QVariant::fromValue<QVector<bool>>(QVector<bool>::fromStdVector(vb));
#endif
  }

  STANDARD_EDGE_CHECKS(GET_EDGE_DEFAULT_VALUE)
  return QVariant();
}

#define SET_EDGE_VALUE(PROP, TYPE)                               \
  else if (dynamic_cast<PROP *>(prop) != nullptr) {              \
    static_cast<PROP *>(prop)->setEdgeValue(e, v.value<TYPE>()); \
  }

bool GraphModel::setEdgeValue(uint id, PropertyInterface *prop, QVariant v) {
  edge e(id);

  if (dynamic_cast<IntegerProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewShape") {
      static_cast<IntegerProperty *>(prop)->setEdgeValue(e, v.value<EdgeShape::EdgeShapes>());

    } else if (prop->getName() == "viewTgtAnchorShape") {
      static_cast<IntegerProperty *>(prop)->setEdgeValue(
          e, v.value<EdgeExtremityShape::EdgeExtremityShapes>());

    } else if (prop->getName() == "viewSrcAnchorShape") {
      static_cast<IntegerProperty *>(prop)->setEdgeValue(
          e, v.value<EdgeExtremityShape::EdgeExtremityShapes>());

    } else if (prop->getName() == "viewLabelPosition") {
      static_cast<IntegerProperty *>(prop)->setEdgeValue(e,
                                                         v.value<LabelPosition::LabelPositions>());

    } else {
      static_cast<IntegerProperty *>(prop)->setEdgeValue(e, v.value<int>());
    }
  } else if (dynamic_cast<StringProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewFont") {
      static_cast<StringProperty *>(prop)->setEdgeValue(e, v.value<Font>().fontName());
    } else if (prop->getName() == "viewIcon") {
      static_cast<StringProperty *>(prop)->setEdgeValue(
          e, QStringToTlpString(v.value<FontIconName>().iconName));
    } else if (prop->getName() == "viewTexture") {
      static_cast<StringProperty *>(prop)->setEdgeValue(
          e, QStringToTlpString(v.value<TextureFile>().texturePath));
    } else {
      static_cast<StringProperty *>(prop)->setEdgeValue(e, QStringToTlpString(v.value<QString>()));
    }
  } else if (dynamic_cast<BooleanVectorProperty *>(prop) != nullptr) {
    auto vb = v.value<QVector<bool>>();
    static_cast<BooleanVectorProperty *>(prop)->setEdgeValue(
        e, std::vector<bool>(vb.begin(), vb.end()));
  }

  STANDARD_EDGE_CHECKS(SET_EDGE_VALUE)
  else return false;

  return true;
}

#define SET_EDGE_DEFAULT_VALUE(PROP, TYPE)                           \
  else if (dynamic_cast<PROP *>(prop) != nullptr) {                  \
    static_cast<PROP *>(prop)->setEdgeDefaultValue(v.value<TYPE>()); \
  }

bool GraphModel::setEdgeDefaultValue(PropertyInterface *prop, QVariant v) {

  if (dynamic_cast<IntegerProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewShape") {
      static_cast<IntegerProperty *>(prop)->setEdgeDefaultValue(v.value<EdgeShape::EdgeShapes>());

    } else if (prop->getName() == "viewTgtAnchorShape") {
      static_cast<IntegerProperty *>(prop)->setEdgeDefaultValue(
          v.value<EdgeExtremityShape::EdgeExtremityShapes>());

    } else if (prop->getName() == "viewSrcAnchorShape") {
      static_cast<IntegerProperty *>(prop)->setEdgeDefaultValue(
          v.value<EdgeExtremityShape::EdgeExtremityShapes>());

    } else if (prop->getName() == "viewLabelPosition") {
      static_cast<IntegerProperty *>(prop)->setEdgeDefaultValue(
          v.value<LabelPosition::LabelPositions>());

    } else {
      static_cast<IntegerProperty *>(prop)->setEdgeDefaultValue(v.value<int>());
    }
  } else if (dynamic_cast<StringProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewFont") {
      static_cast<StringProperty *>(prop)->setEdgeDefaultValue(v.value<Font>().fontName());
    } else if (prop->getName() == "viewIcon") {
      static_cast<StringProperty *>(prop)->setEdgeDefaultValue(
          QStringToTlpString(v.value<FontIconName>().iconName));
    } else if (prop->getName() == "viewTexture") {
      static_cast<StringProperty *>(prop)->setEdgeDefaultValue(
          QStringToTlpString(v.value<TextureFile>().texturePath));
    } else {
      static_cast<StringProperty *>(prop)->setEdgeDefaultValue(
          QStringToTlpString(v.value<QString>()));
    }
  } else if (dynamic_cast<BooleanVectorProperty *>(prop) != nullptr) {
    auto vb = v.value<QVector<bool>>();
    static_cast<BooleanVectorProperty *>(prop)->setEdgeDefaultValue(
        std::vector<bool>(vb.begin(), vb.end()));
  }

  STANDARD_EDGE_CHECKS(SET_EDGE_DEFAULT_VALUE)
  else return false;

  return true;
}

#define SET_ALL_EDGE_VALUE(PROP, TYPE)                                  \
  else if (dynamic_cast<PROP *>(prop) != nullptr) {                     \
    static_cast<PROP *>(prop)->setAllEdgeValue(v.value<TYPE>(), graph); \
  }

bool GraphModel::setAllEdgeValue(PropertyInterface *prop, QVariant v, const Graph *graph) {
  if (dynamic_cast<IntegerProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewShape") {
      static_cast<IntegerProperty *>(prop)->setAllEdgeValue(v.value<EdgeShape::EdgeShapes>(),
                                                            graph);

    } else if (prop->getName() == "viewSrcAnchorShape" || prop->getName() == "viewTgtAnchorShape") {
      static_cast<IntegerProperty *>(prop)->setAllEdgeValue(
          v.value<EdgeExtremityShape::EdgeExtremityShapes>(), graph);

    } else if (prop->getName() == "viewLabelPosition") {
      static_cast<IntegerProperty *>(prop)->setAllEdgeValue(
          v.value<LabelPosition::LabelPositions>(), graph);

    } else {
      static_cast<IntegerProperty *>(prop)->setAllEdgeValue(v.value<int>(), graph);
    }
  } else if (dynamic_cast<StringProperty *>(prop) != nullptr) {
    if (prop->getName() == "viewFont") {
      static_cast<StringProperty *>(prop)->setAllEdgeValue(v.value<Font>().fontName(), graph);

    } else if (prop->getName() == "viewIcon") {
      static_cast<StringProperty *>(prop)->setAllEdgeValue(
          QStringToTlpString(v.value<FontIconName>().iconName), graph);

    } else if (prop->getName() == "viewTexture") {
      static_cast<StringProperty *>(prop)->setAllEdgeValue(
          QStringToTlpString(v.value<TextureFile>().texturePath), graph);

    } else {
      static_cast<StringProperty *>(prop)->setAllEdgeValue(QStringToTlpString(v.value<QString>()),
                                                           graph);
    }
  } else if (dynamic_cast<BooleanVectorProperty *>(prop) != nullptr) {
    auto vb = v.value<QVector<bool>>();
    static_cast<BooleanVectorProperty *>(prop)->setAllEdgeValue(
        std::vector<bool>(vb.begin(), vb.end()), graph);
  }

  STANDARD_EDGE_CHECKS(SET_ALL_EDGE_VALUE)
  else return false;

  return true;
}

// Nodes model
NodesGraphModel::NodesGraphModel(QObject *parent)
    : GraphModel(parent), _nodesAdded(false), _nodesRemoved(false) {}

bool NodesGraphModel::lessThan(uint a, uint b, PropertyInterface *prop) const {
  return prop->compare(node(a), node(b)) <= -1;
}

void NodesGraphModel::setGraph(Graph *g) {
  GraphModel::setGraph(g);

  if (graph() == nullptr) {
    return;
  }

  _elements.resize(graph()->numberOfNodes());
  int i = 0;
  for (auto n : graph()->nodes()) {
    _elements[i++] = n.id;
  }
  // we ensure the ids are ascendingly sorted
  // to ease the display of nodes/edges
  std::sort(_elements.begin(), _elements.end());
  // reset();
}

QString NodesGraphModel::stringValue(uint id, PropertyInterface *pi) const {
  return tlpStringToQString(pi->getNodeStringValue(node(id)));
}

QVariant NodesGraphModel::value(uint id, PropertyInterface *prop) const {
  return nodeValue(id, prop);
}

bool NodesGraphModel::setValue(uint id, PropertyInterface *prop, QVariant v) const {
  prop->getGraph()->push();

  if (setNodeValue(id, prop, v)) {
    prop->getGraph()->popIfNoUpdates();
    return true;
  }

  prop->getGraph()->pop();
  return false;
}

// Edges model
EdgesGraphModel::EdgesGraphModel(QObject *parent)
    : GraphModel(parent), _edgesAdded(false), _edgesRemoved(false) {}
QString EdgesGraphModel::stringValue(uint id, PropertyInterface *pi) const {
  return tlpStringToQString(pi->getEdgeStringValue(edge(id)));
}

void EdgesGraphModel::setGraph(Graph *g) {
  GraphModel::setGraph(g);

  if (graph() == nullptr) {
    return;
  }

  _elements.resize(graph()->numberOfEdges());
  int i = 0;
  for (auto e : graph()->edges()) {
    _elements[i++] = e.id;
  }
  // reset();
}

QVariant EdgesGraphModel::value(uint id, PropertyInterface *prop) const {
  return edgeValue(id, prop);
}
bool EdgesGraphModel::setValue(uint id, PropertyInterface *prop, QVariant v) const {
  prop->getGraph()->push();

  if (setEdgeValue(id, prop, v)) {
    prop->getGraph()->popIfNoUpdates();
    return true;
  }

  prop->getGraph()->pop();
  return false;
}

bool EdgesGraphModel::lessThan(uint a, uint b, PropertyInterface *prop) const {
  return prop->compare(edge(a), edge(b)) <= -1;
}

// Filter proxy
GraphSortFilterProxyModel::GraphSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent), _properties(QVector<PropertyInterface *>()),
      _filterProperty(nullptr) {}

bool GraphSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
  auto *graphModel = static_cast<GraphModel *>(sourceModel());
  return graphModel->lessThan(graphModel->elementAt(left.row()), graphModel->elementAt(right.row()),
                              static_cast<PropertyInterface *>(left.internalPointer()));
}
void GraphSortFilterProxyModel::setProperties(QVector<PropertyInterface *> properties) {
  _properties = properties;
}

bool GraphSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &) const {
  auto *graphModel = static_cast<GraphModel *>(sourceModel());

  if (graphModel->graph() == nullptr) {
    return true;
  }

  uint id = graphModel->elementAt(sourceRow);

  bool selected = true;

  if (_filterProperty != nullptr) {
    if (graphModel->isNode()) {
      selected = _filterProperty->getNodeValue(node(id));
    } else {
      selected = _filterProperty->getEdgeValue(edge(id));
    }
  }

  if (!selected) {
    return false;
  }

  if (filterRegularExpression().pattern().isEmpty()) {
    return true;
  }

  for (auto *pi : _properties) {
    if (graphModel->stringValue(id, pi).contains(filterRegularExpression())) {
      return true;
    }
  }

  return false;
}

void GraphSortFilterProxyModel::setFilterProperty(BooleanProperty *prop) {
  if (_filterProperty != nullptr) {
    _filterProperty->removeListener(this);
  }

  _filterProperty = prop;

  if (_filterProperty != nullptr) {
    _filterProperty->addListener(this);
  }

  invalidateFilter();
}

void GraphSortFilterProxyModel::treatEvent(const Event &e) {
  if (e.sender() == _filterProperty) {
    invalidateFilter();
  }
}

BooleanProperty *GraphSortFilterProxyModel::filterProperty() const {
  return _filterProperty;
}

void GraphModel::addRemoveRowsSequence(QVector<uint> &rowsSequence, bool add) {
  if (add) {
    beginInsertRows(QModelIndex(), _elements.size(), _elements.size() + rowsSequence.size() - 1);

    std::sort(rowsSequence.begin(), rowsSequence.end());
    for (auto id : rowsSequence) {
      _elements.push_back(id);
    }

    endInsertRows();
  } else {
    beginRemoveRows(QModelIndex(), rowsSequence.front(),
                    rowsSequence.front() + rowsSequence.size() - 1);
    _elements.remove(rowsSequence.front(), rowsSequence.size());
    endRemoveRows();
  }
}

/*
 *Event handling
 */
void GraphModel::treatEvents(const std::vector<tlp::Event> &) {
  // vector to hold a sequence of graph elements ids to add to / remove from the model
  QVector<uint> rowsSequence;
  bool lastAdded = false;
  typedef QPair<uint, bool> PUB;

  for (const PUB &e : _elementsToModify) {
    bool add = e.second;
    uint id = e.first;

    // current operation changed, flush the rows to add/remove if any
    if (lastAdded != add && !rowsSequence.isEmpty()) {
      addRemoveRowsSequence(rowsSequence, lastAdded);
      rowsSequence.clear();
    }

    if (add) {

      // id of element to add is greater than the last one currently stored in the model,
      // meaning its index in the model will be contiguous with the one of the last added element.
      // So add it to the current rows sequence that will be further added in the model
      if (_elements.empty() || id > uint(_elements.back())) {
        rowsSequence.push_back(id);
      }
      // case where an element previously deleted, whose id is lower than the last one stored in the
      // model, is added again in the graph
      else {

        // if the current rows sequence is not empty, flush it to add the rows in the model
        if (!rowsSequence.isEmpty()) {
          addRemoveRowsSequence(rowsSequence, add);
          rowsSequence.clear();
        }

        // insert according to id
        // to ensure that deleted elements are re-inserted at the
        // same place on undo (graph->pop())
        uint idx = _elements.size();

        while (idx && _elements[idx - 1] > id) {
          --idx;
        }

        beginInsertRows(QModelIndex(), idx, idx);
        _elements.insert(idx, id);
        endInsertRows();
      }
    } else {
      // get model index of the element to remove
      // as _elements vector is always sorted in ascending order,
      // for performance improvement with large graphs,
      // we perform a binary search instead of using QVector::indexOf method
      uint index = std::lower_bound(_elements.begin(), _elements.end(), id) - _elements.begin();

      // if the index to remove is not contiguous with the last one stored in the current sequence
      // of indices to remove, flush that sequence to remove the elements from the model
      if (!rowsSequence.isEmpty() && index != rowsSequence.back() + 1) {
        addRemoveRowsSequence(rowsSequence, add);
        rowsSequence.clear();
        // get updated index of the element to remove
        index = std::lower_bound(_elements.begin(), _elements.end(), id) - _elements.begin();
      }

      // add the index to remove to the sequence
      rowsSequence.push_back(index);
    }

    // backup last operation (add or remove)
    lastAdded = add;
  }

  // if the rows sequence is not empty, flush it to perform elements add/removal in the model
  if (!rowsSequence.isEmpty()) {
    addRemoveRowsSequence(rowsSequence, lastAdded);
  }

  _elementsToModify.clear();

  for (PropertyInterface *prop : _propertiesModified) {
    int col = _properties.indexOf(prop);

    if (col != -1) {
      QModelIndex firstIndex = index(0, col);
      QModelIndex lastIndex = index(_elements.size() - 1, col);
      emit dataChanged(firstIndex, lastIndex);
    }
  }
  _propertiesModified.clear();
}

void NodesGraphModel::treatEvent(const Event &ev) {
  GraphModel::treatEvent(ev);

  const auto *graphEv = dynamic_cast<const GraphEvent *>(&ev);
  const auto *propEv = dynamic_cast<const PropertyEvent *>(&ev);

  if (graphEv) {

    if (graphEv->getType() == GraphEventType::TLP_ADD_NODE) {
      _nodesAdded = true;
      // if the node was removed then added again before the call to Observable::unholdObservers(),
      // remove it from the elementsToModify list as no update has to be performed in the model for
      // that element.
      int wasDeleted =
          _nodesRemoved ? _elementsToModify.indexOf(qMakePair(graphEv->getNode().id, false)) : -1;

      if (wasDeleted == -1) {
        _elementsToModify.push_back(QPair<uint, bool>(graphEv->getNode().id, true));
      } else {
        _elementsToModify.remove(wasDeleted);
      }
    } else if (graphEv->getType() == GraphEventType::TLP_ADD_NODES) {
      _nodesAdded = true;

      for (auto n : graphEv->getNodes()) {
        // if the node was removed then added again before the call to
        // Observable::unholdObservers(), remove it from the elementsToModify list as no update has
        // to be performed in the model for that element
        int wasDeleted = _nodesRemoved ? _elementsToModify.indexOf(qMakePair(n.id, false)) : -1;

        if (wasDeleted == -1) {
          _elementsToModify.push_back(QPair<uint, bool>(n.id, true));
        } else {
          _elementsToModify.remove(wasDeleted);
        }
      }
    } else if (graphEv->getType() == GraphEventType::TLP_DEL_NODE) {
      _nodesRemoved = true;
      // if the node was added then deleted before the call to Observable::unholdObservers(), remove
      // it from the elementsToModify list as no update has to be performed in the model for that
      // element
      int wasAdded =
          _nodesAdded ? _elementsToModify.indexOf(qMakePair(graphEv->getNode().id, true)) : -1;

      if (wasAdded == -1) {
        auto id = graphEv->getNode().id;
        _elementsToModify.push_back(QPair<uint, bool>(id, false));
      } else {
        _elementsToModify.remove(wasAdded);
      }
    }
  } else if (propEv) {
    if (propEv->getType() == PropertyEventType::TLP_AFTER_SET_NODE_VALUE ||
        propEv->getType() == PropertyEventType::TLP_AFTER_SET_ALL_NODE_VALUE) {
      _propertiesModified.insert(propEv->getProperty());
    }
  }
}

void NodesGraphModel::treatEvents(const std::vector<tlp::Event> &events) {
  GraphModel::treatEvents(events);
  _nodesAdded = false;
  _nodesRemoved = false;
}

QVariant NodesGraphModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Vertical) {
    if (role == Qt::ToolTipRole && (section >= 0 && section < _elements.size())) {
      return getNodeTooltip(_graph, node(_elements[section]));
    }
  }

  return GraphModel::headerData(section, orientation, role);
}

QString NodesGraphModel::getNodeTooltip(Graph *graph, node n) {
  const std::string &label = graph->getProperty("viewLabel")->getNodeStringValue(n);
  return QString("<b>Node #")
      .append(QString::number(n.id))
      .append(label.empty() ? "</b>" : "</b> (<b><i>" + tlpStringToQString(label) + "</i></b>)")
      .append("\ninput degree: ")
      .append(QString::number(graph->indeg(n)))
      .append("\noutput degree: ")
      .append(QString::number(graph->outdeg(n)));
}

void EdgesGraphModel::treatEvent(const Event &ev) {
  GraphModel::treatEvent(ev);

  const auto *graphEv = dynamic_cast<const GraphEvent *>(&ev);
  const auto *propEv = dynamic_cast<const PropertyEvent *>(&ev);

  if (dynamic_cast<const GraphEvent *>(&ev) != nullptr) {

    if (graphEv->getType() == GraphEventType::TLP_ADD_EDGE) {
      _edgesAdded = true;
      // if the edge was removed then added again before the call to Observable::unholdObservers(),
      // remove it from the elementsToModify list as no update has to be performed in the model for
      // that element
      int wasDeleted =
          _edgesRemoved ? _elementsToModify.indexOf(qMakePair(graphEv->getEdge().id, false)) : -1;

      if (wasDeleted == -1) {
        _elementsToModify.push_back(QPair<uint, bool>(graphEv->getEdge().id, true));
      } else {
        _elementsToModify.remove(wasDeleted);
      }
    } else if (graphEv->getType() == GraphEventType::TLP_ADD_EDGES) {
      _edgesAdded = true;

      for (auto e : graphEv->getEdges()) {
        // if the edge was removed then added again before the call to
        // Observable::unholdObservers(), remove it from the elementsToModify list as no update has
        // to be performed in the model for that element
        int wasDeleted = _edgesRemoved ? _elementsToModify.indexOf(qMakePair(e.id, false)) : -1;

        if (wasDeleted == -1) {
          _elementsToModify.push_back(QPair<uint, bool>(e.id, true));
        } else {
          _elementsToModify.remove(wasDeleted);
        }
      }
    } else if (graphEv->getType() == GraphEventType::TLP_DEL_EDGE) {
      _edgesRemoved = true;
      // if the edge was added then deleted before the call to Observable::unholdObservers(), remove
      // it from the elementsToModify list as no update has to be performed in the model for that
      // element
      int wasAdded =
          _edgesAdded ? _elementsToModify.indexOf(qMakePair(graphEv->getEdge().id, true)) : -1;

      if (wasAdded == -1) {
        _elementsToModify.push_back(QPair<uint, bool>(graphEv->getEdge().id, false));
      } else {
        _elementsToModify.remove(wasAdded);
      }
    }
  } else if (propEv) {
    if (propEv->getType() == PropertyEventType::TLP_AFTER_SET_EDGE_VALUE ||
        propEv->getType() == PropertyEventType::TLP_AFTER_SET_ALL_EDGE_VALUE) {
      _propertiesModified.insert(propEv->getProperty());
    }
  }
}

void EdgesGraphModel::treatEvents(const std::vector<tlp::Event> &events) {
  GraphModel::treatEvents(events);
  _edgesAdded = false;
  _edgesRemoved = false;
}

QVariant EdgesGraphModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Vertical) {
    if (role == Qt::ToolTipRole && (section >= 0 && section < _elements.size())) {
      edge e = edge(_elements[section]);
      return getEdgeTooltip(_graph, e);
    }
  }

  return GraphModel::headerData(section, orientation, role);
}

QString EdgesGraphModel::getEdgeTooltip(Graph *graph, edge e) {
  const std::string &label = graph->getProperty("viewLabel")->getEdgeStringValue(e);
  const auto &[src, tgt] = graph->ends(e);
  const std::string &sourceLabel = graph->getProperty("viewLabel")->getNodeStringValue(src);
  const std::string &targetLabel = graph->getProperty("viewLabel")->getNodeStringValue(tgt);
  return QString("<b>Edge #")
      .append(QString::number(e.id))
      .append(label.empty() ? "</b>" : "</b> (<b><i>" + tlpStringToQString(label) + ")</b>")
      .append("\nsource: <b>node #")
      .append(QString::number(src.id))
      .append(sourceLabel.empty() ? "</b>" : "</b> (" + tlpStringToQString(sourceLabel) + ")")
      .append("\ntarget: <b>node #")
      .append(QString::number(tgt.id))
      .append(targetLabel.empty() ? "</b>" : "</b> (" + tlpStringToQString(targetLabel) + ")");
}
