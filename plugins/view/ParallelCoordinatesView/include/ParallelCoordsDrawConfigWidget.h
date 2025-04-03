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

#ifndef PARALLEL_COORDS_DRAW_CONFIG_WIDGET_H
#define PARALLEL_COORDS_DRAW_CONFIG_WIDGET_H

#include <QWidget>

#include <talipot/Size.h>
#include <talipot/Color.h>

namespace Ui {
class ParallelCoordsDrawConfigWidget;
}

namespace tlp {

class ParallelCoordsDrawConfigWidget : public QWidget {

    Q_OBJECT

  public:
    ParallelCoordsDrawConfigWidget(QWidget *parent = nullptr);
    ~ParallelCoordsDrawConfigWidget() override;

    uint getAxisHeight() const;
    void setAxisHeight(const uint axisHeight);
    bool drawPointOnAxis() const;
    void setDrawPointOnAxis(const bool drawPointOnAxis);
    Size getAxisPointMinSize() const;
    void setAxisPointMinSize(const uint axisPointMinSize);
    Size getAxisPointMaxSize() const;
    void setAxisPointMaxSize(const uint axisPointMaxSize);
    bool displayNodeLabels() const;
    void setLinesColorAlphaValue(uint value);
    uint getLinesColorAlphaValue() const;
    Color getBackgroundColor() const;
    void setBackgroundColor(const Color &color);
    uint getUnhighlightedEltsColorsAlphaValue() const;
    void setUnhighlightedEltsColorsAlphaValue(const uint alphaValue);
    std::string getLinesTextureFilename() const;
    void setLinesTextureFilename(const std::string &linesTextureFileName);
    void setDisplayNodeLabels(const bool set);
    bool configurationChanged();

  private slots:

    void pressButtonBrowse();
    void userTextureRbToggled(const bool checked);
    void minAxisPointSizeValueChanged(const int newValue);
    void maxAxisPointSizeValueChanged(const int newValue);

  private:
    bool oldValuesInitialized;
    uint oldAxisHeight;
    bool oldDrawPointOnAxis;
    Size oldAxisPointMinSize;
    Size oldAxisPointMaxSize;
    bool oldDisplayNodesLabels;
    uint oldLinesColorAlphaValue;
    Color oldBackgroundColor;
    uint oldUnhighlightedEltsColorsAlphaValue;
    std::string oldLinesTextureFilename;
    Ui::ParallelCoordsDrawConfigWidget *_ui;
};
}

#endif // PARALLEL_COORDS_DRAW_CONFIG_WIDGET_H
