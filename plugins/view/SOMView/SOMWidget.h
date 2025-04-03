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

#ifndef SOM_WIDGET_H
#define SOM_WIDGET_H

#include <QWidget>

namespace Ui {
class SOMWidget;
}

namespace tlp {
class SOMWidget : public QWidget {
    Q_OBJECT

    Ui::SOMWidget *_ui;

  public:
    SOMWidget(QWidget *parent = nullptr);
    ~SOMWidget() override;
};
}

#endif // SOM_WIDGET_H
