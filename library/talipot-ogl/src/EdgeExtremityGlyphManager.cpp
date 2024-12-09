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

#include <talipot/EdgeExtremityGlyph.h>
#include <talipot/EdgeExtremityGlyphManager.h>
#include <talipot/Glyph.h>
#include <talipot/ViewSettings.h>

using namespace std;

namespace tlp {
static std::list<std::string> plugins;
static flat_hash_map<int, std::string> eeglyphIdToName;
static flat_hash_map<std::string, int> nameToEeGlyphId;

//====================================================
string EdgeExtremityGlyphManager::glyphName(int id) {
  if (id == EdgeExtremityShape::None) {
    return string("NONE");
  }

  auto it = eeglyphIdToName.find(id);
  if (it != eeglyphIdToName.end()) {
    return it->second;
  } else {
    tlp::warning() << __PRETTY_FUNCTION__ << endl;
    tlp::warning() << "Invalid glyph id" << endl;
    return string("invalid");
  }
}
//====================================================
int EdgeExtremityGlyphManager::glyphId(const string &name) {
  if (name == "NONE") {
    return EdgeExtremityShape::None;
  }

  auto it = nameToEeGlyphId.find(name);
  if (it != nameToEeGlyphId.end()) {
    return it->second;
  } else {
    tlp::warning() << __PRETTY_FUNCTION__ << endl;
    tlp::warning() << "Invalid glyph name" << endl;
    return 0;
  }
}
//====================================================
void EdgeExtremityGlyphManager::loadGlyphPlugins() {
  plugins = PluginsManager::availablePlugins<EdgeExtremityGlyph>();

  for (const auto &pluginName : plugins) {
    int pluginId = PluginsManager::pluginInformation(pluginName).id();
    eeglyphIdToName[pluginId] = pluginName;
    nameToEeGlyphId[pluginName] = pluginId;
  }
}
//====================================================
EdgeExtremityGlyphManager::EdgeExtremityGlyphManager(GlGraphInputData *inputData) {
  GlyphContext gc = GlyphContext(inputData);
  for (const auto &glyphName : plugins) {
    auto *newGlyph = PluginsManager::getPluginObject<EdgeExtremityGlyph>(glyphName, &gc);
    _glyphs[PluginsManager::pluginInformation(glyphName).id()] = newGlyph;
  }
}

EdgeExtremityGlyphManager::~EdgeExtremityGlyphManager() {
  for (const auto &[id, glyph] : _glyphs) {
    delete glyph;
  }
}

EdgeExtremityGlyph *EdgeExtremityGlyphManager::getGlyph(int id) const {
  if (const auto &it = _glyphs.find(id); it != _glyphs.end()) {
    return it->second;
  }
  return nullptr;
}
}
