/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_TLP_QT_TOOLS_H
#define TALIPOT_TLP_QT_TOOLS_H

#include <QColor>
#include <QDebug>

#include <talipot/Color.h>
#include <talipot/config.h>
#include <talipot/PropertyInterface.h>

class QWidget;
class QString;

namespace tlp {

struct PluginLoader;

TLP_QT_SCOPE bool getColorDialog(const QColor &color, QWidget *parent, const QString &title,
                                 QColor &result);

inline QColor colorToQColor(const Color &color) {
  return QColor(color.getR(), color.getG(), color.getB(), color.getA());
}
inline Color QColorToColor(const QColor &color) {
  return Color(color.red(), color.green(), color.blue(), color.alpha());
}
/**
 * @brief Convert a QString to a Tulip UTF-8 encoded std::string.
 **/
inline std::string QStringToTlpString(const QString &toConvert) {
  return std::string(toConvert.toUtf8());
}
/**
 * @brief Convert a Tulip UTF-8 encoded std::string to a QString
 **/
inline QString tlpStringToQString(const std::string &toConvert) {
  return QString::fromUtf8(toConvert.c_str());
}

/**
 * @brief Case insensitive comparison of two QStrings
 **/
inline bool QStringCaseCmp(const QString &s1, const QString &s2) {
  return QString::localeAwareCompare(s1, s2) < 0;
}

/**
 * @brief Convert the property type string to a label to display in the GUI.
 * The property type label is the string to display in the GUI instead of the basic property type
 *string.
 **/
TLP_QT_SCOPE QString propertyTypeToPropertyTypeLabel(const std::string &typeName);

/**
 * @brief Get the string to display as property type for the given property.
 * The property type label is the string to display in the GUI instead of the property type string.
 * By example for a property of type "double" the label displayed in the GUI will be "Metric".
 **/
inline QString propertyInterfaceToPropertyTypeLabel(const tlp::PropertyInterface *const property) {
  return propertyTypeToPropertyTypeLabel(property->getTypename());
}

/**
 * @brief Convert the label of a property type to it's corresponding property type.
 * The property type label is the string to display in the GUI instead of the property type string.
 * By example for a property of type "double" the label displayed in the GUI will be "Metric".
 **/
TLP_QT_SCOPE std::string propertyTypeLabelToPropertyType(const QString &typeNameLabel);

/**
 * @brief Gets the name of the package to retrieve for this version of talipot.
 * The package name uses the Tulip release, platform (windows, unix, ...), architecture (x86,
 *x86_64), and compiler used (GCC, Clang, MSVC) to determine which package this version can use.
 *
 * @param pluginName The name of the plugin for which we want the package name.
 **/
TLP_QT_SCOPE QString getPluginPackageName(const QString &pluginName);

TLP_QT_SCOPE QString getPluginStagingDirectory();

TLP_QT_SCOPE QString getPluginLocalInstallationDir();

TLP_QT_SCOPE QString localPluginsPath();

/**
 @brief Sets up environment when creating an executable using Tulip libraries
 This method performs basic operations when starting a software using Tulip:
 @list
 @li It initializes the talipot library
 @li it checks plugins to be discarded and uninstalls them
 @li It loads plugins from the application path
 @endlist
 */
extern TLP_QT_SCOPE void initTalipotSoftware(PluginLoader *loader = nullptr,
                                             bool removeDiscardedPlugins = false);

/**
 * @brief redirect tlp::debug() to qDebug()
 */
TLP_QT_SCOPE void redirectDebugOutputToQDebug();

/**
 * @brief redirect tlp::warning() to qWarning()
 */
TLP_QT_SCOPE void redirectWarningOutputToQWarning();

/**
 * @brief redirect tlp::error() to qCritical()
 */
TLP_QT_SCOPE void redirectErrorOutputToQCritical();

TLP_QT_SCOPE void disableQtUserInput();

TLP_QT_SCOPE void enableQtUserInput();
} // namespace tlp

// QDebug extension
inline QDebug operator<<(QDebug dbg, const std::string &s) {
  dbg.nospace() << s.c_str();
  return dbg.space();
}

// useful macros needed for menu actions building
#ifdef __APPLE__
#define SET_TOOLTIP_WITH_CTRL_SHORTCUT(a, tt, sc)                                                  \
  a->setToolTip(QString(tt) + tlpStringToQString(" [⌘+") + sc + "]")
#else
#define SET_TOOLTIP_WITH_CTRL_SHORTCUT(a, tt, sc) a->setToolTip(QString(tt) + " [Ctrl+" + sc + "]")
#endif

#define SET_TIPS_WITH_CTRL_SHORTCUT(a, tt, sc)                                                     \
  SET_TOOLTIP_WITH_CTRL_SHORTCUT(a, tt, sc);                                                       \
  a->setStatusTip(a->toolTip())

#endif // TALIPOT_TLP_QT_TOOLS_H
