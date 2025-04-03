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

#include "PluginErrorReport.h"

#include <QFileInfo>

PluginErrorReport::PluginErrorReport(const QString &fileName, const QString &errorMsg,
                                     QWidget *parent)
    : QLabel(parent) {
    setStyleSheet("#PluginErrorReportData { background-color: white; }");

    QFileInfo fileInfo(fileName);
    setText("<html><head/><body><p><b>" + fileInfo.fileName() +
            "</b></p><p><span style=\"font-size:small;\">" + errorMsg +
            "</span></p></body></head>");
}

void PluginErrorReport::focusInEvent(QFocusEvent *) {
    setStyleSheet(
        "#PluginErrorReportData { background-color: rgb(232, 238, 244); border: 1px solid "
        "#C9C9C9; }");
    repaint();
}

void PluginErrorReport::focusOutEvent(QFocusEvent *) {
    setStyleSheet("#PluginErrorReportData { background-color: white; }");
    repaint();
}
