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

#ifndef TALIPOT_GRAPH_TABLE_ITEM_DELEGATE_H
#define TALIPOT_GRAPH_TABLE_ITEM_DELEGATE_H

#include <talipot/config.h>
#include <talipot/ItemDelegate.h>

class QStyleOptionViewItem;

namespace tlp {

/**
 * @brief QItemDelegate to display and edit data from a GraphTableModel in the Qt model/view
 *architecture.
 **/
class TLP_QT_SCOPE GraphTableItemDelegate : public ItemDelegate {
    Q_OBJECT
  public:
    GraphTableItemDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
};
}
#endif // TALIPOT_GRAPH_TABLE_ITEM_DELEGATE_H
