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

#include <talipot/GlSceneObserver.h>
#include <talipot/GlScene.h>

using namespace std;

namespace tlp {

GlSceneEvent::GlSceneEvent(const GlScene &scene, GlSceneEventType sceneEventType,
                           const std::string &layerName, GlLayer *layer)
    : Event(scene, EventType::TLP_MODIFICATION), sceneEventType(sceneEventType),
      layerName(layerName), layer(layer) {}

GlSceneEvent::GlSceneEvent(const GlScene &scene, GlSceneEventType sceneEventType, GlEntity *entity)
    : Event(scene, EventType::TLP_MODIFICATION), sceneEventType(sceneEventType), entity(entity) {}

GlEntity *GlSceneEvent::getGlEntity() const {
  return entity;
}

std::string GlSceneEvent::getLayerName() const {
  return layerName;
}

GlLayer *GlSceneEvent::getLayer() const {
  return layer;
}

GlSceneEvent::GlSceneEventType GlSceneEvent::getSceneEventType() const {
  return sceneEventType;
}
}
