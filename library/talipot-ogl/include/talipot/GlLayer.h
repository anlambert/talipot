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

#ifndef TALIPOT_GL_LAYER_H
#define TALIPOT_GL_LAYER_H

#include <talipot/config.h>
#include <talipot/GlComposite.h>

namespace tlp {

class Graph;
class GlScene;
class GlSceneVisitor;
class GlGraph;

/**
 * @ingroup OpenGL
 * @brief A GlLayer is like an 2D drawing software layer system
 *
 * A layer is an entity with a Camera and a GlComposite to store GlEntity
 * Layers are used with GlScene : you can add a layer to a scene and a scene can have many layers
 * @see Camera
 * @see GlComposite
 * @see GlEntity
 * @see GlScene
 *
 *
 * You have two constructor for GlLayer : one with a camera pointer and one without
 * The constructor without camera pointer create a layer with a new camera and delete this camera at
 * the destruction
 * The constructor with camera pointer create a layer and use the camera pointer but you have the
 * responsibility of camera destruction
 *
 * After you have created a layer, you can populate the layer with GlEntity and addGlEntity()
 * functions
 */
class TLP_GL_SCOPE GlLayer {

  public:
    /**
     * @brief Layer constructor : construct a layer with his name
     * A new camera is created for this layer and this camera will be deleted in the GlLayer
     * destructor
     * @param name layer name
     * @param workingLayer a working layer is not displayed on overview
     */
    GlLayer(const std::string &name, bool workingLayer = false);

    /**
     * @brief Layer constructor : construct a layer with his name and use the camera : camera
     * You have the responsibility of camera destruction
     * @param name layer name
     * @param camera camera to use in this layer
     * @param workingLayer a working layer is not displayed on overview
     */
    GlLayer(const std::string &name, Camera *camera, bool workingLayer = false);

    /**
     * @brief Destructor
     */
    ~GlLayer();

    /**
     * @brief Return the scene where the layer is
     */
    GlScene *getScene() {
        return scene;
    }

    /**
     * @brief Return the layer's name
     */
    std::string getName() {
        return name;
    }

    /**
     * @brief Set the layer's camera
     * GlLayer now use a copy of the camera parameters
     */
    void setCamera(Camera *camera);

    /**
     * Set the layer's camera
     * GlLayer now use camera parameters and you have the responsibility of camera destruction
     */
    void setSharedCamera(Camera *camera);

    /**
     * @brief Replace the layer's camera with a new 2D one
     */
    void set2DMode();

    /**
     * @brief Return the layer's camera
     */
    Camera &getCamera() {
        return *camera;
    }

    /**
     * @brief Set if the layer is visible
     */
    void setVisible(bool visible);

    /**
     * @brief Return if the layer is visible
     */
    bool isVisible() {
        return composite.isVisible();
    }

    /**
     * @brief Add an entity to GlComposite of the layer
     */
    void addGlEntity(GlEntity *entity, const std::string &name);

    /**
     * @brief A Convenience function that adds a graph to the layer
     *
     * This method will automatically create a GlGraph entity and add it to the layer.
     */
    void addGraph(tlp::Graph *graph, const std::string &name);

    /**
     * @brief Remove entity with name : key
     * This entity is not deleted
     */
    void deleteGlEntity(const std::string &key);

    /**
     * @brief Remove entity
     * This entity is not deleted
     */
    void deleteGlEntity(GlEntity *entity);

    /**
     * @brief Return entity with name : key
     */
    GlEntity *findGlEntity(const std::string &key);

    /**
     * @brief Return the map of layer's entities
     */
    const std::map<std::string, GlEntity *> &getGlEntities() const;

    /**
     * @brief Return the GlComposite used by the layer
     * A GlLayer is only a container of a camera and a composite, so to manipulate GlEntity on this
     * layer you can get the GlComposite and add/remove entities on this composite
     */
    GlComposite *getComposite() {
        return &composite;
    }

    /**
     * @brief Remove all entities of the layer
     * Entities are not deleted so before call this function you have to get the entities list and
     * you have the responsibility of entities destruction
     */
    void clear() {
        composite.reset(false);
    }

    /**
     * @brief Return if this layer is a working layer
     * A working layer is not displayed on overview
     */
    bool isAWorkingLayer() const {
        return workingLayer;
    }

    /**
     * @brief Return if this layer use a shared camera
     * A shared camera is a camera used by more than one Layer, so if this layer use a shared camera
     * we don't have to delete it when the layer is destroyed
     */
    bool useSharedCamera() const {
        return sharedCamera;
    }

    /**
     * Get XML description of the layer and children and store it in out string
     */
    void getXML(std::string &outString);

    /**
     * @brief Get XML description of cameras of the layer and store it in out string
     */
    void getXMLOnlyForCameras(std::string &outString);

    /**
     * @brief Function to set data with inString (in XML format)
     */
    void setWithXML(const std::string &inString, uint &currentPosition);

    /**
     * This function is automatically call when a GlGraph is added in this layer
     * You don't have to call this function
     */
    void glGraphAdded(GlGraph *composite);

    /**
     * This function is automatically call when a GlGraph is removed in this layer
     * You don't have to call this function
     */
    void glGraphRemoved(GlGraph *composite);

    /**
     * function used by visitors to visit this layer
     */
    void acceptVisitor(GlSceneVisitor *visitor);

  protected:
    /**
     * Set the scene where the layer is
     */
    void setScene(GlScene *scene);

  private:
    std::string name;

    GlComposite composite;
    GlScene *scene;

    Camera *camera;
    bool sharedCamera;

    bool workingLayer;

    friend class GlScene;
};
}

#endif // TALIPOT_GL_LAYER_H
