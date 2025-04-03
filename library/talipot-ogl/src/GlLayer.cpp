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

#include <talipot/GlXMLTools.h>
#include <talipot/GlGraph.h>

using namespace std;

namespace tlp {

GlLayer::GlLayer(const std::string &name, bool workingLayer)
    : name(name), scene(nullptr), camera(new Camera(nullptr)), sharedCamera(false),
      workingLayer(workingLayer) {
    composite.addLayerParent(this);
}

GlLayer::GlLayer(const std::string &name, Camera *camera, bool workingLayer)
    : name(name), scene(nullptr), camera(camera), sharedCamera(true), workingLayer(workingLayer) {
    composite.addLayerParent(this);
}

GlLayer::~GlLayer() {
    if (!sharedCamera) {
        delete camera;
    }

    composite.removeLayerParent(this);
}

void GlLayer::setScene(GlScene *scene) {
    this->scene = scene;
    camera->setScene(scene);
}

void GlLayer::setCamera(Camera *camera) {
    Camera *oldCamera = this->camera;
    this->camera = camera;

    if (!sharedCamera) {
        delete oldCamera;
    }

    sharedCamera = false;
}

void GlLayer::setSharedCamera(Camera *camera) {
    Camera *oldCamera = this->camera;
    this->camera = camera;

    if (!sharedCamera) {
        delete oldCamera;
    }

    sharedCamera = true;
}

void GlLayer::set2DMode() {
    Camera *oldCamera = this->camera;
    this->camera = new Camera(oldCamera->getScene(), false);

    if (!sharedCamera) {
        delete oldCamera;
    }

    sharedCamera = false;
}

void GlLayer::setVisible(bool visible) {
    if (composite.isVisible() == visible) {
        return;
    }

    composite.setVisible(visible);

    if (scene) {
        scene->notifyModifyLayer(this->name, this);
    }
}

void GlLayer::acceptVisitor(GlSceneVisitor *visitor) {
    if (composite.isVisible()) {
        visitor->visit(this);
        composite.acceptVisitor(visitor);
    }
}

void GlLayer::addGlEntity(GlEntity *entity, const std::string &name) {
    composite.addGlEntity(entity, name);

    if (scene) {
        scene->notifyModifyLayer(this->name, this);
    }
}

void GlLayer::addGraph(tlp::Graph *graph, const string &name) {
    auto *glGraph = new GlGraph(graph, scene);
    addGlEntity(glGraph, name);
}

void GlLayer::deleteGlEntity(const std::string &key) {
    composite.deleteGlEntity(key);

    if (scene) {
        scene->notifyModifyLayer(this->name, this);
    }
}

void GlLayer::deleteGlEntity(GlEntity *entity) {
    composite.deleteGlEntity(entity);

    if (scene) {
        scene->notifyModifyLayer(this->name, this);
    }
}

GlEntity *GlLayer::findGlEntity(const std::string &key) {
    return composite.findGlEntity(key);
}

const std::map<std::string, GlEntity *> &GlLayer::getGlEntities() const {
    return composite.getGlEntities();
}

void GlLayer::getXML(string &outString) {
    GlXMLTools::beginDataNode(outString);

    GlXMLTools::beginChildNode(outString, "camera");
    camera->getXML(outString);
    GlXMLTools::endChildNode(outString, "camera");

    bool visible = composite.isVisible();
    GlXMLTools::getXML(outString, "visible", visible);

    GlXMLTools::endDataNode(outString);

    GlXMLTools::beginChildNode(outString, "GlEntity");
    composite.getXML(outString);
    GlXMLTools::endChildNode(outString, "GlEntity");
}

void GlLayer::getXMLOnlyForCameras(string &outString) {
    GlXMLTools::beginDataNode(outString);

    GlXMLTools::beginChildNode(outString, "camera");
    camera->getXML(outString);
    GlXMLTools::endChildNode(outString, "camera");

    bool visible = composite.isVisible();
    GlXMLTools::getXML(outString, "visible", visible);

    GlXMLTools::endDataNode(outString);
}

void GlLayer::setWithXML(const string &inString, uint &currentPosition) {

    GlXMLTools::enterDataNode(inString, currentPosition);
    string childName = GlXMLTools::enterChildNode(inString, currentPosition);
    assert(childName == "camera");
    camera->setWithXML(inString, currentPosition);
    GlXMLTools::leaveChildNode(inString, currentPosition, "camera");

    bool visible;
    GlXMLTools::setWithXML(inString, currentPosition, "visible", visible);
    composite.setVisible(visible);

    GlXMLTools::leaveDataNode(inString, currentPosition);

    childName = GlXMLTools::enterChildNode(inString, currentPosition);

    if (!childName.empty()) {

        map<string, string> childMap = GlXMLTools::getProperties(inString, currentPosition);
        assert(childMap["type"] == "GlComposite");
        composite.setWithXML(inString, currentPosition);

        GlXMLTools::leaveChildNode(inString, currentPosition, childName);
    }
}

void GlLayer::glGraphAdded(GlGraph *composite) {
    assert(scene);
    scene->glGraphAdded(this, composite);
}

void GlLayer::glGraphRemoved(GlGraph *composite) {
    assert(scene);
    scene->glGraphRemoved(this, composite);
}
}
