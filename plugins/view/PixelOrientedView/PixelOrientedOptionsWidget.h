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

#ifndef PIXEL_ORIENTED_OPTIONS_WIDGET_H
#define PIXEL_ORIENTED_OPTIONS_WIDGET_H

#include <QWidget>

#include <talipot/Color.h>

namespace Ui {
class PixelOrientedOptionsWidget;
}

namespace tlp {

class PixelOrientedOptionsWidget : public QWidget {

    Q_OBJECT

    Ui::PixelOrientedOptionsWidget *_ui;

  public:
    PixelOrientedOptionsWidget(QWidget *parent = nullptr);
    ~PixelOrientedOptionsWidget() override;

    Color getBackgroundColor() const;
    void setBackgroundColor(const Color &color);

    std::string getLayoutType() const;
    void setLayoutType(const std::string &layoutType);

    bool configurationChanged();

  private:
    bool oldValuesInitialized;
    Color oldBackgroundColor;
    std::string oldLayoutType;
};
}
#endif // PIXEL_ORIENTED_OPTIONS_WIDGET_H
