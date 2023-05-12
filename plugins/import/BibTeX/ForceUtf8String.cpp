/**
 *
 * Copyright (C) 2021-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/config.h>

using namespace std;
using namespace tlp;

// the intent of this function is to detect
// ill-formed utf8 chars (ascii or windows 1252 chars)
// latex special chars
// and to replace them by the corresponding utf8 sequence
static char c3[2] = {'\303', ' '};
static char c4[2] = {'\304', ' '};
static char c5[2] = {'\305', ' '};
static char *utf8seq;
static char *seq(char cc[2]) {
  return utf8seq = cc;
}

string &forceUtf8String(string &str) {

  char charComposing = 0;

  for (uint i = 0; i < str.size(); ++i) {
    if ((str[i] > '\301') && (str[i] < '\340') && (i + 1) < str.size() &&
        // begin of a 2 chars utf8 sequence
        // skip next char if it is valid utf8
        (uchar(str[i + 1]) > '\177') && (str[i + 1] < '\300')) {
      ++i;
      continue;
    }

    if ((str[i] > '\337') && (str[i] < '\360') && (i + 2) < str.size() &&
        // begin of a 3 chars utf8 sequence
        // skip next 2 chars if it is valid utf8
        (uchar(str[i + 1]) > '\177') && (str[i + 1] < '\300') && (uchar(str[i + 2]) > '\177') &&
        (str[i + 2] < '\300')) {
      i += 2;
      continue;
    }

    switch (/*(unsigned char)*/ str[i]) {
    // é
    case '\202': // extended ascii
    case '\351': // windows 1252
      seq(c3)[1] = '\251';
      break;

    // ê
    case '\210': // extended ascii
    case '\352': // windows 1252
      seq(c3)[1] = '\252';
      break;

    // ë
    case '\211': // extended ascii
    case '\353': // windows 1252
      seq(c3)[1] = '\253';
      break;

    // è
    case '\212': // extended ascii
    case '\350': // windows 1252
      seq(c3)[1] = '\250';
      break;

    // á
    case '\240': // extended ascii
    case '\341': // windows 1252
      seq(c3)[1] = '\241';
      break;

    // â
    case '\203': // extended ascii
    case '\342': // windows 1252
      seq(c3)[1] = '\242';
      break;

    // ä
    case '\204': // extended ascii
    case '\344': // windows 1252
      seq(c3)[1] = '\244';
      break;

    // à
    case '\205': // extended ascii
    case '\340': // windows 1252
      seq(c3)[1] = '\240';
      break;

    // å
    case '\206': // extended ascii
    case '\345': // windows 1252
      seq(c3)[1] = '\245';
      break;

    // ã
    case '\343': // windows 1252
      seq(c3)[1] = '\243';
      break;

    // ï
    case '\213': // extended ascii
    case '\357': // windows 1252
      seq(c3)[1] = '\257';
      break;

    // î
    case '\214': // extended ascii
    case '\356': // windows 1252
      seq(c3)[1] = '\256';
      break;

    // ì
    case '\215': // extended ascii
    case '\354': // windows 1252
      seq(c3)[1] = '\254';
      break;

    // í
    case '\241': // extended ascii
    case '\355': // windows 1252
      seq(c3)[1] = '\255';
      break;

    // ô
    case '\223': // extended ascii
    case '\364': // windows 1252
      seq(c3)[1] = '\264';
      break;

    // ö
    case '\224': // extended ascii
    case '\366': // windows 1252
      seq(c3)[1] = '\266';
      break;

    // ò
    case '\225': // extended ascii
    case '\362': // windows 1252
      seq(c3)[1] = '\262';
      break;

    // ø
    case '\233': // extended ascii
    case '\370': // windows 1252
      seq(c3)[1] = '\270';
      break;

    // õ
    case '\365': // windows 1252
      seq(c3)[1] = '\265';
      break;

    // ó
    case '\242': // extended ascii
    case '\363': // windows 1252
      seq(c3)[1] = '\263';
      break;

    // ü
    case '\201': // extended ascii
    case '\374': // windows 1252
      seq(c3)[1] = '\274';
      break;

    // û
    case '\226': // extended ascii
    case '\373': // windows 1252
      seq(c3)[1] = '\273';
      break;

    // ù
    case '\227': // extended ascii
    case '\371': // windows 1252
      seq(c3)[1] = '\271';
      break;

    // ú
    case '\243': // extended ascii
    case '\372': // windows 1252
      seq(c3)[1] = '\272';
      break;

    // ÿ
    case '\230': // extended ascii
    case '\377': // windows 1252
      seq(c3)[1] = '\277';
      break;

    // ç
    case '\347': // extended ascii & windows 1252
      seq(c3)[1] = '\247';
      break;

    // ñ
    case '\244': // extended ascii
    case '\361': // windows 1252
      seq(c3)[1] = '\261';
      break;

    // É
    case '\220': // extended ascii
    case '\311': // windows 1252
      seq(c3)[1] = '\211';
      break;

    // Ê
    case '\322': // extended ascii
    case '\312': // windows 1252
      seq(c3)[1] = '\212';
      break;

    // Ë
    case '\323': // extended ascii
    case '\313': // windows 1252
      seq(c3)[1] = '\213';
      break;

    // È
    case '\324': // extended ascii
    case '\310': // windows 1252
      seq(c3)[1] = '\210';
      break;

    // ß
    // '\341' is the extended ascii code of ß
    // but is also the windows 1252 code of á
    // case '\341': // extended ascii
    case '\337': // windows 1252
      seq(c3)[1] = '\237';
      break;

    case '\303': // begin of a 2 bytes utf8 char
      ++i;
      continue;

    case '}': // end of latex command
    case '{': // begin of latex command
      // remove {
      str.replace(i, 1, 0, ' ');
      --i;
      continue;

    case '\\': // character composition
      if (charComposing) {
        // next one should be an i
        if (str[i + 1] == 'i') {
          switch (charComposing) {
          case '`': // ì
            seq(c3)[1] = '\254';
            break;

          case '\'': // í
            seq(c3)[1] = '\255';
            break;

          case '^': // î
            seq(c3)[1] = '\256';
            break;

          case '"': // ï
            seq(c3)[1] = '\257';
            break;

          case '~': // ĩ
            seq(c4)[1] = '\251';
            break;

          case '=': // ī
            seq(c4)[1] = '\253';
            break;

          case 'u': // ĭ
            seq(c4)[1] = '\254';
            break;

          default:
            continue;
          }
        }

        str.replace(i, 2, utf8seq, 2);
        ++i;
        continue;
      }

      switch (str[i + 1]) {
      case '`':  // grave
      case '\'': // acute
      case '^':  // circumflex
      case '"':  // diaeresis
      case '~':  // tilde
      case '=':  // macron
      case '.':  // dot over
      case 'b':  // bar under
      case 'c':  // cedilla
      case 'k':  // ogonek
      case 'r':  // ring over
      case 's':  // sharp s
      case 'u':  // breve over
      case 'v':  // caron
        // record char
        charComposing = str[i + 1];
        // remove \\+char
        str.replace(i, 2, 0, ' ');
        --i;
        continue;

      case 'a':
        if (str.size() > i + 2 && str[i + 2] == 'e') {
          // replace by æ
          c3[1] = '\246';
          str.replace(i, 3, c3, 2);
          ++i;
        }

        continue;

      case 'A':
        if (str.size() > i + 2 && str[i + 2] == 'E') {
          // replace by Æ
          c3[1] = '\206';
          str.replace(i, 3, c3, 2);
          ++i;
        }

        continue;

      case 'd':
        // replace by đ
        c4[1] = '\221';
        str.replace(i, 2, c4, 2);
        ++i;
        continue;

      case 'D':
        // replace by Đ
        c4[1] = '\220';
        str.replace(i, 2, c4, 2);
        ++i;
        continue;

      case 'h':
        // replace by ħ
        c4[1] = '\247';
        str.replace(i, 2, c4, 2);
        ++i;
        continue;

      case 'H':
        if (str.size() > i + 2) {
          switch (str[i + 2]) {
          case '0': // Ő
            seq(c5)[1] = '\220';
            str.replace(i, 3, c5, 2);
            ++i;
            continue;

          case 'o': // ő
            seq(c5)[1] = '\221';
            str.replace(i, 3, c5, 2);
            ++i;
            continue;

          case 'U': // Ű
            seq(c5)[1] = '\260';
            str.replace(i, 3, c5, 2);
            ++i;
            continue;

          case 'u': // ű
            seq(c5)[1] = '\261';
            str.replace(i, 3, c5, 2);
            ++i;
            continue;

          default:
            break;
          }
        }

        // replace by Ħ
        c4[1] = '\246';
        str.replace(i, 2, c4, 2);
        ++i;
        continue;

      case 'l':
        // replace by ł
        c5[1] = '\202';
        str.replace(i, 2, c5, 2);
        ++i;
        continue;

      case 'L':
        // replace by Ł
        c5[1] = '\201';
        str.replace(i, 2, c5, 2);
        ++i;
        continue;

      case 'o':
        if (str.size() > i + 2 && str[i + 2] == 'e') {
          // replace by œ
          c5[1] = '\223';
          str.replace(i, 3, c5, 2);
        } else {
          // replace by ø
          c3[1] = '\270';
          str.replace(i, 2, c3, 2);
        }

        ++i;
        continue;

      case 'O':
        if (str.size() > i + 2 && str[i + 2] == 'E') {
          // replace by Œ
          c5[1] = '\222';
          str.replace(i, 3, c5, 2);
        } else {
          // replace by Ø
          c3[1] = '\230';
          str.replace(i, 2, c3, 2);
        }

        ++i;
        continue;

      case 't':
        // replace by ŧ
        c5[1] = '\245';
        str.replace(i, 2, c5, 2);
        ++i;
        continue;

      case 'T':
        // replace by Ŧ
        c5[1] = '\246';
        str.replace(i, 2, c5, 2);
        ++i;
        continue;

      default:
        break;
      }
      [[fallthrough]];
    case ' ':
      if (charComposing) {
        str.replace(i, 1, 0, ' ');
        --i;
        continue;
      }
      [[fallthrough]];

    default:
      if (charComposing) {
        switch (str[i]) {
        case 'a':
          switch (charComposing) {
          case '`': // à
            seq(c3)[1] = '\240';
            break;

          case '\'': // á
            seq(c3)[1] = '\241';
            break;

          case '^': // â
            seq(c3)[1] = '\242';
            break;

          case '~': // ã
            seq(c3)[1] = '\243';
            break;

          case '"': // ä
            seq(c3)[1] = '\244';
            break;

          case 'r': // å
            seq(c3)[1] = '\245';
            break;

          case '=': // ā
            seq(c4)[1] = '\201';
            break;

          case 'u': // ă
            seq(c4)[1] = '\203';
            break;

          case 'k': // ą
            seq(c4)[1] = '\205';
            break;

          default:
            continue;
          }

          break;

        case 'c':
          switch (charComposing) {
          case 'c': // ç
            seq(c3)[1] = '\247';
            break;

          case '\'': // ć
            seq(c4)[1] = '\207';
            break;

          case '^': // ĉ
            seq(c4)[1] = '\211';
            break;

          case '.': // ċ
            seq(c4)[1] = '\213';
            break;

          case 'v': // č
            seq(c4)[1] = '\215';
            break;

          default:
            continue;
          }

          break;

        case 'd':
          if (charComposing == 'v') {
            // ď
            seq(c4)[1] = '\217';
            break;
          } else {
            continue;
          }

        case 'e':
          switch (charComposing) {
          case '`': // è
            seq(c3)[1] = '\250';
            break;

          case '\'': // é
            seq(c3)[1] = '\251';
            break;

          case '^': // ê
            seq(c3)[1] = '\252';
            break;

          case '"': // ë
            seq(c3)[1] = '\253';
            break;

          case '=': // ë
            seq(c4)[1] = '\223';
            break;

          case 'u': // ĕ
            seq(c4)[1] = '\225';
            break;

          case '.': // ė
            seq(c4)[1] = '\227';
            break;

          case 'k': // ę
            seq(c4)[1] = '\231';
            break;

          case 'v': // ě
            seq(c4)[1] = '\233';
            break;

          default:
            continue;
          }

          break;

        case 'g':
          switch (charComposing) {
          case '^': // ĝ
            seq(c4)[1] = '\235';
            break;

          case 'u': // ğ
            seq(c4)[1] = '\237';
            break;

          case '.': // ġ
            seq(c4)[1] = '\241';
            break;

          case 'c': // ģ
            seq(c4)[1] = '\243';
            break;

          default:
            continue;
          }

          break;

        case 'h':
          if (charComposing == '^') {
            seq(c4)[1] = '\245';
            break;
          } else {
            continue;
          }

        case 'j':
          if (charComposing == '^') {
            // ĵ
            seq(c4)[1] = '\265';
            break;
          } else {
            continue;
          }

        case 'k':
          if (charComposing == 'c') {
            // ķ
            seq(c4)[1] = '\267';
            break;
          } else {
            continue;
          }

        case 'l':
          switch (charComposing) {
          case '\'': // ĺ
            seq(c4)[1] = '\272';
            break;

          case 'c': // ļ
            seq(c4)[1] = '\274';
            break;

          case 'v': // ľ
            seq(c4)[1] = '\276';
            break;

          default:
            continue;
          }

          break;

        case 'n':
          switch (charComposing) {
          case '~': //  ñ
            seq(c3)[1] = '\261';
            break;

          case '\'': // ń
            seq(c5)[1] = '\204';
            break;

          case 'c': // ņ
            seq(c5)[1] = '\206';
            break;

          case 'v': // ň
            seq(c5)[1] = '\210';
            break;

          default:
            continue;
          }

          break;

        case 'o':
          switch (charComposing) {
          case '`': // ò
            seq(c3)[1] = '\262';
            break;

          case '\'': // ó
            seq(c3)[1] = '\263';
            break;

          case '^': // ô
            seq(c3)[1] = '\264';
            break;

          case '~': // õ
            seq(c3)[1] = '\265';
            break;

          case '"': // ö
            seq(c3)[1] = '\266';
            break;

          case '=': // ō
            seq(c5)[1] = '\215';
            break;

          case 'u': // ŏ
            seq(c5)[1] = '\217';
            break;

          default:
            continue;
          }

          break;

        case 'r':
          switch (charComposing) {
          case '\'': // ŕ
            seq(c5)[1] = '\225';
            break;

          case 'c': // ŗ
            seq(c5)[1] = '\227';
            break;

          case 'v': // ř
            seq(c5)[1] = '\230';
            break;

          default:
            continue;
          }

          break;

        case 's':
          switch (charComposing) {
          case 's': // ß
            seq(c3)[1] = '\237';
            break;

          case '\'': // ś
            seq(c5)[1] = '\233';
            break;

          case '^': // ŝ
            seq(c5)[1] = '\235';
            break;

          case 'c': // ş
            seq(c5)[1] = '\237';
            break;

          case 'v': // š
            seq(c5)[1] = '\241';
            break;

          default:
            continue;
          }

          break;

        case 't':
          switch (charComposing) {
          case 'c': // ţ
            seq(c5)[1] = '\243';
            break;

          case 'v': // ť
            seq(c5)[1] = '\245';
            break;

          default:
            continue;
          }

          break;

        case 'u':
          switch (charComposing) {
          case '`': // ù
            seq(c3)[1] = '\271';
            break;

          case '\'': // ú
            seq(c3)[1] = '\272';
            break;

          case '^': // û
            seq(c3)[1] = '\273';
            break;

          case '"': // ü
            seq(c3)[1] = '\274';
            break;

          case '~': // ũ
            seq(c5)[1] = '\251';
            break;

          case '=': // ū
            seq(c5)[1] = '\253';
            break;

          case 'u': // ŭ
            seq(c5)[1] = '\255';
            break;

          case 'r': // ů
            seq(c5)[1] = '\257';
            break;

          case 'k': // ų
            seq(c5)[1] = '\263';
            break;

          default:
            continue;
          }

          break;

        case 'w':
          if (charComposing == '^') {
            // ŵ
            seq(c5)[1] = '\265';
            break;
          } else {
            continue;
          }

        case 'y':
          switch (charComposing) {
          case '\'': // ý
            seq(c3)[1] = '\275';
            break;

          case '"': // ÿ
            seq(c3)[1] = '\276';
            break;

          case '^': // ŷ
            seq(c5)[1] = '\267';
            break;

          default:
            continue;
          }

          break;

        case 'z':
          switch (charComposing) {
          case '\'': // ź
            seq(c5)[1] = '\272';
            break;

          case '.': // ż
            seq(c5)[1] = '\274';
            break;

          case 'v': // ž
            seq(c5)[1] = '\276';
            break;

          default:
            continue;
          }

          break;

        case 'A':
          switch (charComposing) {
          case '`': // À
            seq(c3)[1] = '\200';
            break;

          case '\'': // Á
            seq(c3)[1] = '\201';
            break;

          case '^': // Â
            seq(c3)[1] = '\202';
            break;

          case '~': // Ã
            seq(c3)[1] = '\203';
            break;

          case '"': // Ä
            seq(c3)[1] = '\204';
            break;

          case 'r': // Å
            seq(c3)[1] = '\205';
            break;

          case '=': // Ā
            seq(c4)[1] = '\200';
            break;

          case 'u': // Ă
            seq(c4)[1] = '\202';
            break;

          case 'k': // Ą
            seq(c4)[1] = '\204';
            break;

          default:
            continue;
          }

          break;

        case 'C':
          switch (charComposing) {
          case 'c':
            // Ç
            seq(c3)[1] = '\207';
            break;

          case '\'': // Ć
            seq(c4)[1] = '\206';
            break;

          case '^': // Ĉ
            seq(c4)[1] = '\210';
            break;

          case '.': // Ċ
            seq(c4)[1] = '\212';
            break;

          case 'v': // Č
            seq(c4)[1] = '\214';

          default:
            continue;
          }

          break;

        case 'D':
          if (charComposing == 'v') {
            // Ď
            seq(c4)[1] = '\216';
            break;
          } else {
            continue;
          }

        case 'E':
          switch (charComposing) {
          case '`': // È
            seq(c3)[1] = '\210';
            break;

          case '\'': // É
            seq(c3)[1] = '\211';
            break;

          case '^': // Ê
            seq(c3)[1] = '\212';
            break;

          case '"': // Ë
            seq(c3)[1] = '\213';
            break;

          case '=': // Ē
            seq(c4)[1] = '\222';
            break;

          case 'u': // Ĕ
            seq(c4)[1] = '\224';
            break;

          case '.': // Ė
            seq(c4)[1] = '\226';
            break;

          case 'k': // Ę
            seq(c4)[1] = '\230';
            break;

          case 'v': // Ě
            seq(c4)[1] = '\232';
            break;

          default:
            continue;
          }

          break;

        case 'G':
          switch (charComposing) {
          case '^': // Ĝ
            seq(c4)[1] = '\234';
            break;

          case 'u': // Ğ
            seq(c4)[1] = '\236';
            break;

          case '.': // Ġ
            seq(c4)[1] = '\240';
            break;

          case 'c': // Ģ
            seq(c4)[1] = '\242';
            break;

          default:
            continue;
          }

          break;

        case 'H':
          if (charComposing == '^') {
            // Ĥ
            seq(c4)[1] = '\244';
            break;
          } else {
            continue;
          }

        case 'I':
          switch (charComposing) {
          case '`': // Ì
            seq(c3)[1] = '\214';
            break;

          case '\'': // Í
            seq(c3)[1] = '\215';
            break;

          case '^': // Î
            seq(c3)[1] = '\216';
            break;

          case '"': // Ï
            seq(c3)[1] = '\217';
            break;

          case '~': // Ĩ
            seq(c4)[1] = '\250';
            break;

          default:
            continue;
          }

          break;

        case 'J':
          if (charComposing == '^') {
            // Ĵ
            seq(c4)[1] = '\264';
            break;
          } else {
            continue;
          }

        case 'K':
          if (charComposing == 'c') {
            // Ķ
            seq(c4)[1] = '\266';
            break;
          } else {
            continue;
          }

        case 'L':
          switch (charComposing) {
          case '\'': // Ĺ
            seq(c4)[1] = '\271';
            break;

          case 'c': // Ļ
            seq(c4)[1] = '\273';
            break;

          case 'v': // Ľ
            seq(c4)[1] = '\275';
            break;

          default:
            continue;
          }

          break;

        case 'N':
          switch (charComposing) {
          case '~': // Ñ
            seq(c3)[1] = '\221';
            break;

          case '\'': // Ń
            seq(c5)[1] = '\203';
            break;

          case 'c': // Ņ
            seq(c5)[1] = '\205';
            break;

          case 'v': // Ň
            seq(c5)[1] = '\207';
            break;

          default:
            continue;
          }

          break;

        case 'O':
          switch (charComposing) {
          case '`': // Ò
            seq(c3)[1] = '\222';
            break;

          case '\'': // Ó
            seq(c3)[1] = '\223';
            break;

          case '^': // Ô
            seq(c3)[1] = '\224';
            break;

          case '~': // Õ
            seq(c3)[1] = '\225';
            break;

          case '"': // Ö
            seq(c3)[1] = '\226';
            break;

          case '=': // Ō
            seq(c5)[1] = '\214';
            break;

          case 'u': // Ŏ
            seq(c5)[1] = '\216';
            break;

          default:
            continue;
          }

          break;

        case 'R':
          switch (charComposing) {
          case '\'': // Ŕ
            seq(c5)[1] = '\224';
            break;

          case 'c': // Ŗ
            seq(c5)[1] = '\226';
            break;

          case 'v': // Ř
            seq(c5)[1] = '\230';
            break;

          default:
            continue;
          }

          break;

        case 'S':
          switch (charComposing) {
          case '\'': // Ś
            seq(c5)[1] = '\232';
            break;

          case '^': // Ŝ
            seq(c5)[1] = '\234';
            break;

          case 'c': // Ş
            seq(c5)[1] = '\236';
            break;

          case 'v': // Š
            seq(c5)[1] = '\240';
            break;

          default:
            continue;
          }

          break;

        case 'T':
          switch (charComposing) {
          case 'c': // Ţ
            seq(c5)[1] = '\242';
            break;

          case 'v': // Ť
            seq(c5)[1] = '\244';
            break;

          default:
            continue;
          }

          break;

        case 'U':
          switch (charComposing) {
          case '`': // Ù
            seq(c3)[1] = '\231';
            break;

          case '\'': // Ú
            seq(c3)[1] = '\232';
            break;

          case '^': // Û
            seq(c3)[1] = '\233';
            break;

          case '"': // Ü
            seq(c3)[1] = '\234';
            break;

          case '~': // Ũ
            seq(c5)[1] = '\250';
            break;

          case '=': // Ū
            seq(c5)[1] = '\252';
            break;

          case 'u': // Ŭ
            seq(c5)[1] = '\254';
            break;

          case 'r': // Ů
            seq(c5)[1] = '\256';
            break;

          case 'k': // Ų
            seq(c5)[1] = '\262';
            break;

          default:
            continue;
          }

          break;

        case 'W':
          if (charComposing == '^') {
            // Ŵ
            seq(c5)[1] = '\264';
            break;
          } else {
            continue;
          }

        case 'Y':
          switch (charComposing) {
          case '\'': // Ý
            seq(c3)[1] = '\235';
            break;

          case '^': // Ŷ
            seq(c5)[1] = '\266';
            break;

          case '"': // Ÿ
            seq(c5)[1] = '\270';
            break;

          default:
            continue;
          }

          break;

        case 'Z':
          switch (charComposing) {
          case '\'': // Ź
            seq(c5)[1] = '\271';
            break;

          case '.': // Ż
            seq(c5)[1] = '\273';
            break;

          case 'v': // Ž
            seq(c5)[1] = '\275';
            break;

          default:
            continue;
          }

          break;

        default:
          continue;
        }
      } else {
        continue;
      }
    }

    str.replace(i, 1, utf8seq, 2);
    charComposing = 0;
    ++i;
  }

  return str;
}
