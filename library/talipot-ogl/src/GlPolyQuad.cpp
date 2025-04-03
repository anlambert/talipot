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

#include <talipot/GlTextureManager.h>
#include <talipot/GlTools.h>
#include <talipot/GlXMLTools.h>
#include <talipot/GlPolyQuad.h>

using namespace std;

namespace tlp {

GlPolyQuad::GlPolyQuad(const string &textureName, const bool outlined, const int outlineWidth,
                       const Color &outlineColor)
    : textureName(textureName), outlined(outlined), outlineWidth(outlineWidth),
      outlineColor(outlineColor) {}

GlPolyQuad::GlPolyQuad(const vector<Coord> &polyQuadEdges, const vector<Color> &polyQuadEdgesColors,
                       const string &textureName, const bool outlined, const int outlineWidth,
                       const Color &outlineColor)
    : textureName(textureName), outlined(outlined), outlineWidth(outlineWidth),
      outlineColor(outlineColor) {

    assert(polyQuadEdges.size() % 2 == 0 && polyQuadEdges.size() > 2 &&
           polyQuadEdgesColors.size() == (polyQuadEdges.size() / 2));

    for (size_t i = 0; i < (polyQuadEdges.size() / 2); ++i) {
        addQuadEdge(polyQuadEdges[2 * i], polyQuadEdges[2 * i + 1], polyQuadEdgesColors[i]);
    }
}

GlPolyQuad::GlPolyQuad(const std::vector<Coord> &polyQuadEdges, const Color &polyQuadColor,
                       const std::string &textureName, const bool outlined, const int outlineWidth,
                       const Color &outlineColor)
    : textureName(textureName), outlined(outlined), outlineWidth(outlineWidth),
      outlineColor(outlineColor) {

    assert(polyQuadEdges.size() % 2 == 0 && polyQuadEdges.size() > 2);

    for (size_t i = 0; i < (polyQuadEdges.size() / 2); ++i) {
        addQuadEdge(polyQuadEdges[2 * i], polyQuadEdges[2 * i + 1], polyQuadColor);
    }
}

void GlPolyQuad::addQuadEdge(const Coord &startEdge, const Coord &endEdge, const Color &edgeColor) {
    polyQuadEdges.push_back(startEdge);
    polyQuadEdges.push_back(endEdge);
    boundingBox.expand(startEdge);
    boundingBox.expand(endEdge);
    polyQuadEdgesColors.push_back(edgeColor);
}

void GlPolyQuad::draw(float, Camera *) {

    assert(polyQuadEdges.size() % 2 == 0 && polyQuadEdges.size() > 2 &&
           polyQuadEdgesColors.size() == (polyQuadEdges.size() / 2));

    vector<Coord> vertexArray;
    vector<float> texCoordsArray;
    vector<Vec4f> colorsArray;
    vector<unsigned short> quadIndices;
    vector<unsigned short> outlineIndices;

    uint nbSubdivisionsPerSegment = 1;
    uint nbVertices = polyQuadEdges.size();
    vector<Coord> *vertices = &polyQuadEdges;

    texCoordsArray.reserve(nbVertices * 2);
    colorsArray.reserve(nbVertices);
    quadIndices.reserve(nbVertices);
    outlineIndices.resize(nbVertices);

    for (size_t i = 0; i < (polyQuadEdges.size() / 2) - 1; ++i) {
        Vec4f startColor;
        Vec4f endColor;
        startColor[0] = polyQuadEdgesColors[i].getRGL();
        startColor[1] = polyQuadEdgesColors[i].getGGL();
        startColor[2] = polyQuadEdgesColors[i].getBGL();
        startColor[3] = polyQuadEdgesColors[i].getAGL();
        endColor[0] = polyQuadEdgesColors[i + 1].getRGL();
        endColor[1] = polyQuadEdgesColors[i + 1].getGGL();
        endColor[2] = polyQuadEdgesColors[i + 1].getBGL();
        endColor[3] = polyQuadEdgesColors[i + 1].getAGL();

        if (nbSubdivisionsPerSegment == 1) {

            texCoordsArray.push_back(GLfloat(i));
            texCoordsArray.push_back(0.0f);
            texCoordsArray.push_back(GLfloat(i));
            texCoordsArray.push_back(1.0f);
            colorsArray.push_back(startColor);
            colorsArray.push_back(startColor);

            quadIndices.push_back(2 * i);
            quadIndices.push_back(2 * i + 1);

            outlineIndices[i] = 2 * i;
            outlineIndices[nbVertices - (i + 1)] = 2 * i + 1;

        } else {

            for (uint j = 0; j < nbSubdivisionsPerSegment; ++j) {

                uint n = i * nbSubdivisionsPerSegment + j;

                Coord v1 =
                    polyQuadEdges[2 * i] + (j / float(nbSubdivisionsPerSegment - 1)) *
                                               (polyQuadEdges[2 * (i + 1)] - polyQuadEdges[2 * i]);
                Coord v2 = polyQuadEdges[2 * i + 1] +
                           (j / float(nbSubdivisionsPerSegment - 1)) *
                               (polyQuadEdges[2 * (i + 1) + 1] - polyQuadEdges[2 * i + 1]);
                vertexArray.push_back(v1);
                vertexArray.push_back(v2);

                float texCoordFactor = ((polyQuadEdges[2 * i].dist(polyQuadEdges[2 * i + 2])) /
                                        (nbSubdivisionsPerSegment - 1)) /
                                       (polyQuadEdges[2 * i].dist(polyQuadEdges[2 * i + 1]));
                texCoordsArray.push_back(GLfloat(i) + GLfloat(j) * texCoordFactor);
                texCoordsArray.push_back(0.0f);
                texCoordsArray.push_back(GLfloat(i) + GLfloat(j) * texCoordFactor);
                texCoordsArray.push_back(1.0f);

                Vec4f color = startColor +
                              (j / float(nbSubdivisionsPerSegment - 1)) * (endColor - startColor);
                colorsArray.push_back(color);
                colorsArray.push_back(color);

                quadIndices.push_back(2 * n);
                quadIndices.push_back(2 * n + 1);

                outlineIndices[n] = 2 * n;
                outlineIndices[nbVertices - (n + 1)] = 2 * n + 1;
            }
        }

        if (nbSubdivisionsPerSegment == 1 && i == (polyQuadEdges.size() / 2) - 2) {

            quadIndices.push_back(2 * (i + 1));
            quadIndices.push_back(2 * (i + 1) + 1);
            outlineIndices[i + 1] = 2 * (i + 1);
            outlineIndices[nbVertices - (i + 2)] = 2 * (i + 1) + 1;
            texCoordsArray.push_back(GLfloat(i + 1));
            texCoordsArray.push_back(0.0f);
            texCoordsArray.push_back(GLfloat(i + 1));
            texCoordsArray.push_back(1.0f);
            colorsArray.push_back(endColor);
            colorsArray.push_back(endColor);
        }
    }

    outlineIndices.push_back(0);

    if (!textureName.empty()) {
        GlTextureManager::activateTexture(textureName);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    }

    glDisable(GL_CULL_FACE);

    glDisable(GL_LIGHTING);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 3 * sizeof(float), &((*vertices)[0][0]));
    glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(float), &texCoordsArray[0]);
    glColorPointer(4, GL_FLOAT, 4 * sizeof(float), &colorsArray[0][0]);

    if (nbSubdivisionsPerSegment > 1) {
        glDrawElements(GL_QUAD_STRIP, vertexArray.size(), GL_UNSIGNED_SHORT, &quadIndices[0]);
    } else {
        glDrawElements(GL_QUAD_STRIP, polyQuadEdges.size(), GL_UNSIGNED_SHORT, &quadIndices[0]);
    }

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    if (!textureName.empty()) {
        GlTextureManager::deactivateTexture();
    }

    if (outlined) {
        glLineWidth(outlineWidth);
        setMaterial(outlineColor);

        if (nbSubdivisionsPerSegment > 1) {
            glDrawElements(GL_LINE_LOOP, vertexArray.size(), GL_UNSIGNED_SHORT, &outlineIndices[0]);
        } else {
            glDrawElements(GL_LINE_LOOP, polyQuadEdges.size(), GL_UNSIGNED_SHORT,
                           &outlineIndices[0]);
        }

        if (outlineWidth != 1) {
            glLineWidth(1);
        }
    }

    glDisableClientState(GL_VERTEX_ARRAY);

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
}

