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

#ifndef MIS_FILTERING_H
#define MIS_FILTERING_H

#include <talipot/hash.h>
#include <unordered_set>

#include <talipot/PluginHeaders.h>
#include <talipot/MutableContainer.h>

class MISFiltering {

public:
  MISFiltering(tlp::Graph *);
  ~MISFiltering();

  void computeFiltering();
  void getNearest(tlp::node, std::vector<tlp::node> &, std::vector<uint> &, uint, uint = 3);

  std::vector<tlp::node> ordering;
  std::vector<uint> index;

private:
  uint level;
  tlp::MutableContainer<bool> inLastVi;
  tlp::MutableContainer<bool> inCurVi;
  tlp::MutableContainer<bool> removed;
  std::vector<uint> v_dist;

  flat_hash_map<uint, std::unordered_set<tlp::node>> levelToNodes;

  void bfsDepth(tlp::node, uint);
  void updateVectors();

  tlp::Graph *g_copy;
  tlp::MutableContainer<bool> removedVisit;
  tlp::MutableContainer<bool> attendedVisit;
  tlp::MutableContainer<bool> visited;

  std::vector<tlp::node> toVisit;
};
#endif // MIS_FILTERING_H
