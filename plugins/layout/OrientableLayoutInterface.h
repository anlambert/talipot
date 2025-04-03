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

#ifndef ORIENTABLE_LAYOUT_INTERFACE_H
#define ORIENTABLE_LAYOUT_INTERFACE_H

#include <vector>
#include <talipot/LayoutProperty.h>
#include "OrientableConstants.h"
#include "OrientableCoord.h"

class OrientableLayoutInterface {
  public:
    typedef OrientableCoord PointType;
    typedef std::vector<OrientableCoord> LineType;

    virtual ~OrientableLayoutInterface() = default;

    virtual OrientableCoord createCoord(const float x = 0, const float y = 0,
                                        const float z = 0) = 0;
    virtual OrientableCoord createCoord(const tlp::Coord &v) = 0;

    virtual void setOrientation(orientationType mask) = 0;

    virtual void setAllNodeValue(const PointType &v) = 0;
    virtual void setAllEdgeValue(const LineType &v) = 0;

    virtual void setEdgeValue(const tlp::edge e, const LineType &v) = 0;
    virtual void setNodeValue(tlp::node n, const PointType &v) = 0;

    virtual PointType getNodeValue(const tlp::node n) = 0;
    virtual LineType getEdgeValue(const tlp::edge e) = 0;

    virtual PointType getNodeDefaultValue() = 0;
    virtual LineType getEdgeDefaultValue() = 0;
};

#endif // ORIENTABLE_LAYOUT_INTERFACE_H
