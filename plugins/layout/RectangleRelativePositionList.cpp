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

#include "RectangleRelativePositionList.h"

using namespace std;
using namespace tlp;

void RectangleRelativePositionList::addRectangleRelativePosition(
    vector<Rectangle<float>>::iterator itr, int numRect, float wdth, float hght, float x, float y,
    list<RectangleRelativePosition>::iterator itRectangleRelativePosition) {

    RectangleRelativePosition newRectangleRelativePosition;
    newRectangleRelativePosition.rectangleIterator = itr;
    newRectangleRelativePosition.rectangleNumber = numRect;
    newRectangleRelativePosition.rectangleWidth = wdth;
    newRectangleRelativePosition.rectangleHeight = hght;
    newRectangleRelativePosition.rectangleLeftAbscissa = x;
    newRectangleRelativePosition.rectangleLowOrdinate = y;

    this->insert(itRectangleRelativePosition, newRectangleRelativePosition);
}

void RectangleRelativePositionList::allocateCoordinates() {

    for (auto &itr : *this) {
        (*(itr.rectangleIterator))[0][0] = itr.rectangleLeftAbscissa;
        (*(itr.rectangleIterator))[0][1] = itr.rectangleLowOrdinate;
        (*(itr.rectangleIterator))[1][0] = (*(itr.rectangleIterator))[0][0] + itr.rectangleWidth;
        (*(itr.rectangleIterator))[1][1] = (*(itr.rectangleIterator))[0][1] + itr.rectangleHeight;
    }
}

void RectangleRelativePositionList::stockOfTemporaryBestCoordinates(int bestPlaceInFirstSequence) {

    auto itRectToReposition = this->rbegin();
    int positionRectToReposition;

    for (positionRectToReposition = this->size();
         positionRectToReposition >= bestPlaceInFirstSequence && itRectToReposition != this->rend();
         --positionRectToReposition) {
        itRectToReposition->rectangleTemporaryBestLeftAbscissa =
            itRectToReposition->rectangleTemporaryLeftAbscissa;
        itRectToReposition->rectangleTemporaryBestLowOrdinate =
            itRectToReposition->rectangleTemporaryLowOrdinate;
        ++itRectToReposition;
    }
}
