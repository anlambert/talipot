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

#include "FisheyeInteractor.h"
#include "FisheyeConfigWidget.h"
#include "../../utils/PluginNames.h"
#include "../../utils/InteractorIcons.h"

#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QWheelEvent>

#include <talipot/MouseInteractors.h>
#include <talipot/GlShaderProgram.h>
#include <talipot/NodeLinkDiagramView.h>
#include <talipot/OpenGlConfigManager.h>
#include <talipot/GlTextureManager.h>
#include <talipot/GlGraph.h>
#include <talipot/GlSceneZoomAndPan.h>

using namespace std;
using namespace tlp;

static const string fisheyeVertexShaderSrc = R"(#version 120

uniform mat4 u_modelviewMatrix;
uniform mat4 u_projectionMatrix;

attribute vec3 a_position;
attribute vec2 a_texCoord;

varying vec2 v_texCoord;

void main() {
  gl_Position = u_projectionMatrix * u_modelviewMatrix * vec4(a_position, 1.0);
  v_texCoord = a_texCoord;
}

)";

static const string fisheyeFragmentShaderSrc = R"(#version 120

uniform sampler2D u_texture;
uniform sampler2D u_fisheyeTexture;
uniform float u_fisheyeRadius;
uniform float u_fisheyeHeight;
uniform float u_fisheyeTextureSize;
uniform vec2 u_mouse;
uniform vec2 u_resolution;
varying vec2 v_texCoord;

void main() {
  vec2 pos = v_texCoord * u_resolution;
  vec2 center = u_mouse;
  float radius = u_fisheyeRadius;
  float height = u_fisheyeHeight;
  float dist = distance(center, pos);
  if (dist < radius && u_fisheyeHeight < -0.05) {
    float coeff = (height + 1.0) * dist / (height * dist /  radius + 1.0);
    vec2 dir = normalize(pos - center) * coeff;
    pos = center + dir;
    vec2 fisheyePos = vec2(pos.x - (u_mouse.x - radius),
                           pos.y - (u_mouse.y - radius)) / vec2(2.0 * radius);

    gl_FragColor = texture2D(u_fisheyeTexture, fisheyePos);
  } else {
    gl_FragColor = texture2D(u_texture, pos / u_resolution);
  }
}

)";

FisheyeInteractor::FisheyeInteractor(const PluginContext *)
    : GLInteractorComposite(interactorIcon(InteractorType::Fisheye), "Fisheye"),
      fisheyeConfigWidget(nullptr) {}

void FisheyeInteractor::construct() {
  fisheyeConfigWidget = new FisheyeConfigWidget();
  push_back(new MousePanNZoomNavigator());
  push_back(new FisheyeInteractorComponent(fisheyeConfigWidget));
}

bool FisheyeInteractor::isCompatible(const string &viewName) const {
  return ((viewName == NodeLinkDiagramView::viewName) ||
          (viewName == ViewName::HistogramViewName) || (viewName == ViewName::MatrixViewName) ||
          (viewName == ViewName::ParallelCoordinatesViewName) ||
          (viewName == ViewName::PixelOrientedViewName) ||
          (viewName == ViewName::ScatterPlot2DViewName));
}

void FisheyeInteractor::uninstall() {
  InteractorComposite::uninstall();
}

FisheyeInteractor::~FisheyeInteractor() {
  delete fisheyeConfigWidget;
}

QWidget *FisheyeInteractor::configurationWidget() const {
  return fisheyeConfigWidget;
}

PLUGIN(FisheyeInteractor)

unique_ptr<GlShaderProgram> FisheyeInteractorComponent::fisheyeShader;

FisheyeInteractorComponent::FisheyeInteractorComponent(FisheyeConfigWidget *configWidget)
    : _configWidget(configWidget), _activateFisheye(false) {
  _fboTextureId = "fisheyeTexture" + to_string(reinterpret_cast<unsigned long long>(this));
}

FisheyeInteractorComponent::FisheyeInteractorComponent(
    const FisheyeInteractorComponent &fisheyeInteractorComponent) {
  _configWidget = fisheyeInteractorComponent._configWidget;
  _activateFisheye = false;
  _fboTextureId = "fisheyeTexture" + to_string(reinterpret_cast<unsigned long long>(this));
}

