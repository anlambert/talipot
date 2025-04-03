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

#ifndef TALIPOT_FONT_H
#define TALIPOT_FONT_H

#include <map>
#include <vector>

#include <talipot/config.h>

namespace tlp {

class Font;

typedef std::map<std::string, std::map<std::string, Font>> FontsMap;

class TLP_GL_SCOPE Font {

    static FontsMap _fonts;
    static const std::string _defaultFontFamily;
    static const std::string _defaultFontStyle;
    static Font _defaultFont;

    std::string _fontFamily;
    std::string _fontStyle;
    std::string _fontFile;

  public:
    Font() = default;
    Font(const std::string &fontFamily, const std::string &fontStyle, const std::string &fontFile);
    Font(const Font &);
    Font &operator=(const Font &other);

    std::string fontFamily() const;
    std::string fontStyle() const;
    std::string fontFile() const;

    std::string fontName() const;

    static std::string talipotFontsDirectory();
    static const Font &defaultFont();
    static const FontsMap &availableFonts();
    static const Font &font(const std::string &fontFamily, const std::string &fontStyle);
    static const Font &fromName(const std::string &fontName);
    static std::vector<Font> addFontsFromDir(const std::string &fontsDir);
    static void removeFont(const std::string &fontFile);
};
}

#endif // TALIPOT_FONT_H
