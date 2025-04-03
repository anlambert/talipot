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

#include <talipot/GlEntityItemModel.h>

#include <QFont>

namespace tlp {

QStringList GlEntityItemEditor::propertiesNames() const {
    return QStringList();
}

QVariantList GlEntityItemEditor::propertiesQVariant() const {
    return QVariantList();
}

void GlEntityItemEditor::setProperty(const QString &, const QVariant &) {}

GlEntityItemModel::GlEntityItemModel(GlEntityItemEditor *itemEditor, QObject *parent)
    : QAbstractItemModel(parent), editor(itemEditor) {}

GlEntityItemModel::~GlEntityItemModel() = default;

int GlEntityItemModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }

    return editor->propertiesNames().size();
}

int GlEntityItemModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }

    return 1;
}

QModelIndex GlEntityItemModel::parent(const QModelIndex &) const {
    return QModelIndex();
}

QVariant GlEntityItemModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
            return headerText();
        } else if (role == Qt::TextAlignmentRole) {
            return Qt::AlignCenter;
        }
    } else if (role == Qt::DisplayRole) {
        return editor->propertiesNames()[section];
    }

    if (orientation == Qt::Horizontal && role == Qt::FontRole) {
        QFont f;
        f.setBold(true);
        f.setPointSize(f.pointSize() - 1);
        return f;
    }

    return QVariant();
}

QModelIndex GlEntityItemModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    return QAbstractItemModel::createIndex(row, column, static_cast<void *>(nullptr));
}

QVariant GlEntityItemModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        return editor->propertiesQVariant()[index.row()];
    }

    return QVariant();
}

bool GlEntityItemModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == Qt::EditRole) {
        editor->setProperty(editor->propertiesNames()[index.row()], value);
        return true;
    }

    return false;
}
}
