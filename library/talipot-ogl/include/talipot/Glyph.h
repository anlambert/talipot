/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_GLYPH_H
#define TALIPOT_GLYPH_H

#include <talipot/Plugin.h>
#include <talipot/Size.h>
#include <talipot/Coord.h>
#include <talipot/PluginContext.h>
#include <talipot/MaterialDesignIcons.h>

namespace tlp {

static const std::string GLYPH_CATEGORY = "Node shape";

struct BoundingBox;
class Graph;
struct node;
class GlGraphInputData;
class GlRect;

class GlyphContext : public PluginContext {
public:
  GlGraphInputData *glGraphInputData;
  ///
  GlyphContext(GlGraphInputData *glGraphInputData = nullptr) : glGraphInputData(glGraphInputData) {}
  ///
  ~GlyphContext() override = default;
};
//==========================================================
class TLP_GL_SCOPE Glyph : public tlp::Plugin {
public:
  std::string category() const override {
    return GLYPH_CATEGORY;
  }
  std::string icon() const override {
    return MaterialDesignIcons::ShapePlus;
  }

  Glyph(const tlp::PluginContext *context);
  ~Glyph() override;

  virtual std::string getGroup() const {
    return "";
  }

  virtual BoundingBox getIncludeBoundingBox(node n);

  virtual BoundingBox getTextBoundingBox(node n);

  virtual void draw(node, float) = 0;
  /*
   * return a point where an edge coming from "from" can be attached
   * by default, the point will be on the surface of the largest sphere contained
   * inside the unit cube (before scaling).
   */
  virtual Coord getAnchor(const Coord &nodeCenter, const Coord &from, const Size &scale,
                          const double zRotation) const;

  /**
   * Return if the Glyph render its label (return true) or if GlNode have to render label (return
   * false)
   */
  virtual bool renderLabel() {
    return false;
  }

  /**
   * Return if the Glyph supports shader rendering optimization (see GlNode.cpp)
   */
  virtual bool shaderSupported() const {
    return true;
  }

  /**
   * draw a preconfigured GlRect in the screen plane
   */
  static void drawRectInScreenPlane(GlRect &rect, const Size &size, bool disableMasks);

  GlGraphInputData *glGraphInputData;

protected:
  /*
   * called by public method getAnchor to actually compute the anchor point
   * vector is coordinate of the point to anchor to, relative to nodecenter
   * glyph size is (1,1,1)
   * this is the method to redefine for each glyph where the default 'getAnchor' method
   * is inapropriated
   * Returned value is a vector to be applied to 'nodeCenter' in the public method
   */
  virtual Coord getAnchor(const Coord &vector) const;
};
//==========================================================
class TLP_GL_SCOPE NoShaderGlyph : public Glyph {
public:
  NoShaderGlyph(const tlp::PluginContext *context = nullptr) : Glyph(context) {}
  bool shaderSupported() const override {
    return false;
  }
};
}

#define GLYPHINFORMATION(NAME, AUTHOR, DATE, INFO, RELEASE, ID) \
  PLUGININFORMATION(NAME, AUTHOR, DATE, INFO, RELEASE, "")      \
  int id() const override {                                     \
    return ID;                                                  \
  }

#endif // TALIPOT_GLYPH_H
