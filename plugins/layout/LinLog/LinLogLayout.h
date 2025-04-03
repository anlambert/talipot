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

#ifndef LIN_LOG_LAYOUT_H
#define LIN_LOG_LAYOUT_H

#include <talipot/PluginHeaders.h>
#include "OctTree.h"

class LinLogLayout {

  public:
    LinLogLayout(tlp::Graph *_graph, tlp::PluginProgress *pluginProgress);
    bool initAlgo(tlp::LayoutProperty *_layoutResult, tlp::NumericProperty *_weight,
                  double _attrExponent, double _repuExponent, double _gravFactor, uint _max_iter,
                  bool _is3D, bool _useOctTree, tlp::BooleanProperty *_skipNodes);

    bool startAlgo();

  private:
    tlp::NumericProperty *edgeWeight;
    tlp::LayoutProperty *layoutResult;
    tlp::DoubleProperty linLogWeight;
    tlp::BooleanProperty *skipNodes;
    tlp::Graph *graph;

    tlp::PluginProgress *pluginProgress;

    uint _dim;     // 2 or 3;
    uint _nbNodes; // number of nodes in the graph
    uint max_iter; // the max number of iterations
    bool useOctTree;

    /** Factor for repulsion energy. */
    double repuFactor;
    /** Exponent of the Euclidean distance in the repulsion energy. */
    double repuExponent;
    /** Exponent of the Euclidean distance in the attraction energy. */
    double attrExponent;
    /** Factor for the gravitation energy = attraction to the barycenter.
        Set to 0.0 for no gravitation. */
    double gravFactor;

    /** Position of the barycenter of the nodes. */
    Coord baryCenter;

    double getGravitationEnergy(node u);
    double getAttractionEnergy(node u);
    double getRepulsionEnergy(node u);
    double getEnergy(node u);
    double getDist(const Coord &pos1, const Coord &pos2) const;
    double getDistForComparison(const Coord &pos1, const Coord &pos2) const;

    double addRepulsionDir(node u, double *dir);
    double addAttractionDir(node u, double *dir);
    double addGravitationDir(node u, double *dir);
    void getDirection(node u, double *dir);

    bool minimizeEnergyNoTree(int nrIterations);

    void initEnergyFactors();
    void computeBaryCenter();

    void setComputationalWeigths();

    void initWeights();

    OctTree *buildOctTree();
    bool minimizeEnergy(int nrIterations);
    double addRepulsionDir(node u, double *dir, OctTree *tree);
    double getRepulsionEnergy(node u, OctTree *tree);
    double getEnergy(node u, OctTree *tree);
    void getDirection(node u, double *dir, OctTree *tree);
};
#endif // LIN_LOG_LAYOUT_H
