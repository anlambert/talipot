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

#include "talipot/Font.h"

#include <fontconfig/fontconfig.h>

#include <talipot/TlpTools.h>

using namespace std;
using namespace tlp;

FontsMap Font::_fonts;
const std::string Font::_defaultFontFamily = "DejaVu Sans";
const std::string Font::_defaultFontStyle = "Book";
Font Font::_defaultFont;

string Font::talipotFontsDirectory() {
    return TalipotShareDir + "fonts";
}

static vector<Font> addFontSet(FcFontSet *fs, FontsMap &fonts) {
    vector<Font> addedFonts;
    for (int i = 0; i < fs->nfont; ++i) {
        FcPattern *font = fs->fonts[i];
        FcChar8 *file, *style, *family;
        if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch &&
            FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch &&
            FcPatternGetString(font, FC_STYLE, 0, &style) == FcResultMatch) {
            Font font =
                Font(reinterpret_cast<const char *>(family), reinterpret_cast<const char *>(style),
                     reinterpret_cast<const char *>(file));
            if (fonts[font.fontFamily()].find(font.fontStyle()) == fonts[font.fontFamily()].end()) {
                fonts[font.fontFamily()][font.fontStyle()] = font;
                addedFonts.push_back(font);
            }
        }
    }
    return addedFonts;
}

const FontsMap &Font::availableFonts() {
    if (_fonts.empty()) {
        FcConfig *config = FcInitLoadConfigAndFonts();
        string fontDir = talipotFontsDirectory();
        FcConfigAppFontAddDir(config,
                              reinterpret_cast<const FcChar8 *>((fontDir + "/DejaVu").c_str()));
        FcObjectSet *os = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, nullptr);
        FcPattern *pat = FcPatternCreate();
        FcFontSet *fs = FcFontList(config, pat, os);
        addFontSet(fs, _fonts);
        if (!_fonts.empty()) {
            _defaultFont = font(_defaultFontFamily, _defaultFontStyle);
        }
        FcObjectSetDestroy(os);
        FcPatternDestroy(pat);
        FcFontSetDestroy(fs);
        FcConfigDestroy(config);
    }

    return _fonts;
}

vector<Font> Font::addFontsFromDir(const std::string &fontsDir) {
    FcFontSet *set = FcFontSetCreate();
    FcDirScan(set, nullptr, nullptr, nullptr, reinterpret_cast<const FcChar8 *>(fontsDir.c_str()),
              FcTrue);
    vector<Font> addedFonts = addFontSet(set, _fonts);
    FcFontSetDestroy(set);
    return addedFonts;
}

void Font::removeFont(const string &fontFile) {
    Font f;
    for (const auto &itFamily : _fonts) {
        for (const auto &itStyle : itFamily.second) {
            if (itStyle.second.fontFile() == fontFile) {
                f = itStyle.second;
                break;
            }
        }
    }
    if (f.fontFile() == fontFile) {
        _fonts[f.fontFamily()].erase(f.fontStyle());
        if (_fonts[f.fontFamily()].empty()) {
            _fonts.erase(f.fontFamily());
        }
    }
}

Font::Font(const string &fontFamily, const string &fontStyle, const string &fontFile)
    : _fontFamily(fontFamily), _fontStyle(fontStyle), _fontFile(fontFile) {}

Font::Font(const Font &f) = default;

Font &Font::operator=(const Font &other) = default;

string Font::fontFamily() const {
    return _fontFamily;
}

string Font::fontStyle() const {
    return _fontStyle;
}

string Font::fontFile() const {
    return _fontFile;
}

string Font::fontName() const {
    return fontFamily() + "-" + fontStyle();
}

const Font &Font::font(const string &fontFamily, const string &fontStyle) {
    const auto &fontsMap = availableFonts();
    auto itF = fontsMap.find(fontFamily);
    if (itF != fontsMap.end()) {
        auto itS = itF->second.find(fontStyle);
        if (itS != itF->second.end()) {
            return itS->second;
        }
    }
    return _defaultFont;
}

const Font &Font::fromName(const std::string &fontName) {
    auto parts = tokenize(fontName, "-");
    if (parts.size() == 2) {
        return font(parts[0], parts[1]);
    } else {
        return _defaultFont;
    }
}

const Font &Font::defaultFont() {
    return _defaultFont;
}
