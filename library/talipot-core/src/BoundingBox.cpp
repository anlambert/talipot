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

#include <talipot/BoundingBox.h>

static bool getIntersection(float fDst1, float fDst2, const tlp::Coord &p1, const tlp::Coord &p2,
                            tlp::Coord &hit) {
    if ((fDst1 * fDst2) >= 0.0f) {
        return false;
    }

    if (fDst1 == fDst2) {
        return false;
    }

    hit = p1 + (p2 - p1) * (-fDst1 / (fDst2 - fDst1));
    return true;
}

namespace tlp {

BoundingBox::BoundingBox() {
    (*this)[0].fill(1);
    (*this)[1].fill(-1);
    assert(!isValid());
}

BoundingBox::BoundingBox(const tlp::Coord &min, const tlp::Coord &max) {
    tlp::minMaxVectors(min, max, (*this)[0], (*this)[1]);
    assert(isValid());
}

void BoundingBox::expand(const tlp::Coord &coord) {
    if (isValid()) {
        (*this)[0] = tlp::minVector((*this)[0], coord);
        (*this)[1] = tlp::maxVector((*this)[1], coord);
    } else {
        (*this)[0] = coord;
        (*this)[1] = coord;
    }
}

void BoundingBox::expand(const tlp::BoundingBox &bb) {
    if (bb.isValid()) {
        expand(bb[0]);
        expand(bb[1]);
    }
}

void BoundingBox::translate(const tlp::Coord &vec) {
    (*this)[0] += vec;
    (*this)[1] += vec;
}

void BoundingBox::scale(const tlp::Coord &vec) {
    if (isValid()) {
        (*this)[0] *= vec;
        (*this)[1] *= vec;
    }
}

bool BoundingBox::isValid() const {
    return (*this)[0][0] <= (*this)[1][0] && (*this)[0][1] <= (*this)[1][1] &&
           (*this)[0][2] <= (*this)[1][2];
}

bool BoundingBox::contains(const tlp::Coord &coord) const {
    if (isValid()) {
        return (coord[0] >= (*this)[0][0] && coord[1] >= (*this)[0][1] &&
                coord[2] >= (*this)[0][2]) &&
               (coord[0] <= (*this)[1][0] && coord[1] <= (*this)[1][1] &&
                coord[2] <= (*this)[1][2]);
    } else {
        return false;
    }
}

bool BoundingBox::contains(const tlp::BoundingBox &boundingBox) const {
    if (isValid() && boundingBox.isValid()) {
        return contains(boundingBox[0]) && contains(boundingBox[1]);
    } else {
        return false;
    }
}

bool BoundingBox::intersect(const tlp::BoundingBox &boundingBox) const {
    if (!isValid() || !boundingBox.isValid()) {
        return false;
    }

    if ((*this)[1][0] < boundingBox[0][0]) {
        return false;
    }

    if (boundingBox[1][0] < (*this)[0][0]) {
        return false;
    }

    if ((*this)[1][1] < boundingBox[0][1]) {
        return false;
    }

    if (boundingBox[1][1] < (*this)[0][1]) {
        return false;
    }

    if ((*this)[1][2] < boundingBox[0][2]) {
        return false;
    }

    if (boundingBox[1][2] < (*this)[0][2]) {
        return false;
    }

    return true;
}

bool BoundingBox::intersect(const Coord &segStart, const Coord &segEnd) const {
    if (!isValid()) {
        return false;
    }

    if (segEnd[0] < (*this)[0][0] && segStart[0] < (*this)[0][0]) {
        return false;
    }

    if (segEnd[0] > (*this)[1][0] && segStart[0] > (*this)[1][0]) {
        return false;
    }

    if (segEnd[1] < (*this)[0][1] && segStart[1] < (*this)[0][1]) {
        return false;
    }

    if (segEnd[1] > (*this)[1][1] && segStart[1] > (*this)[1][1]) {
        return false;
    }

    if (segEnd[2] < (*this)[0][2] && segStart[2] < (*this)[0][2]) {
        return false;
    }

    if (segEnd[2] > (*this)[1][2] && segStart[2] > (*this)[1][2]) {
        return false;
    }

    if (segStart[0] > (*this)[0][0] && segStart[0] < (*this)[1][0] && segStart[1] > (*this)[0][1] &&
        segStart[1] < (*this)[1][1] && segStart[2] > (*this)[0][2] && segStart[2] < (*this)[1][2]) {
        return true;
    }

    Coord hit;

    return (getIntersection(segStart[0] - (*this)[0][0], segEnd[0] - (*this)[0][0], segStart,
                            segEnd, hit) &&
            contains(hit)) ||
           (getIntersection(segStart[1] - (*this)[0][1], segEnd[1] - (*this)[0][1], segStart,
                            segEnd, hit) &&
            contains(hit)) ||
           (getIntersection(segStart[2] - (*this)[0][2], segEnd[2] - (*this)[0][2], segStart,
                            segEnd, hit) &&
            contains(hit)) ||
           (getIntersection(segStart[0] - (*this)[1][0], segEnd[0] - (*this)[1][0], segStart,
                            segEnd, hit) &&
            contains(hit)) ||
           (getIntersection(segStart[1] - (*this)[1][1], segEnd[1] - (*this)[1][1], segStart,
                            segEnd, hit) &&
            contains(hit)) ||
           (getIntersection(segStart[2] - (*this)[1][2], segEnd[2] - (*this)[1][2], segStart,
                            segEnd, hit) &&
            contains(hit));
}

void BoundingBox::getCompleteBB(Coord bb[8]) const {
    bb[0] = (*this)[0];
    bb[1] = (*this)[0];
    bb[1][0] = (*this)[1][0];
    bb[2] = bb[1];
    bb[2][1] = (*this)[1][1];
    bb[3] = (*this)[0];
    bb[3][1] = (*this)[1][1];
    bb[4] = bb[0];
    bb[4][2] = (*this)[1][2];
    bb[5] = bb[1];
    bb[5][2] = (*this)[1][2];
    bb[6] = bb[2];
    bb[6][2] = (*this)[1][2];
    bb[7] = bb[3];
    bb[7][2] = (*this)[1][2];
}
}
