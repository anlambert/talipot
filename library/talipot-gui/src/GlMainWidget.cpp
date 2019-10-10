/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/GlMainWidget.h>

#include <QOpenGLFramebufferObject>
#include <QGLFormat>
#include <QWindow>

#include <talipot/TlpQtTools.h>
#include <talipot/Settings.h>
#include <talipot/Graph.h>
#include <talipot/GlTools.h>
#include <talipot/GlTextureManager.h>
#include <talipot/Gl2DRect.h>
#include <talipot/GlQuadTreeLODCalculator.h>
#include <talipot/GLInteractor.h>
#include <talipot/GlGraphComposite.h>
#include <talipot/QGlBufferManager.h>
#include <talipot/Interactor.h>
#include <talipot/GlCompositeHierarchyManager.h>
#include <talipot/GlVertexArrayManager.h>
#include <talipot/View.h>
#include <talipot/Camera.h>
#include <talipot/OpenGlConfigManager.h>

using namespace std;

namespace tlp {

QGLWidget *GlMainWidget::firstQGLWidget = nullptr;
bool GlMainWidget::inRendering = false;

//==================================================
static void setRasterPosition(unsigned int x, unsigned int y) {
  GL_THROW_ON_ERROR();
  float val[4];
  unsigned char tmp[10];
  glGetFloatv(GL_CURRENT_RASTER_POSITION, val);
  glBitmap(0, 0, 0, 0, -val[0] + x, -val[1] + y, tmp);
  glGetFloatv(GL_CURRENT_RASTER_POSITION, val);
  GL_THROW_ON_ERROR();
}
//==================================================
static QGLFormat GlInit() {
  QGLFormat tmpFormat = QGLFormat::defaultFormat();
  tmpFormat.setDirectRendering(true);
  tmpFormat.setDoubleBuffer(true);
  tmpFormat.setAccum(false);
  tmpFormat.setStencil(true);
  tmpFormat.setOverlay(false);
  tmpFormat.setDepth(true);
  tmpFormat.setRgba(true);
  tmpFormat.setAlpha(true);
  tmpFormat.setOverlay(false);
  tmpFormat.setStereo(false);
  tmpFormat.setSampleBuffers(true);

  static int maxSamples = -1;

  if (maxSamples < 0) {
    maxSamples = 0;
    GlMainWidget::getFirstQGLWidget()->makeCurrent();
    maxSamples = OpenGlConfigManager::maxNumberOfSamples();
    GlMainWidget::getFirstQGLWidget()->doneCurrent();
  }

  tmpFormat.setSamples(maxSamples);

  return tmpFormat;
}

QGLWidget *GlMainWidget::getFirstQGLWidget() {
  if (!GlMainWidget::firstQGLWidget) {
    QGLWidget *glWidget = new QGLWidget(GlInit());
    // a first QGLWidget will be created as a side effect of calling GlInit() in order to query
    // the maximum number of samples available, so we must delete it to avoid a memory leak
    delete GlMainWidget::firstQGLWidget;
    GlMainWidget::firstQGLWidget = glWidget;
    assert(GlMainWidget::firstQGLWidget->isValid());
  }

  return GlMainWidget::firstQGLWidget;
}

void GlMainWidget::clearFirstQGLWidget() {
  if (GlMainWidget::firstQGLWidget) {
    delete GlMainWidget::firstQGLWidget;
  }
}

//==================================================
GlMainWidget::GlMainWidget(QWidget *parent, View *view)
    : QGLWidget(GlInit(), parent, getFirstQGLWidget()), scene(new GlQuadTreeLODCalculator),
      view(view), widthStored(0), heightStored(0), useFramebufferObject(false), glFrameBuf(nullptr),
      glFrameBuf2(nullptr), keepPointOfViewOnSubgraphChanging(false), advancedAntiAliasing(false) {
  assert(this->isValid());
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
  grabGesture(Qt::PinchGesture);
  grabGesture(Qt::PanGesture);
  grabGesture(Qt::SwipeGesture);
  renderingStore = nullptr;
  getScene()->setViewOrtho(Settings::instance().isViewOrtho());
  OpenGlConfigManager::initExtensions();
}
//==================================================
GlMainWidget::~GlMainWidget() {
  delete glFrameBuf;
  delete glFrameBuf2;
  delete[] renderingStore;
}
//==================================================
void GlMainWidget::paintEvent(QPaintEvent *) {
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
void GlMainWidget::closeEvent(QCloseEvent *e) {
  emit closing(this, e);
}
//==================================================
void GlMainWidget::setupOpenGlContext() {
  assert(context()->isValid());
  makeCurrent();
}
//==================================================
void GlMainWidget::createRenderingStore(int width, int height) {

  useFramebufferObject =
      advancedAntiAliasing && QOpenGLFramebufferObject::hasOpenGLFramebufferBlit();

  if (useFramebufferObject && (!glFrameBuf || glFrameBuf->size().width() != width ||
                               glFrameBuf->size().height() != height)) {
    makeCurrent();
    deleteRenderingStore();
    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    fboFormat.setSamples(OpenGlConfigManager::maxNumberOfSamples());
    glFrameBuf = new QOpenGLFramebufferObject(width, height, fboFormat);
    glFrameBuf2 = new QOpenGLFramebufferObject(width, height);
    useFramebufferObject = glFrameBuf->isValid();
    widthStored = width;
    heightStored = height;
  }

  if (!useFramebufferObject) {
    int size = width * height;

    if (renderingStore == nullptr || (size > (widthStored * heightStored))) {
      deleteRenderingStore();
      renderingStore = new unsigned char[width * height * 4];
      widthStored = width;
      heightStored = height;
    }
  }
}
//==================================================
void GlMainWidget::deleteRenderingStore() {
  delete glFrameBuf;
  glFrameBuf = nullptr;
  delete glFrameBuf2;
  glFrameBuf2 = nullptr;
  delete[] renderingStore;
  renderingStore = nullptr;
}

//==================================================
void GlMainWidget::render(RenderingOptions options, bool checkVisibility) {

  if ((isVisible() || !checkVisibility) && !inRendering) {

    //  assert(contentsRect().width() != 0 && contentsRect().height() != 0);
    // Begin rendering process
    inRendering = true;
    makeCurrent();

    // Get the content width and height
    int width = screenToViewport(contentsRect().width());
    int height = screenToViewport(contentsRect().height());

    // If the rendering store is not valid need to regenerate new one force the RenderGraph flag.
    if (widthStored != width || heightStored != height) {
      options |= RenderScene;
    }

    computeInteractors();

    if (options.testFlag(RenderScene) || renderingStore == nullptr) {
      createRenderingStore(width, height);

      if (useFramebufferObject) {
        glFrameBuf->bind();
      }

      // Render the graph in the frame buffer.
      scene.draw();

      if (useFramebufferObject) {
        glFrameBuf->release();
        QRect fbRect(0, 0, width, height);
        QOpenGLFramebufferObject::blitFramebuffer(glFrameBuf2, fbRect, glFrameBuf, fbRect);
      }
    } else {
      scene.initGlParameters();
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);

    if (useFramebufferObject) {
      QRect fbRect(0, 0, width, height);
      QOpenGLFramebufferObject::blitFramebuffer(nullptr, fbRect, glFrameBuf2, fbRect);
    } else {
      if (options.testFlag(RenderScene)) {
        // Copy the back buffer (containing the graph render) in the rendering store to reuse it
        // later.
        glReadBuffer(GL_BACK);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, renderingStore);
        glFlush();
      } else {
        // Copy the rendering store into the back buffer : restore the last graph render.
        glDrawBuffer(GL_BACK);
        setRasterPosition(0, 0);

        if (renderingStore != nullptr)
          glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, renderingStore);
      }
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_LIGHTING);

    // Draw interactors and foreground entities.
    drawInteractors();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_STENCIL_TEST);

