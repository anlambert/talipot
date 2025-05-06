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

#include <talipot/TlpQtTools.h>
#include <talipot/Release.h>
#include <talipot/OpenGlConfigManager.h>
#include <talipot/PythonVersionChecker.h>
#include <talipot/GlOffscreenRenderer.h>

#include <ogdf/basic/internal/version.h>

#include "AboutDialog.h"
#include "ui_AboutDialog.h"

#include <QFile>
#include <QDesktopServices>
#include <QOpenGLContext>

using namespace tlp;

const QString TalipotRepoUrl = "https://github.com/anlambert/talipot";

QString getSipVersion() {
  return SIP_VERSION;
}

QString getTalipotGitRevision() {
  QFile gitCommitFile(tlpStringToQString(TalipotShareDir + "GIT_COMMIT"));

  if (gitCommitFile.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream in(&gitCommitFile);
    return in.readAll().replace("\n", "");
  }
  return "";
}

QString getCppStandard() {
  return CPP_STANDARD;
}

QString getCppCompilerInfo() {
  return QString(CPP_COMPILER_ID).replace("GNU", "GCC") + QString(" ") + CPP_COMPILER_VERSION;
}

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent, Qt::Window), _ui(new Ui::AboutDialog()) {
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

  _ui->logolabel->setPixmap(QPixmap(tlpStringToQString(TalipotBitmapDir + "/logo.png"))
                                .scaled(200, 200, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
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
      <b> C++ </b> %1
      <br/>
      <a href="https://www.cplusplus.com">https://www.cplusplus.com</a>
    </li>
    <li>
      <b> Qt </b> %2
      <br/>
      <a href="https://www.qt.io">https://www.qt.io</a>
    </li>
    <li>
      <b> OpenGL </b> %3 (from vendor %4)
      <br/>
      <a href="https://www.opengl.org">https://www.opengl.org</a>
    </li>
    <li>
      <b>OGDF</b> v%8 aka the <i>Open Graph Drawing Framework</i>
      <br/>
      <a href="https://ogdf.uos.de">https://ogdf.uos.de</a>
    </li>
    <li>
      <b> Python </b> %5
      <br/>
      <a href="https://www.python.org">https://www.python.org</a>
    </li>
    <li>
      <b> SIP </b> %6
      <br/>
      <a href="https://github.com/Python-SIP/sip">
        https://github.com/Python-SIP/sip
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
      PythonVersionChecker::compiledVersion(), getSipVersion(), getCppCompilerInfo(), OGDF_VERSION);

  if (openGlOk) {
    GlOffscreenRenderer::instance().doneOpenGLContextCurrent();
  }

  _ui->dependenciesInfo->setText(talipotDependenciesInfo);
  connect(_ui->TalipotLabel, &QLabel::linkActivated, this, &AboutDialog::openUrlInBrowser);

  QFile authorsFile(tlpStringToQString(TalipotShareDir + "AUTHORS"));
  QFile licenseFile(tlpStringToQString(TalipotShareDir + "LICENSE"));

  if (authorsFile.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream in(&authorsFile);
    _ui->authorsTextEdit->setText(in.readAll());
  }

  if (licenseFile.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream in(&licenseFile);
    _ui->licenseTextEdit->setText(in.readAll());
  }
}

AboutDialog::~AboutDialog() {
  delete _ui;
}

void AboutDialog::openUrlInBrowser(const QString &url) {
  QDesktopServices::openUrl(QUrl(url));
}
