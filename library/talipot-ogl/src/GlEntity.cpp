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

#include <talipot/GlSceneVisitor.h>

#include <talipot/GlComposite.h>

#include <algorithm>

using namespace std;

namespace tlp {
GlEntity::~GlEntity() {
    for (auto *parent : parents) {
        parent->deleteGlEntity(this, false);
    }
}

void GlEntity::addParent(GlComposite *composite) {
    parents.push_back(composite);
}

void GlEntity::setVisible(bool visible) {
    if (this->visible == visible) {
        return;
    }

    this->visible = visible;

    for (auto *parent : parents) {
        parent->notifyModified(this);
    }
}

void GlEntity::removeParent(GlComposite *composite) {
    auto it = find(parents.begin(), parents.end(), composite);

    if (it != parents.end()) {
        parents.erase(it);
    }
}

void GlEntity::acceptVisitor(GlSceneVisitor *visitor) {
    visitor->visit(this);
}

}
