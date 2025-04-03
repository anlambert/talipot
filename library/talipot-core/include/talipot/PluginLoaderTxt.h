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

#ifndef TALIPOT_PLUGIN_LOADER_TXT_H
#define TALIPOT_PLUGIN_LOADER_TXT_H

#include <talipot/PluginLoader.h>

namespace tlp {

/**
 * @ingroup Plugins
 * @brief A simple implementation of tlp::PluginLoader to display load state into the console
 */
struct TLP_SCOPE PluginLoaderTxt : public PluginLoader {
    void start(const std::string &path) override;
    void loading(const std::string &filename) override;
    void loaded(const Plugin *info, const std::list<Dependency> &) override;
    void aborted(const std::string &filename, const std::string &erreurmsg) override;
    void finished(bool state, const std::string &msg) override;
};
}
#endif // TALIPOT_PLUGIN_LOADER_TXT_H
