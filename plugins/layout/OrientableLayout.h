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

#ifndef ORIENTABLE_LAYOUT_H
#define ORIENTABLE_LAYOUT_H

#include "OrientableLayoutInterface.h"

class OrientableLayout : public OrientableLayoutInterface {
    friend class OrientableCoord;

  public:
    OrientableLayout(tlp::LayoutProperty *layout, orientationType mask = ORI_DEFAULT);

    OrientableCoord createCoord(const float x = 0, const float y = 0, const float z = 0) override;
    OrientableCoord createCoord(const tlp::Coord &v) override;

    void setOrientation(orientationType mask) override;

    void setAllNodeValue(const PointType &v) override;
    void setAllEdgeValue(const LineType &v) override;

    void setEdgeValue(const tlp::edge e, const LineType &v) override;
    void setNodeValue(tlp::node n, const PointType &v) override;

    PointType getNodeValue(const tlp::node n) override;
    LineType getEdgeValue(const tlp::edge e) override;

    PointType getNodeDefaultValue() override;
    LineType getEdgeDefaultValue() override;

    void setOrthogonalEdge(const tlp::Graph *tree, float interNodeDistance);

  private:
    typedef std::vector<tlp::Coord> CoordLineType;
    std::vector<OrientableCoord> convertEdgeLinetype(const std::vector<tlp::Coord> &v);

    tlp::LayoutProperty *layout;
    orientationType orientation;

    float (OrientableCoord::*readX)() const;
    float (OrientableCoord::*readY)() const;
    float (OrientableCoord::*readZ)() const;
    void (OrientableCoord::*writeX)(float);
    void (OrientableCoord::*writeY)(float);
    void (OrientableCoord::*writeZ)(float);
    void addControlPoints(const tlp::Graph *tree, OrientableCoord fatherCoord, tlp::edge e,
                          float interNodeDistance);
};

#endif // ORIENTABLE_LAYOUT_H
