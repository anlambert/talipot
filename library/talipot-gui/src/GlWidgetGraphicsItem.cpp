/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "talipot/GlWidgetGraphicsItem.h"

#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QGraphicsProxyWidget>
#include <QPainter>

#include <talipot/GlWidget.h>
#include <talipot/GlOffscreenRenderer.h>

using namespace std;
using namespace tlp;

GlWidgetGraphicsItem::GlWidgetGraphicsItem(GlWidget *_glWidget, int width, int height)
    : QGraphicsObject(), _glWidget(_glWidget), _redrawNeeded(true), _graphChanged(true) {

  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setAcceptHoverEvents(true);
  setHandlesChildEvents(false);
  setAcceptDrops(true);

  connect(_glWidget, &GlWidget::viewDrawn, this, &GlWidgetGraphicsItem::glWidgetDraw);
  connect(_glWidget, &GlWidget::viewRedrawn, this, &GlWidgetGraphicsItem::glWidgetRedraw);

  resize(width, height);
  _glWidget->installEventFilter(this);
  setHandlesChildEvents(false);
}

void GlWidgetGraphicsItem::setGlWidget(GlWidget *glmw) {
  if (glmw) {
    disconnect(_glWidget, &GlWidget::viewDrawn, this, &GlWidgetGraphicsItem::glWidgetDraw);
    disconnect(_glWidget, &GlWidget::viewRedrawn, this, &GlWidgetGraphicsItem::glWidgetRedraw);
    _glWidget->removeEventFilter(this);
    _glWidget = glmw;
    connect(_glWidget, &GlWidget::viewDrawn, this, &GlWidgetGraphicsItem::glWidgetDraw);
    connect(_glWidget, &GlWidget::viewRedrawn, this, &GlWidgetGraphicsItem::glWidgetRedraw);
    _glWidget->installEventFilter(this);
  }
}

GlWidgetGraphicsItem::~GlWidgetGraphicsItem() {
  delete _glWidget;
}

QRectF GlWidgetGraphicsItem::boundingRect() const {
  return QRectF(0, 0, width, height);
}

void GlWidgetGraphicsItem::resize(int width, int height) {

  this->width = width;
  this->height = height;
  _glWidget->resize(width, height);
  _glWidget->resizeGL(width, height);
  _redrawNeeded = true;
  _graphChanged = true;
  prepareGeometryChange();
}

void GlWidgetGraphicsItem::glWidgetDraw(GlWidget *, bool graphChanged) {
  _redrawNeeded = true;
  _graphChanged = graphChanged;
  update();
}

void GlWidgetGraphicsItem::glWidgetRedraw(GlWidget *) {
  update();
}

void GlWidgetGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {

  if (_redrawNeeded) {
    emit widgetPainted(_graphChanged);
  }

  GlOffscreenRenderer::instance().setViewPortSize(width, height);

  if (_redrawNeeded) {
    GlOffscreenRenderer::instance().renderGlWidget(_glWidget);
    _redrawNeeded = false;
  } else {
    GlOffscreenRenderer::instance().renderGlWidget(_glWidget, false);
  }

  painter->drawImage(QRect(0, 0, width, height), GlOffscreenRenderer::instance().getImage());
}

void GlWidgetGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  QMouseEvent eventModif(QEvent::MouseMove, QPoint(event->pos().x(), event->pos().y()),
                         Qt::NoButton, event->buttons(), event->modifiers());
  QApplication::sendEvent(_glWidget, &eventModif);
  event->setAccepted(eventModif.isAccepted());
}

void GlWidgetGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  QMouseEvent eventModif(QEvent::MouseButtonPress, QPoint(event->pos().x(), event->pos().y()),
                         event->button(), event->buttons(), event->modifiers());
  QApplication::sendEvent(_glWidget, &eventModif);
  event->setAccepted(eventModif.isAccepted());
}

void GlWidgetGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
  QMouseEvent eventModif(QEvent::MouseButtonDblClick, QPoint(event->pos().x(), event->pos().y()),
                         event->button(), event->buttons(), event->modifiers());
  QApplication::sendEvent(_glWidget, &eventModif);
  event->setAccepted(eventModif.isAccepted());
}

void GlWidgetGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  QMouseEvent eventModif(QEvent::MouseButtonRelease, QPoint(event->pos().x(), event->pos().y()),
                         event->button(), event->buttons(), event->modifiers());
  QApplication::sendEvent(_glWidget, &eventModif);
  event->setAccepted(eventModif.isAccepted());
}

