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

#ifndef SOM_VIEW_INTERACTOR_H
#define SOM_VIEW_INTERACTOR_H

#include <talipot/NodeLinkDiagramViewInteractor.h>
#include "../../utils/PluginNames.h"

namespace tlp {

class SOMViewInteractor : public tlp::NodeLinkDiagramViewInteractor {
  public:
    SOMViewInteractor(const QIcon &icon, const QString &text, const uint priority = 0);

    bool isCompatible(const std::string &viewName) const override;
};

class SOMViewNavigation : public SOMViewInteractor {

    PLUGININFORMATION(InteractorName::SOMViewNavigation, "Dubois Jonathan", "02/04/2009",
                      "Standard navigation with scale", "1.0", "Navigation")

  public:
    SOMViewNavigation(tlp::PluginContext *);

    void construct() override;
};

class SOMViewSelection : public SOMViewInteractor {

    PLUGININFORMATION("SOMViewSelection", "Dubois Jonathan", "02/04/2009",
                      "Standard selection with scale", "1.0", "Selection")

  public:
    SOMViewSelection(tlp::PluginContext *);

    void construct() override;
};

class SOMViewProperties : public SOMViewInteractor {

    PLUGININFORMATION("SOMViewProperties", "Dubois Jonathan", "02/04/2009",
                      "Standard show element info with scale", "1.0", "Information")

  public:
    SOMViewProperties(tlp::PluginContext *);

    void construct() override;
};

class SOMViewThreshold : public SOMViewInteractor {

    PLUGININFORMATION("SOMViewThreshold", "Dubois Jonathan", "02/04/2009",
                      "Threshold selection on SOM", "1.0", "Information")

  public:
    SOMViewThreshold(tlp::PluginContext *);

    void construct() override;
};
}
#endif // SOM_VIEW_INTERACTOR_H
