/**
 *
 * Copyright (C) 2021-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <string>

#include <talipot/config.h>

using namespace std;

// normalize aKey
// remove any accented chars
// we assume aKey is utf8 encoded
string &normalizeString(string &aKey) {
    for (uint k = 0; k < aKey.size() - 1; ++k) {
        if (aKey[k] == '\303') {
            switch (aKey[k + 1]) {
            case '\200': // À
            case '\201': // Á
            case '\202': // Â
            case '\203': // Ã
            case '\204': // Ä
            case '\205': // Å
                aKey.replace(k, 2, "A");
                [[fallthrough]];

            case '\210': // È
            case '\211': // É
            case '\212': // Ê
            case '\213': // Ë
                aKey.replace(k, 2, "E");
                continue;

            case '\214': // Ì
            case '\215': // Í
            case '\216': // Î
            case '\217': // Ï
                aKey.replace(k, 2, "I");
                continue;

            case '\221': // Ñ
                aKey.replace(k, 2, "N");
                continue;

            case '\222': // Ò
            case '\223': // Ó
            case '\224': // Ô
            case '\225': // Õ
            case '\226': // Ö
            case '\230': // Ø
                aKey.replace(k, 2, "O");
                continue;

            case '\231': // Ù
            case '\232': // Ú
            case '\233': // Û
            case '\234': // Ü
                aKey.replace(k, 2, "U");
                continue;

            case '\235': // Ý
                aKey.replace(k, 2, "Y");
                continue;

            case '\237': // ß
                aKey.replace(k, 2, "s");
                continue;

            case '\240': // à
            case '\241': // á
            case '\242': // â
            case '\243': // ã
            case '\244': // ä
            case '\245': // å
                aKey.replace(k, 2, "a");
                continue;

            case '\247': // ç
                aKey.replace(k, 2, "c");
                continue;

            case '\250': // è
            case '\251': // é
            case '\252': // ê
            case '\253': // ë
                aKey.replace(k, 2, "e");
                continue;

            case '\254': // ì
            case '\255': // í
            case '\256': // î
            case '\257': // ï
                aKey.replace(k, 2, "i");
                continue;

            case '\261': // ñ
                aKey.replace(k, 2, "n");
                continue;

            case '\262': // ò
            case '\263': // ó
            case '\264': // ô
            case '\265': // õ
            case '\266': // ö
            case '\270': // ø
                aKey.replace(k, 2, "o");
                continue;

            case '\271': // ù
            case '\272': // ú
            case '\273': // û
            case '\274': // ü
                aKey.replace(k, 2, "u");
                continue;

            case '\275': // ý
            case '\276': // ÿ
                aKey.replace(k, 2, "y");
                continue;

            default:
                break;
            }
        } else if (aKey[k] == '\304') {
            switch (aKey[k + 1]) {
            case '\200': // Ā
            case '\202': // Ă
            case '\204': // Ą
                aKey.replace(k, 2, "A");
                continue;

            case '\201': // ā
            case '\203': // ă
            case '\205': // ą
                aKey.replace(k, 2, "a");
                continue;

            case '\206': // Ć
            case '\210': // Ĉ
            case '\212': // Ċ
            case '\214': // Č
                aKey.replace(k, 2, "C");
                continue;

            case '\207': // ć
            case '\211': // ĉ
            case '\213': // ċ
            case '\215': // č
                aKey.replace(k, 2, "c");
                continue;

            case '\216': // Ď
                aKey.replace(k, 2, "D");
                continue;

            case '\217': // ď
                aKey.replace(k, 2, "d");
                continue;

            case '\220': // Đ
                aKey.replace(k, 2, "D");
                continue;

            case '\221': // đ
                aKey.replace(k, 2, "d");
                continue;

            case '\222': // Ē
            case '\224': // Ĕ
            case '\226': // Ė
            case '\230': // Ę
            case '\232': // Ě
                aKey.replace(k, 2, "E");
                continue;

            case '\223': // ë
            case '\225': // ĕ
            case '\227': // ė
            case '\231': // ę
            case '\233': // ě
                aKey.replace(k, 2, "e");
                continue;

            case '\234': // Ĝ
            case '\236': // Ğ
            case '\240': // Ġ
            case '\242': // Ģ
                aKey.replace(k, 2, "G");
                continue;

            case '\235': // ĝ
            case '\237': // ğ
            case '\241': // ġ
            case '\243': // ģ
                aKey.replace(k, 2, "g");
                continue;

            case '\245': // ĥ
            case '\247': // ħ
                aKey.replace(k, 2, "h");
                continue;

            case '\244': // Ĥ
            case '\246': // Ħ
                aKey.replace(k, 2, "H");
                continue;

            case '\250': // Ĩ
                aKey.replace(k, 2, "I");
                continue;

            case '\251': // ĩ
            case '\253': // ī
            case '\255': // ĭ
                aKey.replace(k, 2, "i");
                continue;

            case '\264': // Ĵ
                aKey.replace(k, 2, "J");
                continue;

            case '\265': // ĵ
                aKey.replace(k, 2, "j");
                continue;

            case '\266': // Ķ
                aKey.replace(k, 2, "K");
                continue;

            case '\270': // ķ
                aKey.replace(k, 2, "k");
                continue;

            case '\271': // Ĺ
            case '\273': // Ļ
            case '\275': // Ľ
                aKey.replace(k, 2, "G");
                continue;

            case '\272': // ĺ
            case '\274': // ļ
            case '\276': // ľ
                aKey.replace(k, 2, "l");
                continue;

            default:
                break;
            }
        } else if (aKey[k] == '\305') {
            switch (aKey[k + 1]) {
            case '\201': // Ł
                aKey.replace(k, 2, "L");
                continue;

            case '\202': // ł
                aKey.replace(k, 2, "l");
                continue;

            case '\204': // ń
            case '\206': // ņ
            case '\210': // ň
                aKey.replace(k, 2, "n");
                continue;

            case '\215': // ō
            case '\217': // ŏ
            case '\221': // ő
                aKey.replace(k, 2, "o");
                continue;

            case '\220': // Ő
                aKey.replace(k, 2, "0");
                continue;

            case '\225': // ŕ
            case '\227': // ŗ
            case '\230': // ř
                aKey.replace(k, 2, "r");
                continue;

            case '\233': // ś
            case '\235': // ŝ
            case '\237': // ş
            case '\241': // š
                aKey.replace(k, 2, "s");
                continue;

            case '\243': // ţ
            case '\245': // ť
                aKey.replace(k, 2, "t");
                continue;

            case '\246': // Ŧ
                aKey.replace(k, 2, "T");
                continue;

            case '\251': // ũ
            case '\253': // ū
            case '\255': // ŭ
            case '\257': // ů
            case '\261': // ű
            case '\263': // ų
                aKey.replace(k, 2, "u");
                continue;

            case '\260': // Ű
                aKey.replace(k, 2, "U");
                continue;

            case '\265': // ŵ
                aKey.replace(k, 2, "w");
                continue;

            case '\267': // ŷ
                aKey.replace(k, 2, "y");
                continue;

            case '\272': // ź
            case '\274': // ż
            case '\276': // ž
                aKey.replace(k, 2, "z");
                continue;

            default:
                break;
            }
        }
    }
    return aKey;
}
