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

#ifndef TIME_DECREASING_FUNCTION_H
#define TIME_DECREASING_FUNCTION_H

/**
 * @brief Interface for building time rate function for learning coefficient computation.
 * Interface for building time rate function for learning coefficient computation. Compute the time
 * coefficient in function of the current iteration, the max iteration and the input sample size.
 * See SOM algorithm for more details on time coefficient in SOM computation.
 */
class TimeDecreasingFunction {
  public:
    TimeDecreasingFunction() = default;
    virtual ~TimeDecreasingFunction() = default;
    /**
     * Return the time coefficient for the given parameters.
     * @param currentIteration The current iteration.
     * @param maxIteration The maximum iteration number.
     * @param inputSampleSize The size of the input sample.
     * @return
     */
    virtual double computeCurrentTimeRate(unsigned int currentIteration, unsigned int maxIteration,
                                          unsigned int inputSampleSize) = 0;
};

#endif // TIME_DECREASING_FUNCTION_H
