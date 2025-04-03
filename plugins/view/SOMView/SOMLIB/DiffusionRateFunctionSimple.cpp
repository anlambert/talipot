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

#include "DiffusionRateFunctionSimple.h"
DiffusionRateFunctionSimple::DiffusionRateFunctionSimple(TimeDecreasingFunction *timeFunction,
                                                         unsigned int neighborhood)
    : DiffusionRateFunction(timeFunction), neighborhoodMax(neighborhood) {}

DiffusionRateFunctionSimple::~DiffusionRateFunctionSimple() {
    // TODO Auto-generated destructor stub
}

double DiffusionRateFunctionSimple::computeSpaceRate(unsigned int distance,
                                                     unsigned int currentIteration,
                                                     unsigned int maxIteration,
                                                     unsigned int inputSampleSize) {

    if (distance <= neighborhoodMax) {
        return 1 * timeFunction->computeCurrentTimeRate(currentIteration, maxIteration,
                                                        inputSampleSize);
    } else {
        return 0;
    }
}
