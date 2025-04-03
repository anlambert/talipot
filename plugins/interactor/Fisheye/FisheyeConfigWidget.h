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

#ifndef FISH_EYE_CONFIG_WIDGET_H
#define FISH_EYE_CONFIG_WIDGET_H

#include <QWidget>

namespace Ui {
class FisheyeConfigWidget;
}

namespace tlp {

class FisheyeConfigWidget : public QWidget {

    Ui::FisheyeConfigWidget *_ui;

  public:
    FisheyeConfigWidget(QWidget *parent = nullptr);
    ~FisheyeConfigWidget() override;

    float getFisheyeRadius() const;
    void setFisheyeRadius(const float radius);

    float getFisheyeHeight() const;
    void setFisheyeHeight(const float height);

    float getFisheyeRadiusIncrementStep() const;
    float getFisheyeHeightIncrementStep() const;
};
}
#endif // FISH_EYE_CONFIG_WIDGET_H