FisheyeInteractorComponent::~FisheyeInteractorComponent() {
  delete _fbo;
  delete _fbo2;
}

void FisheyeInteractorComponent::viewChanged(View *view) {
  if (view == nullptr) {
    return;
  }

  auto *glView = static_cast<GlView *>(view);
  GlWidget *glWidget = glView->glWidget();

  if (!glWidget->hasMouseTracking()) {
    glWidget->setMouseTracking(true);
  }

  if (_configWidget->getFisheyeRadius() == 0) {
    _configWidget->setFisheyeRadius(200);
    _configWidget->setFisheyeHeight(0.5);
  }
}

bool FisheyeInteractorComponent::eventFilter(QObject *obj, QEvent *e) {

  auto *glWidget = static_cast<GlWidget *>(obj);

  _activateFisheye = false;

  if (e->type() == QEvent::MouseMove || e->type() == QEvent::MouseButtonPress ||
      e->type() == QEvent::MouseButtonRelease) {
    _activateFisheye = true;
    auto *me = static_cast<QMouseEvent *>(e);
    _x = me->pos().x();
    _y = me->pos().y();
  } else if (e->type() == QEvent::Wheel) {
    auto *wheelEvent = static_cast<QWheelEvent *>(e);
    int numSteps = wheelEvent->angleDelta().y() / 120;

    if (numSteps && (wheelEvent->modifiers() == Qt::ControlModifier)) {
      _activateFisheye = true;
      double newRadius = _configWidget->getFisheyeRadius() +
                         _configWidget->getFisheyeRadiusIncrementStep() * numSteps * 10;
      if (newRadius < 0) {
        newRadius = 0;
      }
      _configWidget->setFisheyeRadius(newRadius);
    } else if (numSteps && (wheelEvent->modifiers() == Qt::ShiftModifier)) {
      _activateFisheye = true;
      double newHeight = _configWidget->getFisheyeHeight() +
                         _configWidget->getFisheyeHeightIncrementStep() * numSteps * 0.05;
      newHeight = clamp(newHeight, 0.2, 0.9);
      _configWidget->setFisheyeHeight(newHeight);
    }
  }

  if (_activateFisheye) {
    generateFisheyeTexture(glWidget);
    glWidget->redraw();
  }

  return _activateFisheye;
}

void FisheyeInteractorComponent::generateFisheyeTexture(GlWidget *glWidget) {
  if (_maxTextureSize == 0) {
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_maxTextureSize);
    if (_maxTextureSize > 4096) {
      _maxTextureSize = 4096;
    }
  }
  GlScene *glScene = glWidget->scene();
  Vec4i viewport = glScene->getViewport();

  float factor = 1;
  float fisheyeHeight = _configWidget->getFisheyeHeight();
  float fisheyeRadius = _configWidget->getFisheyeRadius();
  if (fisheyeHeight > 0.1) {
    factor = fisheyeHeight * 10;
  }
  int fboSize = factor * 2 * fisheyeRadius;
  fboSize = min(fboSize, _maxTextureSize);

  if (!_fbo || _fbo->width() != fboSize) {
    delete _fbo;
    delete _fbo2;

    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    fboFormat.setSamples(OpenGlConfigManager::maxNumberOfSamples());

    _fbo = new QOpenGLFramebufferObject(fboSize, fboSize, fboFormat);
    _fbo2 = new QOpenGLFramebufferObject(fboSize, fboSize);
    GlTextureManager::registerExternalTexture(_fboTextureId, _fbo2->texture());
  }

  Camera *camera = &glWidget->scene()->getGraphCamera();
  Camera camBackup = *camera;

  Coord bbMin = camera->viewportTo3DWorld(
      glWidget->screenToViewport(Coord(viewport[2] - (_x - fisheyeRadius), _y - fisheyeRadius)));
  Coord bbMax = camera->viewportTo3DWorld(
      glWidget->screenToViewport(Coord(viewport[2] - (_x + fisheyeRadius), _y + fisheyeRadius)));

  BoundingBox bb = {bbMin, bbMax};

  GlGraph *glGraph = glScene->getGlGraph();
  int labelMinSizeBackup = 0;
  if (glGraph) {
    labelMinSizeBackup = glGraph->getRenderingParameters().getMinSizeOfLabel();
    glGraph->getRenderingParameters().setMinSizeOfLabel(labelMinSizeBackup + factor * 3);
  }

  _fbo->bind();
  glScene->setViewport(0, 0, fboSize, fboSize);
  GlSceneZoomAndPan znp(glScene, bb);
  znp.zoomAndPanAnimationStep(znp.getNbAnimationsStep());
  glScene->draw();
  _fbo->release();

  QOpenGLFramebufferObject::blitFramebuffer(_fbo2, _fbo, GL_COLOR_BUFFER_BIT, GL_LINEAR);

  if (glGraph) {
    glGraph->getRenderingParameters().setMinSizeOfLabel(labelMinSizeBackup);
  }

  glScene->setViewport(viewport);
  *camera = camBackup;
}

