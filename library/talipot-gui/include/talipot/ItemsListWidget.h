/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

///@cond DOXYGEN_HIDDEN

#ifndef TALIPOT_ITEMS_LIST_WIDGET_H
#define TALIPOT_ITEMS_LIST_WIDGET_H

#ifndef DOXYGEN_NOTFOR_DEVEL

#include <talipot/config.h>

#include <QListWidget>

namespace tlp {

struct itemInfo {
  QString attribute;
  bool choice;
};

// A Custom List Widget which supports drag and drop
class TLP_QT_SCOPE ItemsListWidget : public QListWidget {

  Q_OBJECT

public:
  ItemsListWidget(QWidget *parent = nullptr, const unsigned int maxListSize = 0);

  // Method which adds an item in the list
  // return true if the item has been added, false if the maximum size of the list is already
  // reached
  bool addItemList(QString item);

  void deleteItemList(QListWidgetItem *item);

  // Method which sets the maximum size of the list
  // if 0, there is no size restriction
  void setMaxListSize(const unsigned int maxListSize) {
    this->maxListSize = maxListSize;
  }
  unsigned int getMaxListSize() const {
    return maxListSize;
  }

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dropEvent(QDropEvent *event) override;

private:
  void beginDrag(QListWidgetItem *item);
  void changeStatus(QListWidgetItem *item);
  void dragMoveOrEnterEvent(QDragMoveEvent *event);
  QPoint startPos;
  QHash<QString, itemInfo> hashDataBase;
  unsigned int maxListSize;
};
}

#endif // DOXYGEN_NOTFOR_DEVEL

#endif // TALIPOT_ITEMS_LIST_WIDGET_H

///@endcond
