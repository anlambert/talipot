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

#include <talipot/GraphTableItemDelegate.h>

using namespace tlp;
using namespace std;

GraphTableItemDelegate::GraphTableItemDelegate(QObject *parent) : ItemDelegate(parent) {}

void GraphTableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const {
    auto *pi = index.data(Model::PropertyRole).value<PropertyInterface *>();

    if (index.data().canConvert<double>() && dynamic_cast<DoubleProperty *>(pi) != nullptr) {
        auto *prop = static_cast<DoubleProperty *>(pi);
        auto value = index.data().value<double>();
        double min = 0, max = 0;

        if (index.data(Model::IsNodeRole).value<bool>()) {
            min = prop->getNodeMin(index.data(Model::GraphRole).value<tlp::Graph *>());
            max = prop->getNodeMax(index.data(Model::GraphRole).value<tlp::Graph *>());
        }

        if (max != min) {
            painter->setBrush(QColor(200, 200, 200));
            painter->setPen(QColor(200, 200, 200));
            painter->drawRect(option.rect.x(), option.rect.y() + 1,
                              ((value - min) / (max - min)) * option.rect.width(),
                              option.rect.height() - 2);
        }
    }

    ItemDelegate::paint(painter, option, index);
}