    if (options.testFlag(SwapBuffers)) {
      swapBuffers();
    }

    inRendering = false;
  }
}
//==================================================
void GlMainWidget::redraw() {
  render(SwapBuffers);
  emit viewRedrawn(this);
}
//==================================================
void GlMainWidget::draw(bool graphChanged) {
  render(RenderingOptions(RenderScene | SwapBuffers));
  emit viewDrawn(this, graphChanged);
}
//==================================================
void GlMainWidget::computeInteractors() {
  if (!view)
    return;

  GLInteractorComposite *interactor =
      dynamic_cast<GLInteractorComposite *>(view->currentInteractor());

  if (interactor == nullptr)
    return;

  interactor->compute(this);
}
//==================================================
void GlMainWidget::drawInteractors() {
  if (!view)
    return;

  GLInteractorComposite *interactor =
      dynamic_cast<GLInteractorComposite *>(view->currentInteractor());

  if (!interactor)
    return;

  interactor->draw(this);
}
//==================================================
// QGLWidget slots
//==================================================
void GlMainWidget::resizeGL(int w, int h) {

  if (w == 0 || h == 0) {
    return;
  }

  int width = contentsRect().width();
  int height = contentsRect().height();

  deleteRenderingStore();

  scene.setViewport(0, 0, screenToViewport(width), screenToViewport(height));

  emit glResized(w, h);
}
//==================================================
void GlMainWidget::makeCurrent() {
  if (isVisible()) {
    QGLWidget::makeCurrent();
    GlTextureManager::changeContext(reinterpret_cast<uintptr_t>(GlMainWidget::firstQGLWidget));
    int width = contentsRect().width();
    int height = contentsRect().height();
    scene.setViewport(0, 0, screenToViewport(width), screenToViewport(height));
  }
}
//==================================================
bool GlMainWidget::pickGlEntities(const int x, const int y, const int width, const int height,
                                  std::vector<SelectedEntity> &pickedEntities, GlLayer *layer) {
  makeCurrent();
  return scene.selectEntities(
      static_cast<RenderingEntitiesFlag>(RenderingSimpleEntities | RenderingWithoutRemove),
      screenToViewport(x), screenToViewport(y), screenToViewport(width), screenToViewport(height),
      layer, pickedEntities);
}
//==================================================
bool GlMainWidget::pickGlEntities(const int x, const int y,
                                  std::vector<SelectedEntity> &pickedEntities, GlLayer *layer) {
  return pickGlEntities(x, y, 2, 2, pickedEntities, layer);
}
//==================================================
void GlMainWidget::pickNodesEdges(const int x, const int y, const int width, const int height,
                                  std::vector<SelectedEntity> &selectedNodes,
                                  std::vector<SelectedEntity> &selectedEdges, GlLayer *layer,
                                  bool pickNodes, bool pickEdges) {
  makeCurrent();

  if (pickNodes) {
    scene.selectEntities(
        static_cast<RenderingEntitiesFlag>(RenderingNodes | RenderingWithoutRemove),
        screenToViewport(x), screenToViewport(y), screenToViewport(width), screenToViewport(height),
        layer, selectedNodes);
  }

  if (pickEdges) {
    scene.selectEntities(
        static_cast<RenderingEntitiesFlag>(RenderingEdges | RenderingWithoutRemove),
        screenToViewport(x), screenToViewport(y), screenToViewport(width), screenToViewport(height),
        layer, selectedEdges);
  }
}
//=====================================================
bool GlMainWidget::pickNodesEdges(const int x, const int y, SelectedEntity &selectedEntity,
                                  GlLayer *layer, bool pickNodes, bool pickEdges) {
  makeCurrent();
  vector<SelectedEntity> selectedEntities;

  if (pickNodes && scene.selectEntities(
                       static_cast<RenderingEntitiesFlag>(RenderingNodes | RenderingWithoutRemove),
                       screenToViewport(x - 1), screenToViewport(y - 1), screenToViewport(3),
                       screenToViewport(3), layer, selectedEntities)) {
    selectedEntity = selectedEntities[0];
    return true;
  }

  if (pickEdges && scene.selectEntities(
                       static_cast<RenderingEntitiesFlag>(RenderingEdges | RenderingWithoutRemove),
                       screenToViewport(x - 1), screenToViewport(y - 1), screenToViewport(3),
                       screenToViewport(3), layer, selectedEntities)) {
    selectedEntity = selectedEntities[0];
    return true;
  }

  return false;
}
//=====================================================
void GlMainWidget::getTextureRealSize(int width, int height, int &textureRealWidth,
                                      int &textureRealHeight) {
  textureRealWidth = 1;
  textureRealHeight = 1;

  while (textureRealWidth <= width)
    textureRealWidth *= 2;

  while (textureRealHeight <= height)
    textureRealHeight *= 2;

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
QOpenGLFramebufferObject *GlMainWidget::createTexture(const std::string &textureName, int width,
                                                      int height) {

  makeCurrent();
  scene.setViewport(0, 0, width, height);
  scene.adjustSceneToSize(width, height);

  QOpenGLFramebufferObject *glFrameBuf = QGlBufferManager::getFramebufferObject(width, height);
  assert(glFrameBuf->size() == QSize(width, height));

  glFrameBuf->bind();

  scene.draw();
  glFrameBuf->release();

  GLuint textureId = 0;
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  unsigned char *buff = new unsigned char[width * height * 4];
  glBindTexture(GL_TEXTURE_2D, glFrameBuf->texture());
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buff);

  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buff);

  delete[] buff;

  glFrameBuf->release();

  GlTextureManager::registerExternalTexture(textureName, textureId);

  return nullptr;
}

