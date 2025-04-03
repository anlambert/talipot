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

#ifndef DIFFUSION_RATE_FUNCTION_H
#define DIFFUSION_RATE_FUNCTION_H

#include "TimeDecreasingFunction.h"

/**
 * @brief Interface for distance rate computation functions.
 * Interface for distance rate computation functions. Distance rate is used in learning rate
 * computation to handle the distance factor.
 * These objects take a TimeDecreasingFunction as the diffusion coefficient evolve with distance but
 * also with time.
 * See SOM algorithm for more details on time coefficient in SOM computation.
 */
class DiffusionRateFunction {
  public:
    DiffusionRateFunction(TimeDecreasingFunction *decreasingFunction);
    virtual ~DiffusionRateFunction();
    /**
     * Compute the diffusion rate in function of the current time and the distance.
     * @param distance The distance between the BMU and the node. This distance is the number of
     * node to go through from BMU to current node.
     * @param currentIteration The current iteration number.
     * @param maxIteration The maximum iteration number.
     * @param inputSampleSize The sample size.
     * @return the diffusion rate.
     */
    virtual double computeSpaceRate(unsigned int distance, unsigned int currentIteration,
                                    unsigned int maxIteration, unsigned int inputSampleSize) = 0;
    TimeDecreasingFunction *getTimeDecreasingFunction();
    void setTimeDecreasingFunction(TimeDecreasingFunction *timeFunction);

  protected:
    TimeDecreasingFunction *timeFunction;
};

#endif // DIFFUSION_RATE_FUNCTION_H
