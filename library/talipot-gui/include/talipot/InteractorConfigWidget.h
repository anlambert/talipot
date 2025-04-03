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

#ifndef TALIPOT_INTERACTOR_CONFIG_WIDGET_H
#define TALIPOT_INTERACTOR_CONFIG_WIDGET_H

#include <QDialog>

namespace Ui {
class InteractorConfigWidget;
}

class QShowEvent;

namespace tlp {
class Interactor;

class InteractorConfigWidget : public QWidget {

    Ui::InteractorConfigWidget *_ui;
    Interactor *_interactor;

  public:
    explicit InteractorConfigWidget(QWidget *parent = nullptr);
    ~InteractorConfigWidget();
    bool setWidgets(Interactor *interactor);
    void clearWidgets();
};

}

#endif // TALIPOT_INTERACTOR_CONFIG_WIDGET_H
