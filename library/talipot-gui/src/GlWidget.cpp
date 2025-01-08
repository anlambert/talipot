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

#include <GL/glew.h>

#include <talipot/GlWidget.h>

// remove warnings about qt5/glew incompatibility
// as we do not rely on QOpenGLFunctions for rendering
#undef __GLEW_H__
#include <QOpenGLFramebufferObject>
#define __GLEW_H__

#include <QMainWindow>

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
#include <talipot/GlBoundingBoxSceneVisitor.h>
#include <talipot/QtGlSceneZoomAndPanAnimator.h>

using namespace std;

namespace tlp {

bool GlWidget::inRendering = false;

//==================================================
GlWidget::GlWidget(QWidget *parent, View *_view)
    : QOpenGLWidget(parent), _scene(new GlQuadTreeLODCalculator), _view(_view), _widthStored(0),
      _heightStored(0), _glFrameBufAntialiased(nullptr), _glFrameBufSceneTexture(nullptr),
      _glFrameBufSceneAndInteractorTexture(nullptr), _keepPointOfViewOnSubgraphChanging(false),
      _sceneTextureId("scene" + to_string(reinterpret_cast<uintptr_t>(this))),
      _sceneAndInteractorTextureId("sceneAndInteractor" +
                                   to_string(reinterpret_cast<uintptr_t>(this))) {
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
  // this GlWidget is likely to be embedded in a GlWidgetGraphicsItem with no
  // windows attached to it, making device pixel ratio value unreliable so we
  // grab a pointer to the adequate main window to override the implementation
  // of the devicePixelRatio method
  _windows = window()->windowHandle() ? nullptr : getMainWindow();
}
//==================================================
GlWidget::~GlWidget() {
  delete _glFrameBufAntialiased;
  delete _glFrameBufSceneTexture;
  delete _glFrameBufSceneAndInteractorTexture;
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

  if (!_glFrameBufAntialiased || _glFrameBufAntialiased->size().width() != width ||
      _glFrameBufAntialiased->size().height() != height) {
    makeCurrent();
    deleteFramebuffers();
    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    fboFormat.setSamples(OpenGlConfigManager::maxNumberOfSamples());
    _glFrameBufAntialiased = new QOpenGLFramebufferObject(width, height, fboFormat);
    _glFrameBufSceneTexture = new QOpenGLFramebufferObject(width, height);
    _glFrameBufSceneAndInteractorTexture = new QOpenGLFramebufferObject(width, height);
    GlTextureManager::registerExternalTexture(_sceneTextureId, _glFrameBufSceneTexture->texture());
    GlTextureManager::registerExternalTexture(_sceneAndInteractorTextureId,
                                              _glFrameBufSceneAndInteractorTexture->texture());
    _widthStored = width;
    _heightStored = height;
  }
}
//==================================================
void GlWidget::deleteFramebuffers() {
  delete _glFrameBufAntialiased;
  _glFrameBufAntialiased = nullptr;
  delete _glFrameBufSceneTexture;
  _glFrameBufSceneTexture = nullptr;
  delete _glFrameBufSceneAndInteractorTexture;
  _glFrameBufSceneAndInteractorTexture = nullptr;
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
    if (_widthStored != width || _heightStored != height) {
      options |= RenderScene;
    }

    auto renderTexture = [width, height, this](const string &textureId) {
      Camera camera2D(_scene.graphCamera().getScene(), false);
      camera2D.setScene(_scene.graphCamera().getScene());
      camera2D.initGl();
      Gl2DRect rect(height, 0, 0, width, textureId);
      rect.draw(0, &camera2D);
    };

    auto blitFramebuffer = [width, height](QOpenGLFramebufferObject *target,
                                           QOpenGLFramebufferObject *source) {
      QRect fbRect(0, 0, width, height);
      QOpenGLFramebufferObject::blitFramebuffer(target, fbRect, source, fbRect);
    };

    computeInteractor();

    createFramebuffers(width, height);

    // render the graph in the antialiased framebuffer.
    _glFrameBufAntialiased->bind();
    if (options.testFlag(RenderScene)) {
      _scene.draw();
      // copy antialiased rendered scene into a texture compatible framebuffer
      blitFramebuffer(_glFrameBufSceneTexture, _glFrameBufAntialiased);
    } else {
      _scene.initGlParameters();
      // draw rendered scene from texture
      renderTexture(_sceneTextureId);
    }
    // draw current interactor
    _scene.setClearBufferAtDraw(false);
    _scene.initGlParameters();
    _scene.setClearBufferAtDraw(true);
    drawInteractor();
    _glFrameBufAntialiased->release();

    // copy antialiased rendered scene and interactor into a texture compatible framebuffer
    blitFramebuffer(_glFrameBufSceneAndInteractorTexture, _glFrameBufAntialiased);

    // restore internal QOpenGLWidget framebuffer binding
    makeCurrent();
    glBindFramebuffer(GL_FRAMEBUFFER, drawFboId);

    // draw rendered scene and interactor from texture
    _scene.initGlParameters();
    renderTexture(_sceneAndInteractorTextureId);

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
void GlWidget::computeInteractor() {
  if (!_view) {
    return;
  }

  auto *interactor = dynamic_cast<GLInteractorComposite *>(_view->currentInteractor());

  if (interactor == nullptr) {
    return;
  }

  interactor->compute(this);
}
//==================================================
void GlWidget::drawInteractor() {
  if (!_view) {
    return;
  }

  auto *interactor = dynamic_cast<GLInteractorComposite *>(_view->currentInteractor());

  if (!interactor) {
    return;
  }

  glDisable(GL_STENCIL_TEST);
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
#if defined(Q_OS_MAC)
  _glFrameBufAntialiased->bind();
#endif

  bool pickedEntity = _scene.selectEntities(
      static_cast<RenderingEntitiesFlag>(RenderingEntities | RenderingWithoutRemove),
      screenToViewport(x), screenToViewport(y), screenToViewport(width), screenToViewport(height),
      layer, pickedEntities);

#if defined(Q_OS_MAC)
  _glFrameBufAntialiased->release();
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
#if defined(Q_OS_MAC)
  _glFrameBufAntialiased->bind();
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

#if defined(Q_OS_MAC)
  _glFrameBufAntialiased->release();
#endif
}
//=====================================================
bool GlWidget::pickNodesEdges(const int x, const int y, SelectedEntity &selectedEntity,
                              GlLayer *layer, bool pickNodes, bool pickEdges) {
  makeCurrent();
#if defined(Q_OS_MAC)
  _glFrameBufAntialiased->bind();
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

#if defined(Q_OS_MAC)
  _glFrameBufAntialiased->release();
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
  QOpenGLFramebufferObject frameBuf{width, height, fboFormat};

  if (frameBuf.isValid()) {
    frameBuf.bind();

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

    computeInteractor();
    _scene.draw();
    drawInteractor();
    frameBuf.release();

    resultImage = frameBuf.toImage();

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

  // The QOpenGLFramebufferObject has an image format of QImage::Format_ARGB32_Premultiplied
  // so need to create an image from original data with the right format QImage::Format_ARGB32.
  return QImage(resultImage.constBits(), resultImage.width(), resultImage.height(),
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
  _keepPointOfViewOnSubgraphChanging = k;
}

bool GlWidget::keepScenePointOfViewOnSubgraphChanging() const {
  return _keepPointOfViewOnSubgraphChanging;
}

GlGraphRenderingParameters &GlWidget::renderingParameters() {
  return _scene.glGraph()->renderingParameters();
}

GlGraphInputData *GlWidget::inputData() const {
  return _scene.glGraph()->inputData();
}

void GlWidget::zoomAndPanAnimation(const tlp::BoundingBox &boundingBox, const double duration,
                                   AdditionalGlSceneAnimation *additionalAnimation) {
  BoundingBox bb = boundingBox;
  if (!boundingBox.isValid()) {
    GlGraphInputData *inputData = scene()->glGraph()->inputData();
    GlBoundingBoxSceneVisitor bbVisitor(inputData);
    scene()->getLayer("Main")->acceptVisitor(&bbVisitor);
    bb = bbVisitor.getBoundingBox();
  }
  QtGlSceneZoomAndPanAnimator zoomAndPan(this, bb, duration);
  if (additionalAnimation) {
    zoomAndPan.setAdditionalGlSceneAnimation(additionalAnimation);
  }
  zoomAndPan.animateZoomAndPan();
}
}
