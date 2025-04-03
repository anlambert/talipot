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

#include "DiffusionRateFunction.h"

DiffusionRateFunction::DiffusionRateFunction(TimeDecreasingFunction *decreasingFunction)
    : timeFunction(decreasingFunction) {}

DiffusionRateFunction::~DiffusionRateFunction() {
    delete timeFunction;
}

TimeDecreasingFunction *DiffusionRateFunction::getTimeDecreasingFunction() {
    return timeFunction;
}

void DiffusionRateFunction::setTimeDecreasingFunction(TimeDecreasingFunction *timeFunction) {
    if (this->timeFunction) {
        delete this->timeFunction;
    }

    this->timeFunction = timeFunction;
}
