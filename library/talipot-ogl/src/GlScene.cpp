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

#include <talipot/OpenGlConfigManager.h>
#include <talipot/GlXMLTools.h>
#include <talipot/GlCPULODCalculator.h>
#include <talipot/GlBoundingBoxSceneVisitor.h>
#include <talipot/GlGraph.h>
#include <talipot/GlSceneObserver.h>

using namespace std;

namespace tlp {

/** \brief Storage class for Z ordering
 * Storage class for Z ordering
 */
struct EntityWithDistance {

  EntityWithDistance(double dist, const EntityLODUnit *entity)
      : distance(dist), entity(entity), isComplexEntity(false), isNode(true) {}
  EntityWithDistance(double dist, const GraphElementLODUnit *entity, bool isNode)
      : distance(dist), entity(entity), isComplexEntity(true), isNode(isNode) {}

  double distance;
  const LODUnit *entity;
  bool isComplexEntity;
  bool isNode;
};

/** \brief Comparator to order entities with Z
 * Comparator to order entities with Z
 */
struct entityWithDistanceCompare {
  static const GlGraphInputData *inputData;
  bool operator()(const EntityWithDistance &e1, const EntityWithDistance &e2) const {
    if (e1.distance > e2.distance) {
      return true;
    }

    if (e1.distance < e2.distance) {
      return false;
    }

    const BoundingBox &bb1 = e1.entity->boundingBox;
    const BoundingBox &bb2 = e2.entity->boundingBox;

    return bb1[1][0] - bb1[0][0] <= bb2[1][0] - bb2[0][0];
  }
};

//====================================================

GlScene::GlScene(GlLODCalculator *calculator)
    : backgroundColor(255, 255, 255, 255), viewOrtho(true), glGraph(nullptr), graphLayer(nullptr),
      clearBufferAtDraw(true), inDraw(false), clearDepthBufferAtDraw(true),
      clearStencilBufferAtDraw(true) {

  if (calculator != nullptr) {
    lodCalculator = calculator;
  } else {
    lodCalculator = new GlCPULODCalculator();
  }

  lodCalculator->setScene(*this);
}

GlScene::~GlScene() {
  delete lodCalculator;

  for (const auto &it : layersList) {
    delete it.second;
  }
}

void GlScene::initGlParameters() {
  OpenGlConfigManager::initExtensions();

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
  glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glLineWidth(1.0);
  glPointSize(1.0);
  glEnable(GL_SCISSOR_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
  glClearStencil(0xFFFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);
  glPolygonMode(GL_FRONT, GL_FILL);
  glColorMask(1, 1, 1, 1);
  glIndexMask(UINT_MAX);

  if (OpenGlConfigManager::antiAliasing()) {
    OpenGlConfigManager::activateAntiAliasing();
  } else {
    OpenGlConfigManager::deactivateAntiAliasing();
  }

  if (clearBufferAtDraw) {
    glClearColor(backgroundColor.getRGL(), backgroundColor.getGGL(), backgroundColor.getBGL(),
                 backgroundColor.getAGL());
    glClear(GL_COLOR_BUFFER_BIT);
  }

  if (clearDepthBufferAtDraw) {
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  if (clearStencilBufferAtDraw) {
    glClear(GL_STENCIL_BUFFER_BIT);
  }

  glDisable(GL_TEXTURE_2D);
}

void GlScene::draw() {

  assert(inDraw == false);

  inDraw = true;

  initGlParameters();

  /**********************************************************************
  LOD Compute
  **********************************************************************/
  lodCalculator->clear();
  lodCalculator->setRenderingEntitiesFlag(RenderingAll);

  /**
   * If LOD Calculator need entities to compute LOD, we use visitor system
   */
  if (lodCalculator->needEntities()) {
    for (const auto &it : layersList) {
      it.second->acceptVisitor(lodCalculator);
    }
  }

  lodCalculator->compute(viewport, viewport);
  LayersLODVector &layersLODVector = lodCalculator->getResult();
  BoundingBox sceneBoundingBox = lodCalculator->getSceneBoundingBox();

  Camera *camera;
  // Iterate on Camera
  Camera *oldCamera = nullptr;
  for (const auto &itLayer : layersLODVector) {
    camera = itLayer.camera;
    camera->setSceneRadius(camera->getSceneRadius(), sceneBoundingBox);

    if (camera != oldCamera) {
      camera->initGl();
      oldCamera = camera;
    }

    // Draw simple entities
    if (getGlGraph() && !getGlGraph()->getInputData()->parameters->isElementZOrdered()) {
      for (const auto &it : itLayer.entitiesLODVector) {
        if (it.lod < 0) {
          continue;
        }

        glStencilFunc(GL_LEQUAL, it.entity->getStencil(), 0xFFFF);
        it.entity->draw(it.lod, camera);
      }
    } else {

      entityWithDistanceCompare::inputData = glGraph->getInputData();
      multiset<EntityWithDistance, entityWithDistanceCompare> entitiesSet;
      Coord camPos = camera->getEyes();
      BoundingBox bb;
      double dist;

      for (const auto &it : itLayer.entitiesLODVector) {
        if (it.lod < 0) {
          continue;
        }

        bb = it.boundingBox;
        Coord middle = (bb[1] + bb[0]) / 2.f;
        dist = (double(middle[0]) - double(camPos[0])) * (double(middle[0]) - double(camPos[0]));
        dist += (double(middle[1]) - double(camPos[1])) * (double(middle[1]) - double(camPos[1]));
        dist += (double(middle[2]) - double(camPos[2])) * (double(middle[2]) - double(camPos[2]));
        entitiesSet.insert(EntityWithDistance(dist, &it));
      }

      for (const auto &it : entitiesSet) {
        // Simple entities
        GlEntity *entity = static_cast<const EntityLODUnit *>(it.entity)->entity;
        glStencilFunc(GL_LEQUAL, entity->getStencil(), 0xFFFF);
        entity->draw(it.entity->lod, camera);
      }
    }
  }

  inDraw = false;

  OpenGlConfigManager::deactivateAntiAliasing();
}

/******************************************************************************
 * GlLayer management functions
 *******************************************************************************/

GlLayer *GlScene::createLayer(const std::string &name) {
  GlLayer *oldLayer = getLayer(name);

  if (oldLayer != nullptr) {
    tlp::warning()
        << "Warning : You have a layer in the scene with same name : old layer will be deleted"
        << endl;
    removeLayer(oldLayer);
  }

  auto *newLayer = new GlLayer(name);
  layersList.push_back(std::pair<std::string, GlLayer *>(name, newLayer));
  newLayer->setScene(this);

  if (hasOnlookers()) {
    sendEvent(GlSceneEvent(*this, GlSceneEvent::TLP_ADDLAYER, name, newLayer));
  }

  return newLayer;
}

GlLayer *GlScene::createLayerBefore(const std::string &layerName,
                                    const std::string &beforeLayerWithName) {
  GlLayer *newLayer = nullptr;
  GlLayer *oldLayer = getLayer(layerName);

  for (auto it = layersList.begin(); it != layersList.end(); ++it) {
    if (it->first == beforeLayerWithName) {
      newLayer = new GlLayer(layerName);
      layersList.insert(it, pair<string, GlLayer *>(layerName, newLayer));
      newLayer->setScene(this);

      if (hasOnlookers()) {
        sendEvent(GlSceneEvent(*this, GlSceneEvent::TLP_ADDLAYER, layerName, newLayer));
      }

      if (oldLayer != nullptr) {
        removeLayer(oldLayer);
        tlp::warning()
            << "Warning : You have a layer in the scene with same name : old layer will be deleted"
            << endl;
      }

      break;
    }
  }

  return newLayer;
}

GlLayer *GlScene::createLayerAfter(const std::string &layerName,
                                   const std::string &afterLayerWithName) {
  GlLayer *newLayer = nullptr;
  GlLayer *oldLayer = getLayer(layerName);

  for (auto it = layersList.begin(); it != layersList.end(); ++it) {
    if (it->first == afterLayerWithName) {
      ++it;
      newLayer = new GlLayer(layerName);
      layersList.insert(it, pair<string, GlLayer *>(layerName, newLayer));
      newLayer->setScene(this);

      if (hasOnlookers()) {
        sendEvent(GlSceneEvent(*this, GlSceneEvent::TLP_ADDLAYER, layerName, newLayer));
      }

      if (oldLayer != nullptr) {
        tlp::warning()
            << "Warning : You have a layer in the scene with same name : old layer will be deleted"
            << endl;
        removeLayer(oldLayer);
      }

      break;
    }
  }

  return newLayer;
}

void GlScene::addExistingLayer(GlLayer *layer) {
  GlLayer *oldLayer = getLayer(layer->getName());

  if (oldLayer != nullptr) {
    tlp::warning()
        << "Warning : You have a layer in the scene with same name : old layer will be deleted"
        << endl;
    removeLayer(oldLayer);
  }

  layersList.push_back(std::pair<std::string, GlLayer *>(layer->getName(), layer));
  layer->setScene(this);

  if (hasOnlookers()) {
    sendEvent(GlSceneEvent(*this, GlSceneEvent::TLP_ADDLAYER, layer->getName(), layer));
  }
}

bool GlScene::addExistingLayerBefore(GlLayer *layer, const std::string &beforeLayerWithName) {
  bool insertionOk = false;
  GlLayer *oldLayer = getLayer(layer->getName());

  for (auto it = layersList.begin(); it != layersList.end(); ++it) {
    if (it->first == beforeLayerWithName) {
      layersList.insert(it, pair<string, GlLayer *>(layer->getName(), layer));
      layer->setScene(this);

      if (hasOnlookers()) {
        sendEvent(GlSceneEvent(*this, GlSceneEvent::TLP_ADDLAYER, layer->getName(), layer));
      }

      if (oldLayer != nullptr) {
        tlp::warning()
            << "Warning : You have a layer in the scene with same name : old layer will be deleted"
            << endl;
        removeLayer(oldLayer);
      }

      insertionOk = true;
      break;
    }
  }

  return insertionOk;
}

bool GlScene::addExistingLayerAfter(GlLayer *layer, const std::string &afterLayerWithName) {
  bool insertionOk = false;
  GlLayer *oldLayer = getLayer(layer->getName());

  for (auto it = layersList.begin(); it != layersList.end(); ++it) {
    if (it->first == afterLayerWithName) {
      ++it;
      layersList.insert(it, pair<string, GlLayer *>(layer->getName(), layer));
      layer->setScene(this);

      if (hasOnlookers()) {
        sendEvent(GlSceneEvent(*this, GlSceneEvent::TLP_ADDLAYER, layer->getName(), layer));
      }

      if (oldLayer != nullptr) {
        tlp::warning()
            << "Warning : You have a layer in the scene with same name : old layer will be deleted"
            << endl;
        removeLayer(oldLayer);
      }

      insertionOk = true;
      break;
    }
  }

  return insertionOk;
}

GlLayer *GlScene::getLayer(const std::string &name) {
  for (const auto &it : layersList) {
    if (it.first == name) {
      return it.second;
    }
  }

  return nullptr;
}

void GlScene::removeLayer(const std::string &name, bool deleteLayer) {
  for (auto it = layersList.begin(); it != layersList.end(); ++it) {
    if (it->first == name) {
      if (hasOnlookers()) {
        sendEvent(GlSceneEvent(*this, GlSceneEvent::TLP_DELLAYER, name, it->second));
      }

      if (deleteLayer) {
        delete it->second;
      } else {
        it->second->setScene(nullptr);
      }

      layersList.erase(it);
      return;
    }
  }
}

void GlScene::removeLayer(GlLayer *layer, bool deleteLayer) {
  for (auto it = layersList.begin(); it != layersList.end(); ++it) {
    if (it->second == layer) {
      if (hasOnlookers()) {
        sendEvent(GlSceneEvent(*this, GlSceneEvent::TLP_DELLAYER, layer->getName(), layer));
      }

      if (deleteLayer) {
        delete it->second;
      } else {
        it->second->setScene(nullptr);
      }

      layersList.erase(it);
      return;
    }
  }
}

void GlScene::notifyModifyLayer(const std::string &name, GlLayer *layer) {
  if (hasOnlookers()) {
    sendEvent(GlSceneEvent(*this, GlSceneEvent::TLP_MODIFYLAYER, name, layer));
  }
}

void GlScene::notifyModifyEntity(GlEntity *entity) {
  if (hasOnlookers()) {
    sendEvent(GlSceneEvent(*this, GlSceneEvent::TLP_MODIFYENTITY, entity));
  }
}

void GlScene::notifyDeletedEntity(GlEntity *entity) {
  if (hasOnlookers()) {
    sendEvent(GlSceneEvent(*this, GlSceneEvent::TLP_DELENTITY, entity));
  }
}

void GlScene::centerScene() {
  adjustSceneToSize(viewport[2], viewport[3]);
}

void GlScene::computeAdjustSceneToSize(int width, int height, Coord *center, Coord *eye,
                                       float *sceneRadius, float *xWhiteFactor, float *yWhiteFactor,
                                       BoundingBox *sceneBoundingBox, float *zoomFactor) {
  if (xWhiteFactor) {
    *xWhiteFactor = 0.;
  }

  if (yWhiteFactor) {
    *yWhiteFactor = 0.;
  }

  GlBoundingBoxSceneVisitor *visitor;

  if (glGraph) {
    visitor = new GlBoundingBoxSceneVisitor(glGraph->getInputData());
  } else {
    visitor = new GlBoundingBoxSceneVisitor(nullptr);
  }

  for (const auto &it : layersList) {
    if (it.second->getCamera().is3D() && (!it.second->useSharedCamera())) {
      it.second->acceptVisitor(visitor);
    }
  }

  BoundingBox boundingBox = visitor->getBoundingBox();
  delete visitor;

  if (!boundingBox.isValid()) {
    if (center) {
      *center = Coord(0, 0, 0);
    }

    if (sceneRadius) {
      *sceneRadius = float(sqrt(300.0));
    }

    if (eye && center && sceneRadius) {
      *eye = Coord(0, 0, *sceneRadius);
      *eye = *eye + *center;
    }

    if (zoomFactor) {
      *zoomFactor = 1.;
    }

    return;
  }

  Coord maxC = boundingBox[1];
  Coord minC = boundingBox[0];

  double dx = maxC[0] - minC[0];
  double dy = maxC[1] - minC[1];
  double dz = maxC[2] - minC[2];

  double dxZoomed = (maxC[0] - minC[0]);
  double dyZoomed = (maxC[1] - minC[1]);

  if (center) {
    *center = (maxC + minC) / 2.f;
  }

  if ((dx == 0) && (dy == 0) && (dz == 0)) {
    dx = dy = /*dz =*/10.0;
  }

  double wdx = width / dxZoomed;
  double hdy = height / dyZoomed;

  float sceneRadiusTmp;

  if (dx < dy) {
    if (wdx < hdy) {
      sceneRadiusTmp = float(dx);

      if (yWhiteFactor) {
        *yWhiteFactor = float((1. - (dy / (sceneRadiusTmp * (height / width)))) / 2.);
      }
    } else {
      if (width < height) {
        sceneRadiusTmp = float(dx * wdx / hdy);
      } else {
        sceneRadiusTmp = float(dy);
      }

      if (xWhiteFactor) {
        *xWhiteFactor = float((1. - (dx / sceneRadiusTmp)) / 2.);
      }
    }
  } else {
    if (wdx > hdy) {
      sceneRadiusTmp = float(dy);

      if (xWhiteFactor) {
        *xWhiteFactor = float((1. - (dx / (sceneRadiusTmp * (width / height)))) / 2.);
      }
    } else {
      if (height < width) {
        sceneRadiusTmp = float(dy * hdy / wdx);
      } else {
        sceneRadiusTmp = float(dx);
      }

      if (yWhiteFactor) {
        *yWhiteFactor = float((1. - (dy / sceneRadiusTmp)) / 2.);
      }
    }
  }

  if (sceneRadius) {
    *sceneRadius = sceneRadiusTmp;
  }

  if (eye) {
    *eye = Coord(0, 0, sceneRadiusTmp);
    Coord centerTmp = (maxC + minC) / 2.f;
    *eye = *eye + centerTmp;
  }

  if (sceneBoundingBox) {
    *sceneBoundingBox = boundingBox;
  }

  if (zoomFactor) {
    *zoomFactor = 1.;
  }
}

void GlScene::adjustSceneToSize(int width, int height) {

  Coord center;
  Coord eye;
  float sceneRadius;
  float zoomFactor;
  BoundingBox sceneBoundingBox;

  computeAdjustSceneToSize(width, height, &center, &eye, &sceneRadius, nullptr, nullptr,
                           &sceneBoundingBox, &zoomFactor);

  for (const auto &it : layersList) {
    Camera &camera = it.second->getCamera();
    camera.setCenter(center);

    camera.setSceneRadius(sceneRadius, sceneBoundingBox);

    camera.setEyes(eye);
    camera.setUp(Coord(0, 1., 0));
    camera.setZoomFactor(zoomFactor);
  }
}

void GlScene::zoomXY(int step, const int x, const int y) {
  zoom(step);

  if (step < 0) {
    step *= -1;
  }

  int factX = int(step * (double(viewport[2]) / 2.0 - x) / 7.0);
  int factY = int(step * (double(viewport[3]) / 2.0 - y) / 7.0);
  translateCamera(factX, -factY, 0);
}

void GlScene::zoom(float, const Coord &dest) {
  for (const auto &it : layersList) {
    if (it.second->getCamera().is3D() && (!it.second->useSharedCamera())) {
      it.second->getCamera().setEyes(
          dest + (it.second->getCamera().getEyes() - it.second->getCamera().getCenter()));
      it.second->getCamera().setCenter(dest);
    }
  }
}

void GlScene::translateCamera(const int x, const int y, const int z) {
  for (const auto &it : layersList) {
    if (it.second->getCamera().is3D() && (!it.second->useSharedCamera())) {
      Coord v1;
      Coord v2 = Coord(x, y, z);
      v1 = it.second->getCamera().viewportTo3DWorld(v1);
      v2 = it.second->getCamera().viewportTo3DWorld(v2);
      Coord move = v2 - v1;
      it.second->getCamera().setEyes(it.second->getCamera().getEyes() + move);
      it.second->getCamera().setCenter(it.second->getCamera().getCenter() + move);
    }
  }
}

void GlScene::zoomFactor(float factor) {
  for (const auto &it : layersList) {
    if (it.second->getCamera().is3D() && (!it.second->useSharedCamera())) {
      it.second->getCamera().setZoomFactor(it.second->getCamera().getZoomFactor() * factor);
    }
  }
}

void GlScene::rotateCamera(const int x, const int y, const int z) {
  for (const auto &it : layersList) {
    if (it.second->getCamera().is3D() && (!it.second->useSharedCamera())) {
      it.second->getCamera().rotate(float(x / 360.0 * M_PI), 1.0f, 0, 0);
      it.second->getCamera().rotate(float(y / 360.0 * M_PI), 0, 1.0f, 0);
      it.second->getCamera().rotate(float(z / 360.0 * M_PI), 0, 0, 1.0f);
    }
  }
}
//========================================================================================================
void GlScene::glGraphAdded(GlLayer *layer, GlGraph *glGraph) {
  this->graphLayer = layer;
  this->glGraph = glGraph;
}
//========================================================================================================
void GlScene::glGraphRemoved(GlLayer *layer, GlGraph *glGraph) {
  if (this->glGraph == glGraph) {
    // fixes unused warning in release
    (void)layer;
    assert(graphLayer == layer);
    graphLayer = nullptr;
    this->glGraph = nullptr;
  }
}

// original gluPickMatrix code from Mesa
static void pickMatrix(GLdouble x, GLdouble y, GLdouble width, GLdouble height,
                       const Vec4i &viewport) {
  GLfloat m[16];
  GLfloat sx, sy;
  GLfloat tx, ty;

  sx = viewport[2] / width;
  sy = viewport[3] / height;
  tx = (viewport[2] + 2.0 * (viewport[0] - x)) / width;
  ty = (viewport[3] + 2.0 * (viewport[1] - y)) / height;

#define M(row, col) m[col * 4 + row]
  M(0, 0) = sx;
  M(0, 1) = 0.0;
  M(0, 2) = 0.0;
  M(0, 3) = tx;
  M(1, 0) = 0.0;
  M(1, 1) = sy;
  M(1, 2) = 0.0;
  M(1, 3) = ty;
  M(2, 0) = 0.0;
  M(2, 1) = 0.0;
  M(2, 2) = 1.0;
  M(2, 3) = 0.0;
  M(3, 0) = 0.0;
  M(3, 1) = 0.0;
  M(3, 2) = 0.0;
  M(3, 3) = 1.0;
#undef M

  glMultMatrixf(m);
}

//========================================================================================================
bool GlScene::selectEntities(RenderingEntitiesFlag type, int x, int y, int w, int h, GlLayer *layer,
                             vector<SelectedEntity> &selectedEntities) {
  if (w == 0) {
    w = 1;
  }

  if (h == 0) {
    h = 1;
  }

  // check if the layer is in the scene
  bool layerInScene = true;

  if (layer) {
    layerInScene = false;

    for (const auto &it : layersList) {
      if (it.second == layer) {
        layerInScene = true;
      }
    }
  }

  GlLODCalculator *selectLODCalculator = layerInScene ? lodCalculator : lodCalculator->clone();

  selectLODCalculator->setRenderingEntitiesFlag(
      static_cast<RenderingEntitiesFlag>(RenderingAll | RenderingWithoutRemove));
  selectLODCalculator->clear();

  // Collect entities if needed
  if (layerInScene) {
    if (selectLODCalculator->needEntities()) {
      for (const auto &it : layersList) {
        it.second->acceptVisitor(selectLODCalculator);
      }
    }
  } else {
    layer->acceptVisitor(selectLODCalculator);
  }

  Vec4i selectionViewport;
  selectionViewport[0] = x;
  selectionViewport[1] = y;
  selectionViewport[2] = w;
  selectionViewport[3] = h;

  glViewport(selectionViewport[0], selectionViewport[1], selectionViewport[2],
             selectionViewport[3]);

  selectLODCalculator->compute(viewport, selectionViewport);

  LayersLODVector &layersLODVector = selectLODCalculator->getResult();

  for (const auto &itLayer : layersLODVector) {
    Camera *camera = itLayer.camera;

    vector<GlGraph *> compositesToRender;

    const Vec4i &viewport = camera->getViewport();

    uint size = itLayer.entitiesLODVector.size();

    if (size == 0) {
      continue;
    }

    glPushAttrib(GL_ALL_ATTRIB_BITS);              // save previous attributes
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS); // save previous attributes

    // Allocate memory to store the result of the selection
    auto *selectBuf = new GLuint[size][4];
    glSelectBuffer(size * 4, reinterpret_cast<GLuint *>(selectBuf));
    // Activate Open Gl Selection mode
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(0);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); // save previous projection matrix

    // initialize picking matrix
    glLoadIdentity();
    int newX = x + w / 2;
    int newY = viewport[3] - (y + h / 2);
    pickMatrix(newX, newY, w, h, viewport);

    camera->initProjection(false);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); // save previous model view matrix

    camera->initModelView();

    glPolygonMode(GL_FRONT, GL_FILL);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);

