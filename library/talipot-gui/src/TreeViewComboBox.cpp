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

#include <talipot/TreeViewComboBox.h>

#include <QHeaderView>
#include <QStyledItemDelegate>

using namespace tlp;

class TreeViewDelegate : public QStyledItemDelegate {
public:
  TreeViewDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
    QSize result = QStyledItemDelegate::sizeHint(option, index);
    result.setHeight(result.height() + 10);
    return result;
  }
};

TreeViewComboBox::TreeViewComboBox(QWidget *parent)
    : QComboBox(parent), _treeView(nullptr), _popupVisible(false) {
  _treeView = new QTreeView(this);
  _treeView->setEditTriggers(QTreeView::NoEditTriggers);
  _treeView->setAlternatingRowColors(true);
  _treeView->setSelectionBehavior(QTreeView::SelectRows);
  _treeView->setRootIsDecorated(false);
  _treeView->setAllColumnsShowFocus(true);
  _treeView->header()->setVisible(false);
  _treeView->setItemDelegate(new TreeViewDelegate(_treeView));
  _treeView->setItemsExpandable(true);
  setView(_treeView);
  view()->viewport()->installEventFilter(this);
  connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &TreeViewComboBox::currentIndexChanged);
}

void TreeViewComboBox::setModel(QAbstractItemModel *model) {
  QComboBox::setModel(model);
  connect(model, &QAbstractItemModel::rowsRemoved, this, &TreeViewComboBox::rowsRemoved);

  for (int i = 1; i < model->columnCount(); ++i) {
    _treeView->hideColumn(i);
  }
}

void TreeViewComboBox::showPopup() {
  setRootModelIndex(QModelIndex());
  _treeView->expandAll();
  _treeView->resizeColumnToContents(0);
  QComboBox::showPopup();
  QWidget *popup = findChild<QFrame *>();

  if (_treeView->columnWidth(0) > popup->width()) {
    popup->resize(_treeView->columnWidth(0), popup->height());
  }

  _popupVisible = true;
}

void TreeViewComboBox::hidePopup() {
  auto mousePos = view()->mapFromGlobal(QCursor::pos());
  _popupVisible = false;
  QModelIndex index = view()->indexAt(mousePos);
  if (view()->visualRect(index).contains(mousePos)) {
    selectIndex(view()->currentIndex());
  }
  QComboBox::hidePopup();
}

void TreeViewComboBox::selectIndex(const QModelIndex &index) {
  if (!_popupVisible && index != _lastIndex) {
    _lastIndex = index;
    setRootModelIndex(index.parent());
    setCurrentIndex(index.row());
    emit currentItemChanged();
  }
}

QModelIndex TreeViewComboBox::selectedIndex() const {
  return model()->index(currentIndex(), 0, rootModelIndex());
}

void TreeViewComboBox::rowsRemoved(const QModelIndex &parent, int, int) {
  QModelIndex currentIndex = selectedIndex();

  if (currentIndex.isValid()) {
    selectIndex(currentIndex);
  } else {
    selectIndex(parent);
  }
}

void TreeViewComboBox::currentIndexChanged() {
  selectIndex(selectedIndex());
}
