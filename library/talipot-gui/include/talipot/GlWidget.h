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

#ifndef TALIPOT_GL_MAIN_WIDGET_H
#define TALIPOT_GL_MAIN_WIDGET_H

#include <QOpenGLWidget>
#include <QWindow>

#include <talipot/config.h>
#include <talipot/GlScene.h>
#include <talipot/Graph.h>

class QOpenGLFramebufferObject;

namespace tlp {

class AdditionalGlSceneAnimation;
class View;
class GlCompositeHierarchyManager;

/** @ingroup OpenGL
 * @brief This widget provide a simple system to visualize data/graph with OpenGL 3D engine
 *
 * This widget is an interface between Qt Widget system and talipot OpenGL engine
 * The central object of GlWidget is the GlScene member
 * @see GlScene
 *
 * To use this class you have to :
 *   - create a GlWidget
 *   - get the GlScene with scene() function
 *   - add GlLayer and GlEntity to this scene
 *   - call centerScene() to compute a good GlCamera
 *   - see the result
 *
 * @see GlLayer
 * @see GlEntity
 *
 *
 * If you only want to visualize a graph, you can call the setGraph function
 *
 *
 * After scene construction you can perform some operation on GlWidget :
 * - Selection with selectGlEntities()
 * - Image output with getImage(), createPicture()
 * - others operation on GlScene and QOpenGLWidget
 */
class TLP_QT_SCOPE GlWidget : public QOpenGLWidget {
  Q_OBJECT

public:
  /**
   * @brief Configure the rendering process ( see render function)
   * @see render
   **/
  enum RenderingOption {
    RenderScene = 0x1, /** Force to render the graph even if there is a previous buffered render.
                          You need to call this option if the graph is updated to regenerate the
                          buffer. If not set try to use the last buffered graph render, if there is
                          no valid buffer this flag is forced. **/
    SwapBuffers = 0x2  /** Call the swapBuffer functions at the end of the rendering process. If the
                          disabled  it's up to you to call the swapBuffer function. **/
  };
  Q_DECLARE_FLAGS(RenderingOptions, RenderingOption)

  /**
   * @brief Constructor of GlWidget
   *
   * Create a GlWidget with the GlScene associated to it
   * @param parent Qt Widget parent system
   * @param view if you want to link this GlWidget to a view : use this parameter
   */
  GlWidget(QWidget *parent = nullptr, View *view = nullptr);
  ~GlWidget() override;

  /**
   * @brief Get the GlScene of this GlWidget
   * You have to add yours GlLayer and GlEntity to this GlScene
   * At the construction this GlScene is empty
   * @see GlScene
   * @see GlScene::createLayer(const std::string &name)
   * @see GlLayer::addGlEntity(GlEntity *entity,const std::string& name)
   */
  GlScene *scene() {
    return &_scene;
  }

  GlGraphRenderingParameters &renderingParameters();

  GlGraphInputData *inputData() const;

  /** @brief Select nodes and edges in a region of the screen
   *
   *  Select all nodes and edges lying in the area of the screen of given width and height,
   *  and with its upper-left corner at (x,y)
   *  @param selectedNodes filled by the method with the nodes found in the region
   *  @param selectedEdges filled by the method with the edges found in the region
   *  @param layer specify the layer in which to perform the picking
   *  @param pickNodes enable or disable the picking of nodes
   *  @param pickEdges enable or disable the picking of edges
   */
  void pickNodesEdges(const int x, const int y, const int width, const int height,
                      std::vector<SelectedEntity> &selectedNodes,
                      std::vector<SelectedEntity> &selectedEdges, GlLayer *layer = nullptr,
                      bool pickNodes = true, bool pickEdges = true);

