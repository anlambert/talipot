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

#include <GL/glew.h>

#include <QColorDialog>
#include <QMetaEnum>
#include <QApplication>
#include <QDir>
#include <QApplication>
#include <QStandardPaths>
#include <QMainWindow>
#include <QFontDatabase>
#include <QLayout>

#include <talipot/Settings.h>
#include <talipot/Interactor.h>
#include <talipot/SystemDefinition.h>
#include <talipot/PluginLibraryLoader.h>
#include <talipot/GlyphManager.h>
#include <talipot/EdgeExtremityGlyphManager.h>
#include <talipot/OpenGlConfigManager.h>
#include <talipot/GlTextureManager.h>
#include <talipot/FileDownloader.h>
#include <talipot/ItemEditorCreators.h>
#include <talipot/GlOffscreenRenderer.h>
#include <talipot/FontIcon.h>
#include <talipot/MaterialDesignIcons.h>

/**
 * For openDataSetDialog function : see OpenDataSet.cpp
 */
using namespace std;
using namespace tlp;

/**
 * Init property type to property label conversion map
 **/
static flat_hash_map<string, QString> &buildPropertyTypeToPropertyTypeLabelMap() {
  static flat_hash_map<string, QString> propertyTypeToPropertyTypeLabel;
  propertyTypeToPropertyTypeLabel[BooleanProperty::propertyTypename] = QString("Boolean");
  propertyTypeToPropertyTypeLabel[ColorProperty::propertyTypename] = QString("Color");
  propertyTypeToPropertyTypeLabel[DoubleProperty::propertyTypename] = QString("Double");
  propertyTypeToPropertyTypeLabel[GraphProperty::propertyTypename] = QString("Graph");
  propertyTypeToPropertyTypeLabel[IntegerProperty::propertyTypename] = QString("Integer");
  propertyTypeToPropertyTypeLabel[LayoutProperty::propertyTypename] = QString("Layout");
  propertyTypeToPropertyTypeLabel[SizeProperty::propertyTypename] = QString("Size");
  propertyTypeToPropertyTypeLabel[StringProperty::propertyTypename] = QString("String");
  propertyTypeToPropertyTypeLabel[BooleanVectorProperty::propertyTypename] =
      QString("BooleanVector");
  propertyTypeToPropertyTypeLabel[ColorVectorProperty::propertyTypename] = QString("ColorVector");
  propertyTypeToPropertyTypeLabel[CoordVectorProperty::propertyTypename] = QString("CoordVector");
  propertyTypeToPropertyTypeLabel[DoubleVectorProperty::propertyTypename] = QString("DoubleVector");
  propertyTypeToPropertyTypeLabel[IntegerVectorProperty::propertyTypename] =
      QString("IntegerVector");
  propertyTypeToPropertyTypeLabel[SizeVectorProperty::propertyTypename] = QString("SizeVector");
  propertyTypeToPropertyTypeLabel[StringVectorProperty::propertyTypename] = QString("StringVector");
  return propertyTypeToPropertyTypeLabel;
}

// Property type to property label conversion map
static const flat_hash_map<string, QString> &propertyTypeToPropertyTypeLabelMap =
    buildPropertyTypeToPropertyTypeLabelMap();
/**
 * Init property type label to property type conversion map
 **/
static map<QString, string> buildPropertyTypeLabelToPropertyTypeMap() {
  static map<QString, string> propertyTypeLabelToPropertyType;
  propertyTypeLabelToPropertyType[QString("Boolean")] = BooleanProperty::propertyTypename;
  propertyTypeLabelToPropertyType[QString("Color")] = ColorProperty::propertyTypename;
  propertyTypeLabelToPropertyType[QString("Double")] = DoubleProperty::propertyTypename;
  propertyTypeLabelToPropertyType[QString("Graph")] = GraphProperty::propertyTypename;
  propertyTypeLabelToPropertyType[QString("Integer")] = IntegerProperty::propertyTypename;
  propertyTypeLabelToPropertyType[QString("Layout")] = LayoutProperty::propertyTypename;
  propertyTypeLabelToPropertyType[QString("Size")] = SizeProperty::propertyTypename;
  propertyTypeLabelToPropertyType[QString("String")] = StringProperty::propertyTypename;
  propertyTypeLabelToPropertyType[QString("BooleanVector")] =
      BooleanVectorProperty::propertyTypename;
  propertyTypeLabelToPropertyType[QString("ColorVector")] = ColorVectorProperty::propertyTypename;
  propertyTypeLabelToPropertyType[QString("CoordVector")] = CoordVectorProperty::propertyTypename;
  propertyTypeLabelToPropertyType[QString("DoubleVector")] = DoubleVectorProperty::propertyTypename;
  propertyTypeLabelToPropertyType[QString("IntegerVector")] =
      IntegerVectorProperty::propertyTypename;
  propertyTypeLabelToPropertyType[QString("SizeVector")] = SizeVectorProperty::propertyTypename;
  propertyTypeLabelToPropertyType[QString("StringVector")] = StringVectorProperty::propertyTypename;
  return propertyTypeLabelToPropertyType;
}
// Property type label to property type conversion map
static const map<QString, string> &propertyTypeLabelToPropertyTypeMap =
    buildPropertyTypeLabelToPropertyTypeMap();

