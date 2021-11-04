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

#include "talipot/GlOverviewGraphicsItem.h"

#include <QGraphicsSceneMouseEvent>
#include <QPen>

#include <talipot/GlOffscreenRenderer.h>
#include <talipot/GlView.h>
#include <talipot/GlGraph.h>
#include <talipot/GlOffscreenRenderer.h>

using namespace std;

namespace tlp {

GlOverviewGraphicsItem::GlOverviewGraphicsItem(GlView *view, GlScene &scene)
    : QGraphicsRectItem(0, 0, 128, 128), view(view), baseScene(scene), width(128), height(128),
      mouseClicked(false), _frameColor(Color::Gray), _frameWidth(2) {}

GlOverviewGraphicsItem::~GlOverviewGraphicsItem() {
  overview.setParentItem(nullptr);
}

void GlOverviewGraphicsItem::setSize(uint width, uint height) {
  this->width = width;
  this->height = height;
  draw(true);
}

void GlOverviewGraphicsItem::setLayerVisible(const string &name, bool visible) {
  if (!visible) {
    _hiddenLayers.insert(name);
  } else {
    _hiddenLayers.erase(name);
  }
}

void GlOverviewGraphicsItem::draw(bool generatePixmap) {

  if (baseScene.getLayersList().empty()) {
    return;
  }

  if (_frameWidth % 2 == 1) {
    ++_frameWidth;
  }

  if (!overviewBorder.parentItem()) {

    // This flag is needed to not display overview rectangle outside overview
    setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    overview.setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    overview.setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setBrush(QBrush(QColor(255, 255, 255, 255)));

    QPainterPath path;
    path.addRect(_frameWidth / 2, _frameWidth / 2, width - _frameWidth, height - _frameWidth);
    overviewBorder.setPath(path);
    overviewBorder.setParentItem(this);

    overview.setParentItem(&overviewBorder);

    // Init lines and polygons item
    for (uint i = 0; i < 8; ++i) {
      line[i].setParentItem(&overview);

      if (i < 4) {
        poly[i].setParentItem(&overview);
        poly[i].setBrush(QBrush(QColor(0, 0, 0, 64)));
        poly[i].setPen(Qt::NoPen);
      }
    }
  }

  Color backgroundColor = baseScene.getBackgroundColor();
  int bgV = backgroundColor.getV();

  for (uint i = 0; i < 8; ++i) {
    if (bgV < 128) {
      line[i].setPen(QColor(255, 255, 255));
    } else {
      line[i].setPen(QColor(0, 0, 0));
    }

    if (i < 4) {
      if (bgV < 128) {
        poly[i].setBrush(QBrush(QColor(255, 255, 255, 64)));
      } else {
        poly[i].setBrush(QBrush(QColor(0, 0, 0, 64)));
      }
    }
  }

  // Backup initial viewport
  Vec4i backupViewport = baseScene.getViewport();

  // Backup initial cameras
  vector<Camera> cameras;
  const auto &layerList = baseScene.getLayersList();

  for (const auto &it : layerList) {
    cameras.push_back(it.second->getCamera());
  }

  // Compute visible part of the scene
  Camera &baseCamera = baseScene.graphCamera();

  vector<Coord> cameraBoundingBox;
  cameraBoundingBox.push_back(
      baseCamera.viewportTo3DWorld(Coord(backupViewport[0], backupViewport[1], 0)));
  cameraBoundingBox.push_back(baseCamera.viewportTo3DWorld(
      Coord(backupViewport[0] + backupViewport[2], backupViewport[1], 0)));
  cameraBoundingBox.push_back(baseCamera.viewportTo3DWorld(
      Coord(backupViewport[0] + backupViewport[2], backupViewport[1] + backupViewport[3], 0)));
  cameraBoundingBox.push_back(baseCamera.viewportTo3DWorld(
      Coord(backupViewport[0], backupViewport[1] + backupViewport[3], 0)));

  // This code modify cameraBoundingBox coords to have coords with (x,y,0)
  // If we don't do this we will have invalid polygon when we do worldTo2DViewport transformations
  Coord eyesVector = baseCamera.getEyes() - baseCamera.getCenter();

  eyesVector = eyesVector * (1.f / eyesVector[2]);

  for (uint i = 0; i < 4; i++) {
    cameraBoundingBox[i] = cameraBoundingBox[i] - eyesVector * cameraBoundingBox[i][2];
  }

  // Change viewport of the scene to the overview viewport
  baseScene.setViewport(0, 0, width, height);

  if (generatePixmap || _oldCameras.size() != layerList.size()) {
    // Center the scene
    baseScene.centerScene();
    _oldCameras.clear();

    for (const auto &it : layerList) {
      _oldCameras.push_back(it.second->getCamera());
    }
  } else {
    uint i = 0;

    for (const auto &it : layerList) {
      it.second->getCamera() = _oldCameras[i];
      ++i;
    }
  }

  // Project camera bounding box
  Camera &overviewCamera = baseScene.graphCamera();
  Coord p0 = overviewCamera.worldTo2DViewport(cameraBoundingBox[0]);
  Coord p1 = overviewCamera.worldTo2DViewport(cameraBoundingBox[1]);
  Coord p2 = overviewCamera.worldTo2DViewport(cameraBoundingBox[2]);
  Coord p3 = overviewCamera.worldTo2DViewport(cameraBoundingBox[3]);

  // Rotation of the coordinates to have no crossing lines
  while (p1[0] > p3[0]) {
    Coord tmp = p0;
    p0 = p1;
    p1 = p2;
    p2 = p3;
    p3 = tmp;
  }

  while (p1[1] < p3[1]) {
    Coord tmp = p0;
    p0 = p3;
    p3 = p2;
    p2 = p1;
    p1 = tmp;
  }

  if (generatePixmap) {
    bool edgesLabels = baseScene.glGraph()->renderingParameters().isViewEdgeLabel();
    bool nodesLabels = baseScene.glGraph()->renderingParameters().isViewNodeLabel();
    bool metaNodesLabels = baseScene.glGraph()->renderingParameters().isViewMetaLabel();
    baseScene.glGraph()->renderingParameters().setViewEdgeLabel(false);
    baseScene.glGraph()->renderingParameters().setViewNodeLabel(false);
    baseScene.glGraph()->renderingParameters().setViewMetaLabel(false);

    vector<bool> layersVisibility;

    const auto &layersList = baseScene.getLayersList();

    for (const auto &it : layersList) {
      layersVisibility.push_back(it.second->isVisible());

      if (it.second->isAWorkingLayer()) {
        it.second->setVisible(false);
      }

      if (_hiddenLayers.count(it.first) != 0) {
        it.second->setVisible(false);
      }
    }

    // Draw the scene
    GlOffscreenRenderer::instance().setViewPortSize(width - 2 * _frameWidth,
                                                    height - 2 * _frameWidth);
    GlOffscreenRenderer::instance().renderExternalScene(&baseScene, true);

    auto itTmp = layersVisibility.begin();

    for (const auto &it : layersList) {
      if (*itTmp) {
        it.second->setVisible(true);
      }

      ++itTmp;
    }

    GlGraphRenderingParameters &param = baseScene.glGraph()->renderingParameters();
    param.setViewEdgeLabel(edgesLabels);
    param.setViewNodeLabel(nodesLabels);
    param.setViewMetaLabel(metaNodesLabels);
  }

  // invert applied camera transformations
  uint i = 0;

  for (const auto &it : layerList) {
    it.second->getCamera() = cameras[i];
    ++i;
  }

  // invert applied viewport
  baseScene.setViewport(backupViewport);

  if (generatePixmap) {
    // Load scene pixmap to the item
    QImage img = GlOffscreenRenderer::instance().getImage();
    overview.setPos(_frameWidth, _frameWidth);
    overview.setPixmap(QPixmap::fromImage(img));
  }

  // set lines and polygons coordinates
  line[0].setLine(width - 2 * _frameWidth, 0, p0[0], height - p0[1]);
  line[1].setLine(0, 0, p1[0], height - p1[1]);
  line[2].setLine(0, height - 2 * _frameWidth, p2[0], height - p2[1]);
  line[3].setLine(width - 2 * _frameWidth, height - 2 * _frameWidth, p3[0], height - p3[1]);
  line[4].setLine(p0[0], height - p0[1], p1[0], height - p1[1]);
  line[5].setLine(p1[0], height - p1[1], p2[0], height - p2[1]);
  line[6].setLine(p2[0], height - p2[1], p3[0], height - p3[1]);
  line[7].setLine(p3[0], height - p3[1], p0[0], height - p0[1]);

  QVector<QPointF> tmpVect;
  tmpVect.push_back(QPointF(width - 2 * _frameWidth, 0));
  tmpVect.push_back(QPointF(p0[0], height - p0[1]));
  tmpVect.push_back(QPointF(p1[0], height - p1[1]));
  tmpVect.push_back(QPointF(0, 0));
  poly[0].setPolygon(QPolygonF(tmpVect));
  tmpVect.clear();
  tmpVect.push_back(QPointF(0, 0));
  tmpVect.push_back(QPointF(p1[0], height - p1[1]));
  tmpVect.push_back(QPointF(p2[0], height - p2[1]));
  tmpVect.push_back(QPointF(0, height - 2 * _frameWidth));
  poly[1].setPolygon(QPolygonF(tmpVect));
  tmpVect.clear();
  tmpVect.push_back(QPointF(0, height - 2 * _frameWidth));
  tmpVect.push_back(QPointF(p2[0], height - p2[1]));
  tmpVect.push_back(QPointF(p3[0], height - p3[1]));
  tmpVect.push_back(QPointF(width - 2 * _frameWidth, height - 2 * _frameWidth));
  poly[2].setPolygon(QPolygonF(tmpVect));
  tmpVect.clear();
  tmpVect.push_back(QPointF(width - 2 * _frameWidth, height - 2 * _frameWidth));
  tmpVect.push_back(QPointF(p3[0], height - p3[1]));
  tmpVect.push_back(QPointF(p0[0], height - p0[1]));
  tmpVect.push_back(QPointF(width - 2 * _frameWidth, 0));
  poly[3].setPolygon(QPolygonF(tmpVect));

  QPen pen(QColor(_frameColor[0], _frameColor[1], _frameColor[2], _frameColor[3]));
  pen.setWidth(_frameWidth);
  pen.setJoinStyle(Qt::MiterJoin);

  overviewBorder.setPen(pen);
}

void GlOverviewGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    mouseClicked = true;
    setScenePosition(event->pos());
  }
}

void GlOverviewGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    mouseClicked = false;
  }
}

void GlOverviewGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  if (mouseClicked) {
    setScenePosition(event->pos());
  }
}

void GlOverviewGraphicsItem::setScenePosition(QPointF pos) {

  Coord position = Coord(width + _frameWidth - pos.x(), pos.y() - _frameWidth);

  Vec4i backupViewport = baseScene.getViewport();

  baseScene.setViewport(0, 0, width, height);

  vector<Camera> cameras;
  const auto &layerList = baseScene.getLayersList();

  for (const auto &it : layerList) {
    cameras.push_back(it.second->getCamera());
  }

  baseScene.centerScene();

  vector<Coord> centerPos;

  for (const auto &it : layerList) {
    centerPos.push_back(it.second->getCamera().viewportTo3DWorld(position));
  }

  uint i = 0;

  for (const auto &it : layerList) {
    Coord eyesVector = cameras[i].getEyes() - cameras[i].getCenter();
    Camera &camera = it.second->getCamera();
    camera = cameras[i];
    camera.setCenter(centerPos[i]);
    camera.setEyes(centerPos[i] + eyesVector);
    ++i;
  }

  baseScene.setViewport(backupViewport);

  view->glWidget()->draw(false);
}
}
