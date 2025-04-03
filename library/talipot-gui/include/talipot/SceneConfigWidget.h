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

#ifndef TALIPOT_SCENE_CONFIG_WIDGET_H
#define TALIPOT_SCENE_CONFIG_WIDGET_H

#include <QWidget>

#include <talipot/config.h>

namespace Ui {
class SceneConfigWidget;
}

namespace tlp {
class GlWidget;

class TLP_QT_SCOPE SceneConfigWidget : public QWidget {
    Q_OBJECT

    Ui::SceneConfigWidget *_ui;
    tlp::GlWidget *_glWidget;

    bool _resetting;

  public:
    explicit SceneConfigWidget(QWidget *parent = nullptr);
    ~SceneConfigWidget() override;

  signals:
    void settingsApplied();

  public slots:
    void setGlWidget(tlp::GlWidget *);
    void resetChanges();
    void applySettings();
    void dynamicFontRBToggled(bool);

  protected:
    bool eventFilter(QObject *, QEvent *) override;
};
}

#endif // TALIPOT_SCENE_CONFIG_WIDGET_H
