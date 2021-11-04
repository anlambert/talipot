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

#include <GL/glew.h>

#include <talipot/GlWidget.h>

// remove warnings about qt5/glew incompatibility
// as we do not rely on QOpenGLFunctions for rendering
#undef __GLEW_H__
#include <QOpenGLFramebufferObject>
#define __GLEW_H__

#include <talipot/TlpQtTools.h>
#include <talipot/Settings.h>
#include <talipot/GlTextureManager.h>
#include <talipot/Gl2DRect.h>
#include <talipot/GlQuadTreeLODCalculator.h>
#include <talipot/GLInteractor.h>
#include <talipot/GlGraph.h>
#include <talipot/View.h>
#include <talipot/OpenGlConfigManager.h>
#include <talipot/GlOffscreenRenderer.h>
#include <talipot/GlTextureManager.h>

using namespace std;

namespace tlp {

bool GlWidget::inRendering = false;

//==================================================
GlWidget::GlWidget(QWidget *parent, View *view)
    : QOpenGLWidget(parent), _scene(new GlQuadTreeLODCalculator), view(view), widthStored(0),
      heightStored(0), glFrameBuf(nullptr), glFrameBuf2(nullptr),
      keepPointOfViewOnSubgraphChanging(false),
      sceneTextureId("scene" + to_string(reinterpret_cast<uintptr_t>(this))) {
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
  grabGesture(Qt::PinchGesture);
  grabGesture(Qt::PanGesture);
  grabGesture(Qt::SwipeGesture);
  QOpenGLWidget::makeCurrent();
  QSurfaceFormat format;
  format.setSamples(OpenGlConfigManager::maxNumberOfSamples());
  format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
#ifndef NDEBUG
  format.setOption(QSurfaceFormat::DebugContext);
#endif
  setFormat(format);
  scene()->setViewOrtho(Settings::isViewOrtho());
  OpenGlConfigManager::initExtensions();
  QOpenGLWidget::doneCurrent();
}
//==================================================
GlWidget::~GlWidget() {
  delete glFrameBuf;
  delete glFrameBuf2;
}
//==================================================
void GlWidget::paintEvent(QPaintEvent *) {
  QRegion rect = this->visibleRegion();

  // If the visible are changed we need to draw the entire scene
  // Because the saved snapshot only backup the visible part of the
  // Graph
  if (rect.boundingRect() != _visibleArea.boundingRect()) {
    _visibleArea = rect;
    draw();
  } else {
    redraw();
  }

  _visibleArea = rect; // Save the new visible area.
}
//==================================================
void GlWidget::closeEvent(QCloseEvent *e) {
  emit closing(this, e);
}
//==================================================
void GlWidget::createFramebuffers(int width, int height) {

  if (!glFrameBuf || glFrameBuf->size().width() != width || glFrameBuf->size().height() != height) {
    makeCurrent();
    deleteFramebuffers();
    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    fboFormat.setSamples(OpenGlConfigManager::maxNumberOfSamples());
    glFrameBuf = new QOpenGLFramebufferObject(width, height, fboFormat);
    glFrameBuf2 = new QOpenGLFramebufferObject(width, height);
    GlTextureManager::registerExternalTexture(sceneTextureId, glFrameBuf2->texture());
    widthStored = width;
    heightStored = height;
  }
}
//==================================================
void GlWidget::deleteFramebuffers() {
  delete glFrameBuf;
  glFrameBuf = nullptr;
  delete glFrameBuf2;
  glFrameBuf2 = nullptr;
}

//==================================================
void GlWidget::render(RenderingOptions options, bool checkVisibility) {

  if ((isVisible() || !checkVisibility) && !inRendering) {

    // begin rendering process
    inRendering = true;
    makeCurrent();

    // backup internal QOpenGLWidget bound framebuffer id
    int drawFboId = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &drawFboId);

    // get the content width and height
    int width = screenToViewport(contentsRect().width());
    int height = screenToViewport(contentsRect().height());

    // if the framebuffers have invalid size, new ones need to be created
    // so force the RenderScene flag.
    if (widthStored != width || heightStored != height) {
      options |= RenderScene;
    }

    computeInteractors();

    if (options.testFlag(RenderScene)) {
      createFramebuffers(width, height);

      // render the graph in the antialiased framebuffer.
      glFrameBuf->bind();
      _scene.draw();
      glFrameBuf->release();

      // copy rendered scene in a texture/QImage compatible framebuffer
      QRect fbRect(0, 0, width, height);
      QOpenGLFramebufferObject::blitFramebuffer(glFrameBuf2, fbRect, glFrameBuf, fbRect);

      // restore internal QOpenGLWidget framebuffer binding
      makeCurrent();
      glBindFramebuffer(GL_FRAMEBUFFER, drawFboId);
    } else {
      _scene.initGlParameters();
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);

    // draw rendered scene from texture
    Camera camera2D(_scene.graphCamera().getScene(), false);
    camera2D.setScene(_scene.graphCamera().getScene());
    camera2D.initGl();
    Gl2DRect rect(height, 0, 0, width, sceneTextureId);
    rect.draw(0, &camera2D);
    _scene.graphCamera().initGl();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_LIGHTING);

