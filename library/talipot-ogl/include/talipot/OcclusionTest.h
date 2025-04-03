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

#ifndef TALIPOT_OCCLUSION_TEST_H
#define TALIPOT_OCCLUSION_TEST_H

#include <vector>

#include <talipot/Rectangle.h>

namespace tlp {

typedef Rectangle<int> RectangleInt2D;

/**
 * @brief Manage a set of non overlapping 2D Axis Aligned Bounding Box
 *
 * That class enables to store a set of non overlapping 2D AABB.
 *
 * @todo Use the Talipot quadtree to store AABB and thus speedup testRectangle function
 */
struct TLP_GL_SCOPE OcclusionTest {
    std::vector<RectangleInt2D> data;
    /**
     * Remove all 2D AABB previously added.
     */
    void clear() {
        data.clear();
    }
    /**
     * Add a new 2D AABB to the set of non overlapping AABB
     * if that AABB intersect with AABB already inserted,
     * the AABB is not inserted.
     *
     * @return true if the AABB is inserted else false.
     *
     */
    bool addRectangle(const RectangleInt2D &rec) {
        if (!testRectangle(rec)) {
            data.push_back(rec);
            return true;
        }

        return false;
    }
    /**
     * @brief test wehter or nort the AABB intersect with a AABB already inserted.
     *
     * @return true if the AABB intersect else false.
     */
    bool testRectangle(const RectangleInt2D &rec) {
        for (const auto &r : data) {
            if (rec.intersect(r)) {
                return true;
            }
        }

        return false;
    }
};
}

#endif // TALIPOT_OCCLUSION_TEST_H
