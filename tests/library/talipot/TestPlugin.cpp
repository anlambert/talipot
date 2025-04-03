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

class Test : public tlp::BooleanAlgorithm {
  public:
    PLUGININFORMATION("Test", "Jezequel", "03/11/2004", "0", "1.0", "")
    Test(tlp::PluginContext *context) : tlp::BooleanAlgorithm(context) {
        addDependency("Test", "1.0");
        addInParameter<int>("testParameter", "no help for you :)", "0", false);
    }
    ~Test() override = default;
    bool run() override {
        std::string name = "Test";
        std::string err = "Error";
        return graph->applyPropertyAlgorithm(name, result, err);
    }
};
PLUGIN(Test)

// this plugin is registered with the same identifier as the one above, it should not show up
class Test3 : public tlp::BooleanAlgorithm {
  public:
    PLUGININFORMATION("Test", "Jezequel", "03/11/2004", "0", "1.0", "")
    Test3(tlp::PluginContext *context) : tlp::BooleanAlgorithm(context) {}
    bool run() override {
        return false;
    }
};
PLUGIN(Test3)
