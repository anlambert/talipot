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

#include "OrientableLayout.h"

//====================================================================
OrientableCoord::OrientableCoord(OrientableLayout *fatherParam, const float x, const float y,
                                 const float z)
    : father(fatherParam) {
    set(x, y, z);
}
//====================================================================
OrientableCoord::OrientableCoord(OrientableLayout *fatherParam, const tlp::Coord &v)
    : father(fatherParam) {
    set(v);
}
//====================================================================
void OrientableCoord::set(const float x, const float y, const float z) {
    setX(x);
    setY(y);
    setZ(z);
}
//====================================================================
void OrientableCoord::set(const tlp::Coord &coord) {
    tlp::Coord::setX(coord.getX());
    tlp::Coord::setY(coord.getY());
    tlp::Coord::setZ(coord.getZ());
}
//====================================================================
void OrientableCoord::setX(float x) {
    (this->*(father->writeX))(x);
}
//====================================================================
void OrientableCoord::setY(float y) {
    (this->*(father->writeY))(y);
}
//====================================================================
void OrientableCoord::setZ(float z) {
    (this->*(father->writeZ))(z);
}
//====================================================================
float OrientableCoord::getX() const {
    return (this->*(father->readX))();
}
//====================================================================
float OrientableCoord::getY() const {
    return (this->*(father->readY))();
}
//====================================================================
float OrientableCoord::getZ() const {
    return (this->*(father->readZ))();
}
//====================================================================
void OrientableCoord::setInvertedX(float x) {
    tlp::Coord::setX(-x);
}
//====================================================================
void OrientableCoord::setInvertedY(float y) {
    tlp::Coord::setY(-y);
}
//====================================================================
void OrientableCoord::setInvertedZ(float z) {
    tlp::Coord::setZ(-z);
}
//====================================================================
float OrientableCoord::getInvertedX() const {
    return -tlp::Coord::getX();
}
//====================================================================
float OrientableCoord::getInvertedY() const {
    return -tlp::Coord::getY();
}
//====================================================================
float OrientableCoord::getInvertedZ() const {
    return -tlp::Coord::getZ();
}
//====================================================================
