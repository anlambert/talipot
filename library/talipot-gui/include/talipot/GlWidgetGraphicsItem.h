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

#ifndef TALIPOT_GL_MAIN_WIDGET_GRAPHICS_ITEM_H
#define TALIPOT_GL_MAIN_WIDGET_GRAPHICS_ITEM_H

#include <QGraphicsObject>

#include <talipot/config.h>

namespace tlp {
class GlWidget;

class TLP_QT_SCOPE GlWidgetGraphicsItem : public QGraphicsObject {
  Q_OBJECT
public:
  GlWidgetGraphicsItem(tlp::GlWidget *glWidget, int width, int height);
  ~GlWidgetGraphicsItem() override;

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

  QRectF boundingRect() const override;

  void resize(int width, int height);

  void setRedrawNeeded(bool redrawNeeded) {
    this->_redrawNeeded = redrawNeeded;
  }

  tlp::GlWidget *glWidget() {
    return _glWidget;
  }

  void setGlWidget(tlp::GlWidget *);

  bool eventFilter(QObject *, QEvent *evt) override;

signals:

  void widgetPainted(bool redraw);

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void wheelEvent(QGraphicsSceneWheelEvent *event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
  void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
  void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
  void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
  void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
  void dropEvent(QGraphicsSceneDragDropEvent *event) override;

protected slots:
  void glWidgetDraw(GlWidget *, bool);
  void glWidgetRedraw(GlWidget *);

private:
  tlp::GlWidget *_glWidget;
  bool _redrawNeeded;
  bool _graphChanged;
  int width, height;
};
}

#endif // TALIPOT_GL_MAIN_WIDGET_GRAPHICS_ITEM_H
