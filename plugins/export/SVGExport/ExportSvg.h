
#ifndef EXPORT_SVG_H
#define EXPORT_SVG_H

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

#include "ExportInterface.h"
#include <set>

#include <QXmlStreamWriter>
#include <QByteArray>
#include <QString>

class ExportSvg : public ExportInterface {

    QXmlStreamWriter _res;
    QByteArray _out;
    std::set<std::string> _base64fontAdded;
    bool _gloweffectAdded;
    bool createEdge(const tlp::EdgeShape::EdgeShapes &type, const std::vector<tlp::Coord> &bends,
                    const QString &color, const QString &qcolorA, const double width,
                    const tlp::EdgeExtremityShape::EdgeExtremityShapes src_anchor_shape_type,
                    const unsigned id_src_shape,
                    const tlp::EdgeExtremityShape::EdgeExtremityShapes tgt_anchor_shape_type,
                    const unsigned id_tgt_shape, const std::vector<tlp::Coord> &edgeVertice);
    void addWebFontFromIconName(const std::string &iconName);
    void addGlowEffect();

  public:
    ExportSvg(tlp::PluginProgress *pp, std::ostream &os, const bool autoformatting);
    bool writeHeader(const tlp::BoundingBox &bb) override;
    bool writeGraph(const tlp::BoundingBox &bb, const tlp::Color &background,
                    bool noBackground) override;
    bool writeMetaGraph(const int transform_X, const int transform_Y, float scale) override;
    bool writeEndGraph() override;
    bool groupNode() override;
    bool addShape(const tlp::NodeShape::NodeShapes &type, const tlp::Coord &coord,
                  const tlp::Size &size, const tlp::Color &bordercolor, const double borderwidth,
                  const tlp::Color &color, std::string iconName) override;
    bool addRotation(const double rotation, const tlp::Coord &center) override;
    bool endGroupNode() override;
    bool groupEdge() override;
    bool exportEdge(const tlp::EdgeShape::EdgeShapes &type, const std::vector<tlp::Coord> &bends,
                    const tlp::Color &color, const double width,
                    const tlp::EdgeExtremityShape::EdgeExtremityShapes src_anchor_shape_type,
                    const unsigned id_src_shape,
                    tlp::EdgeExtremityShape::EdgeExtremityShapes tgt_anchor_shape_type,
                    const unsigned id_tgt_shape,
                    const std::vector<tlp::Coord> &edgeVertice) override;
    bool exportEdge(const unsigned id, const tlp::EdgeShape::EdgeShapes &type,
                    const std::vector<tlp::Coord> &bends, const tlp::Color &color1,
                    const tlp::Color &color2, const double width,
                    const tlp::EdgeExtremityShape::EdgeExtremityShapes src_anchor_shape_type,
                    const unsigned id_src_shape,
                    const tlp::EdgeExtremityShape::EdgeExtremityShapes tgt_anchor_shape_type,
                    const unsigned id_tgt_shape,
                    const std::vector<tlp::Coord> &edgeVertice) override;
    bool
    exportEdgeExtremity(const unsigned id_src_shape, const unsigned id_tgt_shape,
                        const tlp::EdgeExtremityShape::EdgeExtremityShapes src_anchor_shape_type,
                        const tlp::EdgeExtremityShape::EdgeExtremityShapes tgt_anchor_shape_type,
                        const tlp::Color &color, const uint id_src_gradient,
                        const uint id_tgt_gradient, const std::string &iconName) override;
    bool addLabel(const std::string &type, const std::string &label, const tlp::Color &labelcolor,
                  const tlp::Coord &coord, const unsigned &fsize, const tlp::Size &size) override;
    bool addBorder(const tlp::Color &borderColor, const double borderwidth) override;
    bool startNode(const unsigned id) override;
    bool endNode() override;
    bool addColor(const tlp::Color &color) override;
    bool startEdge(const unsigned id) override;
    bool endEdge() override;
    bool endGroupEdge() override;
    bool writeEnd() override;
};

#endif // EXPORT_SVG_H