namespace tlp {

bool getColorDialog(const QColor &color, QWidget *parent, const QString &title, QColor &result) {

  QColor newColor = QColorDialog::getColor(
      color, parent, title, QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);

  if (newColor.isValid()) {
    result = newColor;
    return true;
  } else {
    return false;
  }
}

QString propertyTypeToPropertyTypeLabel(const string &typeName) {
  auto it = propertyTypeToPropertyTypeLabelMap.find(typeName);
  return it != propertyTypeToPropertyTypeLabelMap.end() ? it->second : QString();
}

string propertyTypeLabelToPropertyType(const QString &typeNameLabel) {
  auto it = propertyTypeLabelToPropertyTypeMap.find(typeNameLabel);
  return it != propertyTypeLabelToPropertyTypeMap.end() ? it->second : string();
}

QString getPluginPackageName(const QString &pluginName) {
  return pluginName.simplified().remove(' ').toLower() + "-" + TALIPOT_VERSION + "-" + OS_PLATFORM +
         OS_ARCHITECTURE + "-" + OS_COMPILER + ".zip";
}

QString getPluginLocalInstallationDir() {
  return QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0) + "/plugins";
}

QString localPluginsPath() {
  return getPluginLocalInstallationDir() + '/';
}

// we define a specific GlTextureLoader allowing to load a GlTexture
// from a QImage
class GlTextureFromQImageLoader : public GlTextureLoader {
public:
  // redefine the inherited method
  bool loadTexture(const std::string &filename, GlTexture &glTexture) override {

    QImage image;

    QString qFilename = tlpStringToQString(filename);

    if (qFilename.startsWith("http")) {
      FileDownloader fileDownloader;
      QByteArray imageData = fileDownloader.download(QUrl(qFilename));

      if (imageData.isEmpty()) {
        tlp::error() << "Error when downloading texture from url " << filename.c_str() << std::endl;
        return false;
      } else {
        bool imageLoaded = image.loadFromData(imageData);

        if (!imageLoaded) {
          tlp::error() << "Error when loading texture from url " << filename.c_str() << std::endl;
          return false;
        }
      }
    } else {

      QFile imageFile(qFilename);

      if (imageFile.open(QIODevice::ReadOnly)) {
        image.loadFromData(imageFile.readAll());
      }

      if (image.isNull()) {
        if (!imageFile.exists()) {
          tlp::error() << "Error when loading texture, the file named \"" << filename.c_str()
                       << "\" does not exist" << std::endl;
        } else {
          tlp::error() << "Error when loading texture from " << filename.c_str() << std::endl;
        }

        return false;
      }
    }

    // store icon preview of the loaded texture in the icon pool
    // used by the Tulip spreadsheet view
    if (!image.isNull()) {
      addIconToPool(qFilename, QIcon(QPixmap::fromImage(image)));
    }

    bool canUseMipmaps = OpenGlConfigManager::isExtensionSupported("GL_ARB_framebuffer_object") ||
                         OpenGlConfigManager::isExtensionSupported("GL_EXT_framebuffer_object");

    uint width = image.width();
    uint height = image.height();

#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
    image = image.flipped();
#else
    image = image.mirrored();
#endif

    image = image.convertToFormat(QImage::Format_RGBA8888);

    glTexture.width = width;
    glTexture.height = height;

    glGenTextures(1, &glTexture.id);

    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, glTexture.id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 image.constBits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (canUseMipmaps) {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glGenerateMipmap(GL_TEXTURE_2D);
    } else {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    glDisable(GL_TEXTURE_2D);

    return true;
  }
};

void initTalipotSoftware(tlp::PluginLoader *loader) {

  QLocale::setDefault(QLocale(QLocale::English));
  Settings::applyProxySettings();
  Settings::initSeedOfRandomSequence();

  QDir::home().mkpath(tlp::localPluginsPath());
  QLocale::setDefault(QLocale(QLocale::English));

#if defined(__APPLE__)
  QApplication::addLibraryPath(QApplication::applicationDirPath() + "/../");
  QApplication::addLibraryPath(QApplication::applicationDirPath() + "/../lib/");
#endif

  tlp::initTalipotLib();
  initQTypeSerializers();
  // initialize Texture loader
  GlTextureManager::setTextureLoader(new GlTextureFromQImageLoader());
  // load plugins
  tlp::PluginLibraryLoader::loadPluginsFromDir(
      tlp::TalipotPluginsPath, loader,
      QStringToTlpString(tlp::getPluginLocalInstallationDir()) + "/lib/talipot");
  tlp::PluginLibraryLoader::loadPluginsFromDir(
      QStringToTlpString(tlp::getPluginLocalInstallationDir()), loader);
  tlp::PluginsManager::checkLoadedPluginsDependencies(loader);
  tlp::InteractorLister::initInteractorsDependencies();
  tlp::GlyphManager::loadGlyphPlugins();
  tlp::EdgeExtremityGlyphManager::loadGlyphPlugins();

  // explicitly create a shared OpenGL context to ensure it is initialized
  // before using it
  GlOffscreenRenderer::instance().makeOpenGLContextCurrent();
  QString glRenderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
  if (glRenderer.startsWith("Mesa Intel(R) HD Graphics")) {
    // prevent GL_SELECT rendering crash when using the Mesa crocus driver
    // for legacy Intel GPUs
    qputenv("DRAW_USE_LLVM", "0");
  }
  GlOffscreenRenderer::instance().doneOpenGLContextCurrent();

  const auto &fonts = Font::availableFonts();
  for (const auto &itFamily : fonts) {
    for (const auto &itStyle : itFamily.second) {
      addFontToQFontDatabase(itStyle.second);
    }
  }
}

template <QtMsgType msgType>
class QDebugOStream : public std::ostream {
  class QDebugStreamBuf : public std::streambuf {

