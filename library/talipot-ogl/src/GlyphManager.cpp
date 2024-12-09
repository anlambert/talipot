/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/GlyphManager.h>

//====================================================
using namespace std;

namespace tlp {
static std::list<std::string> glyphList;
static flat_hash_map<int, std::string> glyphIdToName;
static flat_hash_map<std::string, int> nameToGlyphId;

//====================================================
string GlyphManager::glyphName(int id) {
  auto it = glyphIdToName.find(id);
  if (it != glyphIdToName.end()) {
    return it->second;
  } else {
    assert(false);
    tlp::warning() << "Invalid glyph id: " << id << endl;
    return string("invalid");
  }
}
//====================================================
int GlyphManager::glyphId(const string &name, bool warnIfNotFound) {
  auto it = nameToGlyphId.find(name);
  if (it != nameToGlyphId.end()) {
    return it->second;
  } else {
    if (warnIfNotFound) {
      assert(false);
      tlp::warning() << "Invalid glyph name: \"" << name.c_str() << '"' << endl;
    }
    return 0;
  }
}
//====================================================
void GlyphManager::loadGlyphPlugins() {
  glyphList = PluginsManager::availablePlugins<Glyph>();

  for (const std::string &pluginName : glyphList) {
    int pluginId = PluginsManager::pluginInformation(pluginName).id();
    glyphIdToName[pluginId] = pluginName;
    nameToGlyphId[pluginName] = pluginId;
  }
}

GlyphManager::GlyphManager(GlGraphInputData *inputData) {
  GlyphContext gc = GlyphContext(inputData);
  _defaultGlyph.reset(PluginsManager::getPluginObject<Glyph>("3D - Cube OutLined", &gc));

  for (const std::string &glyphName : glyphList) {
    auto *newGlyph = PluginsManager::getPluginObject<Glyph>(glyphName, &gc);
    _glyphs[PluginsManager::pluginInformation(glyphName).id()] = newGlyph;
  }
}

GlyphManager::~GlyphManager() {
  for (const auto &[id, glyph] : _glyphs) {
    delete glyph;
  }
}

Glyph *GlyphManager::getGlyph(int id) const {
  if (const auto &it = _glyphs.find(id); it != _glyphs.end()) {
    return it->second;
  }
  return _defaultGlyph.get();
}

}
