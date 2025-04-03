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

#ifndef TALIPOT_GL_OVERVIEW_GRAPHICS_ITEM_H
#define TALIPOT_GL_OVERVIEW_GRAPHICS_ITEM_H

#include <talipot/config.h>
#include <talipot/Camera.h>
#include <talipot/Color.h>

#include <QGraphicsPixmapItem>
#include <QGraphicsPathItem>

#include <set>

namespace tlp {

class GlView;
class GlScene;

class TLP_QT_SCOPE GlOverviewGraphicsItem : public QObject, public QGraphicsRectItem {

    Q_OBJECT

  public:
    GlOverviewGraphicsItem(GlView *view, GlScene &scene);
    ~GlOverviewGraphicsItem() override;

    void setSize(uint width, uint height);
    uint getWidth() const {
        return width;
    }
    uint getHeight() const {
        return height;
    }

    void setFrameColor(const Color &color) {
        _frameColor = color;
    }

    void setFrameWidth(int width) {
        _frameWidth = width;
    }

    void setLayerVisible(const std::string &name, bool visible);

  public slots:

    void draw(bool generatePixmap);

  private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void setScenePosition(QPointF pos);

    GlView *view;
    GlScene &baseScene;
    uint width, height;

    QGraphicsPixmapItem overview;
    QGraphicsPathItem overviewBorder;
    QGraphicsLineItem line[8];
    QGraphicsPolygonItem poly[4];

    bool mouseClicked;

    std::set<std::string> _hiddenLayers;

    std::vector<Camera> _oldCameras;

    Color _frameColor;
    int _frameWidth;
};
}

#endif // TALIPOT_GL_OVERVIEW_GRAPHICS_ITEM_H
