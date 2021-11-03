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

#include "MouseMagnifyingGlass.h"
#include "../../utils/PluginNames.h"
#include "../../utils/InteractorIcons.h"

#include <talipot/GlTextureManager.h>
#include <talipot/MouseInteractors.h>
#include <talipot/NodeLinkDiagramView.h>
#include <talipot/GlCircle.h>
#include <talipot/OpenGlConfigManager.h>

#include <QOpenGLFramebufferObject>
#include <QMouseEvent>

using namespace std;
using namespace tlp;

MouseMagnifyingGlassInteractor::MouseMagnifyingGlassInteractor(const tlp::PluginContext *)
    : GLInteractorComposite(interactorIcon(InteractorType::MagnifyingGlass), "Magnifying glass") {}

void MouseMagnifyingGlassInteractor::construct() {
  push_back(new MousePanNZoomNavigator());
  push_back(new MouseMagnifyingGlassInteractorComponent());
}

bool MouseMagnifyingGlassInteractor::isCompatible(const std::string &viewName) const {
  return ((viewName == NodeLinkDiagramView::viewName) ||
          (viewName == ViewName::HistogramViewName) || (viewName == ViewName::MatrixViewName) ||
          (viewName == ViewName::ParallelCoordinatesViewName) ||
          (viewName == ViewName::PixelOrientedViewName) ||
          (viewName == ViewName::ScatterPlot2DViewName));
}

PLUGIN(MouseMagnifyingGlassInteractor)

MouseMagnifyingGlassInteractorComponent::MouseMagnifyingGlassInteractorComponent()
    : fbo(nullptr), fbo2(nullptr), glWidget(nullptr), camera(nullptr), drawInteractor(false),
      radius(200), magnifyPower(2) {}

MouseMagnifyingGlassInteractorComponent::MouseMagnifyingGlassInteractorComponent(
    const MouseMagnifyingGlassInteractorComponent &mouseMagnifyingGlassInteractorComponent)
    : fbo(nullptr), fbo2(nullptr), glWidget(nullptr), camera(nullptr), drawInteractor(false) {
  boxCenter = mouseMagnifyingGlassInteractorComponent.boxCenter;
  radius = mouseMagnifyingGlassInteractorComponent.radius;
  magnifyPower = mouseMagnifyingGlassInteractorComponent.magnifyPower;
}

MouseMagnifyingGlassInteractorComponent::~MouseMagnifyingGlassInteractorComponent() {
  delete fbo;
  delete fbo2;
  GlTextureManager::deleteTexture(textureName);
}

bool MouseMagnifyingGlassInteractorComponent::eventFilter(QObject *, QEvent *e) {

  bool updateMagnifyingGlass = false;
  Coord screenCoords;

  if (e->type() == QEvent::MouseMove) {
    auto *me = static_cast<QMouseEvent *>(e);
    float x = glWidget->width() - me->pos().x();
    float y = me->pos().y();
    screenCoords = Coord(x, y, 0);
    boxCenter = camera->viewportTo3DWorld(glWidget->screenToViewport(screenCoords));

    updateMagnifyingGlass = true;
  } else if (e->type() == QEvent::Wheel) {
    auto *wheelEvent = static_cast<QWheelEvent *>(e);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    float x = glWidget->width() - wheelEvent->position().x();
    float y = wheelEvent->position().y();
#else
    float x = glWidget->width() - wheelEvent->pos().x();
    float y = wheelEvent->pos().y();
#endif
    screenCoords = Coord(x, y, 0);
    boxCenter = camera->viewportTo3DWorld(glWidget->screenToViewport(screenCoords));
    int numSteps = wheelEvent->angleDelta().y() / 120;

    if (numSteps && (wheelEvent->modifiers() == Qt::ControlModifier)) {
      updateMagnifyingGlass = true;
      radius += numSteps;

      if (radius < 1) {
        radius = 1;
      }

      delete fbo;
      delete fbo2;
      fbo = nullptr;
      fbo2 = nullptr;
    } else if (numSteps && (wheelEvent->modifiers() == Qt::ShiftModifier)) {
      updateMagnifyingGlass = true;
      magnifyPower += numSteps;

      if (magnifyPower < 1) {
        magnifyPower = 1;
      }
    }
  }

  static bool canUseFbo = QOpenGLFramebufferObject::hasOpenGLFramebufferObjects();

  if (canUseFbo && updateMagnifyingGlass) {
    generateMagnifyingGlassTexture(screenCoords);
    drawInteractor = true;
    glWidget->redraw();
    return true;
  }

  return false;
}

