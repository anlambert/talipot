/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef EDGE_BUNDLING_H
#define EDGE_BUNDLING_H

#include <climits>
#include <vector>
#include <talipot/PluginHeaders.h>
#include <talipot/LayoutProperty.h>

/**
 * This plugin is an implementation of the intuitive Edge Bundling technique published in :
 * "Winding Roads: Routing edges into bundles"
 * Antoine Lambert, Romain Bourqui and David Auber
 * Computer Graphics Forum special issue on 12th Eurographics/IEEE-VGTC Symposium on Visualization,
 * June 2010, pages 853-862
 *
 * Visualizations of large graphs usually suffer from visual clutter due to a large amount of edge
 * crossings and node-edge overlaps.
 * Edge bundling is a technique aiming to reduce such clutter by routing edges and grouping them
 * into bundles.
 * It also helps to uncover high level edge patterns to show how information flows between different
 * region of a graph.
 *
 *  \author David Auber, Romain Bourqui, Antoine Lambert, LaBRI University Bordeaux I France:
 *   auber@labri.fr
 */
class EdgeBundling : public tlp::Algorithm {
public:
  EdgeBundling(const tlp::PluginContext *);
  PLUGININFORMATION("Edge bundling", "David Auber/ Romain Bourqui / Antoine Lambert", "2010",
                    "Edges routing algorithm, implementing the intuitive Edge Bundling technique "
                    "published in :<br/>"
                    "<b>Winding Roads: Routing edges into bundles</b>, Antoine Lambert, Romain "
                    "Bourqui and David Auber, Computer Graphics Forum special issue on 12th "
                    "Eurographics/IEEE-VGTC Symposium on Visualization, pages 853-862 (2010).",
                    "1.3", "")
  bool run() override;

private:
  void fixEdgeType(tlp::EdgeVectorProperty<uint> &);
  void computeDistances();
  void computeDistance(tlp::node);
  double longEdges;
  double splitRatio;
  uint optimizationLevel;
  uint maxThread;
  bool forceEdgeTest;
  bool edgeNodeOverlap;
  bool layout3D;
  tlp::LayoutProperty *layout;
  tlp::Graph *vertexCoverGraph, *oriGraph, *gridGraph;
};

#endif // EDGE_BUNDLING_H
