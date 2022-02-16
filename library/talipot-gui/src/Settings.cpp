/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "talipot/Settings.h"

#include <talipot/MetaTypes.h>
#include <talipot/GlyphManager.h>
#include <talipot/TlpQtTools.h>

using namespace tlp;
using namespace std;

static const QString TS_RecentDocuments = "app/recent_documents";
static const QString TS_DefaultColor = "graph/defaults/color/";
static const QString TS_DefaultLabelColor = "graph/defaults/color/labels";
static const QString TS_DefaultSize = "graph/defaults/size/";
static const QString TS_DefaultShape = "graph/defaults/shape/";
static const QString TS_DefaultSelectionColor = "graph/defaults/selectioncolor/";
static const QString TS_FavoriteAlgorithms = "app/algorithms/favorites";

static const QString TS_FirstRun = "app/talipot/firstRun";
static const QString TS_FirstRunMM = QString("app/") + TALIPOT_MM_VERSION + "/firstRun";

static const QString TS_ProxyEnabled = "app/proxy/enabled";
static const QString TS_ProxyType = "app/proxy/type";
static const QString TS_ProxyHost = "app/proxy/host";
static const QString TS_ProxyPort = "app/proxy/port";
static const QString TS_ProxyUseAuth = "app/proxy/user";
static const QString TS_ProxyUsername = "app/proxy/useAuth";
static const QString TS_ProxyPassword = "app/proxy/passwd";

static const QString TS_AutomaticDisplayDefaultViews = "graph/auto/defaultViews";
static const QString TS_AutomaticPerfectAspectRatio = "graph/auto/ratio";
static const QString TS_AutomaticCentering = "graph/auto/center";
static const QString TS_ViewOrtho = "graph/auto/ortho";
static const QString TS_AutomaticMapMetric = "graph/auto/colors";

static const QString TS_ResultPropertyStored = "graph/auto/result";

static const QString TS_LogPluginCall = "graph/auto/log";
static const QString TS_UseTlpbFileFormat = "graph/auto/usetlpb";
static const QString TS_SeedForRandomSequence = "graph/auto/seed";

static const QString TS_LoggerAnchored = "app/gui/logger_anchored";
static const QString TS_PythonIDEAnchored = "app/gui/python_ide_anchored";
static const QString TS_GuiTheme = "app/gui/theme";

Settings::Settings() : QSettings("TalipotFramework", "Talipot") {
  ViewSettings::instance().addListener(this);
}

void Settings::synchronizeViewSettings() {
  ViewSettings::setDefaultColor(ElementType::NODE, defaultColor(ElementType::NODE));
  ViewSettings::setDefaultColor(ElementType::EDGE, defaultColor(ElementType::EDGE));
  ViewSettings::setDefaultSize(ElementType::NODE, defaultSize(ElementType::NODE));
  ViewSettings::setDefaultSize(ElementType::EDGE, defaultSize(ElementType::EDGE));
  ViewSettings::setDefaultShape(ElementType::NODE, defaultShape(ElementType::NODE));
  ViewSettings::setDefaultShape(ElementType::EDGE, defaultShape(ElementType::EDGE));
  ViewSettings::setDefaultLabelColor(defaultLabelColor());
  ViewSettings::setDefaultSelectionColor(defaultSelectionColor());
}

QStringList Settings::recentDocuments() {
  return instance().value(TS_RecentDocuments).toStringList();
}

void Settings::addToRecentDocuments(const QString &name) {
  QList<QVariant> recentDocumentsValue = instance().value(TS_RecentDocuments).toList();

  if (recentDocumentsValue.contains(name)) {
    recentDocumentsValue.removeAll(name);
  }

  recentDocumentsValue.push_front(name);

  while (recentDocumentsValue.size() > 20) {
    recentDocumentsValue.pop_back();
  }

  instance().setValue(TS_RecentDocuments, recentDocumentsValue);
}

QString Settings::elementKey(const QString &configEntry, tlp::ElementType elem) {
  return configEntry + (elem == tlp::ElementType::NODE ? "node" : "edge");
}

tlp::Color Settings::defaultColor(tlp::ElementType elem) {
  QString val = instance()
                    .value(elementKey(TS_DefaultColor, elem),
                           ColorType::toString(ViewSettings::defaultColor(elem)).c_str())
                    .toString();
  Color result;
  ColorType::fromString(result, QStringToTlpString(val));
  return result;
}

void Settings::setDefaultColor(tlp::ElementType elem, const tlp::Color &color) {
  QString value = tlp::ColorType::toString(color).c_str();
  instance().setValue(elementKey(TS_DefaultColor, elem), value);
  ViewSettings::setDefaultColor(elem, color);
}