bool FisheyeInteractorComponent::draw(GlWidget *glWidget) {

  if (!fisheyeShader.get()) {
    fisheyeShader.reset(new GlShaderProgram("fisheye"));
    fisheyeShader->addShaderFromSourceCode(Vertex, fisheyeVertexShaderSrc);
    fisheyeShader->addShaderFromSourceCode(Fragment, fisheyeFragmentShaderSrc);
    fisheyeShader->link();
  }

  if (_activateFisheye) {

    GlScene *glScene = glWidget->scene();
    Vec4i viewport = glScene->getViewport();
    float fisheyeHeight = _configWidget->getFisheyeHeight();
    float fisheyeRadius = _configWidget->getFisheyeRadius();

    Camera camera2D(glScene, false);
    camera2D.setScene(glScene);
    camera2D.initGl();

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    // clang-format off

    float vpw = viewport[2], vph = viewport[3];
    vector<float> quadData = {
        0.f, 0.f, 0.f, 0.f, 0.f,
        0.f, vph, 0.f, 0.f, 1.f,
        vpw, vph, 0.f, 1.f, 1.f,
        vpw, 0.f, 0.f, 1.f, 0.f
    };
    vector<unsigned short> indices = {0, 1, 2, 0, 2, 3};

    // clang-format on

    QOpenGLBuffer vb(QOpenGLBuffer::VertexBuffer);
    vb.create();
    vb.bind();
    vb.allocate(quadData.data(), quadData.size() * sizeof(float));

    QOpenGLBuffer ib(QOpenGLBuffer::IndexBuffer);
    ib.create();
    ib.bind();
    ib.allocate(indices.data(), indices.size() * sizeof(unsigned short));

    GlTextureManager::activateTexture(glWidget->getSceneTextureId());
    GlTextureManager::activateTexture(_fboTextureId, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    fisheyeShader->activate();
    fisheyeShader->setUniformMat4Float("u_modelviewMatrix", camera2D.getModelViewMatrix());
    fisheyeShader->setUniformMat4Float("u_projectionMatrix", camera2D.getProjectionMatrix());
    fisheyeShader->setVertexAttribPointer("a_position", 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                                          BUFFER_OFFSET(0));
    fisheyeShader->setVertexAttribPointer("a_texCoord", 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                                          BUFFER_OFFSET(3 * sizeof(float)));
    fisheyeShader->setUniformTextureSampler("u_texture", 0);
    fisheyeShader->setUniformTextureSampler("u_fisheyeTexture", 1);
    fisheyeShader->setUniformVec2Float("u_mouse", _x, viewport[3] - _y);
    fisheyeShader->setUniformVec2Float("u_resolution", viewport[2], viewport[3]);
    fisheyeShader->setUniformFloat("u_fisheyeRadius", fisheyeRadius);
    fisheyeShader->setUniformFloat("u_fisheyeHeight", -fisheyeHeight);
    fisheyeShader->setUniformFloat("u_fisheyeTextureSize", _fbo2->width());

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
    fisheyeShader->disableAttributesArrays();
    fisheyeShader->deactivate();
    GlTextureManager::deactivateTexture();
    GlTextureManager::deactivateTexture(1);
    ib.release();
    vb.release();

    return true;
  }

  return false;
}
