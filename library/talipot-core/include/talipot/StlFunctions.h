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

#ifndef TALIPOT_STL_FUNCTIONS_H
#define TALIPOT_STL_FUNCTIONS_H

namespace tlp {

class DoubleProperty;
struct node;
struct edge;
/**
   This class enables to compare nodes and edges according to a metric,
   instances of this class are useful for using stl sort function.
 */
class LessByMetric {
  public:
    LessByMetric(DoubleProperty *metric) : metric(metric) {}
    bool operator()(node n1, node n2);
    bool operator()(edge e1, edge e2);

  private:
    DoubleProperty *metric;
};
}
#endif // TALIPOT_STL_FUNCTIONS_H