    // draw interactors and foreground entities.
    drawInteractors();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_STENCIL_TEST);

    if (options.testFlag(SwapBuffers)) {
      update();
    }

    inRendering = false;
  }
}
//==================================================
void GlWidget::redraw() {
  render(SwapBuffers);
  emit viewRedrawn(this);
}
//==================================================
void GlWidget::draw(bool graphChanged) {
  render(RenderingOptions(RenderScene | SwapBuffers));
  emit viewDrawn(this, graphChanged);
}
//==================================================
void GlWidget::computeInteractors() {
  if (!view) {
    return;
  }

  auto *interactor = dynamic_cast<GLInteractorComposite *>(view->currentInteractor());

  if (interactor == nullptr) {
    return;
  }

  interactor->compute(this);
}
//==================================================
void GlWidget::drawInteractors() {
  if (!view) {
    return;
  }

  auto *interactor = dynamic_cast<GLInteractorComposite *>(view->currentInteractor());

  if (!interactor) {
    return;
  }

  interactor->draw(this);
}
//==================================================
// QGLWidget slots
//==================================================
void GlWidget::resizeGL(int w, int h) {

  if (w == 0 || h == 0) {
    return;
  }

  int width = contentsRect().width();
  int height = contentsRect().height();

  deleteFramebuffers();

  _scene.setViewport(0, 0, screenToViewport(width), screenToViewport(height));

  emit glResized(w, h);
}
//==================================================
void GlWidget::makeCurrent() {
  if (isVisible()) {
    QOpenGLWidget::makeCurrent();
    int width = contentsRect().width();
    int height = contentsRect().height();
    _scene.setViewport(0, 0, screenToViewport(width), screenToViewport(height));
  } else {
    GlOffscreenRenderer::instance().makeOpenGLContextCurrent();
  }
}
//==================================================
void GlWidget::doneCurrent() {
  if (isVisible()) {
    QOpenGLWidget::doneCurrent();
  } else {
    GlOffscreenRenderer::instance().doneOpenGLContextCurrent();
  }
}
//==================================================
bool GlWidget::pickGlEntities(const int x, const int y, const int width, const int height,
                              std::vector<SelectedEntity> &pickedEntities, GlLayer *layer) {
  makeCurrent();
#if defined(Q_OS_MAC) && QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
  glFrameBuf->bind();
#endif

  bool pickedEntity = _scene.selectEntities(
      static_cast<RenderingEntitiesFlag>(RenderingEntities | RenderingWithoutRemove),
      screenToViewport(x), screenToViewport(y), screenToViewport(width), screenToViewport(height),
      layer, pickedEntities);

#if defined(Q_OS_MAC) && QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
  glFrameBuf->release();
#endif

  return pickedEntity;
}
//==================================================
bool GlWidget::pickGlEntities(const int x, const int y, std::vector<SelectedEntity> &pickedEntities,
                              GlLayer *layer) {
  return pickGlEntities(x, y, 2, 2, pickedEntities, layer);
}
//==================================================
void GlWidget::pickNodesEdges(const int x, const int y, const int width, const int height,
                              std::vector<SelectedEntity> &selectedNodes,
                              std::vector<SelectedEntity> &selectedEdges, GlLayer *layer,
                              bool pickNodes, bool pickEdges) {
  makeCurrent();
#if defined(Q_OS_MAC) && QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
  glFrameBuf->bind();
#endif

  if (pickNodes) {
    _scene.selectEntities(
        static_cast<RenderingEntitiesFlag>(RenderingNodes | RenderingWithoutRemove),
        screenToViewport(x), screenToViewport(y), screenToViewport(width), screenToViewport(height),
        layer, selectedNodes);
  }

  if (pickEdges) {
    _scene.selectEntities(
        static_cast<RenderingEntitiesFlag>(RenderingEdges | RenderingWithoutRemove),
        screenToViewport(x), screenToViewport(y), screenToViewport(width), screenToViewport(height),
        layer, selectedEdges);
  }

#if defined(Q_OS_MAC) && QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
  glFrameBuf->release();
#endif
}
//=====================================================
bool GlWidget::pickNodesEdges(const int x, const int y, SelectedEntity &selectedEntity,
                              GlLayer *layer, bool pickNodes, bool pickEdges) {
  makeCurrent();
#if defined(Q_OS_MAC) && QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
  glFrameBuf->bind();
#endif

  bool elementPicked = false;
  vector<SelectedEntity> selectedEntities;

  if (pickNodes && _scene.selectEntities(
                       static_cast<RenderingEntitiesFlag>(RenderingNodes | RenderingWithoutRemove),
                       screenToViewport(x - 1), screenToViewport(y - 1), screenToViewport(3),
                       screenToViewport(3), layer, selectedEntities)) {
    selectedEntity = selectedEntities[0];
    elementPicked = true;
  }

  if (!elementPicked && pickEdges &&
      _scene.selectEntities(
          static_cast<RenderingEntitiesFlag>(RenderingEdges | RenderingWithoutRemove),
          screenToViewport(x - 1), screenToViewport(y - 1), screenToViewport(3),
          screenToViewport(3), layer, selectedEntities)) {
    selectedEntity = selectedEntities[0];
    elementPicked = true;
  }

#if defined(Q_OS_MAC) && QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
  glFrameBuf->release();
#endif

  return elementPicked;
}
//=====================================================
void GlWidget::getTextureRealSize(int width, int height, int &textureRealWidth,
                                  int &textureRealHeight) {
  textureRealWidth = 1;
  textureRealHeight = 1;

  while (textureRealWidth <= width) {
    textureRealWidth *= 2;
  }

  while (textureRealHeight <= height) {
    textureRealHeight *= 2;
  }

  if (textureRealWidth > 4096) {
    textureRealHeight = textureRealHeight / (textureRealWidth / 8192);
    textureRealWidth = 4096;
  }

  if (textureRealHeight > 4096) {
    textureRealWidth = textureRealWidth / (textureRealHeight / 8192);
    textureRealHeight = 4096;
  }
}
//=====================================================
void GlWidget::createPicture(const std::string &pictureName, int width, int height, bool center) {
  createPicture(width, height, center).save(tlp::tlpStringToQString(pictureName));
}
//=====================================================
QImage GlWidget::createPicture(int width, int height, bool center, QImage::Format format) {

  QImage resultImage;

  makeCurrent();

  QOpenGLFramebufferObjectFormat fboFormat;
  fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
  fboFormat.setSamples(OpenGlConfigManager::maxNumberOfSamples());
  auto *frameBuf = new QOpenGLFramebufferObject(width, height, fboFormat);
  auto *frameBuf2 = new QOpenGLFramebufferObject(width, height);

  if (frameBuf->isValid() && frameBuf2->isValid()) {
    frameBuf->bind();

    int oldWidth = _scene.getViewport()[2];
    int oldHeight = _scene.getViewport()[3];
    vector<Camera> oldCameras;
    const vector<pair<string, GlLayer *>> &layersList = _scene.getLayersList();

    if (center) {
      for (const auto &it : layersList) {
        if (!it.second->useSharedCamera()) {
          oldCameras.push_back(it.second->getCamera());
        }
      }
    }

    _scene.setViewport(0, 0, width, height);

    if (center) {
      _scene.adjustSceneToSize(width, height);
    }

    computeInteractors();
    _scene.draw();
    drawInteractors();
    frameBuf->release();

    QOpenGLFramebufferObject::blitFramebuffer(frameBuf2, QRect(0, 0, width, height), frameBuf,
                                              QRect(0, 0, width, height));

    resultImage = frameBuf2->toImage();

    _scene.setViewport(0, 0, oldWidth, oldHeight);

    if (center) {
      int i = 0;

      for (const auto &it : layersList) {
        if (!it.second->useSharedCamera()) {
          Camera &camera = it.second->getCamera();
          camera.setCenter(oldCameras[i].getCenter());
          camera.setEyes(oldCameras[i].getEyes());
          camera.setSceneRadius(oldCameras[i].getSceneRadius());
          camera.setUp(oldCameras[i].getUp());
          camera.setZoomFactor(oldCameras[i].getZoomFactor());
        }

        i++;
      }
    }
  }

  delete frameBuf;
  delete frameBuf2;

  // The QOpenGLFramebufferObject returns the wrong image format
  // QImage::Format_ARGB32_Premultiplied. We need to create an image from original data with the
  // right format QImage::Format_ARGB32. We need to clone the data as when the image var will be
  // destroy at the end of the function it's data will be destroyed too and the newly created image
  // object will have invalid data pointer.
  return QImage(resultImage.bits(), resultImage.width(), resultImage.height(),
                QImage::Format_ARGB32)
      .convertToFormat(format);
}

void GlWidget::centerScene(bool graphChanged, float zf) {
  makeCurrent();
  _scene.centerScene();

  if (zf != 1) {
    _scene.zoomFactor(zf);
  }

  draw(graphChanged);
}

void GlWidget::emitGraphChanged() {
  emit graphChanged();
}

void GlWidget::setKeepScenePointOfViewOnSubgraphChanging(bool k) {
  keepPointOfViewOnSubgraphChanging = k;
}

bool GlWidget::keepScenePointOfViewOnSubgraphChanging() const {
  return keepPointOfViewOnSubgraphChanging;
}

GlGraphRenderingParameters &GlWidget::renderingParameters() {
  return _scene.glGraph()->renderingParameters();
}

GlGraphInputData *GlWidget::inputData() const {
  return _scene.glGraph()->inputData();
}
}
