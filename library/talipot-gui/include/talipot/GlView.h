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

#ifndef TALIPOT_GL_MAIN_VIEW_H
#define TALIPOT_GL_MAIN_VIEW_H

#include <talipot/ViewWidget.h>
#include <talipot/GlWidget.h>

class QGraphicsProxyWidget;
class QAction;
class QPushButton;
class QRectF;

namespace tlp {
class GlOverviewGraphicsItem;
class SceneConfigWidget;
class SceneLayersConfigWidget;
class QuickAccessBar;
class ViewActionsManager;

/**
 * @ingroup Plugins
 *
 * @brief An abstract view that displays a GlWidget as its central widget.
 *
 * The GlView subclasses ViewWidget and always uses a GlWidget as the central widget of the
 panel. It also adds the following features:
 * @list
 * @li An overview of the scene that can be toggled on or off.
 * @li Some configuration widgets that modify the rendering parameters.
 * @li A quick access bar widget that allows the user to quickly modify some of the most used
 rendering parameters and graph properties (nodes color, edges display, etc)
 * @li The possibility to make snapshots of the current scene
 * @endlist
 *
 * Subclassing GlView means you will only want to display graphs in a single GlWidget.
 Switching the central widget can only be achieved from the ViewWidget class.
 *
 * @warning It is strongly unadvised to re-implement methods already implemented into tlp::View or
 tlp::ViewWidget. If you have to add custom behavior to those method, make sure to call the
 upper-class methods first:
 @code
 void MyView::setupWidget() { // Where MyView is a subclass of tlp::GlView
   GlView::setupWidget(); // call this first
   // insert custom behavior here
 }
 @endcode

 * @see tlp::ViewWidget
 */
class TLP_QT_SCOPE GlView : public tlp::ViewWidget {

  Q_OBJECT

  tlp::GlWidget *_glWidget;
  tlp::GlOverviewGraphicsItem *_overviewItem;
  tlp::ViewActionsManager *_viewActionsManager;

  QPushButton *_showOvButton, *_showQabButton;

protected:
  bool needQuickAccessBar, _needTooltipAndUrlManager;
  QGraphicsProxyWidget *_quickAccessBarItem;
  tlp::QuickAccessBar *_quickAccessBar;
  tlp::SceneConfigWidget *_sceneConfigurationWidget;
  tlp::SceneLayersConfigWidget *_sceneLayersConfigurationWidget;

public:
  enum OverviewPosition {
    OVERVIEW_TOP_LEFT,
    OVERVIEW_TOP_RIGHT,
    OVERVIEW_BOTTOM_LEFT,
    OVERVIEW_BOTTOM_RIGHT
  };

  GlView(bool needTooltipAndUrlManager = false);
  ~GlView() override;
  tlp::GlWidget *glWidget() const;
  QList<QWidget *> configurationWidgets() const override;
  bool overviewVisible() const;
  bool quickAccessBarVisible() const;
  QPixmap snapshot(const QSize &outputSize = QSize()) const override;

  void setOverviewPosition(const OverviewPosition &position);
  OverviewPosition overviewPosition() const;

  void setUpdateOverview(bool updateOverview);
  bool updateOverview() const;

  void setState(const tlp::DataSet &) override;
  tlp::DataSet state() const override;

  /** @brief Pick a node or an edge at a view position
   *  First take a look at a node located at (x,y); if none is found, take a look at an edge
   *  @param x the x coordinate of the position
   *  @param y the y coordinate of the position
   *  @param n on return holds the node found under the (x,y) position, n.isValid() return false if
   * none has been found
   *  @param e on return holds the edge found under the (x,y) position, e.isValid() return false if
   * none has been found
   *  @param pickNode enable or disable the node picking
   *  @param pickEdge enable or disable the edge picking
   *  @return true if something has been found, false otherwise
   */
  bool pickNodeEdge(const int x, const int y, tlp::node &n, tlp::edge &e, bool pickNode = true,
                    bool pickEdge = true) const;

