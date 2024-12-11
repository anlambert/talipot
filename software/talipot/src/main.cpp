/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <QDir>
#include <QApplication>
#include <QMessageBox>
#include <QProxyStyle>
#include <QRegularExpression>

#include <CrashHandler.h>

#include <talipot/Exception.h>
#include <talipot/Interactor.h>
#include <talipot/Settings.h>
#include <talipot/PythonInterpreter.h>
#include <talipot/GlOffscreenRenderer.h>
#include <talipot/GlTextureManager.h>
#include <talipot/FontIcon.h>

#include "TalipotMainWindow.h"
#include "SplashScreen.h"
#include "PluginsCenter.h"
#include "ThemeUtils.h"

#ifdef WIN32
#include <windows.h>
#endif

#ifdef interface
#undef interface
#endif

using namespace std;
using namespace tlp;

static void talipotLogger(QtMsgType type, const QMessageLogContext &, const QString &msg) {
  QStringList msgPrefixToFilter = {
      "QSocketNotifier: Can only be used with threads started with QThread",
      "This plugin does not support setting window opacity"};

  for (const auto &prefix : msgPrefixToFilter) {
    if (msg.startsWith(prefix)) {
      return;
    }
  }
  if (type == QtFatalMsg) {
    std::cerr << msg.toStdString() << std::endl;
    exit(1);
  } else if (type == QtCriticalMsg) {
    std::cerr << msg.toStdString() << std::endl;
  } else {
    std::cout << msg.toStdString() << std::endl;
  }
}

void usage(const QString &error) {
  int returnCode = 0;

  if (!error.isEmpty()) {
    QMessageBox::warning(nullptr, "Error", error);
    returnCode = 1;
  }

  cout << "Usage: talipot [OPTION] [FILE]" << endl
       << endl
       << "FILE: a graph file supported by Talipot to open. " << endl
       << "List of options:" << endl
       << endl
       << "  --help (-h)\tDisplay this help message and ignore other options." << endl
       << endl;

  exit(returnCode);
}

class TalipotProxyStyle : public QProxyStyle {

public:
  TalipotProxyStyle(const QString &key) : QProxyStyle(key) {
    setObjectName(baseStyle()->metaObject()->className());
  }
  TalipotProxyStyle(QStyle *style) : QProxyStyle(style) {
    setObjectName(style->metaObject()->className());
  }

  QIcon standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption *option = nullptr,
                     const QWidget *widget = nullptr) const override {
    switch (standardIcon) {
    case QStyle::SP_DialogCancelButton:
      return FontIcon::icon(MaterialDesignIcons::Cancel, 0.8);
    case QStyle::SP_DialogCloseButton:
      return FontIcon::icon(MaterialDesignIcons::Close);
    case QStyle::SP_DialogDiscardButton:
      return FontIcon::icon(MaterialDesignIcons::TrashCanOutline);
    case QStyle::SP_DialogNoButton:
      return FontIcon::icon(MaterialDesignIcons::Close);
    case QStyle::SP_DialogOkButton:
      return FontIcon::icon(MaterialDesignIcons::Check);
    case QStyle::SP_DialogSaveButton:
      return FontIcon::icon(MaterialDesignIcons::FileExportOutline);
    case QStyle::SP_DialogYesButton:
      return FontIcon::icon(MaterialDesignIcons::Check);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    case QStyle::SP_DialogYesToAllButton:
      return FontIcon::icon(MaterialDesignIcons::CheckAll);
#endif
    case QStyle::SP_MessageBoxInformation:
      return FontIcon::icon(MaterialDesignIcons::Information, QColor("#407fb2"));
    case QStyle::SP_MessageBoxWarning:
      return FontIcon::icon(MaterialDesignIcons::Alert, QColor("#e18d2b"));
    case QStyle::SP_MessageBoxCritical:
      return FontIcon::icon(MaterialDesignIcons::MinusCircle, QColor("#c42730"));
    case QStyle::SP_MessageBoxQuestion:
      return FontIcon::icon(MaterialDesignIcons::HelpCircle, QColor("#934db1"));

    default:
      return QProxyStyle::standardIcon(standardIcon, option, widget);
    }
  }
};

