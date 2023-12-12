/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/GlQuadTreeLODCalculator.h>

#include <talipot/QuadTree.h>
#include <talipot/GlScene.h>
#include <talipot/GlSceneObserver.h>

using namespace std;

namespace tlp {

BoundingBox computeNewBoundingBox(const BoundingBox &box, const Coord &centerScene, double aX,
                                  double aY) {
  // compute a new bounding box : this bounding box is the rotation of the old bounding box
  Coord size = (box[1] - box[0]) / 2.f;
  Coord center = box[0] + size;
  // size = Coord(size.norm(),size.norm(),size.norm());
  size.fill(size.norm());
  center[0] = centerScene[0] + (cos(aY) * (center[0] - centerScene[0]));
  center[1] = centerScene[1] + (cos(aX) * (center[1] - centerScene[1]));

  return BoundingBox(center - size, center + size);
}

GlQuadTreeLODCalculator::GlQuadTreeLODCalculator()
    : haveToCompute(true), haveToInitObservers(true),
      seBBIndex(2 * ThreadManager::getNumberOfThreads()),
      eBBOffset(ThreadManager::getNumberOfThreads()), currentGraph(nullptr),
      layoutProperty(nullptr), sizeProperty(nullptr), selectionProperty(nullptr) {
  threadSafe = true;
  // we have to deal with
  // ThreadManager::getNumberOfThreads() bounding boxes for nodes
  // ThreadManager::getNumberOfThreads() bounding boxes for edges
  // 1 bounding box for simple entities
  bbs.resize(2 * ThreadManager::getNumberOfThreads() + 1);
}

GlQuadTreeLODCalculator::~GlQuadTreeLODCalculator() {
  setHaveToCompute();
  clearCamerasObservers();

  for (auto *node : nodesQuadTree) {
    delete node;
  }

  for (auto *node : edgesQuadTree) {
    delete node;
  }

  for (auto *node : entitiesQuadTree) {
    delete node;
  }
}

void GlQuadTreeLODCalculator::setScene(GlScene &scene) {
  // If we change scene, we have to remove observer on the graph and the scene
  // in the next rendering, we have to compute quadtree
  setHaveToCompute();
  GlLODCalculator::setScene(scene);
}

void GlQuadTreeLODCalculator::setInputData(const GlGraphInputData *newInputData) {
  setHaveToCompute();

  if (newInputData == nullptr) {
    currentCamera = nullptr;
    currentGraph = nullptr;
    layoutProperty = nullptr;
    sizeProperty = nullptr;
    selectionProperty = nullptr;
  }

  GlLODCalculator::setInputData(newInputData);
}

bool GlQuadTreeLODCalculator::needEntities() {
  if (inputData != nullptr) {
    // Checks if the properties in the GlGraphInputData have changed
    if (layoutProperty != inputData->layout() || sizeProperty != inputData->sizes() ||
        selectionProperty != inputData->selection()) {
      // Remove observers on old properties
      removeObservers();
      // Reinit properties and listen them
      addObservers();
      // Need to recompute
      haveToCompute = true;
      haveToInitObservers = false;
    }
  }

  // Check if quadtree need entities
  if (haveToCompute) {
    if (inputData) {
      oldParameters = *inputData->renderingParameters();
    }

    return true;
  }

  // Check if a camera have changed (diff between old backup camera and current camera)
  for (const auto &it : layerToCamera) {
    if ((it.first->getCamera()).is3D()) {
      Camera camera = it.first->getCamera();
      Camera oldCamera = it.second;
      Coord unitCamera = camera.getEyes() - camera.getCenter();
      unitCamera = unitCamera / unitCamera.norm();
      Coord unitOldCamera = oldCamera.getEyes() - oldCamera.getCenter();
      unitOldCamera = unitOldCamera / unitOldCamera.norm();

      if (unitCamera != unitOldCamera) {
        haveToCompute = true;

        if (inputData) {
          oldParameters = *inputData->renderingParameters();
        }

        return true;
      }
    }
  }

  if (inputData) {
    // Check visibility flags
    GlGraphRenderingParameters *newParameters = inputData->renderingParameters();

    if (oldParameters.isDisplayEdges() != newParameters->isDisplayEdges() ||
        oldParameters.isDisplayMetaNodes() != newParameters->isDisplayMetaNodes() ||
        oldParameters.isDisplayNodes() != newParameters->isDisplayNodes() ||
        oldParameters.isViewNodeLabel() != newParameters->isViewNodeLabel() ||
        oldParameters.isViewEdgeLabel() != newParameters->isViewEdgeLabel() ||
        oldParameters.isViewMetaLabel() != newParameters->isViewMetaLabel()) {
      oldParameters = *inputData->renderingParameters();
      haveToCompute = true;
      return true;
    }
  }

  return false;
}

void GlQuadTreeLODCalculator::setNeedEntities(bool) {
  setHaveToCompute();
}

void GlQuadTreeLODCalculator::addEntityBoundingBox(GlEntity *entity, const BoundingBox &bb) {
  // same check as in GlCPULODCalculator::addEntityBoundingBox
  if (bb[0][0] != numeric_limits<float>::min()) {
    bbs[seBBIndex].expand(bb);
  }
  currentLayerLODUnit->entitiesLODVector.push_back(EntityLODUnit(entity, bb));
}

void GlQuadTreeLODCalculator::addEdgeBoundingBox(Graph *graph, edge e, const BoundingBox &bb) {
  auto ti = eBBOffset + ThreadManager::getThreadNumber();
  bbs[ti].expand(bb);
  currentLayerLODUnit->edgesLODVector[graph->edgePos(e)].init(e.id, bb);
}

void GlQuadTreeLODCalculator::compute(const Vec4i &globalViewport, const Vec4i &currentViewport) {

  if (haveToCompute) {
    // if have to compute : rebuild quadtree

    if (haveToInitObservers) {
      addObservers();
      haveToInitObservers = false;
    }

    clearCamerasObservers();

    // Clear all vectors
    cameras.clear();
    layerToCamera.clear();
    entities.clear();

    for (auto *node : nodesQuadTree) {
      delete node;
    }

    nodesQuadTree.clear();

    for (auto *node : edgesQuadTree) {
      delete node;
    }

    edgesQuadTree.clear();

    for (auto *node : entitiesQuadTree) {
      delete node;
    }

    entitiesQuadTree.clear();

    quadTreesVectorPosition = 0;
    const auto &layersVector = glScene->getLayersList();

    for (auto &it : layersLODVector) {
      Camera *camera = it.camera;

      GlLayer *currentLayer = nullptr;

      for (const auto &itL : layersVector) {
        if (&itL.second->getCamera() == camera) {
          currentLayer = itL.second;
          break;
        }
      }

      cameras.push_back(camera);

      if (currentLayer != nullptr) {
        layerToCamera.insert(pair<GlLayer *, Camera>(currentLayer, *camera));
      }

      const MatrixGL &transformMatrix = camera->getTransformMatrix(globalViewport);

      Coord eye;

      if (camera->is3D()) {
        currentCamera = camera;
        eye = camera->getEyes() +
              (camera->getEyes() - camera->getCenter()) / float(camera->getZoomFactor());
        computeFor3DCamera(&it, eye, transformMatrix, globalViewport, currentViewport);
        quadTreesVectorPosition++;
      } else {
        entities.push_back(it.entitiesLODVector);
        computeFor2DCamera(&it, globalViewport, currentViewport);
      }

      glMatrixMode(GL_MODELVIEW);
    }

    initCamerasObservers();

    haveToCompute = false;

  } else {
    // if don't have to compute : use stored quadtree data

    layersLODVector.clear();

    quadTreesVectorPosition = 0;
    entitiesVectorPosition = 0;

    for (auto *camera : cameras) {
      layersLODVector.push_back(LayerLODUnit());
      LayerLODUnit *layerLODUnit = &(layersLODVector.back());
      layerLODUnit->camera = camera;

      const MatrixGL &transformMatrix = camera->getTransformMatrix(globalViewport);

      Coord eye;

      if (camera->is3D()) {
        currentCamera = camera;
        eye = camera->getEyes() +
              (camera->getEyes() - camera->getCenter()) / float(camera->getZoomFactor());
        computeFor3DCamera(layerLODUnit, eye, transformMatrix, globalViewport, currentViewport);
        quadTreesVectorPosition++;
      } else {
        layerLODUnit->entitiesLODVector = entities[entitiesVectorPosition];
        computeFor2DCamera(layerLODUnit, globalViewport, currentViewport);
        entitiesVectorPosition++;
      }
    }
  }
}

void GlQuadTreeLODCalculator::computeFor3DCamera(LayerLODUnit *layerLODUnit, const Coord &eye,
                                                 const MatrixGL &transformMatrix,
                                                 const Vec4i &globalViewport,
                                                 const Vec4i &currentViewport) {

  // aX,aY : rotation on the camera in x and y
  Coord eyeCenter = currentCamera->getCenter() - currentCamera->getEyes();
  double aX = atan(eyeCenter[1] / eyeCenter[2]);
  double aY = atan(eyeCenter[0] / eyeCenter[2]);

  if (haveToCompute) {
    // Create quadtrees
    if (bbs[seBBIndex].isValid()) { // is bb for simple entities valid
      entitiesQuadTree.push_back(new QuadTreeNode<GlEntity *>(bbs[seBBIndex]));
    } else {
      entitiesQuadTree.push_back(nullptr);
    }

    // compute nodes dedicated bb
    BoundingBox bb = bbs[0];

    for (uint i = 1; i < eBBOffset; ++i) {
      bb.expand(bbs[i]);
    }

    if (bb.isValid()) {
      nodesQuadTree.push_back(new QuadTreeNode<uint>(bb));
    } else {
      nodesQuadTree.push_back(nullptr);
    }

    // compute edges dedicated bb
    bb = bbs[eBBOffset];

    for (uint i = eBBOffset + 1; i < seBBIndex; ++i) {
      bb.expand(bbs[i]);
    }

    if (bb.isValid()) {
      edgesQuadTree.push_back(new QuadTreeNode<uint>(bb));
    } else {
      edgesQuadTree.push_back(nullptr);
    }

    // Add entities in quadtrees
    size_t nbSimples = layerLODUnit->entitiesLODVector.size();
    size_t nbNodes = layerLODUnit->nodesLODVector.size();
    size_t nbEdges = layerLODUnit->edgesLODVector.size();
    auto thrdF1 = [&]() {
      for (size_t i = 0; i < nbSimples; ++i) {
        const auto &entity = layerLODUnit->entitiesLODVector[i];
        entitiesQuadTree[quadTreesVectorPosition]->insert(entity.boundingBox, entity.entity);
      }
    };
    auto thrdF2 = [&]() {
      for (size_t i = 0; i < nbNodes; ++i) {
        const auto &entity = layerLODUnit->nodesLODVector[i];
        nodesQuadTree[quadTreesVectorPosition]->insert(entity.boundingBox, entity.id);
      }
    };
    auto thrdF3 = [&]() {
      for (size_t i = 0; i < nbEdges; ++i) {
        // This code is here to expand edge bounding box when we have an edge with direction
        // (0,0,x)
        auto &entity = layerLODUnit->edgesLODVector[i];
        if (entity.boundingBox[0][0] == entity.boundingBox[1][0] &&
            entity.boundingBox[0][1] == entity.boundingBox[1][1]) {
          entity.boundingBox.expand(entity.boundingBox[1] + Coord(0.01f, 0.01f, 0));
        }

        edgesQuadTree[quadTreesVectorPosition]->insert(entity.boundingBox, entity.id);
      }
    };
    TLP_PARALLEL_SECTIONS(thrdF1, thrdF2, thrdF3);

    layerLODUnit->entitiesLODVector.clear();
    layerLODUnit->nodesLODVector.clear();
    layerLODUnit->edgesLODVector.clear();
  }

  MatrixGL invTransformMatrix(transformMatrix);
  invTransformMatrix.inverse();
  Coord pSrc = projectPoint(Coord(0, 0, 0), transformMatrix, globalViewport);

  Vec4i transformedViewport = currentViewport;
  transformedViewport[1] = globalViewport[3] - (currentViewport[1] + currentViewport[3]);
  BoundingBox cameraBoundingBox;

  // Project camera bondinx box to know visible part of the quadtree
  pSrc[0] = transformedViewport[0];
  pSrc[1] =
      (globalViewport[1] + globalViewport[3]) - (transformedViewport[1] + transformedViewport[3]);
  cameraBoundingBox.expand(unprojectPoint(pSrc, invTransformMatrix, globalViewport));
  pSrc[1] = transformedViewport[1] + transformedViewport[3];
  cameraBoundingBox.expand(unprojectPoint(pSrc, invTransformMatrix, globalViewport));
  pSrc[0] = transformedViewport[0] + transformedViewport[2];
  cameraBoundingBox.expand(unprojectPoint(pSrc, invTransformMatrix, globalViewport));
  pSrc[1] = transformedViewport[1];
  cameraBoundingBox.expand(unprojectPoint(pSrc, invTransformMatrix, globalViewport));

  int ratio;

  if (currentViewport[2] > currentViewport[3]) {
    ratio = currentViewport[2];
  } else {
    ratio = currentViewport[3];
  }

  vector<uint> resNodes;
  vector<uint> resEdges;
  vector<GlEntity *> resEntities;

  // Get result of quadtrees
  auto thrdF1 = [&]() {
    if ((renderingEntitiesFlag & RenderingNodes) != 0) {
      auto &quadTree = nodesQuadTree[quadTreesVectorPosition];
      if (quadTree) {
        if (aX == 0 && aY == 0) {
          if ((renderingEntitiesFlag & RenderingWithoutRemove) == 0) {
            quadTree->getElementsWithRatio(cameraBoundingBox, resNodes, ratio);
          } else {
            quadTree->getElements(cameraBoundingBox, resNodes);
          }
        } else {
          quadTree->getElements(resNodes);
        }
      }
    }
    size_t nbRes = resNodes.size();
    layerLODUnit->nodesLODVector.resize(nbRes);
  };
  auto thrdF2 = [&]() {
    if ((renderingEntitiesFlag & RenderingEdges) != 0) {
      auto &quadTree = edgesQuadTree[quadTreesVectorPosition];
      if (quadTree) {
        if (aX == 0 && aY == 0) {
          if ((renderingEntitiesFlag & RenderingWithoutRemove) == 0) {
            quadTree->getElementsWithRatio(cameraBoundingBox, resEdges, ratio);
          } else {
            quadTree->getElements(cameraBoundingBox, resEdges);
          }
        } else {
          quadTree->getElements(resEdges);
        }
      }
    }
    size_t nbRes = resEdges.size();
    layerLODUnit->edgesLODVector.resize(nbRes);
  };
  auto thrdF3 = [&]() {
    if ((renderingEntitiesFlag & RenderingEntities) != 0) {
      auto &quadTree = entitiesQuadTree[quadTreesVectorPosition];
      if (quadTree) {
        if (aX == 0 && aY == 0) {
          if ((renderingEntitiesFlag & RenderingWithoutRemove) == 0) {
            quadTree->getElementsWithRatio(cameraBoundingBox, resEntities, ratio);
          } else {
            quadTree->getElements(cameraBoundingBox, resEntities);
          }
        } else {
          quadTree->getElements(resEntities);
        }
      }
    }
    size_t nbRes = resEntities.size();
    for (size_t i = 0; i < nbRes; ++i) {
      layerLODUnit->entitiesLODVector.push_back(
          EntityLODUnit(resEntities[i], resEntities[i]->getBoundingBox()));
    }
  };
  TLP_PARALLEL_SECTIONS(thrdF1, thrdF2, thrdF3);
  TLP_PARALLEL_MAP_INDICES(resNodes.size(), [&](uint i) {
    uint nId = resNodes[i];
    GlNode glNode(node(nId), inputData->graph());
    layerLODUnit->nodesLODVector[i].init(nId, glNode.getBoundingBox(inputData));
  });
  TLP_PARALLEL_MAP_INDICES(resEdges.size(), [&](uint i) {
    uint eId = resEdges[i];
    GlEdge glEdge(edge(eId), inputData->graph());
    layerLODUnit->edgesLODVector[i].init(eId, glEdge.getBoundingBox(inputData));
  });

  GlCPULODCalculator::computeFor3DCamera(layerLODUnit, eye, transformMatrix, globalViewport,
                                         currentViewport);
}

void GlQuadTreeLODCalculator::removeObservers() {
  if (inputData) {
    if (currentGraph) {
      currentGraph->removeListener(this);
    }

    if (layoutProperty) {
      layoutProperty->removeListener(this);
      layoutProperty = nullptr;
    }

    if (sizeProperty) {
      sizeProperty->removeListener(this);
      sizeProperty = nullptr;
    }

    if (selectionProperty) {
      selectionProperty->removeListener(this);
      selectionProperty = nullptr;
    }
  }

  if (glScene) {
    glScene->removeListener(this);
  }
}

void GlQuadTreeLODCalculator::addObservers() {
  if (inputData) {
    currentGraph = inputData->graph();
    currentGraph->addListener(this);

    layoutProperty = inputData->layout();

    if (layoutProperty != nullptr) {
      layoutProperty->addListener(this);
    }

    sizeProperty = inputData->sizes();

    if (sizeProperty != nullptr) {
      sizeProperty->addListener(this);
    }

    selectionProperty = inputData->selection();

    if (selectionProperty != nullptr) {
      selectionProperty->addListener(this);
    }
  }

  if (glScene) {
    glScene->addListener(this);
  }
}

void GlQuadTreeLODCalculator::update(PropertyInterface *property) {
  if (property == inputData->layout() || property == inputData->sizes() ||
      property == inputData->selection()) {
    setHaveToCompute();
  }
}

void GlQuadTreeLODCalculator::treatEvent(const Event &ev) {
  const auto *sceneEv = dynamic_cast<const GlSceneEvent *>(&ev);

  if (sceneEv) {
    setHaveToCompute();
  } else if (typeid(ev) == typeid(GraphEvent)) {
    const auto *graphEvent = static_cast<const GraphEvent *>(&ev);

    switch (graphEvent->getType()) {
    case GraphEventType::TLP_ADD_NODE:
    case GraphEventType::TLP_ADD_EDGE:
    case GraphEventType::TLP_DEL_NODE:
    case GraphEventType::TLP_DEL_EDGE:
      setHaveToCompute();
      break;

    case GraphEventType::TLP_ADD_LOCAL_PROPERTY:
    case GraphEventType::TLP_BEFORE_DEL_LOCAL_PROPERTY: {
      const PropertyInterface *property =
          inputData->graph()->getProperty(graphEvent->getPropertyName());

      if (property == inputData->layout() || property == inputData->sizes()) {
        setHaveToCompute();
        removeObservers();
        addObservers();
      }

      break;
    }

    default:
      break;
    }
  } else if (typeid(ev) == typeid(PropertyEvent)) {
    const auto *propertyEvent = static_cast<const PropertyEvent *>(&ev);
    PropertyInterface *property = propertyEvent->getProperty();

    switch (propertyEvent->getType()) {
    case PropertyEventType::TLP_BEFORE_SET_ALL_NODE_VALUE:
    case PropertyEventType::TLP_BEFORE_SET_NODE_VALUE:
    case PropertyEventType::TLP_BEFORE_SET_ALL_EDGE_VALUE:
    case PropertyEventType::TLP_BEFORE_SET_EDGE_VALUE:
      update(property);
      break;

    default:
      break;
    }
  } else if (ev.type() == EventType::TLP_DELETE) {
    auto *camera = dynamic_cast<Camera *>(ev.sender());
    if (camera) {
      auto it = find(cameras.begin(), cameras.end(), camera);
      if (it != cameras.end()) {
        camera->removeListener(this);
        cameras.erase(it);
      }
      haveToCompute = true;
    }

    if (dynamic_cast<tlp::Graph *>(ev.sender())) {
      clear();
      setInputData(nullptr);
    }

    PropertyInterface *property;

    if ((property = dynamic_cast<PropertyInterface *>(ev.sender()))) {
      if (property == layoutProperty) {
        layoutProperty = nullptr;
      } else if (property == sizeProperty) {
        sizeProperty = nullptr;
      } else if (property == selectionProperty) {
        selectionProperty = nullptr;
      }
    }
  }
}

void GlQuadTreeLODCalculator::initCamerasObservers() {
  set<Camera *> treatedCameras;

  for (auto *camera : cameras) {
    if (!treatedCameras.contains(camera)) {
      treatedCameras.insert(camera);
      camera->addListener(this);
    }
  }
}

void GlQuadTreeLODCalculator::clearCamerasObservers() {
  set<Camera *> treatedCameras;

  for (auto *camera : cameras) {
    if (!treatedCameras.contains(camera)) {
      treatedCameras.insert(camera);
      camera->removeListener(this);
    }
  }
}

void GlQuadTreeLODCalculator::setHaveToCompute() {
  if (haveToCompute) {
    return;
  }

  auto *attachedQuadTreeLODCalculator =
      dynamic_cast<GlQuadTreeLODCalculator *>(attachedLODCalculator);

  if (attachedQuadTreeLODCalculator) {
    attachedQuadTreeLODCalculator->setHaveToCompute();
  }

  haveToCompute = true;
  haveToInitObservers = true;
  removeObservers();
}
}
