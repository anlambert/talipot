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

#include <talipot/hash.h>

#include <talipot/Color.h>
#include <talipot/Coord.h>

using namespace std;
using namespace tlp;

static Color HSBtoRGB(unsigned char H, unsigned char S, unsigned char B) {

  float inH = float(H) / 255.f;
  float inS = float(S) / 255.f;
  float inB = float(B) / 255.f;

  float outR, outG, outB;

  if (inS == 0.0f) {
    outR = outG = outB = inB * 255.0f;
  } else {
    float _h = inH * 6.0f, _i = int(_h), _1 = inB * (1.0f - inS),
          _2 = inB * (1.0f - inS * (_h - _i)), _3 = inB * (1.0f - inS * (1.0f - (_h - _i)));

    switch (int(_i)) {
    case 0:
      outR = inB * 255.0f;
      outG = _3 * 255.0f;
      outB = _1 * 255.0f;
      break;

    case 1:
      outR = _2 * 255.0f;
      outG = inB * 255.0f;
      outB = _1 * 255.0f;
      break;

    case 2:
      outR = _1 * 255.0f;
      outG = inB * 255.0f;
      outB = _3 * 255.0f;
      break;

    case 3:
      outR = _1 * 255.0f;
      outG = _2 * 255.0f;
      outB = inB * 255.0f;
      break;

    case 4:
      outR = _3 * 255.0f;
      outG = _1 * 255.0f;
      outB = inB * 255.0f;
      break;

    default:
      outR = inB * 255.0f;
      outG = _1 * 255.0f;
      outB = _2 * 255.0f;
      break;
    }
  }

  return Color(outR, outG, outB);
}

