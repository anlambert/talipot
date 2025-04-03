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

#ifndef TALIPOT_SETTINGS_H
#define TALIPOT_SETTINGS_H

#include <QSettings>
#include <QNetworkProxy>
#include <QString>

#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/Size.h>
#include <talipot/Graph.h>
#include <talipot/Singleton.h>

namespace tlp {

class Settings;
DECLARE_DLL_TEMPLATE_INSTANCE(Singleton<Settings>, TLP_QT_TEMPLATE_DECLARE_SCOPE)

/**
 * @brief This class provides convenience functions to access the Tulip settings file (using
 * QSettings)
 * Settings is a wrapper for QSettings providing quick access to common keys provided in the
 * talipot configuration file.
 * This object does not mask any method from the QSettings class, which mean that the user can
 * still access custom keys by invoking the QSettings::value method.
 */
class TLP_QT_SCOPE Settings : public QSettings, public Observable, public Singleton<Settings> {
    Q_OBJECT
    Q_ENUMS(DisplayProperty)

    friend class Singleton<Settings>;

  public:
    static QString elementKey(const QString &configEntry, tlp::ElementType elem);

    static void synchronizeViewSettings();

    /**
      @brief Retrieves the list of documents recently opened with talipot.
      */
    static QStringList recentDocuments();

    /**
      @brief Registers a file in the list of recently opened documents.
      If the list already contains 20 elements, the last one (in chronological order) will be
      removed.
      */
    static void addToRecentDocuments(const QString &);

    static tlp::Color defaultColor(tlp::ElementType elem);
    static void setDefaultColor(tlp::ElementType elem, const tlp::Color &color);

    static tlp::Color defaultLabelColor();
    static void setDefaultLabelColor(const tlp::Color &color);

    static tlp::Size defaultSize(tlp::ElementType elem);
    static void setDefaultSize(tlp::ElementType elem, const tlp::Size &size);

    static int defaultShape(tlp::ElementType elem);
    static void setDefaultShape(tlp::ElementType elem, int shape);

    static tlp::Color defaultSelectionColor();
    static void setDefaultSelectionColor(const tlp::Color &color);

    static QSet<QString> favoriteAlgorithms();
    static void addFavoriteAlgorithm(const QString &name);
    static void removeFavoriteAlgorithm(const QString &name);

    // Proxy settings
    static bool isProxyEnabled();
    static void setProxyEnabled(bool);

    static QNetworkProxy::ProxyType proxyType();
    static void setProxyType(QNetworkProxy::ProxyType);

    static QString proxyHost();
    static void setProxyHost(const QString &);

    static uint proxyPort();
    static void setProxyPort(uint);

    static bool isUseProxyAuthentification();
    static void setUseProxyAuthentification(bool);

    static QString proxyUsername();
    static void setProxyUsername(const QString &);

    static QString proxyPassword();
    static void setProxyPassword(const QString &);

    static void applyProxySettings();

    static bool isFirstRun();
    static void setFirstRun(bool);

    static bool isFirstTalipotMMRun();
    static void setFirstTalipotMMRun(bool);

    static bool displayDefaultViews();
    static void setDisplayDefaultViews(bool);

    static bool isAutomaticMapMetric();
    static void setAutomaticMapMetric(bool);

    static bool isAutomaticCentering();
    static void setAutomaticCentering(bool);

    static bool isAutomaticRatio();
    static void setAutomaticRatio(bool);

    static bool isViewOrtho();
    static void setViewOrtho(bool);

    static bool isResultPropertyStored();
    static void setResultPropertyStored(bool);

    enum LogPluginCall { NoLog = 0, LogCall = 1, LogCallWithExecutionTime = 2 };
    static uint logPluginCall();
    static void setLogPluginCall(uint);

    static bool isUseTlpbFileFormat();
    static void setUseTlpbFileFormat(bool);

    static uint seedOfRandomSequence();
    static void setSeedOfRandomSequence(uint);
    static void initSeedOfRandomSequence();

    static bool loggerAnchored();
    static void setLoggerAnchored(bool);

    static bool pythonIDEAnchored();
    static void setPythonIDEAnchored(bool);

    static QString guiTheme();
    static void setGuiTheme(const QString &guiTheme);

    void treatEvent(const Event &message) override;

  private:
    Settings();

    static void setFavoriteAlgorithms(const QSet<QString> &lst);
};
}
#endif // TALIPOT_SETTINGS_H