int main(int argc, char **argv) {

  CrashHandler::install();
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif
  QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);

#ifdef Q_OS_LINUX
  // force use of qt xcb platform plugin as Talipot look and feel on Wayland
  // is not as great as on X11
  qputenv("QT_QPA_PLATFORM", "xcb");
#endif

  qInstallMessageHandler(talipotLogger);
  QApplication talipot(argc, argv);
  talipot.setApplicationName("Talipot");

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
  // Use Qt Fusion widgets style on MacOS / Windows as default style
  // does not integrate nicely with Talipot custom stylesheet
  talipot.setStyle(new TalipotProxyStyle("Fusion"));
#else
  talipot.setStyle(new TalipotProxyStyle(talipot.style()));
#endif

  setApplicationGuiTheme(Settings::guiTheme());

  // Check arguments
  QString inputFilePath;
  QVariantMap extraParams;

  static QRegularExpression extraParametersRegexp("^\\-\\-([^=]*)=(.*)");
  QRegularExpressionMatch match;

  QStringList args = QApplication::arguments();

  bool debugPluginsLoad = false;
  bool checkApplicationStarts = false;

  for (int i = 1; i < args.size(); ++i) {
    QString a = args[i];

    if ((a == "--help") || (a == "-h")) {
      usage("");
    } else if (a == "--debug-plugins-load") {
      debugPluginsLoad = true;
    } else if (a == "--check-application-starts") {
      checkApplicationStarts = true;
    } else if (a.indexOf(extraParametersRegexp, 0, &match) && match.hasMatch()) {
      extraParams[match.captured(1)] = match.captured(2);
    } else {
      inputFilePath = a;
    }
  }

  initTalipotLib(QStringToTlpString(QApplication::applicationDirPath()).c_str());

#ifdef _MSC_VER
  // Add path to Talipot pdb files generated by Visual Studio
  // (for configurations Debug and RelWithDebInfo)
  // It allows to get a detailed stack trace when Talipot crashes.
  CrashHandler::setExtraSymbolsSearchPaths(tlp::TalipotShareDir + "pdb");
#endif

  // initialize embedded Python interpreter
  PythonInterpreter::instance();

  // initialize Talipot
  QMap<QString, QString> pluginErrors;
  try {
    SplashScreen loader(debugPluginsLoad);
    tlp::initTalipotSoftware(&loader);
    pluginErrors = loader.errors();
  } catch (tlp::Exception &e) {
    QMessageBox::warning(nullptr, "Error", e.what());
    exit(1);
  }

  auto cleanup = []() {
    // We need to clear allocated OpenGL resources to avoid a
    // segfault when we close talipot
    GlTextureManager::deleteAllTextures();
    GlOffscreenRenderer::instance().deleteEarly();
    PythonInterpreter::instance().deleteEarly();
  };

  if (debugPluginsLoad && !pluginErrors.isEmpty()) {
    cleanup();
    return 1;
  }

  QFileInfo fileInfo(inputFilePath);

  if (!inputFilePath.isEmpty() && (!fileInfo.exists() || fileInfo.isDir())) {
    usage("File " + inputFilePath + " not found or is a directory");
  }

  // Create and initialize Talipot main window
  TalipotMainWindow &mainWindow = TalipotMainWindow::instance();
  mainWindow.pluginsCenter()->reportPluginErrors(pluginErrors);

  mainWindow.show();
  mainWindow.start(inputFilePath);

  Settings::setFirstRun(false);
  Settings::setFirstTalipotMMRun(false);

  int result = 0;

  if (!checkApplicationStarts) {
    result = talipot.exec();
  }

  mainWindow.deleteEarly();

  cleanup();

  return result;
}
