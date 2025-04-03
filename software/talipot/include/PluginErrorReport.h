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

#ifndef PLUGIN_ERROR_REPORT_H
#define PLUGIN_ERROR_REPORT_H

#include <QLabel>

/**
  @brief This widget provides with maintenance actions related to a plugin load error.
  When a plugin crash at load time, the user shall have the following choices:
  @list
  @li Uninstall the plugin (removes the file permanently)
  @li Reinstall it from a distant repository
  @list
  */
class PluginErrorReport : public QLabel {
  public:
    explicit PluginErrorReport(const QString &fileName, const QString &errorMsg,
                               QWidget *parent = nullptr);

  protected:
    void focusInEvent(QFocusEvent *) override;
    void focusOutEvent(QFocusEvent *) override;
};
#endif // PLUGIN_ERROR_REPORT_H