  /**
   * @brief Rotate the view camera by (x,y,z)
   * @param x rotation around the X axis in degree
   * @param y rotation around the Y axis in degree
   * @param z rotation around Z axis in degree
   */
  void rotateCamera(int x, int y, int z) const {
    glWidget()->scene()->rotateCamera(x, y, z);
  }

  /**
   * @brief Translate the view camera by (x,y,z)
   * @param x offset along the X axis
   * @param y offset along the Y axis
   * @param z offset along the Z axis
   */
  void translateCamera(int x, int y, int z) const {
    glWidget()->scene()->translateCamera(x, y, z);
  }

  /**
   * @brief Return the 3D world position for the given view position
   */
  Coord viewToWorld(const Coord &vpos) const {
    return glWidget()->scene()->graphCamera().viewportTo3DWorld(vpos);
  }

  /**
   * @brief Return the view position for the given 3D position
   */
  Coord worldToView(const Coord &wpos) const {
    return glWidget()->scene()->graphCamera().worldTo2DViewport(wpos);
  }

  /**
   * @brief Zoom by step to the given (x,y) view position
   * @param step of zoom
   */
  void zoomXY(int step, const int x, const int y) const {
    glWidget()->scene()->zoomXY(step, x, y);
  }

  /**
   * @brief Zoom by step
   * @param step of zoom
   */
  void zoom(int step) const {
    glWidget()->scene()->zoom(step);
  }

  /**
   * @brief Zoom by factor
   * @param factor of zoom
   */
  void zoomFactor(float factor) const {
    glWidget()->scene()->zoomFactor(factor);
  }

public slots:
  /**
   * @brief Calls GlWidget::draw();
   */
  void draw() override;

  /**
   * @brief Calls GlWidget::redraw();
   */
  void redraw();

  /**
   * @brief Calls GlWidget::redraw();
   */
  void refresh() override;

  /**
   * @brief Force the overview to be redrawn. Since GlView already detects graph's
   * modifications, this method should not be called manually to avoid extra rendering.
   */
  virtual void drawOverview(bool generatePixmap = true);

  /**
   * @brief Centers the scene's camera
   */
  void centerView(bool graphChanged = false) override;

  /**
   * @brief Toggles the overview on or off
   */
  void setOverviewVisible(bool);

  /**
   * @brief allows to control the quick access bar visibility
   */
  void setQuickAccessBarVisible(bool);

  /**
   * @brief Toggles the orthogonal projection on or off, then draws
   */
  void setViewOrtho(bool);

  /**
   * @brief Force the settings set in the configuration widgets to be re-applied.
   */
  void applySettings() override;

  void undoCallback() override;

  static bool getNodeOrEdgeAtViewportPos(GlWidget *glw, int x, int y, node &n, edge &e);

  bool getNodeOrEdgeAtViewportPos(int x, int y, node &n, edge &e) const override {
    return getNodeOrEdgeAtViewportPos(_glWidget, x, y, n, e);
  }

  /**
   * @brief Return the glWidget current display in the RGB888 format.
   */
  QImage getRGBImage() const;

protected slots:
  virtual void glViewDrawn(bool graphChanged);
  virtual void sceneRectChanged(const QRectF &);
  void fillContextMenu(QMenu *menu, const QPointF &) override;

protected:
  void setupWidget() override;
  void assignNewGlWidget(GlWidget *glWidget, bool deleteOldGlWidget = true);
  bool eventFilter(QObject *obj, QEvent *event) override;

  tlp::GlOverviewGraphicsItem *overviewItem() const;
  void updateShowOverviewButton();
  void updateShowQuickAccessBarButton();
  virtual QuickAccessBar *getQuickAccessBarImpl();

  OverviewPosition _overviewPosition;

  bool _updateOverview;
};
}

#endif // TALIPOT_GL_MAIN_VIEW_H
