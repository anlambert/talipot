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

#include <talipot/GlMetaNodeRenderer.h>
#include <talipot/GlCPULODCalculator.h>
#include <talipot/GlGraph.h>
#include <talipot/GlyphManager.h>

using namespace std;

namespace tlp {

GlMetaNodeRenderer::GlMetaNodeRenderer(GlGraphInputData *inputData) : _inputData(inputData) {}

GlMetaNodeRenderer::~GlMetaNodeRenderer() {
  clearScenes();
}

void GlMetaNodeRenderer::setInputData(GlGraphInputData *inputData) {
  _inputData = inputData;
}

GlGraphInputData *GlMetaNodeRenderer::inputData() const {
  return _inputData;
}

void GlMetaNodeRenderer::render(node n, float, Camera *camera) {

  bool viewMeta = _inputData->renderingParameters()
                      ->isDisplayMetaNodes(); // Checks if user wants to see metanode content
  bool viewMetaLabels =
      _inputData->renderingParameters()
          ->isViewMetaLabel(); // Checks if user wants to see metanode content labels

  if (!viewMeta && !viewMetaLabels) {
    return;
  }

  GLint renderMode;
  glGetIntegerv(GL_RENDER_MODE, &renderMode);

  if (renderMode == GL_SELECT) {
    return;
  }

  Graph *metaGraph = _inputData->graph()->getNodeMetaInfo(n);
  GlScene *scene = nullptr;

  if (_metaGraphToSceneMap.count(metaGraph) != 0) {
    scene = _metaGraphToSceneMap[metaGraph];
  } else {
    scene = createScene(metaGraph);
    assert(scene != nullptr);
    _metaGraphToSceneMap[metaGraph] = scene;
    metaGraph->addListener(this);
  }

  int metaStencil = _inputData->renderingParameters()->getMetaNodesStencil();
  int metaSelectedStencil = _inputData->renderingParameters()->getSelectedMetaNodesStencil();
  int metaLabelStencil = _inputData->renderingParameters()->getMetaNodesLabelStencil();
  scene->glGraph()->setRenderingParameters(*(_inputData->renderingParameters()));

  auto &renderingParameters = scene->glGraph()->renderingParameters();
  renderingParameters.setDisplayNodes(viewMeta);
  renderingParameters.setDisplayEdges(viewMeta);
  renderingParameters.setViewEdgeLabel(viewMetaLabels);
  renderingParameters.setViewNodeLabel(viewMetaLabels);
  renderingParameters.setNodesStencil(metaStencil);
  renderingParameters.setEdgesStencil(metaStencil);
  renderingParameters.setSelectedNodesStencil(metaSelectedStencil);
  renderingParameters.setSelectedEdgesStencil(metaSelectedStencil);
  renderingParameters.setNodesLabelStencil(metaLabelStencil);
  renderingParameters.setEdgesLabelStencil(metaLabelStencil);

  GlNode glNode(n, metaGraph);

  BoundingBox includeBB = _inputData->glyphManager()
                              ->getGlyph(_inputData->shapes()->getNodeValue(n))
                              ->getIncludeBoundingBox(n);
  BoundingBox bbTmp = glNode.getBoundingBox(_inputData);
  BoundingBox bb(bbTmp.center() - Coord((bbTmp.width() / 2.f) * (includeBB[0][0] * -2.f),
                                        (bbTmp.height() / 2.f) * (includeBB[0][1] * -2.f),
                                        (bbTmp.depth() / 2.f) * (includeBB[0][2] * -2.f)),
                 bbTmp.center() + Coord((bbTmp.width() / 2.f) * (includeBB[1][0] * 2.f),
                                        (bbTmp.height() / 2.f) * (includeBB[1][1] * 2.f),
                                        (bbTmp.depth() / 2.f) * (includeBB[1][2] * 2.f)));

  Coord eyeDirection = camera->getEyes() - camera->getCenter();
  eyeDirection = eyeDirection / eyeDirection.norm();

  Camera newCamera2(*camera);
  newCamera2.setEyes(newCamera2.getCenter() +
                     Coord(0, 0, 1) * (newCamera2.getEyes() - newCamera2.getCenter()).norm());
  newCamera2.setUp(Coord(0, 1, 0));

  Coord center = camera->worldTo2DViewport((bb[0] + bb[1]) / 2.f);
  Coord first = newCamera2.worldTo2DViewport(bb[0]);
  Coord second = newCamera2.worldTo2DViewport(bb[1]);

  Coord size = second - first;

  Vec4i viewport;
  viewport[0] = center[0] - size[0] / 2;
  viewport[1] = center[1] - size[1] / 2;
  viewport[2] = size[0];
  viewport[3] = size[1];

  viewport[0] = camera->getViewport()[0] + viewport[0] - viewport[2] / 2;
  viewport[1] = camera->getViewport()[1] + viewport[1] - viewport[3] / 2;
  viewport[2] *= 2;
  viewport[3] *= 2;

  if (viewport[2] == 0 || viewport[3] == 0) {
    return;
  }

  scene->setViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
  scene->setClearBufferAtDraw(false);
  scene->setClearDepthBufferAtDraw(false);
  scene->setClearStencilBufferAtDraw(false);
  scene->centerScene();

  float baseNorm =
      (scene->graphLayer()->getCamera().getEyes() - scene->graphLayer()->getCamera().getCenter())
          .norm();
  Camera newCamera = scene->graphLayer()->getCamera();
  auto *oldCamera = new Camera(scene, true);
  newCamera.setScene(scene);
  *oldCamera = newCamera;
  newCamera.setUp(camera->getUp());
  newCamera.setEyes(newCamera.getCenter() + (eyeDirection * baseNorm));
  newCamera.setZoomFactor(newCamera.getZoomFactor() * 0.5);
  scene->graphLayer()->setSharedCamera(&newCamera);

  // small hack to avoid z-fighting between the rendering of the metanode content
  // and the rendering of the metanode that occurs afterwards
  glDepthRange(0.1, 1);
  scene->draw();
  // restore default depth range
  glDepthRange(0, 1);

  scene->graphLayer()->setCamera(oldCamera);

  camera->getScene()->setClearBufferAtDraw(false);
  camera->getScene()->setClearDepthBufferAtDraw(false);
  camera->getScene()->setClearStencilBufferAtDraw(false);
  camera->getScene()->initGlParameters();
  camera->getScene()->setClearBufferAtDraw(true);
  camera->getScene()->setClearDepthBufferAtDraw(true);
  camera->getScene()->setClearStencilBufferAtDraw(true);
  camera->initGl();
}

GlScene *GlMetaNodeRenderer::createScene(Graph *metaGraph) const {
  auto *scene = new GlScene(new GlCPULODCalculator());
  auto *layer = new GlLayer("Main");
  scene->addExistingLayer(layer);
  auto *glGraph = new GlGraph(metaGraph, scene);
  layer->addGlEntity(glGraph, "graph");
  return scene;
}

void GlMetaNodeRenderer::treatEvent(const Event &e) {
  if (e.type() == EventType::TLP_DELETE) {
    delete _metaGraphToSceneMap[static_cast<Graph *>(e.sender())];
    _metaGraphToSceneMap.erase(static_cast<Graph *>(e.sender()));
  }
}

void GlMetaNodeRenderer::clearScenes() {
  for (const auto &it : _metaGraphToSceneMap) {
    delete it.second;
  }

  _metaGraphToSceneMap.clear();
}

GlScene *GlMetaNodeRenderer::getSceneForMetaGraph(Graph *g) const {
  auto sceneit = _metaGraphToSceneMap.find(g);
  return (sceneit == _metaGraphToSceneMap.end()) ? (nullptr) : (sceneit->second);
}
}
