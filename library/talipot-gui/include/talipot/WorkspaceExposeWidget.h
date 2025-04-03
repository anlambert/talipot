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

#ifndef TALIPOT_WORKSPACE_EXPOSE_WIDGET_H
#define TALIPOT_WORKSPACE_EXPOSE_WIDGET_H

#include <QGraphicsObject>
#include <QGraphicsView>

#include <talipot/config.h>

#include <memory>

class QAbstractAnimation;

namespace tlp {
class WorkspacePanel;

class PreviewItem : public QGraphicsObject {
    Q_OBJECT

    static std::unique_ptr<QPixmap> _closeButtonPixmap;
    static QRect _closePixmapRect;

    QPixmap _pixmap;
    WorkspacePanel *_panel;
    bool _hovered;
    bool _closeButtonHovered;

    int textHeight() const;

  public:
    explicit PreviewItem(const QPixmap &pixmap, WorkspacePanel *panel,
                         QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;
    WorkspacePanel *panel() const;
    bool shouldClose(const QPointF &);

  protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *) override;
  signals:
    void opened();
};

class TLP_QT_SCOPE WorkspaceExposeWidget : public QGraphicsView {
    Q_OBJECT

    QAbstractAnimation *_positionAnimation;
    QList<PreviewItem *> _items;
    PreviewItem *_selectedItem;
    QGraphicsRectItem *_placeholderItem;
    bool _switchToSingleMode;

    int _currentPanelIndex;

    void updatePositions(bool resetScenePos = true);
    static const int MARGIN;

    void finish();

  public:
    static QSize previewSize();

    explicit WorkspaceExposeWidget(QWidget *parent = nullptr);
    ~WorkspaceExposeWidget() override;
    int currentPanelIndex() const;
    QVector<WorkspacePanel *> panels() const;

    bool isSwitchToSingleMode() const;

  public slots:
    void setData(const QVector<WorkspacePanel *> &panels, int currentPanelIndex);

  signals:
    void exposeFinished();

  protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *, QEvent *) override;
    bool event(QEvent *) override;

  protected slots:
    void updatePositionsAnimationFinished();
    void resetSceneRect();
    void itemOpened();
};
}

#endif // TALIPOT_WORKSPACE_EXPOSE_WIDGET_H
