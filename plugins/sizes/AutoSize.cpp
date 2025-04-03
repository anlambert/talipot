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

#include <talipot/PluginHeaders.h>

using namespace tlp;

/// AutoSize.cpp - Compute size in order to prevent node-node overlapping
/**
 * This plugin compute the size of nodes and edges such that node-node overlapping does not exist
 * (if it is possible).
 * and edge sizes are proportional to node sizes.
 *
 *  \author David Auber Bordeaux University France: Email:auber@labri.fr
 */
class AutoSize : public SizeAlgorithm {
  public:
    PLUGININFORMATION("Auto Sizing", "Auber", "04/05/2001",
                      "Resize the nodes and edges of a graph so that the graph gets easy to read. "
                      "The size of a node will depend on the number of its sons.",
                      "1.0", "Size")
    AutoSize(const tlp::PluginContext *context) : SizeAlgorithm(context) {}

    bool run() override {

        for (auto n : graph->nodes()) {
            result->setNodeValue(n, getNodeValue(n));
        }

        for (auto e : graph->edges()) {
            result->setEdgeValue(e, getEdgeValue(e));
        }
        return true;
    }

  private:
    Size getNodeValue(const node n) {
        LayoutProperty *entryLayout = graph->getLayoutProperty("viewLayout");
        SizeProperty *entrySize = graph->getSizeProperty("viewSize");

        // Compute the minimal distance to one neighbour.
        const Coord &tmp1 = entryLayout->getNodeValue(n);
        double dist = DBL_MAX;

        for (auto neigh : graph->nodes()) {
            if (neigh != n) {
                const Coord &tmp2 = entryLayout->getNodeValue(neigh);
                double tmpDist = sqrt((tmp1.getX() - tmp2.getX()) * (tmp1.getX() - tmp2.getX()) +
                                      (tmp1.getY() - tmp2.getY()) * (tmp1.getY() - tmp2.getY()) +
                                      (tmp1.getZ() - tmp2.getZ()) * (tmp1.getZ() - tmp2.getZ()));

                dist = std::min(dist, tmpDist);
            }
        }

        if (dist != DBL_MAX) {
            return Size(dist / 2, dist / 2, dist / 2);
        } else {
            return entrySize->getNodeValue(n);
        }
    }

    Size getEdgeValue(const edge e) {
        const auto &[src, tgt] = graph->ends(e);
        const Size &s = result->getNodeValue(src);
        const Size &t = result->getNodeValue(tgt);
        Coord tmp = {s.getW(), s.getH(), s.getD()};
        Coord tmp2 = {t.getW(), t.getH(), t.getD()};
        float sizes = tmp.norm();
        float sizet = tmp2.norm();
        return Size(sizes / 16, sizet / 16, sizet / 4);
    }
};

PLUGIN(AutoSize)
