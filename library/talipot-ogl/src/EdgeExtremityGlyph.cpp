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

#include <talipot/EdgeExtremityGlyph.h>
#include <talipot/Glyph.h>

using namespace tlp;

EdgeExtremityGlyph::EdgeExtremityGlyph(const PluginContext *context)
    : edgeExtGlGraphInputData(nullptr) {
    if (context != nullptr) {
        const auto *glyphContext = static_cast<const GlyphContext *>(context);
        edgeExtGlGraphInputData = glyphContext->glGraphInputData;
    }
}

EdgeExtremityGlyph::~EdgeExtremityGlyph() = default;

void EdgeExtremityGlyph::get3DTransformationMatrix(const Coord &src, const Coord &dest,
                                                   const Size &glyphSize,
                                                   MatrixGL &transformationMatrix,
                                                   MatrixGL &scalingMatrix) {
    // Vecteur AB
    Vec3f vAB;
    // Vecteur V
    Vec3f vV;
    // Vecteur W
    Vec3f vW;

    vAB = dest - src;
    float nAB; //|AB|
    nAB = vAB.norm();

    if (fabs(nAB) > 1E-6) {
        vAB /= nAB;
    }

    // vAB * vV = xAB * xV + yAB*yV + zAB * zV = |AB| * |V| * cos(alpha) = 0;
    if (fabs(vAB[2]) < 1E-6) {
        vV[0] = 0;
        vV[1] = 0;
        vV[2] = 1.0;
    } else if (fabs(vAB[1]) < 1E-6) {
        vV[0] = 0;
        vV[1] = 1.0;
        vV[2] = 0;
    } else {
        vV[0] = 0;
        vV[1] = 1. / vAB[1];
        vV[2] = -1. / vAB[2];
        vV /= vV.norm();
    }

    vW = vAB ^ vV;
    float nW = vW.norm();

    if (fabs(nW) > 1E-6) {
        vW /= nW;
    }

    for (uint i = 0; i < 3; ++i) {
        transformationMatrix[0][i] = vW[i];
        transformationMatrix[1][i] = vV[i];
        transformationMatrix[2][i] = vAB[i];
        transformationMatrix[3][i] = dest[i] + (-glyphSize[0] * .5 * vAB[i]);
    }

    transformationMatrix[0][3] = 0;
    transformationMatrix[1][3] = 0;
    transformationMatrix[2][3] = 0;
    transformationMatrix[3][3] = 1;

    scalingMatrix.fill(0);
    scalingMatrix[0][0] = glyphSize[1];
    scalingMatrix[1][1] = glyphSize[2];
    scalingMatrix[2][2] = glyphSize[0];
    scalingMatrix[3][3] = 1;
}

void EdgeExtremityGlyph::get2DTransformationMatrix(const Coord &src, const Coord &dest,
                                                   const Size &glyphSize,
                                                   MatrixGL &transformationMatrix,
                                                   MatrixGL &scalingMatrix) {
    //  Vecteur AB
    Vec3f vAB;
    // Vecteur V
    Vec3f vV;
    // Vecteur W
    Vec3f vW;

    vAB = dest - src;
    float nAB; //|AB|
    nAB = vAB.norm();

    if (fabs(nAB) > 1E-6) {
        vAB /= nAB;
    }

    // vAB * vV = xAB * xV + yAB*yV + zAB * zV = |AB| * |V| * cos(alpha) = 0;
    if (fabs(vAB[2]) < 1E-6) {
        vV[0] = 0;
        vV[1] = 0;
        vV[2] = 1.0;
    } else if (fabs(vAB[1]) < 1E-6) {
        vV[0] = 0;
        vV[1] = 1.0;
        vV[2] = 0;
    } else {
        vV[0] = 0;
        vV[1] = 1. / vAB[1];
        vV[2] = -1. / vAB[2];
        vV /= vV.norm();
    }

    vW = vAB ^ vV;
    float nW = vW.norm();

    if (fabs(nW) > 1E-6) {
        vW /= nW;
    }

    for (uint i = 0; i < 3; ++i) {
        transformationMatrix[0][i] = vAB[i];
        transformationMatrix[1][i] = vW[i];
        transformationMatrix[2][i] = vV[i];
        transformationMatrix[3][i] = dest[i] + (-glyphSize[0] * .5 * vAB[i]);
    }

    transformationMatrix[0][3] = 0;
    transformationMatrix[1][3] = 0;
    transformationMatrix[2][3] = 0;
    transformationMatrix[3][3] = 1;

    scalingMatrix.fill(0);
    scalingMatrix[0][0] = glyphSize[0];
    scalingMatrix[1][1] = glyphSize[1];
    scalingMatrix[2][2] = glyphSize[2];
    scalingMatrix[3][3] = 1;
}