  /** @brief Select a node or edge at a screen point
   *  Try to select at point (x,y) a node in the first place then if no result try to select an edge
   *  @param type tells what has been found: ElementType::NODE, ElementType::EDGE
   *  @param layer specify the layer in which to perform the picking
   *  @param pickNodes enable or disable the picking of nodes
   *  @param pickEdges enable or disable the picking of edges
   *  @return true if something has been found, false otherwise
   */
  bool pickNodesEdges(const int x, const int y, SelectedEntity &selectedEntity,
                      GlLayer *layer = nullptr, bool pickNodes = true, bool pickEdges = true);

  /**
   * @brief convert a screen measure into a viewport measure
   * @param a measure in screen coordinates specified as an integer
   * @return the converted measure in viewport coordinates as an integer
   */
  int screenToViewport(int l) {
    return l * devicePixelRatio();
  }

  /**
   * @brief convert a screen measure into a viewport measure
   * @param a measure in screen coordinates specified as a double
   * @return the converted measure in viewport coordinates as a double
   */
  double screenToViewport(double l) {
    return l * devicePixelRatio();
  }

  float screenToViewport(float l) {
    return l * devicePixelRatio();
  }

  /**
   * @brief convert a screen point into a viewport point
   * @param a point in screen coordinates
   * @return the converted point in viewport coordinates
   */
  Coord screenToViewport(const Coord &point) {
    qreal dpr = devicePixelRatio();
    return Coord(point.x() * dpr, point.y() * dpr);
  }

  /**
   * @brief convert a viewport measure into a screen measure
   * @param a measure in viewport coordinates specified as a double
   * @return the converted measure in screen coordinates as a double
   */
  double viewportToScreen(double l) {
    return l / devicePixelRatio();
  }

  /**
   * @brief convert a viewport point into a screen point
   * @param a point in viewport coordinates
   * @return the converted point in screen coordinates
   */
  Coord viewportToScreen(const Coord &point) {
    qreal dpr = devicePixelRatio();
    return Coord(point.x() / dpr, point.y() / dpr);
  }

  /**
   * @brief Compute texture size in power of two with given height and width
   * For example if you set width to 94 and height to 256, this function set textureRealWidth to 128
   * and textureRealHeight to 256
   */
  static void getTextureRealSize(int width, int height, int &textureRealWidth,
                                 int &textureRealHeight);

  /**
   * @brief Take a snapshot of the Widget and put it in a picture
   * @param width size
   * @param height size
   * @param center if true this function call a centerScene() before picture output
   */
  void createPicture(const std::string &pictureName, int width, int height, bool center = true);

  /**
   * Take a snapshot of the Widget and return it
   * @param width size
   * @param height size
   * @param center if true this function call a centerScene() before picture output
   * @param format indicates the format of the created image
   */
  QImage createPicture(int width, int height, bool center = true,
                       QImage::Format format = QImage::Format_RGB32);

  /**
   * @brief Function to do picking on entities in a screen region
   * It just calls selectEntities on the GlScene instance.
   * @param x screen coordinates
   * @param y screen coordinates
   * @param width screen size
   * @param height screen size
   * @param pickedEntities filled with entity under the selection screen rectangle
   * @param layer if you want to do the selection only on one GlLayer
   */
  bool pickGlEntities(const int x, const int y, const int width, const int height,
                      std::vector<SelectedEntity> &pickedEntities, GlLayer *layer = nullptr);
  /**
   * @brief Function to do picking on entities.
   * It just calls selectEntities on the GlScene instance with a small window of twelve pixels.
   * @param x screen coordinates
   * @param y screen coordinates
   * @param pickedEntities filled with entity under the selection screen rectangle
   * @param layer if you want to do the selection only on one GlLayer
   */
  bool pickGlEntities(const int x, const int y, std::vector<SelectedEntity> &pickedEntities,
                      GlLayer *layer = nullptr);

  /**
   * Override default makeCurrent/doneCurrent behavior to activate deactivate
   * adequate OpenGL context based on the QOpenGLWidget visibility
   */
  void makeCurrent();
  void doneCurrent();

  /**
   * Resize openGL view
   */
  void resizeGL(int w, int h) override;

  /**
   * Compute current interactor before drawing
   */
  void computeInteractor();

