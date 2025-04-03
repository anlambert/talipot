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

#ifndef BENDS_TOOLS_H
#define BENDS_TOOLS_H

#include <talipot/LayoutProperty.h>
#include <talipot/Node.h>
#include <vector>

class BendsTools {
  public:
    static std::vector<tlp::node> bendsSimplification(std::vector<tlp::node> &bends,
                                                      tlp::LayoutProperty *layout);

  private:
    BendsTools();

    static bool straightLine(tlp::LayoutProperty *layout, const tlp::node a, const tlp::node b,
                             const tlp::node c);
    static double cosAlpha(tlp::LayoutProperty *layout, const tlp::node a, const tlp::node b,
                           const tlp::node c);
};

#endif // BENDS_TOOLS_H
