/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_GL_GRAPH_HIGH_DETAILS_RENDERER_H
#define TALIPOT_GL_GRAPH_HIGH_DETAILS_RENDERER_H

#include <talipot/GlGraphRenderer.h>

#include <talipot/hash.h>

namespace tlp {

class Graph;
class GlScene;
struct OcclusionTest;

/** \brief Class to display graph with old rendering engine
 *
 * This class display graph with :
 *  - Nodes and edges shapes
 *  - LOD system
 * With this renderer you can perform selection
 *
 * See GlGraphRenderer documentation for functions documentations
 */
class TLP_GL_SCOPE GlGraphHighDetailsRenderer : public GlGraphRenderer {

public:
  GlGraphHighDetailsRenderer(const GlGraphInputData *inputData);
  GlGraphHighDetailsRenderer(const GlGraphInputData *inputData, GlScene *scene);

  ~GlGraphHighDetailsRenderer() override;

  void draw(float lod, Camera *camera) override;

  void selectEntities(Camera *camera, RenderingEntitiesFlag type, int x, int y, int w, int h,
                      std::vector<SelectedEntity> &selectedEntities) override;

protected:
  void initSelectionRendering(RenderingEntitiesFlag type, int x, int y, int w, int h,
                              flat_hash_map<uint, SelectedEntity> &idMap, uint &currentId);

  void buildSortedList();

  void drawLabelsForComplexEntities(bool drawSelected, OcclusionTest *occlusionTest,
                                    LayerLODUnit &layerLODUnit);

  GlLODCalculator *lodCalculator;

  GlScene *baseScene;
  GlScene *fakeScene;
  Vec4i selectionViewport;
};
}

#endif // TALIPOT_GL_GRAPH_HIGH_DETAILS_RENDERER_H
