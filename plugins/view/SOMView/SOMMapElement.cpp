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

#include "SOMMapElement.h"
#include <SOMMap.h>

#include <talipot/GlRect.h>
#include <talipot/GlCircle.h>
#include <talipot/ColorProperty.h>

#define DEGTORAD(x) (M_PI / 180.f) * x

using namespace tlp;

SOMMapElement::SOMMapElement(tlp::Coord position, tlp::Size size, SOMMap *map,
                             tlp::ColorProperty *colorProperty)
    : som(map), position(position), size(size) {
    buildMainComposite(position, size, map);

    if (colorProperty) {
        updateColors(colorProperty);
    }

    computeNodeAreaSize();
}

SOMMapElement::~SOMMapElement() {
    reset(true);
}

void SOMMapElement::setData(SOMMap *map, tlp::ColorProperty *colorProperty) {
    som = map;
    reset(true);
    nodesMap.clear();
    buildMainComposite(position, size, som);

    if (colorProperty) {
        updateColors(colorProperty);
    }

    computeNodeAreaSize();
}

float SOMMapElement::computeMaximizedRadiusForHexagone(uint width, uint height, tlp::Size &size) {
    float ry = (4 * size.getH()) / (3 * height + 1) / 2;
    float rx = (size.getW() / (cos(DEGTORAD(30)) * width)) / 2;

    // Take the min
    return rx > ry ? ry : rx;
}

void SOMMapElement::buildMainComposite(tlp::Coord basePos, tlp::Size gridSize, SOMMap *map) {
    reset(true);

    SOMMap::SOMMapConnectivity conn = map->getConnectivity();
    tlp::node n;
    std::ostringstream oss;
    oss.str("");

    if (conn == SOMMap::six) {

        // Compute the best radius
        float r = computeMaximizedRadiusForHexagone(map->getWidth(), map->getHeight(), gridSize);

        float h = r / 2;
        float ri = cos(DEGTORAD(30)) * r;

        float top = basePos.getY() + gridSize.getH();

        for (uint i = 0; i < map->getHeight(); ++i) {
            for (uint j = 0; j < map->getWidth(); ++j) {

                float x = (j * ri * 2) + ri;
                float y = ((i + 1) * ((2 * r) - h)) - h;

                Coord center = {basePos.getX() + x, top - y};

                if (i % 2 != 0) {
                    center.setX(center.getX() + ri);
                }

                n = map->getNodeAt(j, i);
                Color c = Color(255, 255, 255, 0);
                auto *hex = new tlp::GlCircle(center, r, c, c, true, false, float(M_PI / 2), 6);
                oss.str("");
                oss << j << "," << i;
                addGlEntity(hex, oss.str());
                nodesMap[n] = hex;
            }
        }
    } else {

        Coord elementSize = {gridSize.getW() / map->getWidth(), gridSize.getH() / map->getHeight()};

        for (uint i = 0; i < map->getHeight(); ++i) {
            for (uint j = 0; j < map->getWidth(); ++j) {
                Coord topLeft = {j * elementSize.getX(),
                                 (map->getHeight() - i) * elementSize.getY()};
                topLeft += basePos;
                Coord bottomRight = {topLeft.getX() + elementSize.getX(),
                                     topLeft.getY() - elementSize.getY()};

                assert(topLeft.getX() < bottomRight.getX() && topLeft.getY() > bottomRight.getY());
                tlp::GlRect *rec = nullptr;
                n = map->getNodeAt(j, i);
                Color c = Color(255, 255, 255, 0);
                rec = new tlp::GlRect(topLeft, bottomRight, c, c);
                oss.str("");
                oss << j << "," << i;
                addGlEntity(rec, oss.str());
                nodesMap[n] = rec;
            }
        }
    }
}

void SOMMapElement::updateColors(ColorProperty *newColor) {
    SOMMap::SOMMapConnectivity connect = som->getConnectivity();
    for (auto n : som->nodes()) {
        if (connect == SOMMap::six) {
            auto *hex = static_cast<GlCircle *>(nodesMap[n]);
            hex->setFillColor(newColor->getNodeValue(n));
        } else {
            auto *rect = static_cast<GlRect *>(nodesMap[n]);
            rect->setBottomRightColor(newColor->getNodeValue(n));
            rect->setTopLeftColor(newColor->getNodeValue(n));
        }
    }
}

tlp::Coord SOMMapElement::getTopLeftPositionForElement(uint x, uint y) {
    Coord pos;

    if (som->getConnectivity() == SOMMap::six) {

        float r = computeMaximizedRadiusForHexagone(som->getWidth(), som->getHeight(), size);

        float h = r / 2;
        float ri = cos(DEGTORAD(30)) * r;

        if (y % 2 == 0) {
            pos.setX(x * ri * 2);
        } else {
            pos.setX(ri * (x * 2 + 1));
        }

        float ys = ((y + 1) * ((2 * r) - h)) - r;

        pos.setX(position.getX() + pos.getX());

        float top = position.getY() + size.getH();
        pos.setY(top - ys);

    } else {
        pos.set(x * (size.getW() / som->getWidth()),
                (som->getHeight() - y) * (size.getH() / som->getHeight()), 0);
        pos += position;
    }

    return pos;
}
tlp::Size SOMMapElement::getNodeAreaSize() {
    return nodeAreaSize;
}

void SOMMapElement::computeNodeAreaSize() {
    if (som->getConnectivity() == SOMMap::six) {
        // Compute the best radius
        float r = computeMaximizedRadiusForHexagone(som->getWidth(), som->getHeight(), size);
        float ri = cos(DEGTORAD(30)) * r;

        nodeAreaSize = Size(2 * ri, r, 0);
    } else {
        nodeAreaSize.set(size.getW() / som->getWidth(), size.getH() / som->getHeight(), 0);
    }
}
