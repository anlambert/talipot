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

#include <talipot/ClearableLineEdit.h>
#include <talipot/MaterialDesignIcons.h>
#include <talipot/FontIcon.h>

#include <QPaintEvent>
#include <QPainter>

using namespace std;
using namespace tlp;

unique_ptr<QPixmap> ClearableLineEdit::CLEAR_PIXMAP;

void ClearableLineEdit::initPixmap() {
  CLEAR_PIXMAP.reset(new QPixmap(
      FontIcon::icon(MaterialDesignIcons::Backspace, 0.5, 0, QPointF(5, 0)).pixmap(32, 32)));
}

ClearableLineEdit::ClearableLineEdit(QWidget *parent)
    : QLineEdit(parent), _clearButtonHovered(false) {
  setMouseTracking(true);
}

void ClearableLineEdit::paintEvent(QPaintEvent *ev) {
  QLineEdit::paintEvent(ev);
  QPainter p(this);
  p.setOpacity(_clearButtonHovered ? 1 : 0.7);
  initPixmap();
  p.drawPixmap(pixmapRect(), *CLEAR_PIXMAP);
}

QRect ClearableLineEdit::pixmapRect() {
  QRect pixmapRect(width() - CLEAR_PIXMAP->width() - 5, height() / 2 - CLEAR_PIXMAP->height() / 2,
                   CLEAR_PIXMAP->width(), CLEAR_PIXMAP->height());
  return pixmapRect;
}

void ClearableLineEdit::mouseMoveEvent(QMouseEvent *ev) {
  QLineEdit::mouseMoveEvent(ev);
  bool oldValue = _clearButtonHovered;
  _clearButtonHovered = pixmapRect().translated(10, 0).contains(ev->pos());

  if (oldValue != _clearButtonHovered) {
    repaint();
  }
}

void ClearableLineEdit::mousePressEvent(QMouseEvent *ev) {
  QLineEdit::mousePressEvent(ev);

  if (pixmapRect().translated(10, 0).contains(ev->pos())) {
    clear();
    emit textEdited("");
    emit editingFinished();
  }
}

void ClearableLineEdit::leaveEvent(QEvent *) {
  _clearButtonHovered = false;
  repaint();
}
