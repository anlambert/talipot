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

#ifndef CLIQUE_ENUMERATION_H
#define CLIQUE_ENUMERATION_H

#include <string>
#include <set>
#include <vector>

#include <talipot/Algorithm.h>

/**
 * \file
 * \brief Compute all maximal cliques (or maximal cliques whose size is above a given threshold)
 * according to the Eppstein algorithm. See
 *
 * Eppstein, Loffler and Strash, \n
 * "Listing All Maximal Cliques in Sparse Graphs in Near-optimal Time",\n
 * "Experimental Algorithms", Springer\n
 * "2011"
 *
 *
 *  <b>HISTORY</b>
 *
 *
 * - 2017 Version 1.0 (Bruno Pinaud): moved to Tulip trunk and clean the code a bit.
 * - 2013 Version 0.1: Initial release
 *  by François Queyroi, LaBRI, University Bordeaux I, France
 *
 *  <b>LICENCE</b>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
class CliqueEnumeration : public tlp::Algorithm {

    void addClique(const std::vector<tlp::node> &);

    void getDegenerateOrdering(std::vector<tlp::node> &);
    void getNeighborhood(const tlp::node, std::set<tlp::node> &);
    tlp::node choosePivot(const std::set<tlp::node> &);
    void maxCliquePivot(std::set<tlp::node> &, const std::vector<tlp::node> &,
                        std::set<tlp::node> &);

    uint minsize;
    unsigned cliqueid;

  public:
    PLUGININFORMATION(
        "Maximal Cliques Enumeration", "Francois Queyroi", "24/10/2013",
        "Compute all maximal cliques (or maximal cliques whose size is above a given threshold)"
        " according to the Eppstein algorithm. See"
        " Eppstein, Loffler and Strash, "
        "Listing All Maximal Cliques in Sparse Graphs in Near-optimal Time, "
        "Experimental Algorithms, Springer, "
        "2011",
        "1.0", "Clustering")

    CliqueEnumeration(tlp::PluginContext *);
    bool run() override;
    bool check(std::string &) override;
};
#endif // CLIQUE_ENUMERATION_H
