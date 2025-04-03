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

#include "talipot/InteractorComposite.h"

#include <QAction>

using namespace tlp;

void InteractorComponent::setView(tlp::View *view) {
    _view = view;
    viewChanged(view);
}
tlp::View *InteractorComponent::view() const {
    return _view;
}
void InteractorComponent::init() {}
bool InteractorComponent::eventFilter(QObject *, QEvent *) {
    return false;
}
// *******************************

InteractorComposite::InteractorComposite(const QIcon &icon, const QString &text)
    : Interactor(), _action(new QAction(icon, text, this)), _view(nullptr), _lastTarget(nullptr) {}

InteractorComposite::~InteractorComposite() {
    qDeleteAll(_components);
}

QCursor InteractorComposite::cursor() const {
    return QCursor();
}

tlp::View *InteractorComposite::view() const {
    return _view;
}

void InteractorComposite::undoIsDone() {}

void InteractorComposite::setLastTarget(QObject *target) {
    _lastTarget = target;

    if (_lastTarget) {
        connect(_lastTarget, &QObject::destroyed, this, &InteractorComposite::lastTargetDestroyed);
    }
}

void InteractorComposite::lastTargetDestroyed() {
    if (sender() == lastTarget()) {
        _lastTarget = nullptr;
    }
}

QObject *InteractorComposite::lastTarget() const {
    return _lastTarget;
}

void InteractorComposite::setView(tlp::View *view) {
    _view = view;
    // no need to call construct when view is nullptr (called with a nullptr from View::~View())
    if (view != nullptr) {
        construct();
    }

    for (auto *i : _components) {
        i->setView(view);
    }
}

InteractorComposite::iterator InteractorComposite::begin() {
    return _components.begin();
}
InteractorComposite::iterator InteractorComposite::end() {
    return _components.end();
}
InteractorComposite::const_iterator InteractorComposite::begin() const {
    return _components.begin();
}
InteractorComposite::const_iterator InteractorComposite::end() const {
    return _components.end();
}
void InteractorComposite::push_back(InteractorComponent *i) {
    _components.push_back(i);
}
void InteractorComposite::push_front(InteractorComponent *i) {
    _components.push_front(i);
}
void InteractorComposite::install(QObject *target) {
    setLastTarget(target);

    if (target != nullptr) {
        for (auto *i : _components) {
            target->installEventFilter(i);
            i->init();
        }
    }
}
void InteractorComposite::uninstall() {
    if (lastTarget() != nullptr) {
        for (auto *i : _components) {
            lastTarget()->removeEventFilter(i);
            i->clear();
        }
    }

    install(nullptr);
}

QAction *InteractorComposite::action() const {
    return _action;
}