  /**
   * Draw current interactor
   */
  void drawInteractor();

  /**
   * @brief This function performs all the rendering process of the graph.
   * Use this function only for advanced purpose, if you want to perform simple rendering use the
   *draw or redraw functions instead.
   * @param options Configure the rendering process, see the RenderingOption documentation for more
   *information on each rendering option effect.
   * @see RenderingOption
   * @param checkVisibility If check visibility is set as true : the engine check if GlWidget
   *QWidget is visible. If set at false : the engine render the scene in all cases
   **/
  void render(RenderingOptions options = RenderingOptions(RenderScene | SwapBuffers),
              bool checkVisibility = true);

  /**
   * @brief Specify if the scene point of view must be kept
   * when changing between graphs belonging to the same hierarchy
   */
  void setKeepScenePointOfViewOnSubgraphChanging(bool);
  /**
   * @brief Returns if the scene point of view must be kept
   * when changing between graphs belonging to the same hierarchy
   */
  bool keepScenePointOfViewOnSubgraphChanging() const;

  const std::string &getSceneTextureId() const {
    return _sceneTextureId;
  }

  /**
   * @brief Do a zoom and pan animation
   * @param boundingBox the bounding box in scene coordinates on which the view
   * has to be zoomed and panned. If it is not valid, the scene bounding box will be used.
   * At the end of the animation, the view will be zoomed and centered on the content of that
   * bounding box.
   * @param duration the animation duration in msecs
   */
  virtual void zoomAndPanAnimation(const tlp::BoundingBox &boundingBox,
                                   const double duration = 1000,
                                   AdditionalGlSceneAnimation *additionalAnimation = nullptr);

  // devicePixelRatio() must be overridden because the inherited QWidget
  // method always returns 1 when the widget is no attached to a window
  qreal devicePixelRatio() const {
    if (_windows) {
      return _windows->devicePixelRatio();
    } else {
      return QOpenGLWidget::devicePixelRatio();
    }
  }

private:
  void createFramebuffers(int width, int height);
  void deleteFramebuffers();

  GlScene _scene;
  QRegion _visibleArea;
  View *_view;
  int _widthStored;
  int _heightStored;
  QOpenGLFramebufferObject *_glFrameBufAntialiased, *_glFrameBufSceneTexture,
      *_glFrameBufSceneAndInteractorTexture;
  bool _keepPointOfViewOnSubgraphChanging;
  std::string _sceneTextureId;
  std::string _sceneAndInteractorTextureId;
  QWidget *_windows;

  static bool inRendering;

public slots:
  /**
   * Draw the GlScene and the interactors
   */
  void draw(bool graphChanged = true);
  /**
   * This function is given for optimisation purpose only. If the hardware enable it,
   * it enables to redraw only the Augmented display and the interactors and not the graph
   * it is really useful for interactors such as zoom box etc..
   * Warning, if you change the graph or the properties of elements (Colors, size, etc...)
   * applying that function will not display the change, in that case, use the draw function.
   */
  void redraw();

  void closeEvent(QCloseEvent *e) override;

  /**
   * @brief Convenience function that call center function on the current scene, apply a zoom (if
   *needed) and draw the view.
   * Same thing than
   * @code
   * scene()->centerScene();
   * scene()->zoomFactor();
   * draw();
   * @endcode
   **/
  void centerScene(bool graphChanged = false, float zoomFactor = 1.0);

  void emitGraphChanged();

protected slots:
  void paintEvent(QPaintEvent *) override;

signals:
  /**
   * This signal is emit when the GlWidget will be deleted
   */
  void closing(GlWidget *, QCloseEvent *);

  /**
   * This signal is emit when GlWidget::redraw() is call
   */
  void viewRedrawn(GlWidget *glWidget);
  /**
   * This signal is emit when GlWidget::draw() is call
   */
  void viewDrawn(GlWidget *glWidget, bool graphChanged);

  void glResized(int w, int h);

  void graphChanged();
};
}

#endif // TALIPOT_GL_MAIN_WIDGET_H
