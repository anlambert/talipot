/**
 *
 * Copyright (C) 2019-2020  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <cassert>
#include <cstring>

#include <talipot/FontAwesome.h>
#include <talipot/TlpTools.h>

#include <utf8.h>

#include <map>
#include <unordered_map>

using namespace std;
using namespace tlp;

static unordered_map<string, unsigned int> iconCodePoint;
static vector<string> iconsNames;
static map<string, string>
    iconFamily({{"fab", "fa-brands-400"}, {"far", "fa-regular-400"}, {"fas", "fa-solid-900"}});

#include "FontAwesomeData.cpp"

string FontAwesome::getTTFLocation(const string &iconName) {
  initIconCodePoints();
  return TalipotBitmapDir + "fonts/FontAwesome/" + getIconFamily(iconName) + ".ttf";
}

string FontAwesome::getWOFFLocation(const string &iconName) {
  initIconCodePoints();
  return TalipotBitmapDir + "fonts/FontAwesome/" + getIconFamily(iconName) + ".woff";
}

string FontAwesome::getWOFF2Location(const string &iconName) {
  initIconCodePoints();
  return TalipotBitmapDir + "fonts/FontAwesome/" + getIconFamily(iconName) + ".woff2";
}

bool FontAwesome::isIconSupported(const string &iconName) {
  initIconCodePoints();
  return iconCodePoint.find(iconName) != iconCodePoint.end();
}

const vector<string> &FontAwesome::getSupportedIcons() {
  initIconCodePoints();
  if (iconsNames.empty()) {
    iconsNames.reserve(iconCodePoint.size());
    for (auto &it : iconCodePoint) {
      iconsNames.push_back(it.first);
    }
  }
  return iconsNames;
}

unsigned int FontAwesome::getIconCodePoint(const string &iconName) {
  initIconCodePoints();
  auto it = iconCodePoint.find(iconName);
  if (it != iconCodePoint.end()) {
    return (it->second);
  }
  return 0;
}

string FontAwesome::getIconFamily(const string &iconName) {
  initIconCodePoints();
  string prefix = iconName.substr(0, 3);
  if (prefix.length() == 3 && iconFamily.find(prefix) != iconFamily.end()) {
    return iconFamily[prefix];
  }
  return "";
}

string FontAwesome::getIconUtf8String(const string &iconName) {
  initIconCodePoints();
  string iconString;
  utf8::append(static_cast<char32_t>(iconCodePoint[iconName]), iconString);
  return iconString;
}
