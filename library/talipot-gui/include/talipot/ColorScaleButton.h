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

#ifndef TALIPOT_COLOR_SCALE_BUTTON_H
#define TALIPOT_COLOR_SCALE_BUTTON_H

#include <QPushButton>

#include <talipot/ColorScale.h>

namespace tlp {

class ColorScaleConfigDialog;

class TLP_QT_SCOPE ColorScaleButton : public QPushButton {
    Q_OBJECT
    ColorScale _colorScale;
    ColorScaleConfigDialog *dlg;

  public:
    static void paintScale(QPainter *, const QRect &, const ColorScale &);

    ColorScaleButton(ColorScale colorScale = ColorScale(), QWidget *parent = nullptr);

    const ColorScale &colorScale() const;
    void setColorScale(const ColorScale &) const;
    void paintEvent(QPaintEvent *event) override;
    void editColorScale(const ColorScale &colorScale);

  public slots:
    void editColorScale();
};
}
#endif // TALIPOT_COLOR_SCALE_BUTTON_H
