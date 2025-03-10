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

#include <talipot/Graph.h>
#include <talipot/TlpQtTools.h>
#include <talipot/FontIcon.h>
#include <talipot/MaterialDesignIcons.h>

namespace tlp {

template <typename PROPTYPE>
void tlp::GraphPropertiesModel<PROPTYPE>::rebuildCache() {
  _properties.clear();

  if (_graph == nullptr) {
    return;
  }

  for (auto *inheritedProp : _graph->getInheritedObjectProperties()) {
#ifdef NDEBUG

    if (inheritedProp->getName() == "viewMetaGraph") {
      continue;
    }

#endif
    auto *prop = dynamic_cast<PROPTYPE *>(inheritedProp);

    if (prop != nullptr) {
      _properties += prop;
    }
  }
  for (auto *localProp : _graph->getLocalObjectProperties()) {
#ifdef NDEBUG

    if (localProp->getName() == "viewMetaGraph") {
      continue;
    }

#endif
    auto *prop = dynamic_cast<PROPTYPE *>(localProp);

    if (prop != nullptr) {
      _properties += prop;
    }
  }
}

template <typename PROPTYPE>
GraphPropertiesModel<PROPTYPE>::GraphPropertiesModel(tlp::Graph *graph, bool checkable,
                                                     QObject *parent)
    : tlp::Model(parent), _graph(graph), _checkable(checkable), _removingRows(false),
      forcingRedraw(false) {
  if (_graph != nullptr) {
    _graph->addListener(this);
    rebuildCache();
  }
}

template <typename PROPTYPE>
GraphPropertiesModel<PROPTYPE>::GraphPropertiesModel(QString placeholder, tlp::Graph *graph,
                                                     bool checkable, QObject *parent)
    : tlp::Model(parent), _graph(graph), _placeholder(placeholder), _checkable(checkable),
      _removingRows(false), forcingRedraw(false) {
  if (_graph != nullptr) {
    _graph->addListener(this);
    rebuildCache();
  }
}

template <typename PROPTYPE>
QModelIndex GraphPropertiesModel<PROPTYPE>::index(int row, int column,
                                                  const QModelIndex &parent) const {
  if (_graph == nullptr || !hasIndex(row, column, parent)) {
    return QModelIndex();
  }

  int vectorIndex = row;

  if (!_placeholder.isEmpty()) {
    if (row == 0) {
      return createIndex(row, column);
    }

    vectorIndex--;
  }

  return createIndex(row, column, _properties[vectorIndex]);
}

template <typename PROPTYPE>
QModelIndex GraphPropertiesModel<PROPTYPE>::parent(const QModelIndex &) const {
  return QModelIndex();
}

template <typename PROPTYPE>
int GraphPropertiesModel<PROPTYPE>::rowCount(const QModelIndex &parent) const {
  if (parent.isValid() || _graph == nullptr || forcingRedraw) {
    return 0;
  }

  int result = _properties.size();

  if (!_placeholder.isEmpty()) {
    result++;
  }

  return result;
}

template <typename PROPTYPE>
int GraphPropertiesModel<PROPTYPE>::columnCount(const QModelIndex &) const {
  return 3;
}

template <typename PROPTYPE>
QVariant GraphPropertiesModel<PROPTYPE>::data(const QModelIndex &index, int role) const {
  if (_graph == nullptr || (index.internalPointer() == nullptr && index.row() != 0)) {
    return QVariant();
  }

  auto *pi = static_cast<PropertyInterface *>(index.internalPointer());

  if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
    if (!_placeholder.isEmpty() && index.row() == 0) {
      return _placeholder;
    }

    if (pi == nullptr) {
      return QString();
    }

    if (index.column() == 0) {
      return tlpStringToQString(pi->getName());
    } else if (index.column() == 1) {
      return pi->getTypename().c_str();
    } else if (index.column() == 2) {
      return (_graph->existLocalProperty(pi->getName())
                  ? tr("Local")
                  : tr("Inherited from graph ") + QString::number(pi->getGraph()->getId()) + " (" +
                        tlpStringToQString(pi->getGraph()->getName()) + ')');
    }
  } else if (role == Qt::DecorationRole && index.column() == 0 && pi != nullptr &&
             !_graph->existLocalProperty(pi->getName())) {
    return FontIcon::icon(MaterialDesignIcons::TransferUp);
  } else if (role == Qt::FontRole) {
    QFont f;

    if (!_placeholder.isEmpty() && index.row() == 0) {
      f.setItalic(true);
    }

    return f;
  } else if (role == PropertyRole) {
    return QVariant::fromValue<PropertyInterface *>(pi);
  } else if (_checkable && role == Qt::CheckStateRole && index.column() == 0) {
    return (_checkedProperties.contains(static_cast<PROPTYPE *>(pi)) ? Qt::Checked : Qt::Unchecked);
  }

  return QVariant();
}

template <typename PROPTYPE>
int GraphPropertiesModel<PROPTYPE>::rowOf(PROPTYPE *pi) const {
  int result = _properties.indexOf(pi);

  if (result > -1 && !_placeholder.isEmpty()) {
    ++result;
  }

  return result;
}

template <typename PROPTYPE>
int GraphPropertiesModel<PROPTYPE>::rowOf(const QString &pName) const {
  for (int i = 0; i < _properties.size(); ++i) {
    if (pName == tlpStringToQString(_properties[i]->getName())) {
      return i;
    }
  }

  return -1;
}

template <typename PROPTYPE>
QVariant tlp::GraphPropertiesModel<PROPTYPE>::headerData(int section, Qt::Orientation orientation,
                                                         int role) const {
  if (orientation == Qt::Horizontal) {
    if (role == Qt::DisplayRole) {
      if (section == 0) {
        return tr("Name");
      } else if (section == 1) {
        return tr("Type");
      } else if (section == 2) {
        return tr("Scope");
      }
    }
  }

  return Model::headerData(section, orientation, role);
}

template <typename PROPTYPE>
bool tlp::GraphPropertiesModel<PROPTYPE>::setData(const QModelIndex &index, const QVariant &value,
                                                  int role) {
  if (_graph == nullptr) {
    return false;
  }

  if (_checkable && role == Qt::CheckStateRole && index.column() == 0) {
    if (value.value<int>() == int(Qt::Checked)) {

      _checkedProperties.insert(static_cast<PROPTYPE *>(index.internalPointer()));

    } else {
      _checkedProperties.remove(static_cast<PROPTYPE *>(index.internalPointer()));
    }

    emit checkStateChanged(index, static_cast<Qt::CheckState>(value.value<int>()));
    return true;
  }

  return false;
}
}
