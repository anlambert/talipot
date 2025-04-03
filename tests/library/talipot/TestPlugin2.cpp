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

#include <talipot/PluginHeaders.h>

using namespace std;

class Test2 : public tlp::BooleanAlgorithm {
  public:
    PLUGININFORMATION("Test2", "Jezequel", "03/11/2004", "0", "1.0", "")
    Test2(tlp::PluginContext *context) : tlp::BooleanAlgorithm(context) {}
    ~Test2() override = default;
    bool run() override {
        return true;
    }
};
PLUGIN(Test2)
