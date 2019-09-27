/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/GlLine.h>
#include <talipot/GlTools.h>
#include <talipot/GlXMLTools.h>

using namespace std;

namespace tlp {
GlLine::GlLine(const vector<Coord> &points, const vector<Color> &colors)
    : _points(points), _colors(colors), width(1.0), factor(1), pattern(0) {

  for (vector<Coord>::iterator it = _points.begin(); it != _points.end(); ++it)
    boundingBox.expand(*it);
}
//=====================================================
GlLine::~GlLine() {}
//=====================================================
void GlLine::resizePoints(const unsigned int nbPoints) {
  _points.resize(nbPoints);
}
//=====================================================
void GlLine::resizeColors(const unsigned int nbColors) {
  assert(nbColors >= 1);
  _points.resize(nbColors);
}
//=====================================================
const tlp::Coord &GlLine::point(const unsigned int i) const {
  return _points[i];
}
//=====================================================
tlp::Coord &GlLine::point(const unsigned int i) {
  return _points[i];
}
//=====================================================
void GlLine::addPoint(const Coord &point, const Color &color) {
  _points.push_back(point);
  _colors.push_back(color);
  boundingBox.expand(point);
}
//=====================================================
const tlp::Color &GlLine::color(const unsigned int i) const {
  return _colors[i];
}
//=====================================================
tlp::Color &GlLine::color(const unsigned int i) {
  return _colors[i];
}
//=====================================================
void GlLine::draw(float, Camera *) {
  GL_THROW_ON_ERROR();

  glDisable(GL_LIGHTING);
  glLineWidth(width);

  if (pattern != 0) {
    glLineStipple(factor, pattern);
    glEnable(GL_LINE_STIPPLE);
  }

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glVertexPointer(3, GL_FLOAT, 3 * sizeof(float), &_points[0]);
  glColorPointer(4, GL_UNSIGNED_BYTE, 4 * sizeof(unsigned char), &_colors[0]);
  glDrawArrays(GL_LINE_STRIP, 0, _points.size());
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);

  if (pattern != 0)
    glDisable(GL_LINE_STIPPLE);

  glLineWidth(1.0);
  glEnable(GL_LIGHTING);

  GL_THROW_ON_ERROR();
}
//=====================================================
void GlLine::setLineWidth(float width) {
  this->width = width;
}
//=====================================================
void GlLine::setLineStipple(unsigned char factor, unsigned int pattern) {
  this->factor = factor;
  this->pattern = pattern;
}
//=====================================================
void GlLine::translate(const Coord &mouvement) {
  boundingBox.translate(mouvement);

  for (vector<Coord>::iterator it = _points.begin(); it != _points.end(); ++it) {
    (*it) += mouvement;
  }
}
//=====================================================
void GlLine::getXML(string &outString) {
  GlXMLTools::createProperty(outString, "type", "GlLine", "GlEntity");

  GlXMLTools::getXML(outString, "points", _points);
  GlXMLTools::getXML(outString, "colors", _colors);
  GlXMLTools::getXML(outString, "width", width);
  GlXMLTools::getXML(outString, "factor", factor);
  GlXMLTools::getXML(outString, "pattern", pattern);
}
//============================================================
void GlLine::setWithXML(const string &inString, unsigned int &currentPosition) {

  GlXMLTools::setWithXML(inString, currentPosition, "points", _points);
  GlXMLTools::setWithXML(inString, currentPosition, "colors", _colors);
  GlXMLTools::setWithXML(inString, currentPosition, "width", width);
  GlXMLTools::setWithXML(inString, currentPosition, "factor", factor);
  GlXMLTools::setWithXML(inString, currentPosition, "pattern", pattern);

  for (vector<Coord>::iterator it = _points.begin(); it != _points.end(); ++it)
    boundingBox.expand(*it);
}
} // namespace tlp
