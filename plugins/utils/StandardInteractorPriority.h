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

#ifndef STANDARD_INTERACTOR_PRIORITY_H
#define STANDARD_INTERACTOR_PRIORITY_H

namespace tlp {

// List of standard interactors priorities
// just insert a new one at the right place
class StandardInteractorPriority {
  public:
    enum priority {
        None = 0,
        ViewInteractor1,
        ViewInteractor2,
        ViewInteractor3,
        ViewInteractor4,
        ViewInteractor5,
        ViewInteractor6,
        ViewInteractor7,
        MagnifyingGlass,
        Fisheye,
        NeighborhoodHighlighter,
        ZoomOnRectangle,
        DeleteElement,
        EditEdgeBends,
        AddNodesOrEdges,
        PathSelection,
        FreeHandSelection,
        RectangleSelectionModifier,
        RectangleSelection,
        GetInformation,
        Navigation
    };
};
}

#endif // STANDARD_INTERACTOR_PRIORITY_H
