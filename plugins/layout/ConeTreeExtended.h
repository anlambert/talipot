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

#ifndef CONE_TREE_EXTENDED_H
#define CONE_TREE_EXTENDED_H

#include <talipot/hash.h>
#include <talipot/PluginHeaders.h>

/// ConeTreeExtended.h - An implementation of the cone tree layout.
/** This plugin is an extension of the Cone tree layout
 *  algorithm first published as:
 *
 *  A. FJ. Carriere and R. Kazman, \n
 *  "Interacting with Huge Hierarchies: Beyond Cone Trees", \n
 *  In Proceedings of InfoViz'95, \n
 *  IEEE Symposium on Information Visualization \n
 *  pages 74-78, 1995,
 *
 *  Information about the extension can be found in :
 *
 *  D.Auber, PhD Thesis, "Outils de visualisation de larges structures de donnees",
 *  University Bordeaux, december 2002.
 *
 *  <b> HISTORY</b>
 *
 *  - 01/04/99 Version 0.0.1: Initial release
 *
 *  \note This algorithm only works on trees.
 *  Let n be the number of nodes, the algorithm complexity is in O(n).
 *
 *  \author David Auber University Bordeaux I France: Email:auber@labri.fr
 */
class ConeTreeExtended : public tlp::LayoutAlgorithm {
public:
  PLUGININFORMATION(
      "Cone Tree", "David Auber", "01/04/2001",
      "Implements an extension of the Cone tree layout algorithm first published as:<br/>"
      "<b>Interacting with Huge Hierarchies: Beyond Cone Trees </b>, A. FJ. Carriere and R. "
      "Kazman, InfoViz'95, IEEE Symposium on Information Visualization pages 74--78 (1995).",
      "1.0", "Tree")
  ConeTreeExtended(const tlp::PluginContext *context);
  ~ConeTreeExtended() override;
  bool run() override;

private:
  double treePlace3D(tlp::node n, flat_hash_map<tlp::node, double> *posRelX,
                     flat_hash_map<tlp::node, double> *posRelY);
  void calcLayout(tlp::node n, flat_hash_map<tlp::node, double> *px,
                  flat_hash_map<tlp::node, double> *py, double x, double y, int level);
  void computeLayerSize(tlp::node n, uint level);
  void computeYCoodinates(tlp::node root);
  tlp::Graph *tree;
  tlp::SizeProperty *nodeSize;
  std::vector<float> yCoordinates;
  std::vector<float> levelSize;
  float spaceBetweenLevels;
};

#endif // CONE_TREE_EXTENDED_H