static flat_hash_map<string, Color> X11Colors = {
    {"aliceblue", HSBtoRGB(147, 15, 255)},
    {"antiquewhite", HSBtoRGB(24, 35, 250)},
    {"antiquewhite1", HSBtoRGB(23, 36, 255)},
    {"antiquewhite2", HSBtoRGB(23, 36, 238)},
    {"antiquewhite3", HSBtoRGB(23, 36, 205)},
    {"antiquewhite4", HSBtoRGB(24, 34, 139)},
    {"aquamarine", HSBtoRGB(113, 128, 255)},
    {"aquamarine1", HSBtoRGB(113, 128, 255)},
    {"aquamarine2", HSBtoRGB(113, 128, 238)},
    {"aquamarine3", HSBtoRGB(113, 128, 205)},
    {"aquamarine4", HSBtoRGB(113, 128, 139)},
    {"azure", HSBtoRGB(127, 15, 255)},
    {"azure1", HSBtoRGB(127, 15, 255)},
    {"azure2", HSBtoRGB(127, 15, 238)},
    {"azure3", HSBtoRGB(127, 14, 205)},
    {"azure4", HSBtoRGB(127, 14, 139)},
    {"beige", HSBtoRGB(42, 26, 245)},
    {"bisque", HSBtoRGB(23, 58, 255)},
    {"bisque1", HSBtoRGB(23, 58, 255)},
    {"bisque2", HSBtoRGB(23, 58, 238)},
    {"bisque3", HSBtoRGB(22, 58, 205)},
    {"bisque4", HSBtoRGB(23, 58, 139)},
    {"black", HSBtoRGB(0, 0, 0)},
    {"blanchedalmond", HSBtoRGB(25, 49, 255)},
    {"blue", HSBtoRGB(170, 255, 255)},
    {"blue1", HSBtoRGB(170, 255, 255)},
    {"blue2", HSBtoRGB(170, 255, 238)},
    {"blue3", HSBtoRGB(170, 255, 205)},
    {"blue4", HSBtoRGB(170, 255, 139)},
    {"blueviolet", HSBtoRGB(192, 206, 226)},
    {"brown", HSBtoRGB(0, 190, 165)},
    {"brown1", HSBtoRGB(0, 191, 255)},
    {"brown2", HSBtoRGB(0, 191, 238)},
    {"brown3", HSBtoRGB(0, 191, 205)},
    {"brown4", HSBtoRGB(0, 190, 139)},
    {"burlywood", HSBtoRGB(23, 99, 222)},
    {"burlywood1", HSBtoRGB(23, 100, 255)},
    {"burlywood2", HSBtoRGB(23, 99, 238)},
    {"burlywood3", HSBtoRGB(23, 99, 205)},
    {"burlywood4", HSBtoRGB(23, 99, 139)},
    {"cadetblue", HSBtoRGB(128, 103, 160)},
    {"cadetblue1", HSBtoRGB(131, 103, 255)},
    {"cadetblue2", HSBtoRGB(131, 102, 238)},
    {"cadetblue3", HSBtoRGB(131, 103, 205)},
    {"cadetblue4", HSBtoRGB(131, 102, 139)},
    {"chartreuse", HSBtoRGB(63, 255, 255)},
    {"chartreuse1", HSBtoRGB(63, 255, 255)},
    {"chartreuse2", HSBtoRGB(63, 255, 238)},
    {"chartreuse3", HSBtoRGB(63, 255, 205)},
    {"chartreuse4", HSBtoRGB(63, 255, 139)},
    {"chocolate", HSBtoRGB(17, 218, 210)},
    {"chocolate1", HSBtoRGB(17, 219, 255)},
    {"chocolate2", HSBtoRGB(17, 219, 238)},
    {"chocolate3", HSBtoRGB(17, 218, 205)},
    {"chocolate4", HSBtoRGB(17, 220, 139)},
    {"coral", HSBtoRGB(11, 175, 255)},
    {"coral1", HSBtoRGB(7, 169, 255)},
    {"coral2", HSBtoRGB(6, 169, 238)},
    {"coral3", HSBtoRGB(6, 169, 205)},
    {"coral4", HSBtoRGB(6, 168, 139)},
    {"cornflowerblue", HSBtoRGB(154, 147, 237)},
    {"cornsilk", HSBtoRGB(33, 34, 255)},
    {"cornsilk1", HSBtoRGB(33, 34, 255)},
    {"cornsilk2", HSBtoRGB(34, 35, 238)},
    {"cornsilk3", HSBtoRGB(34, 34, 205)},
    {"cornsilk4", HSBtoRGB(35, 34, 139)},
    {"crimson", HSBtoRGB(246, 231, 220)},
    {"cyan", HSBtoRGB(127, 255, 255)},
    {"cyan1", HSBtoRGB(127, 255, 255)},
    {"cyan2", HSBtoRGB(127, 255, 238)},
    {"cyan3", HSBtoRGB(127, 255, 205)},
    {"cyan4", HSBtoRGB(127, 255, 139)},
    {"darkgoldenrod", HSBtoRGB(30, 239, 184)},
    {"darkgoldenrod1", HSBtoRGB(30, 240, 255)},
    {"darkgoldenrod2", HSBtoRGB(30, 240, 238)},
    {"darkgoldenrod3", HSBtoRGB(30, 240, 205)},
    {"darkgoldenrod4", HSBtoRGB(30, 240, 139)},
    {"darkgreen", HSBtoRGB(85, 255, 100)},
    {"darkkhaki", HSBtoRGB(39, 110, 189)},
    {"darkolivegreen", HSBtoRGB(58, 142, 107)},
    {"darkolivegreen1", HSBtoRGB(58, 143, 255)},
    {"darkolivegreen2", HSBtoRGB(58, 143, 238)},
    {"darkolivegreen3", HSBtoRGB(58, 143, 205)},
    {"darkolivegreen4", HSBtoRGB(58, 143, 139)},
    {"darkorange", HSBtoRGB(23, 255, 255)},
    {"darkorange1", HSBtoRGB(21, 255, 255)},
    {"darkorange2", HSBtoRGB(21, 255, 238)},
    {"darkorange3", HSBtoRGB(21, 255, 205)},
    {"darkorange4", HSBtoRGB(21, 255, 139)},
    {"darkorchid", HSBtoRGB(198, 192, 204)},
    {"darkorchid1", HSBtoRGB(198, 193, 255)},
    {"darkorchid2", HSBtoRGB(198, 192, 238)},
    {"darkorchid3", HSBtoRGB(198, 192, 205)},
    {"darkorchid4", HSBtoRGB(198, 192, 139)},
    {"darksalmon", HSBtoRGB(10, 121, 233)},
    {"darkseagreen", HSBtoRGB(85, 61, 188)},
    {"darkseagreen1", HSBtoRGB(85, 62, 255)},
    {"darkseagreen2", HSBtoRGB(85, 62, 238)},
    {"darkseagreen3", HSBtoRGB(85, 62, 205)},
    {"darkseagreen4", HSBtoRGB(85, 62, 139)},
    {"darkslateblue", HSBtoRGB(175, 143, 139)},
    {"darkslategray", HSBtoRGB(127, 103, 79)},
    {"darkslategray1", HSBtoRGB(127, 104, 255)},
    {"darkslategray2", HSBtoRGB(127, 103, 238)},
    {"darkslategray3", HSBtoRGB(127, 104, 205)},
    {"darkslategray4", HSBtoRGB(127, 104, 139)},
    {"darkslategrey", HSBtoRGB(127, 103, 79)},
    {"darkturquoise", HSBtoRGB(128, 255, 209)},
    {"darkviolet", HSBtoRGB(199, 255, 211)},
    {"deeppink", HSBtoRGB(232, 235, 255)},
    {"deeppink1", HSBtoRGB(232, 235, 255)},
    {"deeppink2", HSBtoRGB(232, 235, 238)},
    {"deeppink3", HSBtoRGB(232, 235, 205)},
    {"deeppink4", HSBtoRGB(231, 236, 139)},
    {"deepskyblue", HSBtoRGB(138, 255, 255)},
    {"deepskyblue1", HSBtoRGB(138, 255, 255)},
    {"deepskyblue2", HSBtoRGB(138, 255, 238)},
    {"deepskyblue3", HSBtoRGB(138, 255, 205)},
    {"deepskyblue4", HSBtoRGB(138, 255, 139)},
    {"dimgray", HSBtoRGB(0, 0, 105)},
    {"dimgrey", HSBtoRGB(0, 0, 105)},
    {"dodgerblue", HSBtoRGB(148, 225, 255)},
    {"dodgerblue1", HSBtoRGB(148, 225, 255)},
    {"dodgerblue2", HSBtoRGB(148, 225, 238)},
    {"dodgerblue3", HSBtoRGB(148, 225, 205)},
    {"dodgerblue4", HSBtoRGB(148, 225, 139)},
    {"firebrick", HSBtoRGB(0, 206, 178)},
    {"firebrick1", HSBtoRGB(0, 207, 255)},
    {"firebrick2", HSBtoRGB(0, 207, 238)},
    {"firebrick3", HSBtoRGB(0, 207, 205)},
    {"firebrick4", HSBtoRGB(0, 207, 139)},
    {"floralwhite", HSBtoRGB(28, 15, 255)},
    {"forestgreen", HSBtoRGB(85, 192, 139)},
    {"gainsboro", HSBtoRGB(0, 0, 220)},
    {"ghostwhite", HSBtoRGB(170, 7, 255)},
    {"gold", HSBtoRGB(35, 255, 255)},
    {"gold1", HSBtoRGB(35, 255, 255)},
    {"gold2", HSBtoRGB(35, 255, 238)},
    {"gold3", HSBtoRGB(35, 255, 205)},
    {"gold4", HSBtoRGB(35, 255, 139)},
    {"goldenrod", HSBtoRGB(30, 217, 218)},
    {"goldenrod1", HSBtoRGB(30, 218, 255)},
    {"goldenrod2", HSBtoRGB(30, 218, 238)},
    {"goldenrod3", HSBtoRGB(30, 218, 205)},
    {"goldenrod4", HSBtoRGB(30, 218, 139)},
    {"gray", HSBtoRGB(0, 0, 192)},
    {"gray0", HSBtoRGB(0, 0, 0)},
    {"gray1", HSBtoRGB(0, 0, 3)},
    {"gray10", HSBtoRGB(0, 0, 26)},
    {"gray100", HSBtoRGB(0, 0, 255)},
    {"gray11", HSBtoRGB(0, 0, 28)},
    {"gray12", HSBtoRGB(0, 0, 31)},
    {"gray13", HSBtoRGB(0, 0, 33)},
    {"gray14", HSBtoRGB(0, 0, 36)},
    {"gray15", HSBtoRGB(0, 0, 38)},
    {"gray16", HSBtoRGB(0, 0, 41)},
    {"gray17", HSBtoRGB(0, 0, 43)},
    {"gray18", HSBtoRGB(0, 0, 46)},
    {"gray19", HSBtoRGB(0, 0, 48)},
    {"gray2", HSBtoRGB(0, 0, 5)},
    {"gray20", HSBtoRGB(0, 0, 51)},
    {"gray21", HSBtoRGB(0, 0, 54)},
    {"gray22", HSBtoRGB(0, 0, 56)},
    {"gray23", HSBtoRGB(0, 0, 59)},
    {"gray24", HSBtoRGB(0, 0, 61)},
    {"gray25", HSBtoRGB(0, 0, 64)},
    {"gray26", HSBtoRGB(0, 0, 66)},
    {"gray27", HSBtoRGB(0, 0, 69)},
    {"gray28", HSBtoRGB(0, 0, 71)},
    {"gray29", HSBtoRGB(0, 0, 74)},
    {"gray3", HSBtoRGB(0, 0, 8)},
    {"gray30", HSBtoRGB(0, 0, 77)},
    {"gray31", HSBtoRGB(0, 0, 79)},
    {"gray32", HSBtoRGB(0, 0, 82)},
    {"gray33", HSBtoRGB(0, 0, 84)},
    {"gray34", HSBtoRGB(0, 0, 87)},
    {"gray35", HSBtoRGB(0, 0, 89)},
    {"gray36", HSBtoRGB(0, 0, 92)},
    {"gray37", HSBtoRGB(0, 0, 94)},
    {"gray38", HSBtoRGB(0, 0, 97)},
    {"gray39", HSBtoRGB(0, 0, 99)},
    {"gray4", HSBtoRGB(0, 0, 10)},
    {"gray40", HSBtoRGB(0, 0, 102)},
    {"gray41", HSBtoRGB(0, 0, 105)},
    {"gray42", HSBtoRGB(0, 0, 107)},
    {"gray43", HSBtoRGB(0, 0, 110)},
    {"gray44", HSBtoRGB(0, 0, 112)},
    {"gray45", HSBtoRGB(0, 0, 115)},
    {"gray46", HSBtoRGB(0, 0, 117)},
    {"gray47", HSBtoRGB(0, 0, 120)},
    {"gray48", HSBtoRGB(0, 0, 122)},
    {"gray49", HSBtoRGB(0, 0, 125)},
    {"gray5", HSBtoRGB(0, 0, 13)},
    {"gray50", HSBtoRGB(0, 0, 127)},
    {"gray51", HSBtoRGB(0, 0, 130)},
    {"gray52", HSBtoRGB(0, 0, 133)},
    {"gray53", HSBtoRGB(0, 0, 135)},
    {"gray54", HSBtoRGB(0, 0, 138)},
    {"gray55", HSBtoRGB(0, 0, 140)},
    {"gray56", HSBtoRGB(0, 0, 143)},
    {"gray57", HSBtoRGB(0, 0, 145)},
    {"gray58", HSBtoRGB(0, 0, 148)},
    {"gray59", HSBtoRGB(0, 0, 150)},
    {"gray6", HSBtoRGB(0, 0, 15)},
    {"gray60", HSBtoRGB(0, 0, 153)},
    {"gray61", HSBtoRGB(0, 0, 156)},
    {"gray62", HSBtoRGB(0, 0, 158)},
    {"gray63", HSBtoRGB(0, 0, 161)},
    {"gray64", HSBtoRGB(0, 0, 163)},
    {"gray65", HSBtoRGB(0, 0, 166)},
    {"gray66", HSBtoRGB(0, 0, 168)},
    {"gray67", HSBtoRGB(0, 0, 171)},
    {"gray68", HSBtoRGB(0, 0, 173)},
    {"gray69", HSBtoRGB(0, 0, 176)},
    {"gray7", HSBtoRGB(0, 0, 18)},
    {"gray70", HSBtoRGB(0, 0, 179)},
    {"gray71", HSBtoRGB(0, 0, 181)},
    {"gray72", HSBtoRGB(0, 0, 184)},
    {"gray73", HSBtoRGB(0, 0, 186)},
    {"gray74", HSBtoRGB(0, 0, 189)},
    {"gray75", HSBtoRGB(0, 0, 191)},
    {"gray76", HSBtoRGB(0, 0, 194)},
    {"gray77", HSBtoRGB(0, 0, 196)},
    {"gray78", HSBtoRGB(0, 0, 199)},
    {"gray79", HSBtoRGB(0, 0, 201)},
    {"gray8", HSBtoRGB(0, 0, 20)},
    {"gray80", HSBtoRGB(0, 0, 204)},
    {"gray81", HSBtoRGB(0, 0, 207)},
    {"gray82", HSBtoRGB(0, 0, 209)},
    {"gray83", HSBtoRGB(0, 0, 212)},
    {"gray84", HSBtoRGB(0, 0, 214)},
    {"gray85", HSBtoRGB(0, 0, 217)},
    {"gray86", HSBtoRGB(0, 0, 219)},
    {"gray87", HSBtoRGB(0, 0, 222)},
    {"gray88", HSBtoRGB(0, 0, 224)},
    {"gray89", HSBtoRGB(0, 0, 227)},
    {"gray9", HSBtoRGB(0, 0, 23)},
    {"gray90", HSBtoRGB(0, 0, 229)},
    {"gray91", HSBtoRGB(0, 0, 232)},
    {"gray92", HSBtoRGB(0, 0, 235)},
    {"gray93", HSBtoRGB(0, 0, 237)},
    {"gray94", HSBtoRGB(0, 0, 240)},
    {"gray95", HSBtoRGB(0, 0, 242)},
    {"gray96", HSBtoRGB(0, 0, 245)},
    {"gray97", HSBtoRGB(0, 0, 247)},
    {"gray98", HSBtoRGB(0, 0, 250)},
    {"gray99", HSBtoRGB(0, 0, 252)},
    {"green", HSBtoRGB(85, 255, 255)},
    {"green1", HSBtoRGB(85, 255, 255)},
    {"green2", HSBtoRGB(85, 255, 238)},
    {"green3", HSBtoRGB(85, 255, 205)},
    {"green4", HSBtoRGB(85, 255, 139)},
    {"greenyellow", HSBtoRGB(59, 208, 255)},
    {"grey", HSBtoRGB(0, 0, 192)},
    {"grey0", HSBtoRGB(0, 0, 0)},
    {"grey1", HSBtoRGB(0, 0, 3)},
    {"grey10", HSBtoRGB(0, 0, 26)},
    {"grey100", HSBtoRGB(0, 0, 255)},
    {"grey11", HSBtoRGB(0, 0, 28)},
    {"grey12", HSBtoRGB(0, 0, 31)},
    {"grey13", HSBtoRGB(0, 0, 33)},
    {"grey14", HSBtoRGB(0, 0, 36)},
    {"grey15", HSBtoRGB(0, 0, 38)},
    {"grey16", HSBtoRGB(0, 0, 41)},
    {"grey17", HSBtoRGB(0, 0, 43)},
    {"grey18", HSBtoRGB(0, 0, 46)},
    {"grey19", HSBtoRGB(0, 0, 48)},
    {"grey2", HSBtoRGB(0, 0, 5)},
    {"grey20", HSBtoRGB(0, 0, 51)},
    {"grey21", HSBtoRGB(0, 0, 54)},
    {"grey22", HSBtoRGB(0, 0, 56)},
    {"grey23", HSBtoRGB(0, 0, 59)},
    {"grey24", HSBtoRGB(0, 0, 61)},
    {"grey25", HSBtoRGB(0, 0, 64)},
    {"grey26", HSBtoRGB(0, 0, 66)},
    {"grey27", HSBtoRGB(0, 0, 69)},
    {"grey28", HSBtoRGB(0, 0, 71)},
    {"grey29", HSBtoRGB(0, 0, 74)},
    {"grey3", HSBtoRGB(0, 0, 8)},
    {"grey30", HSBtoRGB(0, 0, 77)},
    {"grey31", HSBtoRGB(0, 0, 79)},
    {"grey32", HSBtoRGB(0, 0, 82)},
    {"grey33", HSBtoRGB(0, 0, 84)},
    {"grey34", HSBtoRGB(0, 0, 87)},
    {"grey35", HSBtoRGB(0, 0, 89)},
    {"grey36", HSBtoRGB(0, 0, 92)},
    {"grey37", HSBtoRGB(0, 0, 94)},
    {"grey38", HSBtoRGB(0, 0, 97)},
    {"grey39", HSBtoRGB(0, 0, 99)},
    {"grey4", HSBtoRGB(0, 0, 10)},
    {"grey40", HSBtoRGB(0, 0, 102)},
    {"grey41", HSBtoRGB(0, 0, 105)},
    {"grey42", HSBtoRGB(0, 0, 107)},
    {"grey43", HSBtoRGB(0, 0, 110)},
    {"grey44", HSBtoRGB(0, 0, 112)},
    {"grey45", HSBtoRGB(0, 0, 115)},
    {"grey46", HSBtoRGB(0, 0, 117)},
    {"grey47", HSBtoRGB(0, 0, 120)},
    {"grey48", HSBtoRGB(0, 0, 122)},
    {"grey49", HSBtoRGB(0, 0, 125)},
    {"grey5", HSBtoRGB(0, 0, 13)},
    {"grey50", HSBtoRGB(0, 0, 127)},
    {"grey51", HSBtoRGB(0, 0, 130)},
    {"grey52", HSBtoRGB(0, 0, 133)},
    {"grey53", HSBtoRGB(0, 0, 135)},
    {"grey54", HSBtoRGB(0, 0, 138)},
    {"grey55", HSBtoRGB(0, 0, 140)},
    {"grey56", HSBtoRGB(0, 0, 143)},
    {"grey57", HSBtoRGB(0, 0, 145)},
    {"grey58", HSBtoRGB(0, 0, 148)},
    {"grey59", HSBtoRGB(0, 0, 150)},
    {"grey6", HSBtoRGB(0, 0, 15)},
    {"grey60", HSBtoRGB(0, 0, 153)},
    {"grey61", HSBtoRGB(0, 0, 156)},
    {"grey62", HSBtoRGB(0, 0, 158)},
    {"grey63", HSBtoRGB(0, 0, 161)},
    {"grey64", HSBtoRGB(0, 0, 163)},
    {"grey65", HSBtoRGB(0, 0, 166)},
    {"grey66", HSBtoRGB(0, 0, 168)},
    {"grey67", HSBtoRGB(0, 0, 171)},
    {"grey68", HSBtoRGB(0, 0, 173)},
    {"grey69", HSBtoRGB(0, 0, 176)},
    {"grey7", HSBtoRGB(0, 0, 18)},
    {"grey70", HSBtoRGB(0, 0, 179)},
    {"grey71", HSBtoRGB(0, 0, 181)},
    {"grey72", HSBtoRGB(0, 0, 184)},
    {"grey73", HSBtoRGB(0, 0, 186)},
    {"grey74", HSBtoRGB(0, 0, 189)},
    {"grey75", HSBtoRGB(0, 0, 191)},
    {"grey76", HSBtoRGB(0, 0, 194)},
    {"grey77", HSBtoRGB(0, 0, 196)},
    {"grey78", HSBtoRGB(0, 0, 199)},
    {"grey79", HSBtoRGB(0, 0, 201)},
    {"grey8", HSBtoRGB(0, 0, 20)},
    {"grey80", HSBtoRGB(0, 0, 204)},
    {"grey81", HSBtoRGB(0, 0, 207)},
    {"grey82", HSBtoRGB(0, 0, 209)},
    {"grey83", HSBtoRGB(0, 0, 212)},
    {"grey84", HSBtoRGB(0, 0, 214)},
    {"grey85", HSBtoRGB(0, 0, 217)},
    {"grey86", HSBtoRGB(0, 0, 219)},
    {"grey87", HSBtoRGB(0, 0, 222)},
    {"grey88", HSBtoRGB(0, 0, 224)},
    {"grey89", HSBtoRGB(0, 0, 227)},
    {"grey9", HSBtoRGB(0, 0, 23)},
    {"grey90", HSBtoRGB(0, 0, 229)},
    {"grey91", HSBtoRGB(0, 0, 232)},
    {"grey92", HSBtoRGB(0, 0, 235)},
    {"grey93", HSBtoRGB(0, 0, 237)},
    {"grey94", HSBtoRGB(0, 0, 240)},
    {"grey95", HSBtoRGB(0, 0, 242)},
    {"grey96", HSBtoRGB(0, 0, 245)},
    {"grey97", HSBtoRGB(0, 0, 247)},
    {"grey98", HSBtoRGB(0, 0, 250)},
    {"grey99", HSBtoRGB(0, 0, 252)},
    {"honeydew", HSBtoRGB(85, 15, 255)},
    {"honeydew1", HSBtoRGB(85, 15, 255)},
    {"honeydew2", HSBtoRGB(85, 15, 238)},
    {"honeydew3", HSBtoRGB(85, 14, 205)},
    {"honeydew4", HSBtoRGB(85, 14, 139)},
    {"hotpink", HSBtoRGB(233, 150, 255)},
    {"hotpink1", HSBtoRGB(234, 145, 255)},
    {"hotpink2", HSBtoRGB(235, 141, 238)},
    {"hotpink3", HSBtoRGB(236, 135, 205)},
    {"hotpink4", HSBtoRGB(234, 148, 139)},
    {"indianred", HSBtoRGB(0, 140, 205)},
    {"indianred1", HSBtoRGB(0, 148, 255)},
    {"indianred2", HSBtoRGB(0, 148, 238)},
    {"indianred3", HSBtoRGB(0, 149, 205)},
    {"indianred4", HSBtoRGB(0, 148, 139)},
    {"indigo", HSBtoRGB(194, 255, 130)},
    {"ivory", HSBtoRGB(42, 15, 255)},
    {"ivory1", HSBtoRGB(42, 15, 255)},
    {"ivory2", HSBtoRGB(42, 15, 238)},
    {"ivory3", HSBtoRGB(42, 14, 205)},
    {"ivory4", HSBtoRGB(42, 14, 139)},
    {"khaki", HSBtoRGB(38, 106, 240)},
    {"khaki1", HSBtoRGB(39, 112, 255)},
    {"khaki2", HSBtoRGB(39, 112, 238)},
    {"khaki3", HSBtoRGB(39, 111, 205)},
    {"khaki4", HSBtoRGB(39, 111, 139)},
    {"lavender", HSBtoRGB(170, 20, 250)},
    {"lavenderblush", HSBtoRGB(240, 15, 255)},
    {"lavenderblush1", HSBtoRGB(240, 15, 255)},
    {"lavenderblush2", HSBtoRGB(239, 15, 238)},
    {"lavenderblush3", HSBtoRGB(240, 14, 205)},
    {"lavenderblush4", HSBtoRGB(239, 14, 139)},
    {"lawngreen", HSBtoRGB(64, 255, 252)},
    {"lemonchiffon", HSBtoRGB(38, 49, 255)},
    {"lemonchiffon1", HSBtoRGB(38, 49, 255)},
    {"lemonchiffon2", HSBtoRGB(37, 50, 238)},
    {"lemonchiffon3", HSBtoRGB(38, 49, 205)},
    {"lemonchiffon4", HSBtoRGB(39, 49, 139)},
    {"lightblue", HSBtoRGB(137, 63, 230)},
    {"lightblue1", HSBtoRGB(138, 64, 255)},
    {"lightblue2", HSBtoRGB(138, 64, 238)},
    {"lightblue3", HSBtoRGB(138, 63, 205)},
    {"lightblue4", HSBtoRGB(137, 64, 139)},
    {"lightcoral", HSBtoRGB(0, 119, 240)},
    {"lightcyan", HSBtoRGB(127, 31, 255)},
    {"lightcyan1", HSBtoRGB(127, 31, 255)},
    {"lightcyan2", HSBtoRGB(127, 31, 238)},
    {"lightcyan3", HSBtoRGB(127, 31, 205)},
    {"lightcyan4", HSBtoRGB(127, 31, 139)},
    {"lightgoldenrod", HSBtoRGB(35, 115, 238)},
    {"lightgoldenrod1", HSBtoRGB(35, 116, 255)},
    {"lightgoldenrod2", HSBtoRGB(35, 115, 238)},
    {"lightgoldenrod3", HSBtoRGB(35, 115, 205)},
    {"lightgoldenrod4", HSBtoRGB(35, 115, 139)},
    {"lightgoldenrodyellow", HSBtoRGB(42, 40, 250)},
    {"lightgray", HSBtoRGB(0, 0, 211)},
    {"lightgrey", HSBtoRGB(0, 0, 211)},
    {"lightpink", HSBtoRGB(248, 73, 255)},
    {"lightpink1", HSBtoRGB(249, 81, 255)},
    {"lightpink2", HSBtoRGB(248, 81, 238)},
    {"lightpink3", HSBtoRGB(249, 80, 205)},
    {"lightpink4", HSBtoRGB(249, 80, 139)},
    {"lightsalmon", HSBtoRGB(12, 132, 255)},
    {"lightsalmon1", HSBtoRGB(12, 132, 255)},
    {"lightsalmon2", HSBtoRGB(11, 132, 238)},
    {"lightsalmon3", HSBtoRGB(12, 133, 205)},
    {"lightsalmon4", HSBtoRGB(12, 133, 139)},
    {"lightseagreen", HSBtoRGB(125, 209, 178)},
    {"lightskyblue", HSBtoRGB(143, 117, 250)},
    {"lightskyblue1", HSBtoRGB(143, 79, 255)},
    {"lightskyblue2", HSBtoRGB(143, 79, 238)},
    {"lightskyblue3", HSBtoRGB(142, 79, 205)},
    {"lightskyblue4", HSBtoRGB(143, 78, 139)},
    {"lightslateblue", HSBtoRGB(175, 143, 255)},
    {"lightslategray", HSBtoRGB(148, 56, 153)},
    {"lightslategrey", HSBtoRGB(148, 56, 153)},
    {"lightsteelblue", HSBtoRGB(151, 52, 222)},
    {"lightsteelblue1", HSBtoRGB(151, 53, 255)},
    {"lightsteelblue2", HSBtoRGB(151, 53, 238)},
    {"lightsteelblue3", HSBtoRGB(151, 53, 205)},
    {"lightsteelblue4", HSBtoRGB(150, 53, 139)},
    {"lightyellow", HSBtoRGB(42, 31, 255)},
    {"lightyellow1", HSBtoRGB(42, 31, 255)},
    {"lightyellow2", HSBtoRGB(42, 31, 238)},
    {"lightyellow3", HSBtoRGB(42, 31, 205)},
    {"lightyellow4", HSBtoRGB(42, 31, 139)},
    {"limegreen", HSBtoRGB(85, 192, 205)},
    {"linen", HSBtoRGB(21, 20, 250)},
    {"magenta", HSBtoRGB(212, 255, 255)},
    {"magenta1", HSBtoRGB(212, 255, 255)},
    {"magenta2", HSBtoRGB(212, 255, 238)},
    {"magenta3", HSBtoRGB(212, 255, 205)},
    {"magenta4", HSBtoRGB(212, 255, 139)},
    {"maroon", HSBtoRGB(239, 185, 176)},
    {"maroon1", HSBtoRGB(228, 203, 255)},
    {"maroon2", HSBtoRGB(228, 203, 238)},
    {"maroon3", HSBtoRGB(228, 204, 205)},
    {"maroon4", HSBtoRGB(228, 203, 139)},
    {"mediumaquamarine", HSBtoRGB(113, 128, 205)},
    {"mediumblue", HSBtoRGB(170, 255, 205)},
    {"mediumorchid", HSBtoRGB(204, 152, 211)},
    {"mediumorchid1", HSBtoRGB(203, 153, 255)},
    {"mediumorchid2", HSBtoRGB(203, 153, 238)},
    {"mediumorchid3", HSBtoRGB(203, 153, 205)},
    {"mediumorchid4", HSBtoRGB(203, 154, 139)},
    {"mediumpurple", HSBtoRGB(183, 124, 219)},
    {"mediumpurple1", HSBtoRGB(183, 125, 255)},
    {"mediumpurple2", HSBtoRGB(183, 125, 238)},
    {"mediumpurple3", HSBtoRGB(183, 125, 205)},
    {"mediumpurple4", HSBtoRGB(183, 124, 139)},
    {"mediumseagreen", HSBtoRGB(103, 169, 179)},
    {"mediumslateblue", HSBtoRGB(176, 143, 238)},
    {"mediumspringgreen", HSBtoRGB(111, 255, 250)},
    {"mediumturquoise", HSBtoRGB(125, 167, 209)},
    {"mediumvioletred", HSBtoRGB(228, 228, 199)},
    {"midnightblue", HSBtoRGB(170, 198, 112)},
    {"mintcream", HSBtoRGB(106, 9, 255)},
    {"mistyrose", HSBtoRGB(4, 30, 255)},
    {"mistyrose1", HSBtoRGB(4, 30, 255)},
    {"mistyrose2", HSBtoRGB(4, 30, 238)},
    {"mistyrose3", HSBtoRGB(3, 29, 205)},
    {"mistyrose4", HSBtoRGB(5, 29, 139)},
    {"moccasin", HSBtoRGB(26, 73, 255)},
    {"navajowhite", HSBtoRGB(25, 81, 255)},
    {"navajowhite1", HSBtoRGB(25, 81, 255)},
    {"navajowhite2", HSBtoRGB(25, 82, 238)},
    {"navajowhite3", HSBtoRGB(25, 82, 205)},
    {"navajowhite4", HSBtoRGB(25, 82, 139)},
    {"navy", HSBtoRGB(170, 255, 128)},
    {"navyblue", HSBtoRGB(170, 255, 128)},
    {"oldlace", HSBtoRGB(27, 23, 253)},
    {"olivedrab", HSBtoRGB(56, 192, 142)},
    {"olivedrab1", HSBtoRGB(56, 193, 255)},
    {"olivedrab2", HSBtoRGB(56, 192, 238)},
    {"olivedrab3", HSBtoRGB(56, 192, 205)},
    {"olivedrab4", HSBtoRGB(56, 192, 139)},
    {"orange", HSBtoRGB(27, 255, 255)},
    {"orange1", HSBtoRGB(27, 255, 255)},
    {"orange2", HSBtoRGB(27, 255, 238)},
    {"orange3", HSBtoRGB(27, 255, 205)},
    {"orange4", HSBtoRGB(27, 255, 139)},
    {"orangered", HSBtoRGB(11, 255, 255)},
    {"orangered1", HSBtoRGB(11, 255, 255)},
    {"orangered2", HSBtoRGB(11, 255, 238)},
    {"orangered3", HSBtoRGB(11, 255, 205)},
    {"orangered4", HSBtoRGB(11, 255, 139)},
    {"orchid", HSBtoRGB(214, 123, 218)},
    {"orchid1", HSBtoRGB(214, 124, 255)},
    {"orchid2", HSBtoRGB(214, 124, 238)},
    {"orchid3", HSBtoRGB(214, 124, 205)},
    {"orchid4", HSBtoRGB(213, 124, 139)},
    {"palegoldenrod", HSBtoRGB(38, 72, 238)},
    {"palegreen", HSBtoRGB(85, 100, 251)},
    {"palegreen1", HSBtoRGB(85, 101, 255)},
    {"palegreen2", HSBtoRGB(85, 100, 238)},
    {"palegreen3", HSBtoRGB(85, 100, 205)},
    {"palegreen4", HSBtoRGB(85, 100, 139)},
    {"paleturquoise", HSBtoRGB(127, 67, 238)},
    {"paleturquoise1", HSBtoRGB(127, 68, 255)},
    {"paleturquoise2", HSBtoRGB(127, 68, 238)},
    {"paleturquoise3", HSBtoRGB(127, 68, 205)},
    {"paleturquoise4", HSBtoRGB(127, 67, 139)},
    {"palevioletred", HSBtoRGB(241, 124, 219)},
    {"palevioletred1", HSBtoRGB(241, 125, 255)},
    {"palevioletred2", HSBtoRGB(241, 125, 238)},
    {"palevioletred3", HSBtoRGB(241, 125, 205)},
    {"palevioletred4", HSBtoRGB(241, 124, 139)},
    {"papayawhip", HSBtoRGB(26, 41, 255)},
    {"peachpuff", HSBtoRGB(20, 70, 255)},
    {"peachpuff1", HSBtoRGB(20, 70, 255)},
    {"peachpuff2", HSBtoRGB(19, 69, 238)},
    {"peachpuff3", HSBtoRGB(19, 69, 205)},
    {"peachpuff4", HSBtoRGB(20, 69, 139)},
    {"peru", HSBtoRGB(20, 176, 205)},
    {"pink", HSBtoRGB(247, 63, 255)},
    {"pink1", HSBtoRGB(245, 73, 255)},
    {"pink2", HSBtoRGB(245, 73, 238)},
    {"pink3", HSBtoRGB(245, 74, 205)},
    {"pink4", HSBtoRGB(245, 73, 139)},
    {"plum", HSBtoRGB(212, 70, 221)},
    {"plum1", HSBtoRGB(212, 68, 255)},
    {"plum2", HSBtoRGB(212, 68, 238)},
    {"plum3", HSBtoRGB(212, 68, 205)},
    {"plum4", HSBtoRGB(212, 67, 139)},
    {"powderblue", HSBtoRGB(132, 59, 230)},
    {"purple", HSBtoRGB(196, 221, 240)},
    {"purple1", HSBtoRGB(191, 207, 255)},
    {"purple2", HSBtoRGB(192, 207, 238)},
    {"purple3", HSBtoRGB(192, 207, 205)},
    {"purple4", HSBtoRGB(192, 207, 139)},
    {"red", HSBtoRGB(0, 255, 255)},
    {"red1", HSBtoRGB(0, 255, 255)},
    {"red2", HSBtoRGB(0, 255, 238)},
    {"red3", HSBtoRGB(0, 255, 205)},
    {"red4", HSBtoRGB(0, 255, 139)},
    {"rosybrown", HSBtoRGB(0, 61, 188)},
    {"rosybrown1", HSBtoRGB(0, 62, 255)},
    {"rosybrown2", HSBtoRGB(0, 62, 238)},
    {"rosybrown3", HSBtoRGB(0, 62, 205)},
    {"rosybrown4", HSBtoRGB(0, 62, 139)},
    {"royalblue", HSBtoRGB(159, 181, 225)},
    {"royalblue1", HSBtoRGB(159, 183, 255)},
    {"royalblue2", HSBtoRGB(159, 183, 238)},
    {"royalblue3", HSBtoRGB(159, 182, 205)},
    {"royalblue4", HSBtoRGB(159, 183, 139)},
    {"saddlebrown", HSBtoRGB(17, 220, 139)},
    {"salmon", HSBtoRGB(4, 138, 250)},
    {"salmon1", HSBtoRGB(9, 150, 255)},
    {"salmon2", HSBtoRGB(9, 150, 238)},
    {"salmon3", HSBtoRGB(9, 150, 205)},
    {"salmon4", HSBtoRGB(9, 150, 139)},
    {"sandybrown", HSBtoRGB(19, 154, 244)},
    {"seagreen", HSBtoRGB(103, 170, 139)},
    {"seagreen1", HSBtoRGB(103, 171, 255)},
    {"seagreen2", HSBtoRGB(103, 171, 238)},
    {"seagreen3", HSBtoRGB(103, 171, 205)},
    {"seagreen4", HSBtoRGB(103, 170, 139)},
    {"seashell", HSBtoRGB(17, 16, 255)},
    {"seashell1", HSBtoRGB(17, 16, 255)},
    {"seashell2", HSBtoRGB(18, 17, 238)},
    {"seashell3", HSBtoRGB(18, 17, 205)},
    {"seashell4", HSBtoRGB(18, 16, 139)},
    {"sienna", HSBtoRGB(13, 183, 160)},
    {"sienna1", HSBtoRGB(13, 184, 255)},
    {"sienna2", HSBtoRGB(13, 184, 238)},
    {"sienna3", HSBtoRGB(13, 184, 205)},
    {"sienna4", HSBtoRGB(13, 185, 139)},
    {"skyblue", HSBtoRGB(139, 108, 235)},
    {"skyblue1", HSBtoRGB(144, 120, 255)},
    {"skyblue2", HSBtoRGB(144, 120, 238)},
    {"skyblue3", HSBtoRGB(144, 120, 205)},
    {"skyblue4", HSBtoRGB(145, 119, 139)},
    {"slateblue", HSBtoRGB(175, 143, 205)},
    {"slateblue1", HSBtoRGB(175, 144, 255)},
    {"slateblue2", HSBtoRGB(175, 144, 238)},
    {"slateblue3", HSBtoRGB(175, 144, 205)},
    {"slateblue4", HSBtoRGB(175, 144, 139)},
    {"slategray", HSBtoRGB(148, 56, 144)},
    {"slategray1", HSBtoRGB(149, 56, 255)},
    {"slategray2", HSBtoRGB(149, 56, 238)},
    {"slategray3", HSBtoRGB(148, 57, 205)},
    {"slategray4", HSBtoRGB(149, 56, 139)},
    {"slategrey", HSBtoRGB(148, 56, 144)},
    {"snow", HSBtoRGB(0, 5, 255)},
    {"snow1", HSBtoRGB(0, 5, 255)},
    {"snow2", HSBtoRGB(0, 5, 238)},
    {"snow3", HSBtoRGB(0, 4, 205)},
    {"snow4", HSBtoRGB(0, 3, 139)},
    {"springgreen", HSBtoRGB(106, 255, 255)},
    {"springgreen1", HSBtoRGB(106, 255, 255)},
    {"springgreen2", HSBtoRGB(106, 255, 238)},
    {"springgreen3", HSBtoRGB(106, 255, 205)},
    {"springgreen4", HSBtoRGB(106, 255, 139)},
    {"steelblue", HSBtoRGB(146, 155, 180)},
    {"steelblue1", HSBtoRGB(146, 156, 255)},
    {"steelblue2", HSBtoRGB(146, 156, 238)},
    {"steelblue3", HSBtoRGB(146, 156, 205)},
    {"steelblue4", HSBtoRGB(147, 155, 139)},
    {"tan", HSBtoRGB(24, 84, 210)},
    {"tan1", HSBtoRGB(20, 176, 255)},
    {"tan2", HSBtoRGB(20, 176, 238)},
    {"tan3", HSBtoRGB(20, 176, 205)},
    {"tan4", HSBtoRGB(20, 176, 139)},
    {"thistle", HSBtoRGB(212, 29, 216)},
    {"thistle1", HSBtoRGB(212, 30, 255)},
    {"thistle2", HSBtoRGB(212, 30, 238)},
    {"thistle3", HSBtoRGB(212, 29, 205)},
    {"thistle4", HSBtoRGB(212, 29, 139)},
    {"tomato", HSBtoRGB(6, 184, 255)},
    {"tomato1", HSBtoRGB(6, 184, 255)},
    {"tomato2", HSBtoRGB(6, 184, 238)},
    {"tomato3", HSBtoRGB(6, 184, 205)},
    {"tomato4", HSBtoRGB(6, 185, 139)},
    {"turquoise", HSBtoRGB(123, 182, 224)},
    {"turquoise1", HSBtoRGB(129, 255, 255)},
    {"turquoise2", HSBtoRGB(129, 255, 238)},
    {"turquoise3", HSBtoRGB(129, 255, 205)},
    {"turquoise4", HSBtoRGB(129, 255, 139)},
    {"violet", HSBtoRGB(212, 115, 238)},
    {"violetred", HSBtoRGB(227, 215, 208)},
    {"violetred1", HSBtoRGB(235, 193, 255)},
    {"violetred2", HSBtoRGB(235, 192, 238)},
    {"violetred3", HSBtoRGB(235, 192, 205)},
    {"violetred4", HSBtoRGB(235, 192, 139)},
    {"wheat", HSBtoRGB(27, 68, 245)},
    {"wheat1", HSBtoRGB(27, 69, 255)},
    {"wheat2", HSBtoRGB(27, 68, 238)},
    {"wheat3", HSBtoRGB(27, 68, 205)},
    {"wheat4", HSBtoRGB(27, 67, 139)},
    {"white", HSBtoRGB(0, 0, 255)},
    {"whitesmoke", HSBtoRGB(0, 0, 245)},
    {"yellow", HSBtoRGB(42, 255, 255)},
    {"yellow1", HSBtoRGB(42, 255, 255)},
    {"yellow2", HSBtoRGB(42, 255, 238)},
    {"yellow3", HSBtoRGB(42, 255, 205)},
    {"yellow4", HSBtoRGB(42, 255, 139)},
    {"yellowgreen", HSBtoRGB(56, 192, 205)},
};

