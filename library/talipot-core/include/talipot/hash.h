/**
 *
 * Copyright (C) 2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TLP_HASH_H
#define TLP_HASH_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
#include <gtl/phmap.hpp>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#define tlp_hash_map gtl::flat_hash_map
#define tlp_hash_map_safe gtl::node_hash_map

// The hash_combine function from the boost library
// Call it repeatedly to incrementally create a hash value from several variables.

// Explanation of the formula from StackOverflow
// (http://stackoverflow.com/questions/4948780/magic-numbers-in-boosthash-combine) :
// The magic number 0x9e3779b9 = 2^32 / ((1 + sqrt(5)) / 2) is the reciprocal of the golden ratio.
// It is supposed to be 32 random bits, where each is equally likely to be 0 or 1, and with no
// simple correlation between the bits.
// So including this number "randomly" changes each bit of the seed; as you say, this means that
// consecutive values will be far apart.
// Including the shifted versions of the old seed makes sure that, even if hash_value() has a fairly
// small range of values,
// differences will soon be spread across all the bits.
namespace std {
template <class T>
inline void tlp_hash_combine(std::size_t &seed, const T &v) {
  hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
} // namespace std

#endif // TLP_HASH_H