    unordered_map<uint, SelectedEntity> idToEntity;

    if (type & RenderingEntities) {
      uint id = 1;

      for (const auto &it : itLayer.entitiesLODVector) {
        if (it.lod < 0) {
          continue;
        }

        idToEntity[id] = SelectedEntity(it.entity);
        glLoadName(id);
        ++id;
        it.entity->draw(20., camera);
      }
    }

    if ((type & RenderingNodes) || (type & RenderingEdges)) {
      for (const auto &it : itLayer.entitiesLODVector) {
        if (it.lod < 0) {
          continue;
        }

        auto *composite = dynamic_cast<GlGraph *>(it.entity);

        if (composite) {
          compositesToRender.push_back(composite);
        }
      }
    }

    glFlush();
    GLint hits = glRenderMode(GL_RENDER);

    while (hits > 0) {
      selectedEntities.push_back(idToEntity[selectBuf[hits - 1][3]]);
      hits--;
    }

    delete[] selectBuf;

    for (auto *glGraph : compositesToRender) {
      glGraph->selectEntities(camera, type, x, y, w, h, selectedEntities);
    }

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glPopClientAttrib();
    glPopAttrib();
  }

  selectLODCalculator->clear();

  if (selectLODCalculator != lodCalculator) {
    delete selectLODCalculator;
  }

  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
  return (!selectedEntities.empty());
}
//====================================================
unsigned char *GlScene::getImage() {
  auto *image =
      static_cast<unsigned char *>(malloc(viewport[2] * viewport[3] * 3 * sizeof(unsigned char)));
  draw();
  glFlush();
  glFinish();
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], GL_RGB, GL_UNSIGNED_BYTE, image);
  return image;
}
//====================================================
void GlScene::getXML(string &out) {

  out.append("<scene>");

  GlXMLTools::beginDataNode(out);

  GlXMLTools::getXML(out, "viewport", viewport);
  GlXMLTools::getXML(out, "background", backgroundColor);

  GlXMLTools::endDataNode(out);

  GlXMLTools::beginChildNode(out);

  for (const auto &it : layersList) {

    // Don't save working layers
    if (it.second->isAWorkingLayer()) {
      continue;
    }

    GlXMLTools::beginChildNode(out, "GlLayer");

    GlXMLTools::createProperty(out, "name", it.first);

    it.second->getXML(out);

    GlXMLTools::endChildNode(out, "GlLayer");
  }

  GlXMLTools::endChildNode(out);

  out.append("</scene>");
}
//====================================================
void GlScene::getXMLOnlyForCameras(string &out) {

  out.append("<scene>");

  GlXMLTools::beginDataNode(out);

  GlXMLTools::getXML(out, "viewport", viewport);
  GlXMLTools::getXML(out, "background", backgroundColor);

  GlXMLTools::endDataNode(out);

  GlXMLTools::beginChildNode(out);

  for (const auto &it : layersList) {

    // Don't save working layers
    if (it.second->isAWorkingLayer()) {
      continue;
    }

    GlXMLTools::beginChildNode(out, "GlLayer");

    GlXMLTools::createProperty(out, "name", it.first);

    it.second->getXMLOnlyForCameras(out);

    GlXMLTools::endChildNode(out, "GlLayer");
  }

  GlXMLTools::endChildNode(out);

  out.append("</scene>");
}
//====================================================
void GlScene::setWithXML(string &in, Graph *graph) {

  if (graph) {
    glGraph = new GlGraph(graph);
  }

  assert(in.substr(0, 7) == "<scene>");
  uint currentPosition = 7;
  GlXMLTools::enterDataNode(in, currentPosition);
  GlXMLTools::setWithXML(in, currentPosition, "viewport", viewport);
  GlXMLTools::setWithXML(in, currentPosition, "background", backgroundColor);
  GlXMLTools::leaveDataNode(in, currentPosition);

  GlXMLTools::enterChildNode(in, currentPosition);
  string childName = GlXMLTools::enterChildNode(in, currentPosition);

  while (!childName.empty()) {
    assert(childName == "GlLayer");

    map<string, string> properties = GlXMLTools::getProperties(in, currentPosition);

    assert(properties.count("name") != 0);

    GlLayer *newLayer = getLayer(properties["name"]);

    if (!newLayer) {
      newLayer = createLayer(properties["name"]);
    }

    newLayer->setWithXML(in, currentPosition);

    GlXMLTools::leaveChildNode(in, currentPosition, "GlLayer");

    childName = GlXMLTools::enterChildNode(in, currentPosition);
  }

  if (graph) {
    getLayer("Main")->addGlEntity(glGraph, "graph");
  }
}

BoundingBox GlScene::getBoundingBox() {
  return lodCalculator->getSceneBoundingBox();
}
}
