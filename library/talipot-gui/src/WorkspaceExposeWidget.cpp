/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "talipot/WorkspaceExposeWidget.h"

#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

#include <talipot/View.h>
#include <talipot/WorkspacePanel.h>
#include <talipot/Graph.h>
#include <talipot/FontIcon.h>

using namespace tlp;

// Helper classes
std::unique_ptr<QPixmap> PreviewItem::_closeButtonPixmap;
QRect PreviewItem::_closePixmapRect;

PreviewItem::PreviewItem(const QPixmap &pixmap, WorkspacePanel *panel, QGraphicsItem *parent)
    : QGraphicsObject(parent), _pixmap(pixmap), _panel(panel), _hovered(false),
      _closeButtonHovered(false) {
  if (!_closeButtonPixmap.get()) {
    _closeButtonPixmap.reset(
        new QPixmap(FontIcon::icon(MaterialDesignIcons::Close).pixmap(16, 16)));
    _closePixmapRect = QRect(boundingRect().width() - _closeButtonPixmap->width() - 5,
                             -0.5 * _closeButtonPixmap->height(), _closeButtonPixmap->width(),
                             _closeButtonPixmap->height());
  }

  setFlag(ItemIsMovable);
  setFlag(ItemIsSelectable);
  setAcceptHoverEvents(true);
}
int PreviewItem::textHeight() const {
  QGraphicsTextItem text;
  QFont f;
  f.setBold(true);
  text.setFont(f);
  text.setPlainText(_panel->windowTitle() + " (" +
                    tlpStringToQString(_panel->view()->graph()->getName()) + ")");
  text.setTextWidth(WorkspaceExposeWidget::previewSize().width());
  return text.boundingRect().height();
}
QRectF PreviewItem::boundingRect() const {
  QRectF result = QRectF(0, 0, WorkspaceExposeWidget::previewSize().width(),
                         WorkspaceExposeWidget::previewSize().height() + textHeight());

  if (_hovered) {
    result.setTop(_closePixmapRect.top());
  }

  return result;
}
tlp::WorkspacePanel *PreviewItem::panel() const {
  return _panel;
}
void PreviewItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
  painter->drawPixmap(0, 0, WorkspaceExposeWidget::previewSize().width(),
                      WorkspaceExposeWidget::previewSize().height(), _pixmap);
  QFont f;
  f.setBold(true);
  painter->setFont(f);
  painter->drawText(0, WorkspaceExposeWidget::previewSize().height() + 5,
                    WorkspaceExposeWidget::previewSize().width(), textHeight(),
                    Qt::AlignHCenter | Qt::TextWordWrap,
                    _panel->windowTitle() + " (" +
                        tlpStringToQString(_panel->view()->graph()->getName()) + ")");

  if (_hovered) {
    painter->setOpacity(_closeButtonHovered ? 1 : 0.5);
    painter->drawPixmap(_closePixmapRect, *_closeButtonPixmap);
  }
}
void PreviewItem::hoverEnterEvent(QGraphicsSceneHoverEvent * /*event*/) {
  _hovered = true;
  prepareGeometryChange();
}
void PreviewItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * /*event*/) {
  _hovered = false;
  prepareGeometryChange();
}
void PreviewItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  bool newCloseButtonHovered = _closePixmapRect.contains(event->pos().toPoint());

  if (newCloseButtonHovered != _closeButtonHovered) {
    _closeButtonHovered = newCloseButtonHovered;
    update();
  }
}
void PreviewItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * /*event*/) {
  emit opened();
}
bool PreviewItem::shouldClose(const QPointF &p) {
  return _closePixmapRect.contains(p.toPoint());
}

// *************************
QSize WorkspaceExposeWidget::previewSize() {
  return QSize(150, 100);
}

const int WorkspaceExposeWidget::MARGIN = 50;

