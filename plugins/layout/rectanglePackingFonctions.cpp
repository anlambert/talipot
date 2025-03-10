/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "rectanglePackingFonctions.h"

using namespace std;
using namespace tlp;

bool RectanglePackingLimitRectangles(vector<Rectangle<float>> &v, const char *quality,
                                     PluginProgress *progress) {

  int numberOfPackedRectangles;
  vector<Rectangle<float>>::iterator itNewRect;
  int numberNewRect;

  RectanglePacking rectPack(v.size());

  /* we calculate the number of rectangles which are going to be placed in an optimal way*/
  numberOfPackedRectangles = rectPack.calculOfNumberOptimalRepositionnedRectangles(quality);

  /* we retrieve a pointer on the last rectangle which will be placed in an optimal way */
  itNewRect = v.begin();

  /* we go over all the rectangles to place in an optimal way */
  for (numberNewRect = 1; numberNewRect <= numberOfPackedRectangles; ++numberNewRect) {

    /* we calculate the coordinates of the new rectangle and those of the rectangles that the new
     * rectangle eventually displaces, that is to say the rectangles placed to its right or above it
     */
    rectPack.optimalPositionOfNewRectangle(itNewRect);

    ++itNewRect;

    /* to follow the algorithm progression on through the PluginProgress*/
    if (progress && (progress->progress(numberNewRect, numberOfPackedRectangles + 1) !=
                     ProgressState::TLP_CONTINUE)) {
      return false;
    }
  }

  /* we definitively change the coordinates of the rectangles which have been placed in an optimal
   * way */
  (rectPack.firstSequence)->allocateCoordinates();

  /* we calculate the coordinates of the rectangles which have not been packed in an optimal way */
  rectPack.defaultPositionRestOfRectangles(itNewRect, v.end());

  /* added to enable the synchronisation of the PluginProgress closing and the end of the
   * algorithm*/
  return progress ? (progress->progress(numberNewRect, numberOfPackedRectangles + 1) !=
                     ProgressState::TLP_CANCEL)
                  : true;
}

bool RectanglePackingLimitPositions(vector<Rectangle<float>> &v, const char *quality,
                                    PluginProgress *progress) {

  /*useful variables for the PluginProgress*/
  int counter = 1;
  int entrySize = v.size();

  uint numberTestedPositions;
  vector<Rectangle<float>>::iterator itNewRect;

  RectanglePacking rectPack(entrySize);

  /* we calculate the number of rectangles which will be placed in an optimal way */
  numberTestedPositions = rectPack.calculNumberOfTestedPositions(quality);

  /* we go over all the rectangles to pack in an optimal way */
  for (itNewRect = v.begin(); itNewRect != v.end(); ++itNewRect) {

    /* we calculate the coordinates of the new rectangle and those of the rectangles that the new
     * rectangle eventually displaces, that is to say the rectangles placed to its right or above it
     */
    rectPack.optimalPositionOfNewRectangleLimPos(itNewRect, numberTestedPositions);

    /* to follow the algorithm progression on through the PluginProgress*/
    if (progress && (progress->progress(counter, entrySize + 1) != ProgressState::TLP_CONTINUE)) {
      return false;
    }

    ++counter;
  }

  /* we definitively change the coordinates of the rectangles which have been placed in an optimal
   * way */
  (rectPack.firstSequence)->allocateCoordinates();

  /* added to enable the synchronisation of the PluginProgress closing and the end of the
   * algorithm*/
  return progress ? (progress->progress(counter, entrySize + 1) != ProgressState::TLP_CANCEL)
                  : true;
}