void GlPolyQuad::translate(const Coord &move) {
    boundingBox.translate(move);

    for (auto &polyQuadEdge : polyQuadEdges) {
        polyQuadEdge += move;
    }
}

void GlPolyQuad::setColor(const Color &color) {
    for (auto &polyQuadEdgesColor : polyQuadEdgesColors) {
        polyQuadEdgesColor = color;
    }
}

void GlPolyQuad::getXML(string &outString) {

    GlXMLTools::createProperty(outString, "type", "GlPolyQuad", "GlEntity");

    GlXMLTools::getXML(outString, "polyQuadEdges", polyQuadEdges);
    GlXMLTools::getXML(outString, "polyQuadEdgesColors", polyQuadEdgesColors);
    GlXMLTools::getXML(outString, "textureName", textureName);
}

void GlPolyQuad::setWithXML(const string &inString, uint &currentPosition) {

    GlXMLTools::setWithXML(inString, currentPosition, "polyQuadEdges", polyQuadEdges);
    GlXMLTools::setWithXML(inString, currentPosition, "polyQuadEdgesColors", polyQuadEdgesColors);
    GlXMLTools::setWithXML(inString, currentPosition, "textureName", textureName);

    for (const auto &p : polyQuadEdges) {
        boundingBox.expand(p);
    }
}
}
