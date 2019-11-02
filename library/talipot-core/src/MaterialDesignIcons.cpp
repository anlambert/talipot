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

#include <cstring>
#include <talipot/MaterialDesignIcons.h>
#include <talipot/TlpTools.h>

#include <utf8.h>

#include <algorithm>
#include <unordered_map>

using namespace std;
using namespace tlp;

namespace tlp {

static unordered_map<string, unsigned int> iconCodePoint;
static vector<string> iconsNames;

#include "MaterialDesignIconsData.cpp"

string MaterialDesignIcons::getTTFLocation() {
  return TalipotBitmapDir + "fonts/MaterialDesignIcons/" + "materialdesignicons-webfont.ttf";
}

string MaterialDesignIcons::getWOFFLocation() {
  return TalipotBitmapDir + "fonts/MaterialDesignIcons/" + "materialdesignicons-webfont.woff";
}

string MaterialDesignIcons::getWOFF2Location() {
  return TalipotBitmapDir + "fonts/MaterialDesignIcons/" + "materialdesignicons-webfont.woff2";
}

bool MaterialDesignIcons::isIconSupported(const string &iconName) {
  initIconCodePoints();
  return iconCodePoint.find(iconName) != iconCodePoint.end();
}

const vector<string> &MaterialDesignIcons::getSupportedIcons() {
  initIconCodePoints();
  if (iconsNames.empty()) {
    iconsNames.reserve(iconCodePoint.size());
    for (auto &it : iconCodePoint) {
      iconsNames.emplace_back(it.first);
    }
  }
  return iconsNames;
}

unsigned int MaterialDesignIcons::getIconCodePoint(const string &iconName) {
  initIconCodePoints();
  auto it = iconCodePoint.find(iconName);
  if (it != iconCodePoint.end())
    return (it->second);
  return 0;
}

string MaterialDesignIcons::getIconFamily(const string &) {
  return "materialdesignicons";
}

string MaterialDesignIcons::getIconUtf8String(const string &iconName) {
  initIconCodePoints();
  string iconString;
  utf8::append(iconCodePoint[iconName.c_str()], back_inserter(iconString));
  return iconString;
}
}
