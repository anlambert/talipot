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

#ifndef TALIPOT_GL_SCENE_OBSERVER_H
#define TALIPOT_GL_SCENE_OBSERVER_H

#include <string>

#include <talipot/config.h>
#include <talipot/Observable.h>

namespace tlp {

class GlEntity;
class GlLayer;
class GlScene;

/** \brief An observer to the scene
 * An observer to the scene who observe layers
 */
class TLP_GL_SCOPE GlSceneEvent : public Event {
  public:
    enum GlSceneEventType {
        TLP_ADDLAYER = 0,
        TLP_DELLAYER,
        TLP_MODIFYLAYER,
        TLP_MODIFYENTITY,
        TLP_DELENTITY
    };

    GlSceneEvent(const GlScene &scene, GlSceneEventType sceneEventType,
                 const std::string &layerName, GlLayer *layer);

    GlSceneEvent(const GlScene &scene, GlSceneEventType sceneEventType, GlEntity *entity);

    GlEntity *getGlEntity() const;

    std::string getLayerName() const;

    GlLayer *getLayer() const;

    GlSceneEventType getSceneEventType() const;

  protected:
    GlSceneEventType sceneEventType;
    std::string layerName;
    GlLayer *layer;
    GlEntity *entity;
};
}

#endif // TALIPOT_GL_SCENE_OBSERVER_H
