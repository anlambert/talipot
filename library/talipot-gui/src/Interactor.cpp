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

#include <talipot/Interactor.h>
#include <talipot/View.h>

using namespace std;
using namespace tlp;

QMap<std::string, QList<std::string>> InteractorLister::_compatibilityMap =
    QMap<std::string, QList<std::string>>();

bool interactorLessThan(Interactor *a, Interactor *b) {
    return a->priority() > b->priority();
}

void InteractorLister::initInteractorsDependencies() {
    _compatibilityMap.clear();

    QMap<Interactor *, string> interactorToName;

    std::list<std::string> interactors(PluginsManager::availablePlugins<Interactor>());

    for (const std::string &interactorName : interactors) {
        interactorToName[PluginsManager::getPluginObject<Interactor>(interactorName, nullptr)] =
            interactorName;
    }

    std::list<std::string> views(PluginsManager::availablePlugins<View>());

    for (const std::string &viewName : views) {
        QList<Interactor *> compatibleInteractors;

        for (auto *i : interactorToName.keys()) {
            if (i->isCompatible(viewName)) {
                compatibleInteractors << i;
            }
        }

        std::sort(compatibleInteractors.begin(), compatibleInteractors.end(), interactorLessThan);

        QList<string> compatibleNames;

        for (auto *i : compatibleInteractors) {
            compatibleNames << interactorToName[i];
        }

        _compatibilityMap[viewName] = compatibleNames;
    }

    for (auto *i : interactorToName.keys()) {
        delete i;
    }
}

QList<string> InteractorLister::compatibleInteractors(const std::string &viewName) {
    return _compatibilityMap[viewName];
}
