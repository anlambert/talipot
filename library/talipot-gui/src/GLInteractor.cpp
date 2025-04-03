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

#include "talipot/GLInteractor.h"

using namespace tlp;

GLInteractorComposite::GLInteractorComposite(const QIcon &icon, const QString &text)
    : InteractorComposite(icon, text) {}

void GLInteractorComposite::draw(tlp::GlWidget *w) {
    for (auto *i : _components) {
        auto *cmp = dynamic_cast<GLInteractorComponent *>(i);

        if (cmp != nullptr) {
            cmp->draw(w);
        }
    }
}

void GLInteractorComposite::compute(tlp::GlWidget *w) {
    for (auto *i : _components) {
        auto *cmp = dynamic_cast<GLInteractorComponent *>(i);

        if (cmp != nullptr) {
            cmp->compute(w);
        }
    }
}
