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

#include <talipot/PluginLoaderTxt.h>
#include <talipot/Plugin.h>

using namespace std;
using namespace tlp;
void PluginLoaderTxt::start(const string &path) {
    std::cout << "Start loading plug-ins in " << path << std::endl;
}

void PluginLoaderTxt::loading(const string &filename) {
    std::cout << "loading file: " << filename << endl;
}

void PluginLoaderTxt::loaded(const Plugin *info, const std::list<Dependency> &deps) {
    std::cout << "Plug-in " << info->name() << " loaded, Author: " << info->author()
              << ", Date: " << info->date() << ", Release: " << info->release()
              << ", Talipot Version: " << info->talipotRelease() << endl;

    // output dependencies if any
    if (deps.size()) {
        uint i = deps.size() - 1;
        std::cout << "depending on ";

        for (const auto &d : deps) {
            std::cout << d.pluginName << (--i ? ", " : "");
        }
        std::cout << endl;
    }
}

void PluginLoaderTxt::aborted(const string &filename, const string &erreurmsg) {
    std::cout << "Aborted loading of " << filename << " Error:" << erreurmsg << endl;
}

void PluginLoaderTxt::finished(bool state, const string &msg) {
    if (state) {
        std::cout << "Loading complete" << endl;
    } else {
        std::cout << "Loading error " << msg << endl;
    }
}
