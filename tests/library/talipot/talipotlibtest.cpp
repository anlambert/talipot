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

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <talipot/TlpTools.h>

#include <CrashHandler.h>

using namespace CppUnit;
using namespace std;
using namespace tlp;

int main() {

    CrashHandler::install();

    initTalipotLib();

    TextUi::TestRunner runner;

    runner.addTest(TestFactoryRegistry::getRegistry().makeTest());
    runner.run();

    return runner.result().wasSuccessful() ? EXIT_SUCCESS : EXIT_FAILURE;
}
