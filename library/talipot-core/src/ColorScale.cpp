/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/ColorScale.h>

using namespace std;

namespace tlp {

ColorScale::ColorScale() {
  setColorScale(vector<Color>(), true);
}

ColorScale::ColorScale(const std::vector<Color> &colors, const bool gradient) {
  setColorScale(colors, gradient);
}

ColorScale::ColorScale(const std::map<float, Color> &colorMap, const bool gradient)
    : colorMap(colorMap), gradient(gradient) {}

ColorScale::ColorScale(const ColorScale &scale) : Observable() {
  setColorMap(scale.colorMap);
  gradient = scale.gradient;
}

ColorScale &ColorScale::operator=(const ColorScale &scale) {
  setColorMap(scale.colorMap);
  gradient = scale.gradient;
  return *this;
}

ColorScale::~ColorScale() = default;

void ColorScale::setColorScale(const std::vector<Color> &colors, const bool gradientV) {
  gradient = gradientV;
  colorMap.clear();

  if (colors.empty()) {
    colorMap[0.0f] = Color(75, 75, 255, 200);
    colorMap[0.25f] = Color(156, 161, 255, 200);
    colorMap[0.5f] = Color(255, 255, 127, 200);
    colorMap[0.75f] = Color(255, 170, 0, 200);
    colorMap[1.0f] = Color(229, 40, 0, 200);
  } else {
    if (colors.size() == 1) {
      colorMap[0.0f] = colors[0];
      colorMap[1.0f] = colors[0];
    } else {
      float shift;

      if (gradient) {
        shift = 1.0f / (colors.size() - 1);
      } else {
        shift = 1.0f / colors.size();
      }

      for (size_t i = 0; i < colors.size(); ++i) {
        // Ensure that the last color will be set to 1
        if (i == colors.size() - 1) {
          if (!gradient) {
            colorMap[1.0f - shift] = colors[i];
          }

          colorMap[1.0f] = colors[i];
        } else {
          colorMap[i * shift] = colors[i];

          if (!gradient) {
            colorMap[((i + 1) * shift) - 1E-6] = colors[i];
          }
        }
      }
    }

    sendEvent(Event(*this, EventType::TLP_MODIFICATION));
  }
}

void ColorScale::setColorAtPos(const float pos, const Color &color) {
  colorMap[pos] = color;
}

Color ColorScale::getColorAtPos(const float pos) const {
  if (colorMap.empty()) {
    return Color(255, 255, 255, 255);
  } else {
    Color startColor;
    Color endColor;
    float startPos, endPos;
    auto it = colorMap.begin();
    startPos = endPos = it->first;
    startColor = endColor = it->second;

    for (++it; it != colorMap.end(); ++it) {
      endColor = it->second;
      endPos = it->first;

      if (pos >= startPos && pos <= endPos) {
        break;
      } else {
        startColor = endColor;
        startPos = endPos;
      }
    }

    if (gradient) {
      Color ret;
      double ratio = (pos - startPos) / (endPos - startPos);

      for (uint i = 0; i < 4; ++i) {
        ret[i] =
            uchar((double(startColor[i]) + (double(endColor[i]) - double(startColor[i])) * ratio));
      }

      return ret;
    } else {
      return startColor;
    }
  }
}

void ColorScale::setColorMap(const map<float, Color> &newColorMap) {
  colorMap.clear();

  // insert all values in [0, 1]
  for (const auto &[t, color] : newColorMap) {
    if (t < 0.f || t > 1.f) {
      continue;
    } else {
      colorMap[t] = color;
    }
  }

  if (!colorMap.empty()) {
    // Ensure color scale is valid
    if (colorMap.size() == 1) {
      // If there is only one value in the map fill the interval with the whole color.
      Color c = (*colorMap.begin()).second;
      colorMap.clear();
      colorMap[0.f] = c;
      colorMap[1.f] = c;
    } else {
      // Ensure the first value is mapped to 0 and last is mapped to 1
      auto begin = colorMap.begin();

      if ((*begin).first != 0) {
        Color c = (*begin).second;
        colorMap.erase(begin);
        colorMap[0.f] = c;
      }

      auto end = colorMap.rbegin();

      if ((*end).first != 1) {
        Color c = (*end).second;
        colorMap.erase(end.base());
        colorMap[1.f] = c;
      }
    }

    sendEvent(Event(*this, EventType::TLP_MODIFICATION));
  }
}

void ColorScale::setColorMapTransparency(unsigned char alpha) {
  // force the alpha value of all mapped colors
  for (auto &[t, color] : colorMap) {
    color.setA(alpha);
  }
}

bool ColorScale::operator==(const std::vector<Color> &colors) const {
  if (colorMap.size() != colors.size()) {
    return false;
  }

  uint i = 0;

  for (const auto &[t, color] : colorMap) {
    if (color != colors[i++]) {
      return false;
    }
  }

  return true;
}

bool ColorScale::hasRegularStops() const {
  if (colorMap.size() <= 2) {
    return true;
  }

  vector<float> v;
  for (const auto &[t, color] : colorMap) {
    v.push_back(t);
  }

  std::sort(v.begin(), v.end());
  float d = v[1] - v[0];

  for (size_t i = 2; i < v.size(); ++i) {
    if (abs((v[i] - v[i - 1]) - d) > 1e-6) {
      return false;
    }
  }

  return true;
}
}
