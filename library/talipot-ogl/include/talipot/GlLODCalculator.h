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

#ifndef TALIPOT_GL_LOD_CALCULATOR_H
#define TALIPOT_GL_LOD_CALCULATOR_H

#include <climits>
#include <vector>

#include <talipot/BoundingBox.h>
#include <talipot/GlSceneVisitor.h>
#include <talipot/GlEntity.h>
#include <talipot/GlNode.h>
#include <talipot/GlEdge.h>
#include <talipot/GlLayer.h>

namespace tlp {

class Camera;
class GlEntity;
class GlEntity;
class GlScene;
class GlGraphInputData;

enum RenderingEntitiesFlag {
    RenderingEntities = 1,
    RenderingNodes = 2,
    RenderingEdges = 4,
    RenderingAll = 7,
    RenderingWithoutRemove = 8
};

struct LODUnit {
    LODUnit() : lod(-1) {}
    LODUnit(const BoundingBox &boundingBox) : boundingBox(boundingBox), lod(-1) {}
    BoundingBox boundingBox;
    float lod;
};

// struct to store simple entity lod
struct EntityLODUnit : public LODUnit {
    EntityLODUnit(GlEntity *entity = nullptr) : LODUnit(), entity(entity) {}
    EntityLODUnit(GlEntity *entity, const BoundingBox &boundingBox)
        : LODUnit(boundingBox), entity(entity) {}
    GlEntity *entity;

    void init(GlEntity *e, const BoundingBox &bb) {
        entity = e;
        boundingBox = bb;
    }
};

// struct to store complex entity (nodes/edges) lod
struct GraphElementLODUnit : public LODUnit {
    GraphElementLODUnit(uint id = UINT_MAX) : LODUnit(), id(id) {}
    GraphElementLODUnit(uint id, const BoundingBox &boundingBox) : LODUnit(boundingBox), id(id) {}

    void init(uint i, const BoundingBox &bb) {
        id = i;
        boundingBox = bb;
    }

    uint id;
};

struct LayerLODUnit {
    std::vector<EntityLODUnit> entitiesLODVector;
    std::vector<GraphElementLODUnit> nodesLODVector;
    std::vector<GraphElementLODUnit> edgesLODVector;
    Camera *camera;
    LayerLODUnit() : camera(nullptr) {}
    LayerLODUnit(Camera *camera) : camera(camera) {}
};

typedef std::vector<LayerLODUnit> LayersLODVector;

/**
 * Class use to calculate lod of scene entities
 */
class TLP_GL_SCOPE GlLODCalculator : public GlSceneVisitor {

  public:
    GlLODCalculator() : glScene(nullptr), inputData(nullptr), attachedLODCalculator(nullptr) {}
    virtual ~GlLODCalculator() = default;
    virtual GlLODCalculator *clone() = 0;

    /**
     * Visit a GlEntity
     */
    void visit(GlEntity *entity) override {
        addEntityBoundingBox(entity, entity->getBoundingBox());
    }

    /**
     * Visit a node
     */
    void visit(GlNode *glNode) override {
        addNodeBoundingBox(glNode->graph, glNode->n, glNode->getBoundingBox(inputData));
    }
    /**
     * Visit an Edge
     */
    void visit(GlEdge *glEdge) override {
        addEdgeBoundingBox(glEdge->graph, glEdge->e, glEdge->getBoundingBox(inputData));
    }
    /**
     * Visit a layer
     */
    void visit(GlLayer *layer) override {
        beginNewCamera(&layer->getCamera());
    }

    /**
     * Reserve memory to store nodes and edges LOD
     */
    void reserveMemoryForGraphElts(uint /*nbNodes*/, uint /*nbEdges*/) override {}
    /**
     * Set scene use by this LOD calculator
     */
    virtual void setScene(GlScene &scene) {
        glScene = &scene;
    }

    /**
     * Set input data use to render
     */
    virtual void setInputData(const GlGraphInputData *inputData) {
        this->inputData = inputData;
    }

    /**
     * Set RenderingEntitiesFlag to :
     * RenderingEntities,RenderingNodes,RenderingEdges,RenderingAll,RenderingWithoutRemove
     */
    virtual void setRenderingEntitiesFlag(RenderingEntitiesFlag flag) {
        renderingEntitiesFlag = flag;
    }

    /**
     * Return if the LODCalculator need to have entities to compute
     */
    virtual bool needEntities() {
        return true;
    }
    /**
     * Set if the LODCalculator need to have entities to compute
     */
    virtual void setNeedEntities(bool) {}
    /**
     * Begin a new camera
     */
    virtual void beginNewCamera(Camera *camera) = 0;
    /**
     * Record a new simple entity in current camera context
     */
    virtual void addEntityBoundingBox(GlEntity *entity, const BoundingBox &bb) = 0;
    /**
     * Record a new node in current camera context
     */
    virtual void addNodeBoundingBox(Graph *graph, node n, const BoundingBox &bb) = 0;
    /**
     * Record a new edge in current camera context
     */
    virtual void addEdgeBoundingBox(Graph *graph, edge e, const BoundingBox &bb) = 0;

    /**
     * Compute all lod
     */
    virtual void compute(const Vec4i &globalViewport, const Vec4i &currentViewport) = 0;

    /**
     * Return a pointer on LOD result
     */
    LayersLODVector &getResult() {
        return layersLODVector;
    }

    /**
     * Clear class data
     */
    virtual void clear() {
        layersLODVector.clear();
    }

    virtual BoundingBox getSceneBoundingBox() = 0;

    void setAttachedLODCalculator(GlLODCalculator *calculator) {
        attachedLODCalculator = calculator;
    }

  protected:
    GlScene *glScene;
    const GlGraphInputData *inputData;

    RenderingEntitiesFlag renderingEntitiesFlag;

    LayersLODVector layersLODVector;

    GlLODCalculator *attachedLODCalculator;
};
}

#endif // TALIPOT_GL_LOD_CALCULATOR_H