bool decodeGraphvizColor(Color &outColor, const string &inValue) {
  // #RRGGBBAA ?
  {
    if (inValue.size() >= 7 && inValue[0] == '#') {
      uint r, g, b, a = 255;
      int n = sscanf(inValue.c_str(), "#%02x%02x%02x%02x", &r, &g, &b, &a);

      if (n == 3 || n == 4) {
        outColor = Color(r, g, b, a);
        return true;
      } else {
        return false;
      }
    }
  }

  // %f,%f,%f,%f ?
  {
    float r, g, b, a = 1;
    int n = sscanf(inValue.c_str(), "%f,%f,%f,%f", &r, &g, &b, &a);

    if (n == 3 || n == 4) {
      outColor = Color(uchar(r * 255.0f), uchar(g * 255.0f), uchar(b * 255.0f), uchar(a * 255.0f));
      return true;
    }
  }

  // %f %f %f %f ?
  {
    float r, g, b, a = 1;
    int n = sscanf(inValue.c_str(), "%f %f %f %f", &r, &g, &b, &a);

    if (n == 3 || n == 4) {
      outColor = Color(uchar(r * 255.0f), uchar(g * 255.0f), uchar(b * 255.0f), uchar(a * 255.0f));
      return true;
    }
  }

  // x11 name ?
  {
    if (X11Colors.contains(inValue)) {
      outColor = X11Colors[inValue];
      return true;
    }
  }

  return false;
}

bool getCoordFromGraphvizPos(Coord &outCoord, const string &inValue) {
  int n = sscanf(inValue.c_str(), "%f,%f,%f", &outCoord[0], &outCoord[1], &outCoord[2]);
  return n >= 2;
}

bool getCoordsFromGraphvizPos(vector<Coord> &outCoords, const string &inValue) {
  outCoords.clear();
  vector<string> points = tokenize(inValue);
  for (const auto &point : points) {
    // skip anchor points
    if (point[0] == 'e' || point[1] == 's') {
      continue;
    }
    Coord coord;
    if (getCoordFromGraphvizPos(coord, point)) {
      outCoords.push_back(coord);
    } else {
      return false;
    }
  }
  return true;
}