Color Settings::defaultLabelColor() {
  QString val = instance()
                    .value(TS_DefaultLabelColor,
                           ColorType::toString(ViewSettings::defaultLabelColor()).c_str())
                    .toString();
  Color result;
  ColorType::fromString(result, QStringToTlpString(val));
  return result;
}

void Settings::setDefaultLabelColor(const Color &color) {
  QString value = tlp::ColorType::toString(color).c_str();
  instance().setValue(TS_DefaultLabelColor, value);
  ViewSettings::setDefaultLabelColor(color);
}

tlp::Size Settings::defaultSize(tlp::ElementType elem) {
  QString val = instance()
                    .value(elementKey(TS_DefaultSize, elem),
                           SizeType::toString(ViewSettings::defaultSize(elem)).c_str())
                    .toString();
  Size result;
  SizeType::fromString(result, QStringToTlpString(val));
  return result;
}

void Settings::setDefaultSize(tlp::ElementType elem, const tlp::Size &size) {
  QString value = tlp::SizeType::toString(size).c_str();
  instance().setValue(elementKey(TS_DefaultSize, elem), value);
  ViewSettings::setDefaultSize(elem, size);
}

int Settings::defaultShape(tlp::ElementType elem) {
  return instance()
      .value(elementKey(TS_DefaultShape, elem), ViewSettings::defaultShape(elem))
      .toInt();
}

void Settings::setDefaultShape(tlp::ElementType elem, int shape) {
  instance().setValue(elementKey(TS_DefaultShape, elem), shape);
  ViewSettings::setDefaultShape(elem, shape);
}

tlp::Color Settings::defaultSelectionColor() {
  QString val = instance()
                    .value(TS_DefaultSelectionColor,
                           ColorType::toString(ViewSettings::defaultSelectionColor()).c_str())
                    .toString();
  Color result;
  ColorType::fromString(result, QStringToTlpString(val));
  return result;
}

void Settings::setDefaultSelectionColor(const tlp::Color &color) {
  QString value = tlp::ColorType::toString(color).c_str();
  instance().setValue(TS_DefaultSelectionColor, value);
  ViewSettings::setDefaultSelectionColor(color);
}

QSet<QString> Settings::favoriteAlgorithms() {
  QStringList list = instance().value(TS_FavoriteAlgorithms, QStringList()).toStringList();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  return QSet<QString>(list.begin(), list.end());
#else
  return list.toSet();
#endif
}

void Settings::addFavoriteAlgorithm(const QString &name) {
  QSet<QString> favAlgs = favoriteAlgorithms();
  favAlgs.insert(name);
  setFavoriteAlgorithms(favAlgs);
}

void Settings::removeFavoriteAlgorithm(const QString &name) {
  QSet<QString> favAlgs = favoriteAlgorithms();
  favAlgs.remove(name);
  setFavoriteAlgorithms(favAlgs);
}

bool Settings::isProxyEnabled() {
  return instance().value(TS_ProxyEnabled).toBool();
}

void Settings::setProxyEnabled(bool f) {
  instance().setValue(TS_ProxyEnabled, f);
}

QNetworkProxy::ProxyType Settings::proxyType() {
  return static_cast<QNetworkProxy::ProxyType>(instance().value(TS_ProxyType).toInt());
}

void Settings::setProxyType(QNetworkProxy::ProxyType t) {
  instance().setValue(TS_ProxyType, int(t));
}

QString Settings::proxyHost() {
  return instance().value(TS_ProxyHost).toString();
}

void Settings::setProxyHost(const QString &h) {
  instance().setValue(TS_ProxyHost, h);
}

uint Settings::proxyPort() {
  return instance().value(TS_ProxyPort).toUInt();
}

void Settings::setProxyPort(uint p) {
  instance().setValue(TS_ProxyPort, p);
}

bool Settings::isUseProxyAuthentification() {
  return instance().value(TS_ProxyUseAuth).toBool();
}

void Settings::setUseProxyAuthentification(bool f) {
  instance().setValue(TS_ProxyUseAuth, f);
}

QString Settings::proxyUsername() {
  return instance().value(TS_ProxyUsername).toString();
}

void Settings::setProxyUsername(const QString &s) {
  instance().setValue(TS_ProxyUsername, s);
}

QString Settings::proxyPassword() {
  return instance().value(TS_ProxyPassword).toString();
}

void Settings::setProxyPassword(const QString &s) {
  instance().setValue(TS_ProxyPassword, s);
}

void Settings::applyProxySettings() {
  if (isProxyEnabled()) {
    QNetworkProxy proxy(proxyType(), proxyHost(), static_cast<qint16>(proxyPort()));

    if (isUseProxyAuthentification()) {
      proxy.setUser(proxyUsername());
      proxy.setPassword(proxyPassword());
    }
    QNetworkProxy::setApplicationProxy(proxy);
  } else {
    QNetworkProxyFactory::setUseSystemConfiguration(true);
  }
}

