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

#ifndef ORIENTABLE_SIZE_PROXY_H
#define ORIENTABLE_SIZE_PROXY_H

#include <vector>
#include <talipot/SizeProperty.h>
#include "OrientableConstants.h"
#include "OrientableSize.h"

class OrientableSizeProxy {
    friend class OrientableSize;

  public:
    typedef OrientableSize PointType;
    typedef OrientableSize LineType;

    OrientableSizeProxy(tlp::SizeProperty *sizesProxy, orientationType mask = ORI_DEFAULT);

    OrientableSize createSize(const float width = 0, const float height = 0, const float depth = 0);
    OrientableSize createSize(const tlp::Size &v);

    void setOrientation(orientationType mask);

    void setAllNodeValue(const PointType &v);
    void setAllEdgeValue(const LineType &v);

    void setEdgeValue(const tlp::edge e, const LineType &v);
    void setNodeValue(tlp::node n, const PointType &v);

    PointType getNodeValue(const tlp::node n);
    LineType getEdgeValue(const tlp::edge e);

    PointType getNodeDefaultValue();
    LineType getEdgeDefaultValue();

  private:
    tlp::SizeProperty *sizesProxy;
    orientationType orientation;

    float (tlp::Size::*readW)() const;
    float (tlp::Size::*readH)() const;
    float (tlp::Size::*readD)() const;
    void (tlp::Size::*writeW)(float);
    void (tlp::Size::*writeH)(float);
    void (tlp::Size::*writeD)(float);
};

#endif // ORIENTABLE_SIZE_PROXY_H