void MouseMagnifyingGlassInteractorComponent::generateMagnifyingGlassTexture(
    const Coord &magnifyingGlassCenterScr) {

  bool antialiased = false;

  bool canUseMultisampleFbo =
      OpenGlConfigManager::isExtensionSupported("GL_EXT_framebuffer_multisample");

  if (QOpenGLFramebufferObject::hasOpenGLFramebufferBlit() && canUseMultisampleFbo) {
    antialiased = true;
  }

  int fboSize = int(radius * 2);

  // instantiate fbo if needed
  if (fbo == nullptr) {
    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    if (antialiased) {
      fboFormat.setSamples(OpenGlConfigManager::maxNumberOfSamples());
    }

    fbo = new QOpenGLFramebufferObject(fboSize, fboSize, fboFormat);

    if (antialiased) {
      fbo2 = new QOpenGLFramebufferObject(fboSize, fboSize);
    }

    if (!antialiased) {
      GlTextureManager::registerExternalTexture(textureName, fbo->texture());
    } else {
      GlTextureManager::registerExternalTexture(textureName, fbo2->texture());
    }
  }

  const Vec4i viewport = glWidget->scene()->getViewport();

  // get the magnifying glass bounding box in screen space
  BoundingBox boundingBox;
  boundingBox[0] =
      Coord(magnifyingGlassCenterScr.getX() - radius, magnifyingGlassCenterScr.getY() - radius);
  boundingBox[1] =
      Coord(magnifyingGlassCenterScr.getX() + radius, magnifyingGlassCenterScr.getY() + radius);

  // compute the zoom factor to apply to scene's camera to get the area under the magnifying glass
  // displayed entirely in the viewport
  float bbWidthScreen = boundingBox[1][0] - boundingBox[0][0];
  float bbHeightScreen = boundingBox[1][1] - boundingBox[0][1];
  float startSize = glWidget->screenToViewport(min(glWidget->width(), glWidget->height()));
  float endSize = max(bbHeightScreen, bbWidthScreen);
  float zoomFactor = startSize / endSize;

  // backup current camera parameters
  float sceneRadiusBak = camera->getSceneRadius();
  float zoomFactorBak = camera->getZoomFactor();
  Coord eyesBak = camera->getEyes();
  Coord centerBak = camera->getCenter();
  Coord upBak = camera->getUp();

  Coord move = boxCenter - centerBak;
  camera->setCenter(camera->getCenter() + move);
  camera->setEyes(camera->getEyes() + move);
  camera->setZoomFactor(magnifyPower * zoomFactor * zoomFactorBak);

  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  // resize the viewport to the size of fbo and render the scene into this last one
  GlScene *scene = glWidget->scene();
  scene->setViewport(0, 0, fboSize, fboSize);
  fbo->bind();
  Color color = scene->getBackgroundColor();
  glClearColor(color.getRGL(), color.getGGL(), color.getBGL(), color.getAGL());
  glClear(GL_COLOR_BUFFER_BIT);
  scene->draw();
  fbo->release();

  if (antialiased) {
    QRect fboRect(0, 0, fboSize, fboSize);
    QOpenGLFramebufferObject::blitFramebuffer(fbo2, fboRect, fbo, fboRect);
  }

  // restore original camera parameters
  scene->setViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
  camera->setSceneRadius(sceneRadiusBak);
  camera->setZoomFactor(zoomFactorBak);
  camera->setEyes(eyesBak);
  camera->setCenter(centerBak);
  camera->setUp(upBak);

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glPopAttrib();

  // need to call this explicitly otherwise we have to redraw the scene
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

bool MouseMagnifyingGlassInteractorComponent::draw(GlWidget *glWidget) {
  if (!drawInteractor) {
    return false;
  }

  camera->initGl();
  Coord boxCenterScr = camera->worldTo2DViewport(boxCenter);
  Camera camera2D(camera->getScene(), false);
  camera2D.setScene(camera->getScene());
  camera2D.initGl();
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);
  glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glPushMatrix();
  glTranslatef(boxCenterScr.getX(), boxCenterScr.getY(), 0);

  Color outlineColor;
  int bgV = glWidget->scene()->getBackgroundColor().getV();

  if (bgV < 128) {
    outlineColor = Color(255, 255, 255);
  } else {
    outlineColor = Color(0, 0, 0);
  }

  GlCircle circle(Coord(0, 0, 0), radius, outlineColor, Color::White, true, true, 0.0, 60);
  circle.setOutlineSize(3);
  circle.setTextureName(textureName);
  circle.draw(0, nullptr);

  glPopMatrix();
  drawInteractor = false;
  return true;
}

void MouseMagnifyingGlassInteractorComponent::viewChanged(View *view) {
  if (view == nullptr) {
    glWidget = nullptr;
    return;
  }

  auto *glView = dynamic_cast<GlView *>(view);
  glWidget = glView->glWidget();
  radius = glWidget->screenToViewport(glWidget->width()) / 4;
  camera = &glWidget->scene()->getLayer("Main")->getCamera();

  if (!glWidget->hasMouseTracking()) {
    glWidget->setMouseTracking(true);
  }

  ostringstream oss;
  oss << "magnifyingglass" << reinterpret_cast<uintptr_t>(glWidget);
  textureName = oss.str();
}
