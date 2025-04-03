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

#include "ParallelCoordinatesView.h"
#include "ParallelCoordsElementDeleter.h"

#include <QMouseEvent>

using namespace std;

namespace tlp {

bool ParallelCoordsElementDeleter::eventFilter(QObject *, QEvent *e) {

    if (e->type() == QEvent::MouseButtonPress) {
        auto *me = static_cast<QMouseEvent *>(e);

        if (me->buttons() == Qt::LeftButton) {
            auto *parallelView = static_cast<ParallelCoordinatesView *>(view());
            Observable::holdObservers();
            parallelView->deleteDataUnderPointer(me->pos().x(), me->pos().y());
            Observable::unholdObservers();
            return true;
        }
    }

    return false;
}
}
