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

#include <talipot/AboutPage.h>
#include <talipot/TlpQtTools.h>
#include <talipot/Release.h>
#include <talipot/OpenGlConfigManager.h>
#include <talipot/PythonVersionChecker.h>
#include <talipot/GlOffscreenRenderer.h>

#include "ui_AboutPage.h"

#include <QFile>
#include <QDesktopServices>
#include <QOpenGLContext>

namespace tlp {
extern QString getSipVersion();
extern QString getTalipotGitRevision();
extern QString getCppStandard();
extern QString getCppCompilerInfo();
}

const QString TalipotRepoUrl = "https://github.com/anlambert/talipot";

using namespace tlp;

AboutPage::AboutPage(QWidget *parent) : QWidget(parent), _ui(new Ui::AboutPage()) {
  _ui->setupUi(this);

  QString title("Talipot ");
  title += TALIPOT_VERSION;
  QString gitCommit(getTalipotGitRevision());

  if (!gitCommit.isEmpty()) {
    title += QString("<br/>(Git commit: <a href=\"%1/commit/%2\">%3</a>)")
                 .arg(TalipotRepoUrl, gitCommit, gitCommit.mid(0, 7));
  }

  QString titleTemplate = R"(
<html>
  <body>
    <p align="center">
      <span style="font-size: 24pt; font-weight: 600;">%1</span>
    </p>
    <p align="center">
      <a href="%2">%2</a>
    </p>
  </body>
</html>)";

  _ui->logolabel->setPixmap(QPixmap(tlpStringToQString(TalipotBitmapDir + "/logo.png")));
  _ui->TalipotLabel->setText(titleTemplate.arg(title, TalipotRepoUrl));

  bool openGlOk = GlOffscreenRenderer::instance().getOpenGLContext()->isValid();

  if (openGlOk) {
    GlOffscreenRenderer::instance().makeOpenGLContextCurrent();
  }

  QString depInfoTemplate = R"(
<p style="font-size: 12pt">
  This free and open-source software is powered by:
  <ul>
    <li>
      <b> C++ </b> %1:
      <a href="https://www.cplusplus.com/">https://www.cplusplus.com/</a>
    </li>
    <li>
      <b> Qt </b> %2:
      <a href="https://www.qt.io">https://www.qt.io</a>
    </li>
    <li>
      <b> OpenGL </b> %3:
      <a href="https://www.opengl.org">https://www.opengl.org</a>
      <br/>
      (from vendor %4)
    </li>
    <li>
      <b>OGDF</b> v2022.02 (Dogwood):
      <a href="http://www.ogdf.net">http://www.ogdf.net</a>
      <br/>
      aka the Open Graph Drawing Framework
    </li>
    <li>
      <b> Python </b> %5:
      <a href="https://www.python.org">https://www.python.org</a>
    </li>
    <li>
      <b> SIP </b> %6:
      <a href="https://www.riverbankcomputing.com/software/sip/">
        https://www.riverbankcomputing.com/software/sip
      </a>
    </li>
  </ul>
</p>
<p style="font-size: 12pt">
  It has been compiled with %7.
</p>
)";

  QString talipotDependenciesInfo = depInfoTemplate.arg(
      getCppStandard(), tlpStringToQString(qVersion()),
      (openGlOk ? tlpStringToQString(OpenGlConfigManager::getOpenGLVersionString())
                : QString("?.?")),
      (openGlOk ? tlpStringToQString(OpenGlConfigManager::getOpenGLVendor()) : QString("unknown")),
      PythonVersionChecker::compiledVersion(), getSipVersion(), getCppCompilerInfo());

  if (openGlOk) {
    GlOffscreenRenderer::instance().doneOpenGLContextCurrent();
  }

  _ui->dependenciesInfo->setText(talipotDependenciesInfo);
  connect(_ui->dependenciesInfo, &QLabel::linkActivated, this, &AboutPage::openUrlInBrowser);
  connect(_ui->TalipotLabel, &QLabel::linkActivated, this, &AboutPage::openUrlInBrowser);

  QFile authorsFile(tlpStringToQString(TalipotShareDir + "AUTHORS"));
  QFile licenseFile(tlpStringToQString(TalipotShareDir + "LICENSE"));

  if (authorsFile.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream in(&authorsFile);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    in.setCodec("UTF-8");
#endif
    _ui->authorsTextEdit->setText(in.readAll());
  }

  if (licenseFile.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream in(&licenseFile);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    in.setCodec("UTF-8");
#endif
    _ui->licenseTextEdit->setText(in.readAll());
  }
}

AboutPage::~AboutPage() {
  delete _ui;
}

void AboutPage::openUrlInBrowser(const QString &url) {
  QDesktopServices::openUrl(QUrl(url));
}
