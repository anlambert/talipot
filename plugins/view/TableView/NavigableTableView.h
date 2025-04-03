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

#ifndef NAVIGABLE_TABLE_VIEW_H
#define NAVIGABLE_TABLE_VIEW_H

#include <QTableView>

class QKeyEvent;

class NavigableTableView : public QTableView {
    Q_OBJECT

  public:
    explicit NavigableTableView(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event) override;

  protected:
    int sizeHintForRow(int row) const override;
    int sizeHintForColumn(int col) const override;
    void paintEvent(QPaintEvent *event) override;

  private:
    void resizeTableRows();
};

#endif // NAVIGABLE_TABLE_VIEW_H