  protected:
    string buf;
    int_type overflow(int c) override {
      if (c == '\n') {
        getQDebug() << buf.c_str();
        buf.clear();
      } else {
        buf += c;
      }

      return c;
    }

    std::streamsize xsputn(const char *p, std::streamsize n) override {
      if (p[n - 1] == '\n') {
        buf += std::string(p, n - 1);
        getQDebug() << buf.c_str();
        buf.clear();
      } else {
        buf += std::string(p, n);
      }

      return n;
    }

    static QDebug getQDebug() {
      if constexpr (msgType == QtDebugMsg) {
        return qDebug();
      } else if constexpr (msgType == QtInfoMsg) {
        return qInfo();
      } else if constexpr (msgType == QtWarningMsg) {
        return qWarning();
      } else if constexpr (msgType == QtCriticalMsg) {
        return qCritical();
      } else {
        return qInfo();
      }
    }
  };

  QDebugStreamBuf qDebugBuf;

public:
  QDebugOStream() : std::ostream(&qDebugBuf) {}
};

// output streams redirection

static unique_ptr<std::ostream> qDebugStream;
static unique_ptr<std::ostream> qWarningStream;
static unique_ptr<std::ostream> qErrorStream;
static unique_ptr<std::ostream> qInfoStream;

void redirectStreamOutputsToQt() {
  if (qDebugStream.get() == nullptr) {
    qDebugStream.reset(new QDebugOStream<QtDebugMsg>());
  }
  if (qWarningStream.get() == nullptr) {
    qWarningStream.reset(new QDebugOStream<QtWarningMsg>());
  }
  if (qErrorStream.get() == nullptr) {
    qErrorStream.reset(new QDebugOStream<QtCriticalMsg>());
  }
  if (qInfoStream.get() == nullptr) {
    qInfoStream.reset(new QDebugOStream<QtInfoMsg>());
  }
  tlp::setDebugOutput(*qDebugStream);
  tlp::setWarningOutput(*qWarningStream);
  tlp::setErrorOutput(*qErrorStream);
  tlp::setInfoOutput(*qInfoStream);
}

class NoQtUserInputFilter : public QObject {
protected:
  bool eventFilter(QObject *, QEvent *event) override {
    switch (event->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::HoverMove:
    case QEvent::DragEnter:
    case QEvent::DragLeave:
    case QEvent::DragMove:
    case QEvent::Drop:
      return true;

    default:
      return false;
    }
  }
};

static unique_ptr<NoQtUserInputFilter> disableQtUserInputFilter;

void disableQtUserInput() {
  if (!disableQtUserInputFilter.get()) {
    disableQtUserInputFilter.reset(new NoQtUserInputFilter());
  }

  QCoreApplication::instance()->installEventFilter(disableQtUserInputFilter.get());
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

void enableQtUserInput() {
  if (!disableQtUserInputFilter.get()) {
    return;
  }

  QCoreApplication::instance()->removeEventFilter(disableQtUserInputFilter.get());
  QApplication::restoreOverrideCursor();
}

QMainWindow *getMainWindow() {
  for (QWidget *widget : qApp->topLevelWidgets()) {
    if (auto *mainWindow = qobject_cast<QMainWindow *>(widget)) {
      return mainWindow;
    }
  }
  return nullptr;
}

bool applicationHasDarkGuiTheme() {
  return backgroundColor().lightnessF() < 0.5;
}

const QColor darkColor = QColor(50, 50, 50);

const QColor &backgroundColor() {
  return QApplication::palette().color(QPalette::Base);
}

const QColor &alternateBackgroundColor() {
  return QApplication::palette().color(QPalette::AlternateBase);
}

const QColor &textColor() {
  return QApplication::palette().color(QPalette::WindowText);
}

static flat_hash_map<string, int> fontIds;

void addFontToQFontDatabase(const Font &font) {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QFontDatabase fontDb;
  if (!fontDb
           .styles(tlpStringToQString(font.fontFamily()))
#else
  if (!QFontDatabase::styles(tlpStringToQString(font.fontFamily()))
#endif
           .contains(tlpStringToQString(font.fontStyle())) &&
      fontIds.find(font.fontFile()) == fontIds.end()) {
    fontIds[font.fontFile()] =
        QFontDatabase::addApplicationFont(tlpStringToQString(font.fontFile()));
  }
}

void removeFontFromQFontDatabase(const string &fontFile) {
  if (fontIds.contains(fontFile)) {
    QFontDatabase::removeApplicationFont(fontIds[fontFile]);
    fontIds.erase(fontFile);
  }
}

void clearLayout(QLayout *layout, bool deleteWidgets) {
  while (QLayoutItem *item = layout->takeAt(0)) {
    if (deleteWidgets) {
      if (QWidget *widget = item->widget()) {
        delete widget;
      }
    } else if (QLayout *childLayout = item->layout()) {
      clearLayout(childLayout, deleteWidgets);
    }

    delete item;
  }
}

QCursor whatsThisCursor() {
#ifndef __APPLE__
  static auto cursor = QCursor(Qt::WhatsThisCursor);
#else
  static auto cursor = QCursor(QPixmap(":/talipot/gui/icons/20/whats_this.png"), 2, 4);
#endif
  return cursor;
}

void qDetailedMessageBox(QMessageBox::Icon icon, const QString &title, const QString &text,
                         const QString &detailedText, QWidget *parent,
                         QMessageBox::StandardButtons buttons, Qt::WindowFlags f) {
  QMessageBox msgBox(icon, title, text, buttons, parent, f);
  if (!detailedText.isEmpty()) {
    auto splitText = detailedText.split("\n");
    int width = 0;
    for (auto &s : splitText) {
      width = max(width, msgBox.fontMetrics().horizontalAdvance(s));
    }
    msgBox.setDetailedText(detailedText);
    QList<QTextEdit *> textBoxes = msgBox.findChildren<QTextEdit *>();
    if (textBoxes.size()) {
      textBoxes[0]->setMinimumWidth(min(width + 20, int(0.75 * getMainWindow()->width())));
      textBoxes[0]->setFixedHeight(min(int(splitText.size() + 1) * msgBox.fontMetrics().height(),
                                       int(0.75 * getMainWindow()->height())));
    }
  }
  msgBox.exec();
}

QIcon addToSelectionIcon() {
  return FontIcon::stackIcons(FontIcon::icon(MaterialDesignIcons::Selection),
                              FontIcon::icon(MaterialDesignIcons::Plus, 0.7));
}
}
