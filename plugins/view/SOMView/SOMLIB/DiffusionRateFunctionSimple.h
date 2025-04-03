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

#ifndef DIFFUSION_RATE_FUNCTION_SIMPLE_H
#define DIFFUSION_RATE_FUNCTION_SIMPLE_H

#include "DiffusionRateFunction.h"

class DiffusionRateFunctionSimple : public DiffusionRateFunction {
  public:
    DiffusionRateFunctionSimple(TimeDecreasingFunction *timeFunction, unsigned int neighborhood);
    ~DiffusionRateFunctionSimple() override;
    double computeSpaceRate(unsigned int distance, unsigned int currentIteration,
                            unsigned int maxIteration, unsigned int inputSampleSize) override;

  protected:
    unsigned int neighborhoodMax;
};

#endif // DIFFUSION_RATE_FUNCTION_SIMPLE_H
