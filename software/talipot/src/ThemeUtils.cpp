/**
 *
 * Copyright (C) 2020-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/PythonCodeEditor.h>

#include "AlgorithmRunner.h"
#include "TalipotLogger.h"

#include <QApplication>
#include <QFile>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QMainWindow>

using namespace tlp;

static QString currentGuiTheme;

static QPalette darkPalette() {
  QPalette palette;
  palette.setColor(QPalette::Active, QPalette::AlternateBase, "#303030");
  palette.setColor(QPalette::Active, QPalette::Base, "#201f1f");
  palette.setColor(QPalette::Active, QPalette::BrightText, "#ffffff");
  palette.setColor(QPalette::Active, QPalette::Button, "#403f3e");
  palette.setColor(QPalette::Active, QPalette::ButtonText, "#e8e6e3");
  palette.setColor(QPalette::Active, QPalette::Dark, "#1c1b1b");
  palette.setColor(QPalette::Active, QPalette::Highlight, "#184880");
  palette.setColor(QPalette::Active, QPalette::HighlightedText, "#ffffff");
  palette.setColor(QPalette::Active, QPalette::Light, "#484646");
  palette.setColor(QPalette::Active, QPalette::Link, "#508ed8");
  palette.setColor(QPalette::Active, QPalette::LinkVisited, "#8e79a5");
  palette.setColor(QPalette::Active, QPalette::Mid, "#2a2929");
  palette.setColor(QPalette::Active, QPalette::Midlight, "#3d3c3c");
  palette.setColor(QPalette::Active, QPalette::Shadow, "#141414");
  palette.setColor(QPalette::Active, QPalette::Text, "#d4d2cf");
  palette.setColor(QPalette::Active, QPalette::ToolTipBase, "#232627");
  palette.setColor(QPalette::Active, QPalette::ToolTipText, "#c4d1e0");
  palette.setColor(QPalette::Active, QPalette::Window, "#302f2f");
  palette.setColor(QPalette::Active, QPalette::WindowText, "#e0dedb");
  palette.setColor(QPalette::Disabled, QPalette::AlternateBase, "#1f1e1e");
  palette.setColor(QPalette::Disabled, QPalette::Base, "#1c1b1b");
  palette.setColor(QPalette::Disabled, QPalette::BrightText, "#ffffff");
  palette.setColor(QPalette::Disabled, QPalette::Button, "#383736");
  palette.setColor(QPalette::Disabled, QPalette::ButtonText, "#6c6a69");
  palette.setColor(QPalette::Disabled, QPalette::Dark, "#181818");
  palette.setColor(QPalette::Disabled, QPalette::Highlight, "#2a2929");
  palette.setColor(QPalette::Disabled, QPalette::HighlightedText, "#605f5e");
  palette.setColor(QPalette::Disabled, QPalette::Light, "#434242");
  palette.setColor(QPalette::Disabled, QPalette::Link, "#2a3d54");
  palette.setColor(QPalette::Disabled, QPalette::LinkVisited, "#3e3744");
  palette.setColor(QPalette::Disabled, QPalette::Mid, "#252424");
  palette.setColor(QPalette::Disabled, QPalette::Midlight, "#383737");
  palette.setColor(QPalette::Disabled, QPalette::Shadow, "#121111");
  palette.setColor(QPalette::Disabled, QPalette::Text, "#535251");
  palette.setColor(QPalette::Disabled, QPalette::ToolTipBase, "#232627");
  palette.setColor(QPalette::Disabled, QPalette::ToolTipText, "#c4d1e0");
  palette.setColor(QPalette::Disabled, QPalette::Window, "#2a2929");
  palette.setColor(QPalette::Disabled, QPalette::WindowText, "#605f5e");
  palette.setColor(QPalette::Inactive, QPalette::AlternateBase, "#303030");
  palette.setColor(QPalette::Inactive, QPalette::Base, "#201f1f");
  palette.setColor(QPalette::Inactive, QPalette::BrightText, "#ffffff");
  palette.setColor(QPalette::Inactive, QPalette::Button, "#403f3e");
  palette.setColor(QPalette::Inactive, QPalette::ButtonText, "#e8e6e3");
  palette.setColor(QPalette::Inactive, QPalette::Dark, "#1c1b1b");
  palette.setColor(QPalette::Inactive, QPalette::Highlight, "#19395f");
  palette.setColor(QPalette::Inactive, QPalette::HighlightedText, "#e0dedb");
  palette.setColor(QPalette::Inactive, QPalette::Light, "#484646");
  palette.setColor(QPalette::Inactive, QPalette::Link, "#508ed8");
  palette.setColor(QPalette::Inactive, QPalette::LinkVisited, "#8e79a5");
  palette.setColor(QPalette::Inactive, QPalette::Mid, "#2a2929");
  palette.setColor(QPalette::Inactive, QPalette::Midlight, "#3d3c3c");
  palette.setColor(QPalette::Inactive, QPalette::Shadow, "#141414");
  palette.setColor(QPalette::Inactive, QPalette::Text, "#d4d2cf");
  palette.setColor(QPalette::Inactive, QPalette::ToolTipBase, "#232627");
  palette.setColor(QPalette::Inactive, QPalette::ToolTipText, "#c4d1e0");
  palette.setColor(QPalette::Inactive, QPalette::Window, "#302f2f");
  palette.setColor(QPalette::Inactive, QPalette::WindowText, "#e0dedb");
  palette.setColor(QPalette::Normal, QPalette::AlternateBase, "#303030");
  palette.setColor(QPalette::Normal, QPalette::Base, "#201f1f");
  palette.setColor(QPalette::Normal, QPalette::BrightText, "#ffffff");
  palette.setColor(QPalette::Normal, QPalette::Button, "#403f3e");
  palette.setColor(QPalette::Normal, QPalette::ButtonText, "#e8e6e3");
  palette.setColor(QPalette::Normal, QPalette::Dark, "#1c1b1b");
  palette.setColor(QPalette::Normal, QPalette::Highlight, "#184880");
  palette.setColor(QPalette::Normal, QPalette::HighlightedText, "#ffffff");
  palette.setColor(QPalette::Normal, QPalette::Light, "#484646");
  palette.setColor(QPalette::Normal, QPalette::Link, "#508ed8");
  palette.setColor(QPalette::Normal, QPalette::LinkVisited, "#8e79a5");
  palette.setColor(QPalette::Normal, QPalette::Mid, "#2a2929");
  palette.setColor(QPalette::Normal, QPalette::Midlight, "#3d3c3c");
  palette.setColor(QPalette::Normal, QPalette::Shadow, "#141414");
  palette.setColor(QPalette::Normal, QPalette::Text, "#d4d2cf");
  palette.setColor(QPalette::Normal, QPalette::ToolTipBase, "#232627");
  palette.setColor(QPalette::Normal, QPalette::ToolTipText, "#c4d1e0");
  palette.setColor(QPalette::Normal, QPalette::Window, "#302f2f");
  palette.setColor(QPalette::Normal, QPalette::WindowText, "#e0dedb");
  return palette;
}

static QPalette lightPalette() {
  QPalette palette;
  palette.setColor(QPalette::Active, QPalette::AlternateBase, "#f8f7f6");
  palette.setColor(QPalette::Active, QPalette::Base, "#fcfcfc");
  palette.setColor(QPalette::Active, QPalette::BrightText, "#ffffff");
  palette.setColor(QPalette::Active, QPalette::Button, "#eff0f1");
  palette.setColor(QPalette::Active, QPalette::ButtonText, "#31363b");
  palette.setColor(QPalette::Active, QPalette::Dark, "#888e93");
  palette.setColor(QPalette::Active, QPalette::Highlight, "#3daee9");
  palette.setColor(QPalette::Active, QPalette::HighlightedText, "#fcfcfc");
  palette.setColor(QPalette::Active, QPalette::Light, "#ffffff");
  palette.setColor(QPalette::Active, QPalette::Link, "#0057ae");
  palette.setColor(QPalette::Active, QPalette::LinkVisited, "#452886");
  palette.setColor(QPalette::Active, QPalette::Mid, "#c4c9cd");
  palette.setColor(QPalette::Active, QPalette::Midlight, "#f7f7f8");
  palette.setColor(QPalette::Active, QPalette::Shadow, "#474a4c");
  palette.setColor(QPalette::Active, QPalette::Text, "#31363b");
  palette.setColor(QPalette::Active, QPalette::ToolTipBase, "#fcfcfc");
  palette.setColor(QPalette::Active, QPalette::ToolTipText, "#31363b");
  palette.setColor(QPalette::Active, QPalette::Window, "#eff0f1");
  palette.setColor(QPalette::Active, QPalette::WindowText, "#31363b");
  palette.setColor(QPalette::Disabled, QPalette::AlternateBase, "#eeece9");
  palette.setColor(QPalette::Disabled, QPalette::Base, "#f1f1f1");
  palette.setColor(QPalette::Disabled, QPalette::BrightText, "#ffffff");
  palette.setColor(QPalette::Disabled, QPalette::Button, "#e3e5e7");
  palette.setColor(QPalette::Disabled, QPalette::ButtonText, "#a5a7a9");
  palette.setColor(QPalette::Disabled, QPalette::Dark, "#82878c");
  palette.setColor(QPalette::Disabled, QPalette::Highlight, "#e3e5e7");
  palette.setColor(QPalette::Disabled, QPalette::HighlightedText, "#a5a7a9");
  palette.setColor(QPalette::Disabled, QPalette::Light, "#ffffff");
  palette.setColor(QPalette::Disabled, QPalette::Link, "#95badd");
  palette.setColor(QPalette::Disabled, QPalette::LinkVisited, "#b4a9ce");
  palette.setColor(QPalette::Disabled, QPalette::Mid, "#bcc0c5");
  palette.setColor(QPalette::Disabled, QPalette::Midlight, "#ecedee");
  palette.setColor(QPalette::Disabled, QPalette::Shadow, "#47494c");
  palette.setColor(QPalette::Disabled, QPalette::Text, "#acaeb0");
  palette.setColor(QPalette::Disabled, QPalette::ToolTipBase, "#fcfcfc");
  palette.setColor(QPalette::Disabled, QPalette::ToolTipText, "#31363b");
  palette.setColor(QPalette::Disabled, QPalette::Window, "#e3e5e7");
  palette.setColor(QPalette::Disabled, QPalette::WindowText, "#a5a7a9");
  palette.setColor(QPalette::Inactive, QPalette::AlternateBase, "#f8f7f6");
  palette.setColor(QPalette::Inactive, QPalette::Base, "#fcfcfc");
  palette.setColor(QPalette::Inactive, QPalette::BrightText, "#ffffff");
  palette.setColor(QPalette::Inactive, QPalette::Button, "#eff0f1");
  palette.setColor(QPalette::Inactive, QPalette::ButtonText, "#31363b");
  palette.setColor(QPalette::Inactive, QPalette::Dark, "#888e93");
  palette.setColor(QPalette::Inactive, QPalette::Highlight, "#c2e0f5");
  palette.setColor(QPalette::Inactive, QPalette::HighlightedText, "#31363b");
  palette.setColor(QPalette::Inactive, QPalette::Light, "#ffffff");
  palette.setColor(QPalette::Inactive, QPalette::Link, "#0057ae");
  palette.setColor(QPalette::Inactive, QPalette::LinkVisited, "#452886");
  palette.setColor(QPalette::Inactive, QPalette::Mid, "#c4c9cd");
  palette.setColor(QPalette::Inactive, QPalette::Midlight, "#f7f7f8");
  palette.setColor(QPalette::Inactive, QPalette::Shadow, "#474a4c");
  palette.setColor(QPalette::Inactive, QPalette::Text, "#31363b");
  palette.setColor(QPalette::Inactive, QPalette::ToolTipBase, "#fcfcfc");
  palette.setColor(QPalette::Inactive, QPalette::ToolTipText, "#31363b");
  palette.setColor(QPalette::Inactive, QPalette::Window, "#eff0f1");
  palette.setColor(QPalette::Inactive, QPalette::WindowText, "#31363b");
  palette.setColor(QPalette::Normal, QPalette::AlternateBase, "#f8f7f6");

  palette.setColor(QPalette::Normal, QPalette::Base, "#fcfcfc");
  palette.setColor(QPalette::Normal, QPalette::BrightText, "#ffffff");
  palette.setColor(QPalette::Normal, QPalette::Button, "#eff0f1");
  palette.setColor(QPalette::Normal, QPalette::ButtonText, "#31363b");
  palette.setColor(QPalette::Normal, QPalette::Dark, "#888e93");
  palette.setColor(QPalette::Normal, QPalette::Highlight, "#3daee9");
  palette.setColor(QPalette::Normal, QPalette::HighlightedText, "#fcfcfc");
  palette.setColor(QPalette::Normal, QPalette::Light, "#ffffff");
  palette.setColor(QPalette::Normal, QPalette::Link, "#0057ae");
  palette.setColor(QPalette::Normal, QPalette::LinkVisited, "#452886");
  palette.setColor(QPalette::Normal, QPalette::Mid, "#c4c9cd");
  palette.setColor(QPalette::Normal, QPalette::Midlight, "#f7f7f8");
  palette.setColor(QPalette::Normal, QPalette::Shadow, "#474a4c");
  palette.setColor(QPalette::Normal, QPalette::Text, "#31363b");
  palette.setColor(QPalette::Normal, QPalette::ToolTipBase, "#fcfcfc");
  palette.setColor(QPalette::Normal, QPalette::ToolTipText, "#31363b");
  palette.setColor(QPalette::Normal, QPalette::Window, "#eff0f1");
  palette.setColor(QPalette::Normal, QPalette::WindowText, "#31363b");

  return palette;
}

static QMap<QString, QPalette> initPalettes() {
  QMap<QString, QPalette> palettes;
  palettes["Dark"] = darkPalette();
  palettes["Light"] = lightPalette();
  return palettes;
}

static QMap<QString, QPalette> palettes = initPalettes();

const QPalette &getGuiThemePalette(const QString &guiTheme) {
  return palettes[guiTheme];
}

static void repolishWidgetAndChildren(QWidget *rootWidget) {
  qApp->style()->unpolish(rootWidget);
  qApp->style()->polish(rootWidget);
  for (QWidget *w : rootWidget->findChildren<QWidget *>()) {
    qApp->style()->unpolish(w);
    qApp->style()->polish(w);
  }
}

void setApplicationGuiTheme(const QString &guiTheme, bool updateGui) {

  if (currentGuiTheme == guiTheme) {
    return;
  }

  currentGuiTheme = guiTheme;

  qApp->setPalette(getGuiThemePalette(guiTheme));

  QFile talipotQssFile(":/talipot/app/style/talipot.qss");
  if (talipotQssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString talipotQss = talipotQssFile.readAll();
    talipotQss = talipotQss.arg(backgroundColor().name(), alternateBackgroundColor().name(),
                                textColor().name());
    if (applicationHasDarkGuiTheme()) {
      QFile talipotDarkQssFile(":/talipot/app/style/talipotDark.qss");
      if (talipotDarkQssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        talipotQss += talipotDarkQssFile.readAll();
        talipotDarkQssFile.close();
      }
    }
    QString styleName = qApp->style()->objectName();
    qApp->setStyleSheet(talipotQss);
    qApp->style()->setObjectName(styleName);
    talipotQssFile.close();
  }

  if (updateGui) {
    // repolish all widgets
    for (QWidget *topLevelWidget : qApp->topLevelWidgets()) {
      repolishWidgetAndChildren(topLevelWidget);
    }
    // QWidget embedded into a GraphicsProxyWidget are not reachable from
    // the QMainWindow
    for (QGraphicsView *gv : getMainWindow()->findChildren<QGraphicsView *>()) {
      QGraphicsScene *scene = gv->scene();
      if (!scene) {
        continue;
      }
      for (QGraphicsItem *item : scene->items()) {
        QGraphicsProxyWidget *w = nullptr;
        if ((w = qgraphicsitem_cast<QGraphicsProxyWidget *>(item))) {
          repolishWidgetAndChildren(w->widget());
        }
      }
    }
    // extra polish operations for special widgets
    getMainWindow()->findChild<AlgorithmRunner *>()->buildPluginsList();
    for (QWidget *topLevelWidget : qApp->topLevelWidgets()) {
      auto *logger = getMainWindow()->findChild<TalipotLogger *>();
      if (logger) {
        logger->updateLogItemsBackgroundColor();
      }
      for (PythonCodeEditor *pce : topLevelWidget->findChildren<PythonCodeEditor *>()) {
        pce->guiThemeChanged();
      }
    }
  }
}
