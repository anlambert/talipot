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

#ifndef PIXEL_ORIENTED_INTERACTORS_H
#define PIXEL_ORIENTED_INTERACTORS_H

#include <talipot/NodeLinkDiagramViewInteractor.h>
#include "../../utils/PluginNames.h"

namespace tlp {

class PixelOrientedInteractor : public NodeLinkDiagramViewInteractor {

  public:
    PixelOrientedInteractor(const QIcon &icon, const QString &text, const uint priority = 0);

    bool isCompatible(const std::string &viewName) const override;
};

class PixelOrientedInteractorNavigation : public PixelOrientedInteractor {

  public:
    PLUGININFORMATION(InteractorName::PixelOrientedInteractorNavigation, "Tulip Team", "02/04/2009",
                      "Pixel Oriented Navigation Interactor", "1.0", "Navigation")

    PixelOrientedInteractorNavigation(const tlp::PluginContext *);

    void construct() override;
};
}

#endif // PIXEL_ORIENTED_INTERACTORS_H
