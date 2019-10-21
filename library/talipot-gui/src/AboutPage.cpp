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

#include <talipot/AboutPage.h>
#include <talipot/TlpTools.h>
#include <talipot/TlpQtTools.h>
#include <talipot/GlMainWidget.h>
#include <talipot/Release.h>
#include <talipot/OpenGlConfigManager.h>
#include <talipot/PythonVersionChecker.h>
#include <talipot/GlOffscreenRenderer.h>

#include "ui_AboutPage.h"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QOpenGLContext>

namespace tlp {
extern QString getSipVersion();
extern QString getTalipotGitRevision();
}

const QString TalipotRepoUrl = "https://github.com/anlambert/talipot";

using namespace tlp;

AboutPage::AboutPage(QWidget *parent) : QWidget(parent), _ui(new Ui::AboutPageData()) {
  _ui->setupUi(this);

  QString title("Talipot ");
  title += TALIPOT_VERSION;
  QString git_rev(getTalipotGitRevision());

  if (!git_rev.isEmpty())
    title += "<br/>(Git commit: <a href=\"" + TalipotRepoUrl + "/commit/" + git_rev + "\">" +
             git_rev.mid(0, 7) + "</a>)";

  _ui->logolabel->setPixmap(QPixmap(tlpStringToQString(TalipotBitmapDir + "/logo.png")));
  _ui->TalipotLabel->setText(
      "<html>"
      "  <head/>"
      "  <body>"
      "    <p align=\"center\"><span style=\" font-size:24pt; font-weight:600;\">" +
      title + "</span></p>" +
      (!git_rev.isEmpty() ? (QString("    <p align=\"center\"><a href=\"") + TalipotRepoUrl +
                             "\">" + TalipotRepoUrl + "</a></p>")
                          : QString()) +
      "  </body>"
      "</html>");

  bool openGlOk = GlOffscreenRenderer::getInstance()->getOpenGLContext()->isValid();

  if (openGlOk) {
    GlOffscreenRenderer::getInstance()->makeOpenGLContextCurrent();
  }

  QString talipotDependenciesInfo =
      "<p style=\"font-size:12pt\">"
      "This open source software is powered by:"
      "<ul>"
      "  <li> <b> Qt </b> " +
      tlpStringToQString(qVersion()) +
      ": <a href=\"https://www.qt.io\">https://www.qt.io</a></li>"
      "  <li> <b> OpenGL </b> " +
      (openGlOk ? QString::number(OpenGlConfigManager::getOpenGLVersion()) : QString("?.?")) +
      " (from vendor " +
      (openGlOk ? tlpStringToQString(OpenGlConfigManager::getOpenGLVendor()) : QString("unknown")) +
      "): <a href=\"https://www.opengl.org\">https://www.opengl.org</a> </li>"
      "  <li> <b>OGDF</b> v2015.05 (Baobab) aka the Open Graph Drawing Framework : <a "
      "href=\"http://www.ogdf.net\">http://www.ogdf.net</a> </li>"
      "  <li> <b> Python </b> " +
      PythonVersionChecker::compiledVersion() +
      ": <a href=\"https://www.python.org\">https://www.python.org</a> </li>"
      "  <li> <b> SIP </b> " +
      getSipVersion() +
      ": <a "
      "href=\"https://www.riverbankcomputing.com/software/sip/\">https://"
      "www.riverbankcomputing.com/software/sip</a> </li>"
      "</ul>"
      "</p>";

  if (openGlOk) {
    GlOffscreenRenderer::getInstance()->doneOpenGLContextCurrent();
  }

  _ui->dependenciesInfo->setText(talipotDependenciesInfo);
  connect(_ui->aboutQt, SIGNAL(clicked()), qApp, SLOT(aboutQt()));
  connect(_ui->dependenciesInfo, SIGNAL(linkActivated(const QString &)), this,
          SLOT(openUrlInBrowser(const QString &)));
  connect(_ui->TalipotLabel, SIGNAL(linkActivated(const QString &)), this,
          SLOT(openUrlInBrowser(const QString &)));

  QFile authorsFile(tlpStringToQString(TalipotShareDir + "AUTHORS"));
  QFile licenseFile(tlpStringToQString(TalipotShareDir + "LICENSE"));

  if (authorsFile.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream in(&authorsFile);
    in.setCodec("UTF-8");
    _ui->authorsTextEdit->setText(in.readAll());
  }

  if (licenseFile.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream in(&licenseFile);
    in.setCodec("UTF-8");
    _ui->licenseTextEdit->setText(in.readAll());
  }
}

AboutPage::~AboutPage() {
  delete _ui;
}

void AboutPage::openUrlInBrowser(const QString &url) {
  QDesktopServices::openUrl(QUrl(url));
}