void GlWidgetGraphicsItem::wheelEvent(QGraphicsSceneWheelEvent *event) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
  QPoint angleDelta =
      event->orientation() == Qt::Vertical ? QPoint(0, event->delta()) : QPoint(event->delta(), 0);
  QWheelEvent eventModif(event->pos(), event->screenPos(), QPoint(), angleDelta, event->buttons(),
                         event->modifiers(), Qt::NoScrollPhase, false);
#else
  QWheelEvent eventModif(QPoint(event->pos().x(), event->pos().y()), event->delta(),
                         event->buttons(), event->modifiers(), event->orientation());
#endif

  QApplication::sendEvent(_glWidget, &eventModif);
  event->setAccepted(eventModif.isAccepted());
}

void GlWidgetGraphicsItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  QMouseEvent eventModif(QEvent::MouseMove, QPoint(event->pos().x(), event->pos().y()),
                         Qt::NoButton, Qt::NoButton, event->modifiers());
  QApplication::sendEvent(_glWidget, &eventModif);
  event->setAccepted(eventModif.isAccepted());
}

void GlWidgetGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QContextMenuEvent eventModif(static_cast<QContextMenuEvent::Reason>(event->reason()),
                               QPoint(event->pos().x(), event->pos().y()));
  QApplication::sendEvent(_glWidget, &eventModif);
  event->setAccepted(eventModif.isAccepted());
}

void GlWidgetGraphicsItem::keyReleaseEvent(QKeyEvent *event) {
  QKeyEvent eventModif(event->type(), event->key(), event->modifiers(), event->text(),
                       event->isAutoRepeat(), event->count());
  QApplication::sendEvent(_glWidget, &eventModif);
  event->setAccepted(eventModif.isAccepted());
}

void GlWidgetGraphicsItem::keyPressEvent(QKeyEvent *event) {
  QKeyEvent eventModif(event->type(), event->key(), event->modifiers(), event->text(),
                       event->isAutoRepeat(), event->count());
  QApplication::sendEvent(_glWidget, &eventModif);
  event->setAccepted(eventModif.isAccepted());
}

void GlWidgetGraphicsItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event) {
  if (_glWidget->isEnabled() && _glWidget->acceptDrops()) {
    QDragEnterEvent proxyDragEnter(event->pos().toPoint(), event->dropAction(), event->mimeData(),
                                   event->buttons(), event->modifiers());
    proxyDragEnter.setAccepted(event->isAccepted());
    QApplication::sendEvent(_glWidget, &proxyDragEnter);
    event->setAccepted(proxyDragEnter.isAccepted());
    event->setDropAction(proxyDragEnter.dropAction());
  } else {
    event->ignore();
  }
}
void GlWidgetGraphicsItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event) {
  QDragLeaveEvent proxyDragLeave;
  QApplication::sendEvent(_glWidget, &proxyDragLeave);
  event->setAccepted(proxyDragLeave.isAccepted());
}

void GlWidgetGraphicsItem::dragMoveEvent(QGraphicsSceneDragDropEvent *event) {
  if (_glWidget->isEnabled() && _glWidget->acceptDrops()) {
    QDragMoveEvent dragMove(event->pos().toPoint(), event->possibleActions(), event->mimeData(),
                            event->buttons(), event->modifiers());
    QApplication::sendEvent(_glWidget, &dragMove);
    event->setAccepted(dragMove.isAccepted());
    event->setDropAction(dragMove.dropAction());
  } else {
    event->ignore();
  }
}

void GlWidgetGraphicsItem::dropEvent(QGraphicsSceneDragDropEvent *event) {
  QDropEvent dropEvent(event->pos().toPoint(), event->possibleActions(), event->mimeData(),
                       event->buttons(), event->modifiers());
  QApplication::sendEvent(_glWidget, &dropEvent);
  event->setAccepted(dropEvent.isAccepted());
}

bool GlWidgetGraphicsItem::eventFilter(QObject *, QEvent *evt) {
  if (evt->type() == QEvent::CursorChange) {
    setCursor(_glWidget->cursor());
  }

// There is a bug with Qt5 on windows that leads to an incorrect viewport size of 160x160
// when initializing a Tulip OpenGL view.
// Seems that a delayed resize event is sent to the GlWidget, don't really know why ...
// In the context of a GlWidgetGraphicsItem, the internally used GlWidget is hidden,
// so it should not receive that event but it does once after it has been created.
// As a workaround to fix that annoying behaviour, catch that resize event, discard it
// and restore the correct size that was previously set.
#if defined(WIN32)

  if (evt->type() == QEvent::Resize) {
    _glWidget->resize(width, height);
    return true;
  }

#endif
  return false;
}
