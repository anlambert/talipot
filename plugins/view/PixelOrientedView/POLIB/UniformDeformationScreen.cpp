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

#include "UniformDeformationScreen.h"

UniformDeformationScreen::UniformDeformationScreen() : _tx(0), _ty(0), _zoom(1) {
    updateMatrix();
}
//=======================================================
tlp::Vec2f UniformDeformationScreen::project(const tlp::Vec2f &point) const {
    tlp::Vec2f scPoint;
    tlp::Vector<double, 3> vect, vectres;
    vect[0] = point[0];
    vect[1] = point[1];
    vect[2] = 1.0;
    vectres = _mat * vect;
    scPoint[0] = (vectres[0] / vectres[2]);
    scPoint[1] = (vectres[1] / vectres[2]);
    return scPoint;
}
//=======================================================
tlp::Vec2f UniformDeformationScreen::unproject(const tlp::Vec2f &point) const {
    tlp::Vec2f scPoint(point);
    tlp::Vector<double, 3> vect, vectres;
    vect[0] = point[0];
    vect[1] = point[1];
    vect[2] = 1.0;
    vectres = _invMat * vect;
    scPoint[0] = vectres[0] / vectres[2];
    scPoint[1] = vectres[1] / vectres[2];
    return scPoint;
}
//=======================================================
void UniformDeformationScreen::setTranslation(double x, double y) {
    _tx = x;
    _ty = y;
    updateMatrix();
}
//=======================================================
void UniformDeformationScreen::getTranslation(double &x, double &y) const {
    x = _tx;
    y = _ty;
}
//=======================================================
void UniformDeformationScreen::setZoom(double zoom) {
    _zoom = zoom;
    updateMatrix();
}
//=======================================================
double UniformDeformationScreen::getZoom() const {
    return _zoom;
}
//=======================================================
void UniformDeformationScreen::updateMatrix() {
    tlp::Matrix<double, 3> tmp;
    // zoom
    tmp.fill(0);
    tmp[0][0] = _zoom;
    tmp[1][1] = _zoom;
    tmp[2][2] = 1.0;
    tmp[2][0] = 0.;
    tmp[2][1] = 0.;
    // translate
    _mat.fill(0);
    _mat[0][0] = 1.;
    _mat[1][1] = 1.;
    _mat[2][2] = 1.;
    _mat[0][2] = -_tx;
    _mat[1][2] = -_ty;
    _mat = tmp * _mat;
    _invMat = _mat;
    _invMat.inverse();
}