bool Settings::isFirstRun() {
  return !instance().contains(TS_FirstRun);
}

void Settings::setFirstRun(bool f) {
  instance().setValue(TS_FirstRun, f);
}

bool Settings::isFirstTalipotMMRun() {
  return !instance().contains(TS_FirstRunMM);
}

void Settings::setFirstTalipotMMRun(bool f) {
  instance().setValue(TS_FirstRunMM, f);
}

bool Settings::displayDefaultViews() {
  return instance().value(TS_AutomaticDisplayDefaultViews, true).toBool();
}

void Settings::setDisplayDefaultViews(bool f) {
  instance().setValue(TS_AutomaticDisplayDefaultViews, f);
}

bool Settings::isAutomaticMapMetric() {
  return instance().value(TS_AutomaticMapMetric, false).toBool();
}

void Settings::setAutomaticMapMetric(bool f) {
  instance().setValue(TS_AutomaticMapMetric, f);
}

bool Settings::isAutomaticRatio() {
  return instance().value(TS_AutomaticPerfectAspectRatio, false).toBool();
}

void Settings::setAutomaticRatio(bool f) {
  instance().setValue(TS_AutomaticPerfectAspectRatio, f);
}

bool Settings::isAutomaticCentering() {
  return instance().value(TS_AutomaticCentering, true).toBool();
}

void Settings::setAutomaticCentering(bool f) {
  instance().setValue(TS_AutomaticCentering, f);
}

bool Settings::isViewOrtho() {
  return instance().value(TS_ViewOrtho, true).toBool();
}

void Settings::setViewOrtho(bool f) {
  instance().setValue(TS_ViewOrtho, f);
}

void Settings::setFavoriteAlgorithms(const QSet<QString> &lst) {
  instance().setValue(TS_FavoriteAlgorithms, static_cast<QStringList>(lst.values()));
}

bool Settings::isResultPropertyStored() {
  return instance().value(TS_ResultPropertyStored, false).toBool();
}

void Settings::setResultPropertyStored(bool f) {
  instance().setValue(TS_ResultPropertyStored, f);
}

uint Settings::logPluginCall() {
  return instance().value(TS_LogPluginCall, NoLog).toUInt();
}

void Settings::setLogPluginCall(uint val) {
  instance().setValue(TS_LogPluginCall, val);
}

bool Settings::isUseTlpbFileFormat() {
  return instance().value(TS_UseTlpbFileFormat, true).toBool();
}

void Settings::setUseTlpbFileFormat(bool f) {
  instance().setValue(TS_UseTlpbFileFormat, f);
}

uint Settings::seedOfRandomSequence() {
  return instance().value(TS_SeedForRandomSequence, tlp::getSeedOfRandomSequence()).toUInt();
}

void Settings::setSeedOfRandomSequence(uint seed) {
  instance().setValue(TS_SeedForRandomSequence, seed);
}

void Settings::initSeedOfRandomSequence() {
  tlp::setSeedOfRandomSequence(seedOfRandomSequence());
}

bool Settings::loggerAnchored() {
  return instance().value(TS_LoggerAnchored, true).toBool();
}

void Settings::setLoggerAnchored(bool f) {
  instance().setValue(TS_LoggerAnchored, f);
}

bool Settings::pythonIDEAnchored() {
  return instance().value(TS_PythonIDEAnchored, true).toBool();
}

void Settings::setPythonIDEAnchored(bool f) {
  instance().setValue(TS_PythonIDEAnchored, f);
}

QString Settings::guiTheme() {
  return instance().value(TS_GuiTheme, "Light").toString();
}

void Settings::setGuiTheme(const QString &guiTheme) {
  instance().setValue(TS_GuiTheme, guiTheme);
}

void Settings::treatEvent(const Event &message) {
  const auto *sev = dynamic_cast<const ViewSettingsEvent *>(&message);

  if (sev && sev->getType() == ViewSettingsEventType::TLP_DEFAULT_COLOR_MODIFIED) {
    setDefaultColor(sev->getElementType(), sev->getColor());
  } else if (sev && sev->getType() == ViewSettingsEventType::TLP_DEFAULT_SIZE_MODIFIED) {
    setDefaultSize(sev->getElementType(), sev->getSize());
  } else if (sev && sev->getType() == ViewSettingsEventType::TLP_DEFAULT_SHAPE_MODIFIED) {
    setDefaultShape(sev->getElementType(), sev->getShape());
  } else if (sev && sev->getType() == ViewSettingsEventType::TLP_DEFAULT_LABEL_COLOR_MODIFIED) {
    setDefaultLabelColor(sev->getColor());
  }
}

INSTANTIATE_DLL_TEMPLATE(tlp::Singleton<tlp::Settings>, TLP_QT_TEMPLATE_DEFINE_SCOPE)