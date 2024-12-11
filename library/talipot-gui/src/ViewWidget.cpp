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

#include "talipot/ViewWidget.h"

#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>

#include <talipot/GlWidget.h>
#include <talipot/GlWidgetGraphicsItem.h>
#include <talipot/Interactor.h>

using namespace tlp;

struct TalipotGraphicsView : public QGraphicsView {
  QGraphicsItem *_centralItem;

  TalipotGraphicsView() : QGraphicsView(new QGraphicsScene()), _centralItem(nullptr) {
    setAcceptDrops(false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  }

  ~TalipotGraphicsView() override {
    delete scene();
  }

  void resizeEvent(QResizeEvent *event) override {
    QGraphicsView::resizeEvent(event);

    if (scene()) {
      scene()->setSceneRect(QRect(QPoint(0, 0), size()));
    }

    auto *glWidgetItem = dynamic_cast<GlWidgetGraphicsItem *>(_centralItem);
    auto *proxyWidget = dynamic_cast<QGraphicsProxyWidget *>(_centralItem);

    if (glWidgetItem) {
      glWidgetItem->resize(width(), height());
    } else if (proxyWidget) {
      proxyWidget->resize(width(), height());
    }

    if (scene()) {
      scene()->update();
    }
  }
};

ViewWidget::ViewWidget()
    : View(), _graphicsView(nullptr), _centralWidget(nullptr), _centralWidgetItem(nullptr) {}

ViewWidget::~ViewWidget() {
  // ensure to uninstall event filter of current interactor to
  // avoid possible crashes when closing the view
  if (currentInteractor()) {
    currentInteractor()->uninstall();
  }

  if (_centralWidgetItem) {
    _graphicsView->scene()->removeItem(_centralWidgetItem);
    delete _centralWidgetItem;
  }
}

QGraphicsView *ViewWidget::graphicsView() const {
  return _graphicsView;
}

void ViewWidget::setupUi() {
  _graphicsView = new TalipotGraphicsView();
  _graphicsView->setFrameStyle(QFrame::NoFrame);
  _graphicsView->setBackgroundBrush(Qt::white);
  setupWidget();
  assert(_centralWidget);
}

void ViewWidget::currentInteractorChanged(tlp::Interactor *i) {
  if (i) {
    i->install(_centralWidget);
  }
}

void ViewWidget::graphDeleted(Graph *parentGraph) {
  setGraph(parentGraph);
}

void ViewWidget::setCentralWidget(QWidget *w, bool deleteOldCentralWidget) {
  assert(w);
  QGraphicsItem *oldCentralItem = _centralWidgetItem;

  if (currentInteractor()) {
    currentInteractor()->uninstall();
  }

  _centralWidget = w;

  if (currentInteractor()) {
    currentInteractor()->install(w);
  }

  auto *glWidget = dynamic_cast<GlWidget *>(w);

  if (glWidget) {
    _graphicsView->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing |
                                  QPainter::TextAntialiasing);
    _graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    auto *glWidgetItem = dynamic_cast<GlWidgetGraphicsItem *>(_centralWidgetItem);

    if (glWidgetItem) {
      deleteOldCentralWidget = false;
      glWidgetItem->setGlWidget(glWidget);
    } else {
      glWidgetItem =
          new GlWidgetGraphicsItem(glWidget, _graphicsView->width(), _graphicsView->height());

      if (_centralWidgetItem) {
        _graphicsView->scene()->removeItem(_centralWidgetItem);
      }

      _centralWidgetItem = glWidgetItem;
      _graphicsView->scene()->addItem(_centralWidgetItem);
    }

    glWidgetItem->resize(_graphicsView->width(), _graphicsView->height());
  } else {
    _graphicsView->setRenderHints(QPainter::TextAntialiasing);
    _graphicsView->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    _centralWidgetItem = _graphicsView->scene()->addWidget(w);
    _centralWidget->resize(_graphicsView->width(), _graphicsView->height());
  }

  static_cast<TalipotGraphicsView *>(_graphicsView)->_centralItem = _centralWidgetItem;

  _centralWidgetItem->setPos(0, 0);

  _centralWidgetItem->setZValue(0);

  refreshItemsParenthood();

  if (deleteOldCentralWidget) {
    delete oldCentralItem;
  }
}

void ViewWidget::addToScene(QGraphicsItem *item) {
  if (_items.contains(item)) {
#ifndef NDEBUG
    qWarning("Trying to double-add an item to an AbstractGraphicsView");
#endif
    return;
  }

  _items.insert(item);
  item->setParentItem(_centralWidgetItem);
}

void ViewWidget::removeFromScene(QGraphicsItem *item) {
  if (!_items.contains(item)) {
    return;
  }

  _items.remove(item);

  if (_graphicsView->scene()) {
    _graphicsView->scene()->removeItem(item);
  }
}

void ViewWidget::refreshItemsParenthood() {
  for (auto *item : _items) {
    item->setParentItem(_centralWidgetItem);
  }
}

QGraphicsItem *ViewWidget::centralItem() const {
  return _centralWidgetItem;
}

QPixmap ViewWidget::snapshot(const QSize &outputSize) const {
  if (_centralWidget == nullptr) {
    return QPixmap();
  }

  QPixmap result(_centralWidget->size());
  _centralWidget->render(&result);

  if (outputSize.isValid()) {
    return result.scaled(outputSize);
  }

  return result;
}
