/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_EDGE_H
#define TALIPOT_EDGE_H

#include <climits>
#include <functional>

namespace tlp {

/**
 * @ingroup Graph
 * @brief The edge struct represents an edge in a Graph object.
 *
 * This structure only contains an identifier, and a function to check if the edge is valid.
 * A edge is considered invalid when its id has the UINT_MAX value.
 *
 * Most operations performed on an edge (getting the source, target, etc) are available into the
 * tlp::Graph object.
 *
 * @see tlp::node
 * @see tlp::Graph
 */
struct edge {
  /**
   * @brief id The identifier of the edge.
   */
  unsigned int id;

  /**
   * @brief edge creates an invalid edge.
   */
  edge() : id(UINT_MAX) {}

  /**
   * @brief edge Create an edge of given identifier.
   * It is your responsibility to make sure an edge of this ID exists when you create the edge.
   * If you want to make sure this edge exists, use Graph::isElement(), as isValid() will only tell
   * is the edge was correctly initialized.
   *
   * @param j the identifier this edge will use.
   */
  explicit edge(unsigned int j) : id(j) {}

  /**
   * @brief operator unsigned int A convenience function to get the id of an edge.
   */
  operator unsigned int() const {
    return id;
  }

  /**
   * @brief operator == checks if two edges are equals.
   * @param n The other edge to compare this one to.
   * @return Whether or not the two edges are equal.
   */
  bool operator==(const edge e) const {
    return id == e.id;
  }

  /**
   * @brief operator != checks if two edges are different.
   * @param n The other edge to compare this one to.
   * @return Whether or not the two edges are different.
   */
  bool operator!=(const edge e) const {
    return id != e.id;
  }

  /**
   * @brief isValid checks if the edge is valid.
   * An invalid edge is an edge whose id is UINT_MAX.
   *
   * @return whether the edge is valid or not.
   */
  bool isValid() const {
    return id != UINT_MAX;
  }
};
} // namespace tlp

#ifdef _MSC_VER
#include <vector>
#include <talipot/config.h>
// needed by MSVC to avoid multiple definitions
struct TLP_SCOPE __tlp_vector_edge : public std::vector<tlp::edge> {};
#endif

///@cond DOXYGEN_HIDDEN
// these three functions allow to use tlp::edge as a key in a hash-based data structure (e.g.
// hashmap).
namespace std {
template <>
struct hash<tlp::edge> {
  size_t operator()(const tlp::edge e) const {
    return e.id;
  }
};
template <>
struct equal_to<tlp::edge> {
  size_t operator()(const tlp::edge e, const tlp::edge e2) const {
    return e.id == e2.id;
  }
};
template <>
struct less<tlp::edge> {
  size_t operator()(const tlp::edge e, const tlp::edge e2) const {
    return e.id < e2.id;
  }
};
} // namespace std
///@endcond

#endif // TALIPOT_EDGE_H
