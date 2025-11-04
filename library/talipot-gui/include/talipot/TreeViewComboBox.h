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

#ifndef TALIPOT_TREE_VIEW_COMBO_BOX_H
#define TALIPOT_TREE_VIEW_COMBO_BOX_H

#include <QComboBox>
#include <QTreeView>

#include <talipot/config.h>

namespace tlp {

class TLP_QT_SCOPE TreeViewComboBox : public QComboBox {

  Q_OBJECT
  QTreeView *_treeView;
  bool _popupVisible;
  QModelIndex _lastIndex;

public:
  explicit TreeViewComboBox(QWidget *parent = nullptr);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  void setModel(QAbstractItemModel *model);
#else
  void setModel(QAbstractItemModel *model) override;
#endif

  void showPopup() override;
  void hidePopup() override;

  QModelIndex selectedIndex() const;

public slots:

  void selectIndex(const QModelIndex &);
  void rowsRemoved(const QModelIndex &, int, int);
  void currentIndexChanged();

signals:

  void currentItemChanged();
};

}

#endif // TALIPOT_TREE_VIEW_COMBO_BOX_H
