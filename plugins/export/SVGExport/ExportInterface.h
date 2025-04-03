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

#ifndef EXPORT_INTERFACE_H
#define EXPORT_INTERFACE_H

#include <talipot/Coord.h>
#include <talipot/Size.h>
#include <talipot/ViewSettings.h>

class QString;

namespace tlp {
class Color;
struct BoundingBox;
class PluginProgress;
}

// Builder
class ExportInterface {

  protected:
    std::ostream &_os;
    tlp::PluginProgress *_prg;

  public:
    ExportInterface(tlp::PluginProgress *prg, std::ostream &os);
    virtual ~ExportInterface() = default;

    // This method writes the header of the exported file
    virtual bool writeHeader(const tlp::BoundingBox &bb) = 0;

    // This method begins a graph
    virtual bool writeGraph(const tlp::BoundingBox &bb, const tlp::Color &background,
                            bool noBackground = false) = 0;

    // This method begins a meta-graph
    virtual bool writeMetaGraph(const int transform_X, const int transform_Y, float scale) = 0;

    // This method ends a graph or a meta-graph
    virtual bool writeEndGraph() = 0;

    // This method ends the file
    virtual bool writeEnd() = 0;

    // This method regroups nodes if we want to modify them later
    virtual bool groupNode() = 0;

    // This method regroups edges if we want to modify them later
    virtual bool groupEdge() = 0;

    // This method ends the group of node
    virtual bool endGroupNode() = 0;

    // This method ends the group of edge
    virtual bool endGroupEdge() = 0;

    // This method represents nodes according to incoming parameters
    virtual bool startNode(const unsigned id) = 0;
    virtual bool endNode() = 0;

    // This method defines the shape of a node
    virtual bool addShape(const tlp::NodeShape::NodeShapes &, const tlp::Coord &coord,
                          const tlp::Size &size, const tlp::Color &bordercolor,
                          const double borderwidth, const tlp::Color &color,
                          std::string iconName) = 0;

    // This method represents edges according to incoming parameters
    // No edge color interpolation
    virtual bool
    exportEdge(const tlp::EdgeShape::EdgeShapes &, const std::vector<tlp::Coord> &bends,
               const tlp::Color &edgeColor, const double width,
               const tlp::EdgeExtremityShape::EdgeExtremityShapes src_anchor_shape_type,
               const unsigned id_src_shape,
               const tlp::EdgeExtremityShape::EdgeExtremityShapes tgt_anchor_shape_type,
               const unsigned id_tgt_shape, const std::vector<tlp::Coord> &edgeVertice) = 0;
    // edge color interpolation between edgeColor1 and edgeColor2
    virtual bool
    exportEdge(const unsigned id, const tlp::EdgeShape::EdgeShapes &,
               const std::vector<tlp::Coord> &bends, const tlp::Color &edgeColor1,
               const tlp::Color &edgeColor2, const double width,
               const tlp::EdgeExtremityShape::EdgeExtremityShapes src_anchor_shape_type,
               const unsigned id_src_shape,
               const tlp::EdgeExtremityShape::EdgeExtremityShapes tgt_anchor_shape_type,
               const unsigned id_tgt_shape, const std::vector<tlp::Coord> &edgeVertice) = 0;

    // This method defines extremities of edges
    virtual bool
    exportEdgeExtremity(const unsigned id_src_shape, const unsigned id_tgt_shape,
                        const tlp::EdgeExtremityShape::EdgeExtremityShapes src_anchor_shape_type,
                        const tlp::EdgeExtremityShape::EdgeExtremityShapes tgt_anchor_shape_type,
                        const tlp::Color &color, const uint id_src_gradient,
                        const uint id_tgt_gradient, const std::string &iconName) = 0;

    // This method represents edges according to incoming parameters
    virtual bool startEdge(const unsigned id) = 0;
    virtual bool endEdge() = 0;

    // Element properties
    virtual bool addLabel(const std::string &type, const std::string &label,
                          const tlp::Color &labelcolor, const tlp::Coord &coord,
                          const unsigned &fsize, const tlp::Size &size) = 0;
    virtual bool addRotation(const double rotation, const tlp::Coord &center) = 0;

  protected:
    virtual bool addBorder(const tlp::Color &borderColor, const double borderwidth) = 0;
    virtual bool addColor(const tlp::Color &nodeColor) = 0;
};

#endif // EXPORT_INTERFACE_H
