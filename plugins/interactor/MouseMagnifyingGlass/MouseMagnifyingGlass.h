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

#ifndef MOUSE_MAGNIFYING_GLASS_H
#define MOUSE_MAGNIFYING_GLASS_H

#include <talipot/GLInteractor.h>
#include <talipot/GlWidget.h>

#include "../../utils/StandardInteractorPriority.h"

class QOpenGLFramebufferObject;

namespace tlp {

class MouseMagnifyingGlassInteractorComponent : public GLInteractorComponent {

  public:
    MouseMagnifyingGlassInteractorComponent();
    MouseMagnifyingGlassInteractorComponent(
        const MouseMagnifyingGlassInteractorComponent &mouseMagnifyingGlassInteractorComponent);
    ~MouseMagnifyingGlassInteractorComponent() override;

    bool eventFilter(QObject *widget, QEvent *e) override;

    bool compute(GlWidget *) override {
        return false;
    }

    bool draw(GlWidget *glWidget) override;

    void viewChanged(View *view) override;

  private:
    void generateMagnifyingGlassTexture(const Coord &magnifyingGlassCenterScr);

    QOpenGLFramebufferObject *fbo;
    QOpenGLFramebufferObject *fbo2;
    GlWidget *glWidget;
    Camera *camera;
    Coord boxCenter;
    bool drawInteractor;
    std::string textureName;
    float radius;
    float magnifyPower;
};

/** \file
 *  \brief Magnifying Glass Interactor

 * This interactor plugin provides a magnifying glass tool for all Tulip views.
 * It allows to zoom on a particular area of a visualization with the help of a lens
 * without having to modify the global zoom level.
 *
 *
 */
class MouseMagnifyingGlassInteractor : public GLInteractorComposite {

  public:
    PLUGININFORMATION("MouseMagnifyingGlassInteractor", "Antoine Lambert", "19/06/2009",
                      "Mouse Magnifying Glass Interactor Interactor", "1.0", "Visualization")

    MouseMagnifyingGlassInteractor(const tlp::PluginContext *);

    void construct() override;

    uint priority() const override {
        return StandardInteractorPriority::MagnifyingGlass;
    }
    QWidget *configurationWidget() const override {
        return nullptr;
    }

    bool isCompatible(const std::string &viewName) const override;
};
}
#endif // MOUSE_MAGNIFYING_GLASS_H
