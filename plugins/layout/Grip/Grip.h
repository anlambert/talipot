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

#ifndef GRIP_H
#define GRIP_H

#include <talipot/PluginHeaders.h>
#include <talipot/hash.h>
#include "MISFiltering.h"

/** \file
 *  \brief  This plugin is an implementation of the GRIP layout
 *
 *  An implementation of a force directed graph drawing
 *  algorithm first published as:
 *
 *  P. Gajer and S.G. Kobourov,
 *  "GRIP: Graph dRawing with Intelligent Placement",
 *  in Journal Graph Algorithm and Applications, vol. 6, no. 3,
 *  p. 203-224, 2002
 *
 *  <b>HISTORY</b>
 *
 *  - 2007 Version 1.0: Initial release
 *  by Romain Bourqui, LaBRI, University Bordeaux I, France
 *
 *
 */

class Grip : public tlp::LayoutAlgorithm {

public:
  PLUGININFORMATION("GRIP", "Romain Bourqui", "01/11/2010",
                    "Implements a force directed graph drawing algorithm first published as:<br/>"
                    "<b>GRIP: Graph dRawing with Intelligent Placement</b>, P. Gajer and S.G. "
                    "Kobourov, Journal Graph Algorithm and Applications, vol. 6, no. 3, pages "
                    "203--224, (2002).",
                    "1.1", "Force Directed")

  Grip(const tlp::PluginContext *);
  ~Grip() override;

  bool run() override;

private:
  void computeCurrentGraphLayout();
  void computeOrdering();
  void firstNodesPlacement();
  void placement();
  void initialPlacement(uint, uint);
  void kk_local_reffinement(tlp::node);
  void kk_reffinement(uint, uint);
  void fr_reffinement(uint, uint);
  void displace(tlp::node);
  void updateLocalTemp(tlp::node);
  void init();
  void init_heat(uint);

  void seeLayout(uint);

  uint rounds(uint, uint, uint, uint, uint);
  void set_nbr_size();
  float sched(int, int, int, int, int);

  MISFiltering *misf;
  float edgeLength;
  int level;
  flat_hash_map<tlp::node, std::vector<uint>> neighbors_dist;
  flat_hash_map<tlp::node, std::vector<tlp::node>> neighbors;
  flat_hash_map<uint, uint> levelToNbNeighbors;
  flat_hash_map<tlp::node, tlp::Coord> disp;
  flat_hash_map<tlp::node, tlp::Coord> oldDisp;
  flat_hash_map<tlp::node, double> heat;
  flat_hash_map<tlp::node, double> oldCos;

  tlp::Graph *currentGraph;
  int _dim;
};
#endif // GRIP_H
