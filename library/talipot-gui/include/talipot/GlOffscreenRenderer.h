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

#ifndef TALIPOT_GL_OFFSCREEN_RENDERER_H
#define TALIPOT_GL_OFFSCREEN_RENDERER_H

#include <talipot/config.h>
#include <talipot/Coord.h>
#include <talipot/GlScene.h>
#include <talipot/OpenGlIncludes.h>
#include <talipot/Singleton.h>

#include <QImage>

class QOpenGLContext;
class QOffscreenSurface;
class QOpenGLFramebufferObject;

namespace tlp {

class GlEntity;
class GlGraph;
class GlWidget;

class GlOffscreenRenderer;
DECLARE_DLL_TEMPLATE_INSTANCE(Singleton<GlOffscreenRenderer>, TLP_QT_TEMPLATE_DECLARE_SCOPE)

/**
 * @brief Render a scene in an image or in a texture.
 *
 * Here is an example to render a graph in a QImage to use it as preview.
 * @code
 * //Get the renderer
 * glOffscreenRenderer *glOffscreenRenderer = GlOffscreenRenderer::instance();
 * //Define the viewport size. Needed to initialize the offscreen rederer.
 * glOffscreenRenderer->setViewPortSize(200,200);
 * //Erase old elements
 * glOffscreenRenderer->clearScene();
 * //Change the background color of the scene to white
 * glOffscreenRenderer->setSceneBackgroundColor(Color(255,255,255,255));
 * //Add
 * //Center and render the scene.
 * glOffscreenRenderer->renderScene(true);
 * //Get the result
 * QImage preview = glOffscreenRenderer->getGLTexture(true);
 * @endcode
 **/
class TLP_QT_SCOPE GlOffscreenRenderer : public Singleton<GlOffscreenRenderer> {

  friend class Singleton<GlOffscreenRenderer>;

public:
  ~GlOffscreenRenderer();

  /**
   * @brief Define the viewport size.
   **/
  void setViewPortSize(const uint viewPortWidth, const uint viewPortHeight);
  uint getViewportWidth();
  uint getViewportHeight();
  bool frameBufferOk() const;

  GlScene *scene() {
    return &_scene;
  }
  void setZoomFactor(double zoomFactor) {
    this->zoomFactor = zoomFactor;
  }
  void setCameraCenter(const Coord &cameraCenter) {
    this->cameraCenter = cameraCenter;
  }

  void setSceneBackgroundColor(const Color &color);
  /**
   * @brief Add an entity to the scene. The scene become the owner of the object.
   **/
  void addGlEntityToScene(GlEntity *entity);
  /**
   * @brief Add a graph composite to the scene. The scene become the owner of the object.
   **/
  void addGlGraphToScene(GlGraph *glGraph);

  /**
   * @brief Add a graph to the scene. Just create a new GlGraph and call GlGraph.
   **/
  void addGraphToScene(Graph *graph);

  /**
   * @brief Delete all the elements of the scene and clear it.
   **/
  void clearScene(bool deleteGlEntities = false);

  /**
   * @brief Render the scene in a buffer. You need to call this function before getting the result
   *with getImage or getGlTexture.
   **/
  void renderScene(const bool centerScene = true, const bool antialiased = false);

  void renderExternalScene(GlScene *scene, const bool antialiased = false);

  void renderGlWidget(GlWidget *glWidget, bool redrawNeeded = true);

  /**
   * @brief Generate a QImage from the scene. You need to call the renderScene function before this
   *function.
   **/
  QImage getImage();
  /**
   * @brief Generate an open gl texture from the scene. You need to call the renderScene function
   *before this function.
   **/
  GLuint getGLTexture(const bool generateMipMaps = false);

  QOpenGLContext *getOpenGLContext();
  void makeOpenGLContextCurrent();
  void doneOpenGLContextCurrent();

private:
  GlOffscreenRenderer();

  void initFrameBuffers(const bool antialiased);

  QOpenGLContext *glContext;
  QOffscreenSurface *offscreenSurface;

  uint vPWidth, vPHeight;
  QOpenGLFramebufferObject *glFrameBuf, *glFrameBuf2;
  GlScene _scene;
  GlLayer *mainLayer;
  uint entitiesCpt;
  double zoomFactor;
  Coord cameraCenter;
  bool antialiasedFbo;
};
}

#endif // TALIPOT_GL_OFFSCREEN_RENDERER_H
