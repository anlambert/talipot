/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_GL_NODE_H
#define TALIPOT_GL_NODE_H

#include <climits>
#include <talipot/Color.h>
#include <talipot/Coord.h>
#include <talipot/Size.h>
#include <talipot/GlBox.h>
#include <talipot/GlLabel.h>
#include <talipot/GlSceneVisitor.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/Singleton.h>

#include <vector>

namespace tlp {

struct OcclusionTest;

/**
 * Class to represent a node of a graph
 */
class TLP_GL_SCOPE GlNode {

public:
  /**
   * Default constructor with id
   * id must be the id of the node in graph
   */
  GlNode(node n = node(), Graph *graph = nullptr)
      : n(n), graph(graph), selectionBox(Coord(0, 0, 0), Size(1, 1, 1), Color(0, 0, 255, 255),
                                         Color(0, 255, 0, 255), false, true, "", 3) {}

  /**
   * Virtual function to accept GlSceneVisitor on this class
   */
  void acceptVisitor(GlSceneVisitor *visitor) {
    visitor->visit(this);
  }

  /**
   * Return the node bounding box
   */
  BoundingBox getBoundingBox(const GlGraphInputData *data);

  /**
   * Draw the node with level of detail : lod and Camera : camera
   */
  void draw(float lod, const GlGraphInputData *data, Camera *camera);

  /**
   * Draw the label of the node if drawNodesLabel is true and if label selection is equal to
   * drawSelect
   */
  void drawLabel(bool drawSelect, OcclusionTest *test, const GlGraphInputData *data, float lod);

  /**
   * Draw the label of the node if drawEdgesLabel is true
   */
  void drawLabel(OcclusionTest *test, const GlGraphInputData *data);

  /**
   * Draw the label of the node if drawEdgesLabel is true
   */
  void drawLabel(OcclusionTest *test, const GlGraphInputData *data, float lod,
                 Camera *camera = nullptr);

  /**
   * This function is used by the engine to get point coordinates of the node
   */
  Coord getPoint(GlGraphInputData *inputData) {
    init(inputData);
    return coord;
  }

  /**
   * This function is used by the engine to get color of the node
   */
  Color getColor(GlGraphInputData *inputData) const {
    return (inputData->borderWidths()->getNodeValue(n) > 0)
               ? inputData->borderColors()->getNodeValue(n)
               : inputData->colors()->getNodeValue(n);
  }

  node n;
  Graph *graph;

protected:
  GlBox selectionBox;
  static Singleton<GlLabel> label;

  // initialize the data member below
  void init(const GlGraphInputData *data);

  tlp::Coord coord;
  int glyph;
  tlp::Size size;
  float rot;
  bool selected;
  float labelRot;
};
}

#endif // TALIPOT_GL_NODE_H
