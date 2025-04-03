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

#include "BendsTools.h"

using namespace tlp;
using namespace std;

BendsTools::BendsTools() = default;

//============================================
double BendsTools::cosAlpha(LayoutProperty *layout, const node a, const node b, const node c) {
    Vec2d point[3];
    Coord aC = layout->getNodeValue(a);
    point[0][0] = aC[0];
    point[0][1] = aC[1];
    Coord bC = layout->getNodeValue(b);
    point[1][0] = bC[0];
    point[1][1] = bC[1];
    Coord cC = layout->getNodeValue(c);
    point[2][0] = cC[0];
    point[2][1] = cC[1];

    Vec2d ba = point[0] - point[1];
    ba /= ba.norm();
    Vec2d bc = point[2] - point[1];
    bc /= bc.norm();

    return ba.dotProduct(bc) / (ba.norm() * bc.norm());
}
//============================================
bool BendsTools::straightLine(LayoutProperty *layout, const node a, const node b, const node c) {
    Vec2d point[3];
    Coord aC = layout->getNodeValue(a);
    point[0][0] = aC[0];
    point[0][1] = aC[1];
    Coord bC = layout->getNodeValue(b);
    point[1][0] = bC[0];
    point[1][1] = bC[1];
    Coord cC = layout->getNodeValue(c);
    point[2][0] = cC[0];
    point[2][1] = cC[1];

    Vec2d ba = point[0] - point[1];
    Vec2d bc = point[2] - point[1];
    Vec2d ac = point[2] - point[0];

    return fabs(ba.norm() + bc.norm() - ac.norm()) < 1E-9;
}
//============================================
vector<node> BendsTools::bendsSimplification(vector<node> &bends, LayoutProperty *layout) {
    // remove all <=90° angles if the 3 point are in a freecell
    vector<node> result1;
    bool redo = true;

    while (redo) {
        redo = false;
        result1.push_back(bends[0]);

        for (size_t i = 1; i < bends.size() - 1; ++i) {
            double cAlpha = cosAlpha(layout, bends[i - 1], bends[i], bends[i + 1]);

            if (fabs(cAlpha) < 1E-9) {
                // found 90
                bends[i] = bends[i - 1];
                redo = true;
            } else {
                result1.push_back(bends[i]);
            }
        }

        result1.push_back(bends[bends.size() - 1]);

        bends = result1;
        result1.clear();
    }

    //  return bends;
    // remove all straight line points
    result1.push_back(bends[0]);

    for (size_t i = 1; i < bends.size() - 1; ++i) {
        bool straight = straightLine(layout, bends[i - 1], bends[i], bends[i + 1]);

        if (straight) {
            // found 180°
            bends[i] = bends[i - 1];
        } else {
            result1.push_back(bends[i]);
        }
    }

    result1.push_back(bends[bends.size() - 1]);
    return result1;
}
