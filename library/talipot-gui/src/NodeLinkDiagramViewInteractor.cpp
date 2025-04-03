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

#include <talipot/NodeLinkDiagramViewInteractor.h>

#include <QLabel>

using namespace tlp;

NodeLinkDiagramViewInteractor::NodeLinkDiagramViewInteractor(const QIcon &icon, const QString &text,
                                                             uint priority)
    : GLInteractorComposite(icon, text), _label(new QLabel), _priority(priority) {
    _label->setWordWrap(true);
    _label->setAlignment(Qt::AlignTop);
    _label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

NodeLinkDiagramViewInteractor::~NodeLinkDiagramViewInteractor() {
    delete _label;
}

void NodeLinkDiagramViewInteractor::setConfigurationWidgetText(const QString &text) {
    _label->setText(text);
}

QLabel *NodeLinkDiagramViewInteractor::docWidget() const {
    return _label->text().isEmpty() ? nullptr : _label;
}

uint NodeLinkDiagramViewInteractor::priority() const {
    return _priority;
}
