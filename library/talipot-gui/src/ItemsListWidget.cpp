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

#include <talipot/ItemsListWidget.h>

#include <QMouseEvent>
#include <QApplication>
#include <QMimeData>
#include <QDrag>

namespace tlp {

ItemsListWidget::ItemsListWidget(QWidget *parent, const uint maxListSize)
    : QListWidget(parent), maxListSize(maxListSize) {
    setAcceptDrops(true);
}

void ItemsListWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        startPos = event->pos();
    }

    QListWidget::mousePressEvent(event);
}

void ItemsListWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - startPos).manhattanLength();

        if (distance >= QApplication::startDragDistance()) {
            QListWidgetItem *item = currentItem();
            beginDrag(item);
        }
    }

    QListWidget::mouseMoveEvent(event);
}

void ItemsListWidget::beginDrag(QListWidgetItem *item) {
    if (item) {
        auto *mimeData = new QMimeData;
        mimeData->setText(item->text());
        auto *drag = new QDrag(this);
        drag->setMimeData(mimeData);

        if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
            changeStatus(item);
            delete item;
        }
    }
}

void ItemsListWidget::dragEnterEvent(QDragEnterEvent *event) {
    dragMoveOrEnterEvent(event);
}

void ItemsListWidget::dragMoveEvent(QDragMoveEvent *event) {
    dragMoveOrEnterEvent(event);
}

void ItemsListWidget::dragMoveOrEnterEvent(QDragMoveEvent *event) {
    auto *source = qobject_cast<ItemsListWidget *>(event->source());

    if (source && source != this) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void ItemsListWidget::dropEvent(QDropEvent *event) {
    auto *source = qobject_cast<ItemsListWidget *>(event->source());

    if (source && source != this) {
        if (addItemList(event->mimeData()->text())) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
    }
}

bool ItemsListWidget::addItemList(QString item) {
    if (maxListSize == 0 || uint(count()) < maxListSize) {
        addItem(item);
        return true;
    } else {
        return false;
    }
}

void ItemsListWidget::deleteItemList(QListWidgetItem *item) {
    changeStatus(item);
    delete item;
}

void ItemsListWidget::changeStatus(QListWidgetItem *item) {
    bool x = (hashDataBase.value(item->text())).choice;
    struct itemInfo temp;

    temp.choice = x == false;

    temp.attribute = (hashDataBase.value(item->text())).attribute;
    hashDataBase.remove(item->text());
    hashDataBase.insert(item->text(), temp);
}
}
