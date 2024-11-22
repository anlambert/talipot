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

#include "NavigableTableView.h"

#include <QKeyEvent>
#include <QHeaderView>

NavigableTableView::NavigableTableView(QWidget *parent) : QTableView(parent) {
  horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void NavigableTableView::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Home) {
    scrollToTop();
  } else if (event->key() == Qt::Key_End) {
    scrollToBottom();
  } else {
    QTableView::keyPressEvent(event);
  }
}

int NavigableTableView::sizeHintForRow(int row) const {
  if (!model()) {
    return -1;
  }

  ensurePolished();
  int left = qMax(0, horizontalHeader()->visualIndexAt(0));
  int right = horizontalHeader()->visualIndexAt(viewport()->width());

  if (right < 0) {
    right = model()->columnCount();
  }

  int hint = 0;

  for (int column = left; column <= right; ++column) {

    if (horizontalHeader()->isSectionHidden(column)) {
      continue;
    }

    QModelIndex index = model()->index(row, column);
    QStyleOptionViewItem option;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    initViewItemOption(&option);
    hint = qMax(hint, itemDelegateForIndex(index)->sizeHint(option, index).height());
#else
    option = viewOptions();
    hint = qMax(hint, itemDelegate(index)->sizeHint(option, index).height());
#endif
  }

  return hint;
}

int NavigableTableView::sizeHintForColumn(int col) const {
  if (!model()) {
    return -1;
  }

  ensurePolished();
  int top = qMax(0, verticalHeader()->visualIndexAt(0));
  int bottom = verticalHeader()->visualIndexAt(viewport()->height());

  int hint = 0;

  if (bottom < 0) {
    bottom = model()->rowCount() - 1;
  }

  for (int row = top; row <= bottom; ++row) {
    QModelIndex index = model()->index(row, col);
    QStyleOptionViewItem option;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    initViewItemOption(&option);
    hint = qMax(hint, itemDelegateForIndex(index)->sizeHint(option, index).width());
#else
    option = viewOptions();
    hint = qMax(hint, itemDelegate(index)->sizeHint(option, index).width());
#endif
  }

  return hint;
}

void NavigableTableView::paintEvent(QPaintEvent *event) {
  resizeTableRows();
  QTableView::paintEvent(event);
}

void NavigableTableView::resizeTableRows() {

  if (!model()) {
    return;
  }

  int top = qMax(0, verticalHeader()->visualIndexAt(0));
  int bottom = verticalHeader()->visualIndexAt(viewport()->height());

  if (bottom < 0) {
    bottom = model()->rowCount() - 1;
  }

  int left = qMax(0, horizontalHeader()->visualIndexAt(0));
  int right = horizontalHeader()->visualIndexAt(viewport()->width());

  if (right < 0) {
    right = model()->columnCount();
  }

  for (int i = top; i <= bottom; ++i) {
    resizeRowToContents(i);
  }

  for (int i = left; i <= right; ++i) {
    resizeColumnToContents(i);
  }

  scrollContentsBy(-columnViewportPosition(left), 0);
}
