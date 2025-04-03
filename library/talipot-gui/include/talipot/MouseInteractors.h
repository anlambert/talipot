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

#ifndef TALIPOT_MOUSE_INTERACTORS_H
#define TALIPOT_MOUSE_INTERACTORS_H

#include <talipot/InteractorComposite.h>
#include <talipot/Camera.h>
#include <talipot/GlScene.h>

namespace tlp {
class GlWidget;
class NodeLinkDiagramView;

/** An interactor class allowing to zoom and pan using the mouse wheel.
 */
class TLP_QT_SCOPE MousePanNZoomNavigator : public InteractorComponent {
  public:
    MousePanNZoomNavigator() : cameraScaleFactor(1), isGesturing(false) {}
    ~MousePanNZoomNavigator() override = default;
    bool eventFilter(QObject *, QEvent *) override;

  protected:
    float cameraScaleFactor;
    bool isGesturing;
};

/** An interactor class used to delete a graph element on mouse left click
 */
class TLP_QT_SCOPE MouseElementDeleter : public InteractorComponent {
  public:
    MouseElementDeleter() : glWidget(nullptr) {}
    ~MouseElementDeleter() override = default;
    bool eventFilter(QObject *, QEvent *) override;
    void clear() override;
    virtual void delElement(tlp::Graph *graph, tlp::SelectedEntity &selectedEntity);

  private:
    GlWidget *glWidget;
};

/** An interactor class to translate/rotate using keys or mouse movements,
    or to zoom and pan using the mouse wheel or
 */
class TLP_QT_SCOPE MouseNKeysNavigator : public MousePanNZoomNavigator {
  private:
    InteractorComponent *currentSpecInteractorComponent;

    QCursor oldCursor;

  public:
    MouseNKeysNavigator() : currentSpecInteractorComponent(nullptr), nldv(nullptr) {}
    ~MouseNKeysNavigator() override = default;
    bool eventFilter(QObject *, QEvent *) override;
    void clear() override;
    void viewChanged(tlp::View *view) override;

  private:
    // member below are to manage meta node navigation
    tlp::NodeLinkDiagramView *nldv;
    std::vector<tlp::Graph *> graphHierarchy;
    std::vector<tlp::Camera> cameraHierarchy;
    std::vector<tlp::node> nodeHierarchy;
    std::vector<float> alphaHierarchy;
};
}
#endif // TALIPOT_MOUSE_INTERACTORS_H
