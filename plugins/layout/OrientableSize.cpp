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

#include "OrientableSizeProxy.h"

//====================================================================
OrientableSize::OrientableSize(OrientableSizeProxy *fatherParam, const float width,
                               const float height, const float depth)
    : father(fatherParam) {
    set(width, height, depth);
}

//====================================================================
OrientableSize::OrientableSize(OrientableSizeProxy *fatherParam, const tlp::Size &size)
    : father(fatherParam) {
    set(size);
}

//====================================================================
void OrientableSize::set(const float width, const float height, const float depth) {
    setW(width);
    setH(height);
    setD(depth);
}

//====================================================================
void OrientableSize::set(const tlp::Size &size) {
    tlp::Size::setW(size.getW());
    tlp::Size::setH(size.getH());
    tlp::Size::setD(size.getD());
}

//====================================================================
void OrientableSize::get(float *width, float *height, float *depth) const {
    *width = getW();
    *height = getH();
    *depth = getD();
}

//====================================================================
void OrientableSize::setW(float w) {
    (this->*(father->writeW))(w);
}

//====================================================================
void OrientableSize::setH(float h) {
    (this->*(father->writeH))(h);
}

//====================================================================
void OrientableSize::setD(float d) {
    (this->*(father->writeD))(d);
}

//====================================================================
float OrientableSize::getW() const {
    return (this->*(father->readW))();
}

//====================================================================
float OrientableSize::getH() const {
    return (this->*(father->readH))();
}

//====================================================================
float OrientableSize::getD() const {
    return (this->*(father->readD))();
}
