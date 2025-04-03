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

#ifndef TALIPOT_CAMERA_H
#define TALIPOT_CAMERA_H

#include <talipot/Coord.h>
#include <talipot/Matrix.h>
#include <talipot/BoundingBox.h>
#include <talipot/Observable.h>
#include <talipot/GlTools.h>

namespace tlp {

class GlScene;

/**
 * \ingroup OpenGL
 * \brief Tulip OpenGL camera object
 *
 * This camera can be a 2D or 3D camera
 * After setup you can do some basic operation :
 *  - Move, rotate, strafeLeftRight and strafeUpDown to modify point of view
 *  - You can directly modify camera parameters with setSceneRadius, setZoomFactor, setEyes,
 * setCenter and setUp
 *  - You can transform viewport coordinates to 3D world coordinates with viewportTo3DWorld()
 * function and 3D world coordinates to viewport coordinates with worldTo2DViewport() function
 * A camera is a main component of GlLayer and GlScene
 * @see GlLayer
 * @see GlScene
 */
class TLP_GL_SCOPE Camera : public Observable {
  public:
    /**
     * @brief Constructor
     * @param scene A layer is attached to a scene so we have to specify it in the constructor
     * @param center 3D coordinates of point visualized by the camera
     * @param eye 3D position of the camera
     * @param up normalized up 3D coordinates of the camera
     * @param zoomFactor level of zoom of the camera
     * @param sceneRadius scene radius of the camera
     */
    Camera(GlScene *scene, Coord center = Coord(0, 0, 0), Coord eyes = Coord(0, 0, 10),
           Coord up = Coord(0, -1, 0), double zoomFactor = 0.5, double sceneRadius = 10);
    /**
     * @brief Constructor : used to create a 2D camera
     */
    Camera(GlScene *scene, bool d3);

    Camera(const Camera &) = default;

    Camera &operator=(const Camera &camera);

    /**
     * @brief Destructor
     */
    ~Camera() override;

    /**
     * @brief Set the camera's scene
     * The viewport is store in the scene, so we must attach camera to a scene
     */
    void setScene(GlScene *scene);

    /**
     * @brief Return the camera's scene
     */
    GlScene *getScene() const {
        return scene;
    }

    /**
     * @brief Return the camera bounding box
     *
     * This bounding box is the part of the scene visualized by this camera.
     */
    BoundingBox getBoundingBox() const;

    void rotate(float angle, float x, float y, float z);

    /**
     * @brief Return if the camera is a 3D one
     */
    bool is3D() const {
        return d3;
    }

    /**
     * @brief Return the viewport of the attached scene
     */
    const Vec4i &getViewport() const;

    /**
     * @brief Return the scene radius
     */
    double getSceneRadius() const {
        return sceneRadius;
    }

    /**
     * @brief Set the zoom factor
     *
     * level of zoom of the camera
     */
    void setZoomFactor(double zoomFactor);
    /**
     * @brief Return the zoom factor
     *
     * level of zoom of the camera
     */
    double getZoomFactor() const {
        return zoomFactor;
    }

    /**
     * @brief Set the eye
     *
     * 3D position of the camera
     */
    void setEyes(const Coord &eyes);
    /**
     * @brief Return the eyes
     *
     * 3D position of the camera
     */
    const Coord &getEyes() const {
        return eyes;
    }

    /**
     * @brief Set the center
     *
     * 3D coordinates of point visualized by the camera
     */
    void setCenter(const Coord &center);
    /**
     * @brief Return the center
     *
     * 3D coordinates of point visualized by the camera
     */
    const Coord &getCenter() const {
        return center;
    }

    /**
     * @brief Set the up vector
     *
     * normalized up 3D coordinates of the camera
     */
    void setUp(const Coord &up);
    /**
     * @brief Return the up vector
     *
     * normalized up 3D coordinates of the camera
     */
    const Coord &getUp() const {
        return up;
    }

    /**
     * @brief Return the 3D world coordinate for the given viewport point
     * \warning This function set up the projection and modelview matrix
     */
    Coord viewportTo3DWorld(const Coord &point) const;

    /**
     * @brief Return the 3D world coordinate for the given viewport point
     * \warning This function set up the projection and modelview matrix
     */
    Coord screenTo3DWorld(const Coord &point) const {
        return viewportTo3DWorld(point);
    }

    /**
     * @brief Return the viewport position for the given 3D coordinate
     * \warning This function set up the projection and modelview matrix
     */
    Coord worldTo2DViewport(const Coord &obj) const;

    /**
     * @brief Return the viewport position for the given 3D coordinate
     * \warning This function set up the projection and modelview matrix
     */
    Coord worldTo2DScreen(const Coord &obj) const {
        return worldTo2DViewport(obj);
    }

    /**
     * @brief Function to export data in outString (in XML format)
     */
    virtual void getXML(std::string &outString);

    /**
     * @brief Function to set data with inString (in XML format)
     */
    virtual void setWithXML(const std::string &inString, uint &currentPosition);

    /**
     * Get the modelview matrix
     */
    const MatrixGL &getModelViewMatrix() const {
        return modelviewMatrix;
    }

    /**
     * Get the projection matrix
     */
    const MatrixGL &getProjectionMatrix() const {
        return projectionMatrix;
    }

    /**
     * Get the transform matrix : transformMatrix = projectionMatrix * modelviewMatrix
     */
    const MatrixGL &getTransformMatrix() const {
        return transformMatrix;
    }

    /**
     * Get the transform matrix generated with the given viewport
     */
    const MatrixGL &getTransformMatrix(const Vec4i &viewport) const;

    /**
     * @brief Init Gl parameters
     */
    void initGl();

    /**
     * @brief Init light
     */
    void initLight();

    /**
     * @brief Init projection with the given viewport. Load identity matrix if reset is set as true
     */
    void initProjection(const Vec4i &viewport, bool reset = true);

    /**
     * @brief Init projection with the scene viewport. Load identity matrix if reset is set as true
     */
    void initProjection(bool reset = true);

    /**
     * @brief Init modelview
     */
    void initModelView();

    /**
     * @brief Set the scene radius
     */
    void setSceneRadius(double sceneRadius, const BoundingBox sceneBoundingBox = BoundingBox());

  private:
    bool matrixCoherent;

    Coord center, eyes, up;
    double zoomFactor;
    double sceneRadius;
    BoundingBox sceneBoundingBox;

    GlScene *scene;

    MatrixGL modelviewMatrix;
    MatrixGL projectionMatrix;
    MatrixGL transformMatrix;

    bool d3;
};
}

#endif // TALIPOT_CAMERA_H
