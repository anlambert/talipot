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

#ifndef FISH_EYE_INTERACTOR_H
#define FISH_EYE_INTERACTOR_H

#include <memory>

#include <talipot/GLInteractor.h>
#include <talipot/Coord.h>

#include "../../utils/StandardInteractorPriority.h"

class QOpenGLFramebufferObject;

namespace tlp {

class FisheyeConfigWidget;
class GlShaderProgram;

class FisheyeInteractorComponent : public GLInteractorComponent {

  public:
    FisheyeInteractorComponent(FisheyeConfigWidget *configWidget);
    FisheyeInteractorComponent(const FisheyeInteractorComponent &fisheyeInteractorComponent);
    ~FisheyeInteractorComponent();

    bool eventFilter(QObject *widget, QEvent *e) override;

    bool compute(GlWidget *) override {
        return false;
    }

    void viewChanged(View *view) override;

    bool draw(GlWidget *glWidget) override;

  private:
    void generateFisheyeTexture(GlWidget *glWidget);

    FisheyeConfigWidget *_configWidget;
    bool _activateFisheye;
    int _x;
    int _y;
    QOpenGLFramebufferObject *_fbo = nullptr;
    QOpenGLFramebufferObject *_fbo2 = nullptr;
    std::string _fboTextureId;
    static std::unique_ptr<GlShaderProgram> fisheyeShader;
    inline static int _maxTextureSize = 0;
};

/** \file
 *  \brief Fisheye Interactor

 * This interactor plugin allow to perform a fisheye distortion effect on OpenGL views.
 * It allows to gain focus on a certain area of a visualization by magnifying it without losing
 * the context.
 *
 * Your graphic card must support shader programs otherwise the plugin won't work.
 *
 *
 */
class FisheyeInteractor : public GLInteractorComposite {

  public:
    PLUGININFORMATION("FisheyeInteractor", "Antoine Lambert", "29/05/2009", "Fisheye Interactor",
                      "1.0", "Visualization")

    FisheyeInteractor(const PluginContext *);
    ~FisheyeInteractor() override;

    void construct() override;

    void uninstall() override;

    QWidget *configurationWidget() const override;

    uint priority() const override {
        return StandardInteractorPriority::Fisheye;
    }

    bool isCompatible(const std::string &viewName) const override;

  private:
    FisheyeConfigWidget *fisheyeConfigWidget;
};
}

#endif // FISH_EYE_INTERACTOR_H
