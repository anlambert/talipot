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

#ifndef STRAHLER_METRIC_H
#define STRAHLER_METRIC_H

#include <talipot/hash.h>
#include <talipot/PluginHeaders.h>
struct Strahler {
  Strahler(int stra = 1, int sta = 0, int used = 0)
      : strahler(stra), stacks(sta), usedStack(used) {}
  int strahler;
  int stacks;
  int usedStack;
};

/** This plugin is an implementation of the Strahler numbers computation,
 *  first published as:
 *
 *  A.N. Strahler, \n
 *  "Hypsomic analysis of erosional topography",\n
 *  "Bulletin Geological Society of America 63,pages 1117-1142.",\n
 *   1952.\n
 *
 *  Extended to graphs in :
 *
 *  D. Auber, \n
 *  Using Strahler numbers for real time visual exploration of huge graphs, \n
 *  ICCVG, International Conference on Computer Vision and Graphics, \n
 *  pages 56-69, \n
 *  2002, September.
 *
 *  \note This algorithm works on general graphs, if no node is selected,
 *  the algorithm automatically choose the nodes with higher outdegree as
 *  starting nodes.
 *
 *  The algorithm use the "viewSelection" boolean property to determine
 *  the starting nodes.
 *
 */
class StrahlerMetric : public tlp::DoubleAlgorithm {
public:
  PLUGININFORMATION(
      "Strahler", "David Auber", "06/04/2000",
      "Computes the Strahler numbers."
      "This is an implementation of the Strahler numbers computation, first published as:<br/>"
      "<b>Hypsomic analysis of erosional topography</b>, A.N. Strahler, Bulletin Geological "
      "Society of America 63,pages 1117-1142 (1952).<br/>"
      "Extended to graphs in :<br/><b>Using Strahler numbers for real time visual exploration of "
      "huge graphs</b>, D. Auber, ICCVG, International Conference on Computer Vision and Graphics, "
      "pages 56-69 (2002)",
      "1.0", "Graph")
  StrahlerMetric(const tlp::PluginContext *context);
  bool run() override;

private:
  Strahler topSortStrahler(tlp::node n, int &curPref, flat_hash_map<tlp::node, int> &tofree,
                           flat_hash_map<tlp::node, int> &prefix,
                           flat_hash_map<tlp::node, bool> &visited,
                           flat_hash_map<tlp::node, bool> &finished,
                           flat_hash_map<tlp::node, Strahler> &cachedValues);
  bool allNodes;
};

#endif // STRAHLER_METRIC_H
