/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "TimeDecreasingFunctionSimple.h"

TimeDecreasingFunctionSimple::TimeDecreasingFunctionSimple(double initialCoef)
    : initialCoef(initialCoef) {}

TimeDecreasingFunctionSimple::~TimeDecreasingFunctionSimple() = default;

double TimeDecreasingFunctionSimple::computeCurrentTimeRate(unsigned int currentIteration,
                                                            unsigned int,
                                                            unsigned int inputSampleSize) {

  // Learning rate
  return initialCoef / (1 + (currentIteration / inputSampleSize));
}
