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

#include "HilbertLayout.h"

using namespace std;
using namespace tlp;

namespace {
enum STATESID { KEY = 1, POINT = 0, NEXT = 2 };

static const unsigned char states[4][3][4] = {{{0, 1, 3, 2}, {0, 1, 3, 2}, {1, 0, 0, 2}},
                                              {{0, 3, 1, 2}, {0, 2, 3, 1}, {0, 1, 1, 3}},
                                              {{2, 1, 3, 0}, {3, 1, 0, 2}, {3, 2, 2, 0}},
                                              {{2, 3, 1, 0}, {3, 2, 0, 1}, {2, 3, 3, 1}}};

inline Vec2i hilbertPoint(const uint key, const unsigned char order) {
    //  cerr << "========" << endl;
    unsigned char state = 0;
    Vec2i point;
    point.fill(0);

    for (char i = order - 1; i >= 0; --i) {
        // read two bits;
        unsigned char bits = (key >> (i << 1)) & 3;
        //    unsigned char co = grState.graph[state].key[bits];
        unsigned char co = states[state][KEY][bits];
        //    cerr << "state :" << (int) state << " bits" << bits << " co:" << (int)co;
        point[1] += (co & 1) << i;
        //    point[0] += ((co >> 1) & 1) << (i-1);
        point[0] += ((co >> 1)) << i;
        //    state = grState.graph[state].next[bits];
        state = states[state][NEXT][bits];
        //    cerr << "  next state :" << (int)state << endl;
    }

    return point;
}

inline uint hilbertKey(const Vec2i &p, const unsigned char order) {
    //  cerr << "========" << endl;
    unsigned char state = 0;
    uint key = 0;

    for (char i = order - 1; i >= 0; --i) {
        // read two bits;
        unsigned char bits = (p[1] >> i) & 1;
        bits += ((p[0] >> i) & 1) << 1;
        //    cerr << "p:" << p << " bits:" << bits << endl;
        //    unsigned char co = grState.graph[state].point[bits];
        unsigned char co = states[state][POINT][bits];
        //    cerr << "state :" << (int) state << " bits" << bits << " co:" << (int)co;
        key += co << (i << 1);
        //    state = grState.graph[state].next[co];
        state = states[state][NEXT][co];
        //    cerr << "  next state :" << (int)state << endl;
    }

    return key;
}
}

HilbertLayout::HilbertLayout(unsigned char order) : order(order) {
    shift = int(rint(sqrt(pow(4., order)) / 2.));
}
//==============================================================
uint HilbertLayout::unproject(const Vec2i &point) const {
    Vec2i p;

    if (point[0] <= -shift || point[0] >= shift) {
        return UINT_MAX;
    }

    if (point[1] <= -shift || point[1] >= shift) {
        return UINT_MAX;
    }

    p[0] = point[0] + shift;
    p[1] = point[1] + shift;
    return hilbertKey(p, order);
}
//==============================================================
Vec2i HilbertLayout::project(const uint id) const {
    return hilbertPoint(id, order) -= shift;
}
