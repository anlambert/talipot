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

#include <talipot/ParameterListModel.h>
#include <talipot/MetaTypes.h>
#include <talipot/FontIcon.h>
#include <talipot/MaterialDesignIcons.h>

#include <QPainter>

namespace tlp {

static QIcon bracketIcon() {
  QPixmap pixmap =
      FontIcon::icon(MaterialDesignIcons::CodeBrackets, QColor(Qt::darkGray), 1.4).pixmap(128, 128);
  QPainter painter(&pixmap);
  painter.fillRect(0, 0, 64, 128, backgroundColor());
  painter.end();
  return QIcon(pixmap);
}

static QIcon inputIcon() {
  return FontIcon::stackIcons(
      bracketIcon(), FontIcon::icon(MaterialDesignIcons::ArrowRightBold, QColor("#0d93f7")));
}

static QIcon outputIcon() {
  return FontIcon::stackIcons(bracketIcon(),
                              FontIcon::icon(MaterialDesignIcons::ArrowLeftBold, QColor(Qt::red)));
}

static QIcon inputOutputIcon() {
  const QIcon &rightArrow =
      FontIcon::icon(MaterialDesignIcons::ArrowRightBold, QColor("#0d93f7"), 1, 0, QPointF(0, -20));
  const QIcon &leftArrow =
      FontIcon::icon(MaterialDesignIcons::ArrowLeftBold, QColor(Qt::red), 1, 0, QPointF(0, 20));
  QIcon tmp = FontIcon::stackIcons(bracketIcon(), rightArrow);
  return FontIcon::stackIcons(tmp, leftArrow);
}

ParameterListModel::ParameterListModel(const tlp::ParameterDescriptionList &params,
                                       tlp::Graph *graph, QObject *parent)
    : Model(parent), _graph(graph) {
  std::vector<ParameterDescription> outParams;
  // first add in parameters
  for (const auto &param : params.getParameters()) {
    if (param.getDirection() != OUT_PARAM) {
      _params.push_back(param);
    } else {
      outParams.push_back(param);
    }
  }

  // then add out parameters
  for (const auto &outParam : outParams) {
    _params.push_back(outParam);
  }

  // no sort, keep the predefined ordering
  params.buildDefaultDataSet(_data, graph);
}

QModelIndex ParameterListModel::index(int row, int column, const QModelIndex &) const {
  return createIndex(row, column);
}

QModelIndex ParameterListModel::parent(const QModelIndex &) const {
  return QModelIndex();
}

int ParameterListModel::rowCount(const QModelIndex &) const {
  return _params.size();
}

int ParameterListModel::columnCount(const QModelIndex &) const {
  return 1;
}

QVariant ParameterListModel::data(const QModelIndex &index, int role) const {
  if (role == GraphRole) {
    return QVariant::fromValue<tlp::Graph *>(_graph);
  }

  const ParameterDescription &info = _params[index.row()];

  if (role == Qt::ToolTipRole) {
    return tlp::tlpStringToQString(info.getHelp());
  } else if (role == Qt::WhatsThisRole) {
    return tlp::tlpStringToQString(info.getHelp());
  } else if (role == Qt::BackgroundRole) {
    if (info.isMandatory()) {
      return QColor(255, 255, 222);
    } else {
      return QColor(222, 255, 222);
    }
  } else if (role == Qt::ForegroundRole) {
    return darkColor;
  } else if (role == Qt::DisplayRole) {
    tlp::DataType *dataType = _data.getData(info.getName());

    if (!dataType) {
      return info.getTypeName().c_str();
    }

    QVariant result = MetaTypes::dataTypeToQvariant(dataType, info.getName());
    delete dataType;
    return result;
  } else if (role == MandatoryRole) {
    return info.isMandatory();
  }

  return QVariant();
}

QVariant ParameterListModel::headerData(int section, Qt::Orientation orientation, int role) const {

  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    if (section == 0) {
      return "Name";
    } else {
      return "Value";
    }
  }

  if (orientation == Qt::Vertical) {
    const ParameterDescription &info = _params[section];

    if (role == Qt::DisplayRole) {
      // remove prefix if any
      size_t pos = info.getName().find("::");

      if (pos != std::string::npos) {
        return tlp::tlpStringToQString(info.getName().c_str() + pos + 2);
      }

      return tlp::tlpStringToQString(info.getName());
    } else if (role == Qt::BackgroundRole) {
      if (info.isMandatory()) {
        return QColor(255, 255, 222);
      } else {
        return QColor(222, 255, 222);
      }
    } else if (role == Qt::ToolTipRole) {
      return tlp::tlpStringToQString(info.getHelp());
    } else if (role == Qt::DecorationRole) {
      if (info.getDirection() == IN_PARAM) {
        return inputIcon();
      } else if (info.getDirection() == OUT_PARAM) {
        return outputIcon();
      } else {
        return inputOutputIcon();
      }
    }
  }

  return Model::headerData(section, orientation, role);
}

Qt::ItemFlags ParameterListModel::flags(const QModelIndex &index) const {
  Qt::ItemFlags result = QAbstractItemModel::flags(index);
  const ParameterDescription &info = _params[index.row()];
  bool editable = info.isEditable();

  if (index.column() == 0) {
    if (editable) {
      result |= Qt::ItemIsEditable;
    }
  } else if (!editable) {
    result ^= Qt::ItemIsEditable;
  }

  return result;
}

bool ParameterListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (role == Qt::EditRole) {
    const ParameterDescription &info = _params[index.row()];

    DataType *dataType = MetaTypes::qVariantToDataType(value);

    if (dataType) {
      _data.setData(info.getName(), dataType);
    }

    return (dataType != nullptr);
  }

  return QAbstractItemModel::setData(index, value, role);
}

DataSet ParameterListModel::parametersValues() const {
  return _data;
}

void ParameterListModel::setParametersValues(const DataSet &data) {
  _data = data;
}
}