//=====================================================
void GlMainWidget::createPicture(const std::string &pictureName, int width, int height,
                                 bool center) {
  createPicture(width, height, center).save(tlp::tlpStringToQString(pictureName));
}

//=====================================================
QImage GlMainWidget::createPicture(int width, int height, bool center, QImage::Format format) {

  QImage resultImage;

  GlMainWidget::getFirstQGLWidget()->makeCurrent();

  QOpenGLFramebufferObjectFormat fboFormat;
  fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
  fboFormat.setSamples(OpenGlConfigManager::maxNumberOfSamples());
  QOpenGLFramebufferObject *frameBuf = new QOpenGLFramebufferObject(width, height, fboFormat);
  QOpenGLFramebufferObject *frameBuf2 = new QOpenGLFramebufferObject(width, height);

  if (frameBuf->isValid() && frameBuf2->isValid()) {
    frameBuf->bind();

    int oldWidth = scene.getViewport()[2];
    int oldHeight = scene.getViewport()[3];
    vector<Camera> oldCameras;
    const vector<pair<string, GlLayer *>> &layersList = scene.getLayersList();

    if (center) {
      for (vector<pair<string, GlLayer *>>::const_iterator it = layersList.begin();
           it != layersList.end(); ++it) {
        if (!(*it).second->useSharedCamera())
          oldCameras.push_back((*it).second->getCamera());
      }
    }

    scene.setViewport(0, 0, width, height);

    if (center)
      scene.adjustSceneToSize(width, height);

    computeInteractors();
    scene.draw();
    drawInteractors();
    frameBuf->release();

    QOpenGLFramebufferObject::blitFramebuffer(frameBuf2, QRect(0, 0, width, height), frameBuf,
                                              QRect(0, 0, width, height));

    resultImage = frameBuf2->toImage();

    scene.setViewport(0, 0, oldWidth, oldHeight);

    if (center) {
      int i = 0;

      for (vector<pair<string, GlLayer *>>::const_iterator it = layersList.begin();
           it != layersList.end(); ++it) {
        if (!(*it).second->useSharedCamera()) {
          Camera &camera = (*it).second->getCamera();
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

void GlMainWidget::centerScene(bool graphChanged, float zf) {
  scene.centerScene();

  if (zf != 1)
    scene.zoomFactor(zf);

  draw(graphChanged);
}

void GlMainWidget::emitGraphChanged() {
  emit graphChanged();
}

void GlMainWidget::setKeepScenePointOfViewOnSubgraphChanging(bool k) {
  keepPointOfViewOnSubgraphChanging = k;
}

bool GlMainWidget::keepScenePointOfViewOnSubgraphChanging() const {
  return keepPointOfViewOnSubgraphChanging;
}
}