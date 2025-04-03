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

#ifndef TALIPOT_SCENE_LAYERS_CONFIG_WIDGET_H
#define TALIPOT_SCENE_LAYERS_CONFIG_WIDGET_H

#include <QWidget>

#include <talipot/config.h>

namespace Ui {
class SceneLayersConfigWidget;
}

namespace tlp {

class GlWidget;

class TLP_QT_SCOPE SceneLayersConfigWidget : public QWidget {
    Q_OBJECT

    Ui::SceneLayersConfigWidget *_ui;
    tlp::GlWidget *_glWidget;

  public:
    explicit SceneLayersConfigWidget(QWidget *parent = nullptr);
    ~SceneLayersConfigWidget() override;

    void setGlWidget(tlp::GlWidget *glWidget);

  public slots:

    void resizeFirstColumn();

  signals:
    void drawNeeded();
};
}

#endif // TALIPOT_SCENE_LAYERS_CONFIG_WIDGET_H
