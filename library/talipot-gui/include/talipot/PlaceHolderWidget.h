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

#ifndef TALIPOT_PLACE_HOLDER_WIDGET_H
#define TALIPOT_PLACE_HOLDER_WIDGET_H

#include <QWidget>

class PlaceHolderWidget : public QWidget {
    Q_OBJECT

    QWidget *_widget;

  public:
    explicit PlaceHolderWidget(QWidget *parent = nullptr);
    QWidget *widget() const;
    void setWidget(QWidget *widget);
    void resetWidget();
};

#endif // TALIPOT_PLACE_HOLDER_WIDGET_H
