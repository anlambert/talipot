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

#include "DiffusionRateFunctionGaussian.h"
#include <cmath>

DifusionRateFunctionGaussian::DifusionRateFunctionGaussian(
    TimeDecreasingFunction *decreasingFunction)
    : DiffusionRateFunction(decreasingFunction) {}

DifusionRateFunctionGaussian::~DifusionRateFunctionGaussian() = default;

double DifusionRateFunctionGaussian::computeSpaceRate(unsigned int distance,
                                                      unsigned int currentIteration,
                                                      unsigned int maxIteration,
                                                      unsigned int inputSampleSize) {

    double timeVal =
        timeFunction->computeCurrentTimeRate(currentIteration, maxIteration, inputSampleSize);
    double value = (sqrt((distance * distance))) / (2 * (timeVal * timeVal));

    return exp(-value);
}
