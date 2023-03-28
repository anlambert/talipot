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

#ifndef NUMBER_H
#define NUMBER_H

/**
 *This class enables the comparison of floats or the initialization of float
 *to infinity.
 */
class Number {
public:
  float value;         /**< Contain the value of the number. */

  static float infini; /**< contain our value of infinity. */

public:
  /**
   *The constructor initializes the field value to the float f.
   */
  Number(float f) {
    value = f;
  }

  /**
   *Overload of the operator >.
   */
  bool operator>(float b) const;
};

#endif // NUMBER_H