WorkspaceExposeWidget::WorkspaceExposeWidget(QWidget *parent)
    : QGraphicsView(parent), _positionAnimation(nullptr), _selectedItem(nullptr),
      _placeholderItem(nullptr), _switchToSingleMode(false) {
  setScene(new QGraphicsScene);
  setSceneRect(0, 0, width(), height());
  setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

WorkspaceExposeWidget::~WorkspaceExposeWidget() {
  delete scene();
}

int WorkspaceExposeWidget::currentPanelIndex() const {
  return _currentPanelIndex;
}

QVector<WorkspacePanel *> WorkspaceExposeWidget::panels() const {
  QVector<WorkspacePanel *> result;

  for (auto *item : _items) {
    result << item->panel();
  }

  return result;
}

bool WorkspaceExposeWidget::isSwitchToSingleMode() const {
  return _switchToSingleMode;
}

void WorkspaceExposeWidget::setData(const QVector<WorkspacePanel *> &panels,
                                    int currentPanelIndex) {
  scene()->clear();
  _items.clear();

  for (auto *p : panels) {
    QPixmap pixmap = p->view()
                         ->snapshot(previewSize() * 3)
                         .scaled(previewSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    auto *item = new PreviewItem(pixmap, p);
    scene()->addItem(item);
    _items << item;
    item->installEventFilter(this);
    connect(item, &PreviewItem::opened, this, &WorkspaceExposeWidget::itemOpened);
  }

  _currentPanelIndex = currentPanelIndex;
  updatePositions();
}

void WorkspaceExposeWidget::resizeEvent(QResizeEvent * /*event*/) {
  updatePositions();
}

qreal distance(const QPointF &a, const QPointF &b) {
  return sqrt(pow((b.x() - a.x()), 2) + pow((b.y() - a.y()), 2));
}

void WorkspaceExposeWidget::updatePositions(bool resetScenePos) {
  //  delete _positionAnimation;
  // Use the reference distance as the distance for a preview to move to the next position. We will
  // index animation speed on that.
  const int referenceDuration = 120;
  qreal referenceDistance = distance(QPointF(0, 0), QPointF(previewSize().width() + MARGIN, 0));

  auto *group = new QParallelAnimationGroup(this);
  int x = MARGIN, y = MARGIN;

  for (auto *i : _items) {
    if (i != _selectedItem) {
      auto *moveAnim = new QPropertyAnimation(i, "pos", group);
      QPointF startPoint = i->pos();
      QPointF endPoint = QPointF(x, y);
      qreal d = distance(startPoint, endPoint);
      int actualDuration =
          std::min<int>(d * referenceDuration / referenceDistance, referenceDuration * 2);
      moveAnim->setDuration(actualDuration);
      moveAnim->setStartValue(startPoint);
      moveAnim->setEndValue(endPoint);
      group->addAnimation(moveAnim);
    } else if (_selectedItem != nullptr) {
      if (_placeholderItem == nullptr) {
        _placeholderItem =
            new QGraphicsRectItem(0, 0, previewSize().width(), previewSize().height());
        _placeholderItem->setBrush(QColor(220, 220, 220));
        _placeholderItem->setPen(QColor(190, 190, 190));
        scene()->addItem(_placeholderItem);
      }

      _placeholderItem->setPos(x, y);
    }

    x += WorkspaceExposeWidget::previewSize().width() + MARGIN;

    if (x >= width() - WorkspaceExposeWidget::previewSize().width() - MARGIN) {
      x = MARGIN;
      y += WorkspaceExposeWidget::previewSize().height() + MARGIN;
    }
  }

  _positionAnimation = group;

  if (resetScenePos) {
    connect(group, &QAbstractAnimation::finished, this, &WorkspaceExposeWidget::resetSceneRect);
  }

  connect(group, &QAbstractAnimation::finished, this,
          &WorkspaceExposeWidget::updatePositionsAnimationFinished);
  group->start(QAbstractAnimation::DeleteWhenStopped);
}

void WorkspaceExposeWidget::updatePositionsAnimationFinished() {
  _positionAnimation = nullptr;
}

void WorkspaceExposeWidget::resetSceneRect() {
  QRectF itemsRect = scene()->itemsBoundingRect();
  setSceneRect(0, 0, std::max<qreal>(width(), itemsRect.width()),
               std::max<qreal>(height(), itemsRect.height()));
}

bool WorkspaceExposeWidget::eventFilter(QObject *obj, QEvent *ev) {
  auto *item = static_cast<PreviewItem *>(obj);

  if (ev->type() == QEvent::GraphicsSceneMousePress) {
    if (item->shouldClose(static_cast<QGraphicsSceneMouseEvent *>(ev)->pos())) {
      _items.removeAll(item);
      item->panel()->close();
      item->deleteLater();

      if (_items.empty()) {
        finish();
      } else {
        updatePositions();
      }
    } else {
      _selectedItem = item;
      _selectedItem->setZValue(1);
    }

    return false;
  }

  else if (item == _selectedItem) {
    if (ev->type() == QEvent::GraphicsSceneMouseMove) {
      auto *mouseEv = static_cast<QGraphicsSceneMouseEvent *>(ev);
      QPointF itemPos = mouseEv->scenePos();
      int itemPerLine = floor(width() / (previewSize().width() + MARGIN));
      int nbLines = _items.size() / itemPerLine;
      int line = itemPos.y() / (previewSize().height() + MARGIN);
      line = std::min<int>(nbLines, line);
      int col = itemPos.x() / (previewSize().width() + MARGIN);
      int index = line * itemPerLine + col;

      if (index != _items.indexOf(item)) {
        _items.removeOne(item);

        if (index < 0) {
          index = 0;
        }

        if (index > _items.size()) {
          index = _items.size();
        }

        _items.insert(index, item);
        updatePositions(false);
      }
    } else if (ev->type() == QEvent::GraphicsSceneMouseRelease) {
      _selectedItem->setZValue(0);
      _selectedItem = nullptr;
      updatePositions();
      delete _placeholderItem;
      _placeholderItem = nullptr;
    }
  }

  return false;
}

bool WorkspaceExposeWidget::event(QEvent *event) {
  if (event->type() == QEvent::KeyPress &&
      static_cast<QKeyEvent *>(event)->key() == Qt::Key_Escape) {
    finish();
  }

  return QGraphicsView::event(event);
}

void WorkspaceExposeWidget::itemOpened() {
  auto *item = static_cast<PreviewItem *>(sender());
  _currentPanelIndex = _items.indexOf(item);
  _switchToSingleMode = true;
  finish();
}

void WorkspaceExposeWidget::finish() {
  emit exposeFinished();
}
