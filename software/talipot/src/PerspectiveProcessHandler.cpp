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

#include "PerspectiveProcessHandler.h"

#include <QDir>
#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QPainter>
#include <QStandardPaths>
#include <QTcpSocket>
#include <talipot/Project.h>

#include <ctime>
#include <iostream>
#include <CrashHandling.h>

#include "PerspectiveCrashHandler.h"
#include "MainWindow.h"

#ifdef _WIN32
#include <windows.h>
#endif

SelectionButton::SelectionButton(QWidget *parent) : QPushButton(parent) {}
void SelectionButton::paintEvent(QPaintEvent *e) {
  QPushButton::paintEvent(e);
  QPainter p(this);
  QPixmap pixmap(":/talipot/app/ui/list_bullet_arrow.png");
  p.drawPixmap(10, height() / 2 - pixmap.height() / 2, pixmap);
}

PerspectiveProcessHandler *PerspectiveProcessHandler::_instance = nullptr;

PerspectiveProcessHandler::PerspectiveProcessHandler() {
  listen(QHostAddress::LocalHost);
  connect(this, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
  QFile f(QDir(QStandardPaths::standardLocations(QStandardPaths::TempLocation).at(0))
              .filePath("talipot.lck"));
  f.open(QIODevice::WriteOnly);
  f.write(QString::number(serverPort()).toUtf8());
  f.flush();
  f.close();
}

QProcess *PerspectiveProcessHandler::fromId(unsigned int id) {
  for (auto k : _processInfo.keys()) {
    if (_processInfo[k]._perspectiveId == static_cast<time_t>(id))
      return k;
  }

  return nullptr;
}

PerspectiveProcessHandler *PerspectiveProcessHandler::instance() {
  if (!_instance) {
    _instance = new PerspectiveProcessHandler;
  }

  return _instance;
}

void PerspectiveProcessHandler::createPerspective(const QString &perspective, const QString &file,
                                                  const QVariantMap &parameters) {
  QStringList args;

  if (!perspective.isEmpty())
    args << "--perspective=" + perspective;

  if (!file.isEmpty())
    args << file;

  for (const QString &k : parameters.keys())
    args << "--" + k + "=" + parameters[k].toString();

  args << "--port=" + QString::number(serverPort());
  time_t perspectiveId = time(nullptr);
  args << "--id=" + QString::number(perspectiveId);

  QDir appDir(QApplication::applicationDirPath());

  QProcess *process = new QProcess;
#ifdef WIN32
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("STDERR_NO_ANSI_ESCAPES", "1");
  process->setProcessEnvironment(env);
#endif
  connect(process, SIGNAL(error(QProcess::ProcessError)), this,
          SLOT(perspectiveCrashed(QProcess::ProcessError)));
  connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this,
          SLOT(perspectiveFinished(int, QProcess::ExitStatus)));
  process->setProcessChannelMode(QProcess::ForwardedChannels);
  process->start(appDir.absoluteFilePath("talipot_perspective"), args);
  _processInfo[process] = PerspectiveProcessInfo(perspective, parameters, file, perspectiveId);
}

void PerspectiveProcessHandler::perspectiveCrashed(QProcess::ProcessError) {
  QProcess *process = static_cast<QProcess *>(sender());
  process->setReadChannel(QProcess::StandardError);
  PerspectiveProcessInfo info = _processInfo[process];

  PerspectiveCrashHandler crashHandler;

  QRegExp plateform("^" + QString(TLP_PLATEFORM_HEADER) + " (.*)\n"),
      arch("^" + QString(TLP_ARCH_HEADER) + " (.*)\n"),
      compiler("^" + QString(TLP_COMPILER_HEADER) + " (.*)\n"),
      version("^" + QString(TLP_VERSION_HEADER) + " (.*)\n");

  // TODO: replace reading process by reading file
  QFile f(QDir(QStandardPaths::standardLocations(QStandardPaths::TempLocation).at(0))
              .filePath("talipot_perspective-" + QString::number(info._perspectiveId) + ".log"));
  f.open(QIODevice::ReadOnly);

  QMap<QRegExp *, QString> envInfo;
  envInfo[&plateform] = "";
  envInfo[&arch] = "";
  envInfo[&compiler] = "";
  envInfo[&version] = "";

  QString stackTrace;
  bool grabStackTrace = false;

  while (!f.atEnd()) {
    QString line(f.readLine());

    if (line.startsWith(TLP_STACK_BEGIN_HEADER)) {
      grabStackTrace = true;
      continue;
    } else if (line.startsWith(TLP_STACK_END_HEADER)) {
      grabStackTrace = false;
      continue;
    }

    if (grabStackTrace) {
      stackTrace += line;
    } else {
      for (auto re : envInfo.keys()) {
        if (re->exactMatch(line)) {
          envInfo[re] = re->cap(1);
          break;
        }
      }
    }
  }

  f.close();
  f.remove();

  // it may happens that there is nothing to show
  // (perhaps if our signal handler was not called)
  if (stackTrace.isEmpty())
    return;

  crashHandler.setEnvData(envInfo[&plateform], envInfo[&arch], envInfo[&compiler],
                          envInfo[&version], stackTrace);
  crashHandler.setPerspectiveData(info);
  crashHandler.exec();
}

void PerspectiveProcessHandler::perspectiveFinished(int, QProcess::ExitStatus) {
  QProcess *process = static_cast<QProcess *>(sender());
  delete process;
  _processInfo.remove(process);
}

void PerspectiveProcessHandler::acceptConnection() {
  QTcpSocket *socket = nextPendingConnection();
  connect(socket, SIGNAL(readyRead()), this, SLOT(perspectiveReadyRead()));
  connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

void PerspectiveProcessHandler::perspectiveReadyRead() {
  QTcpSocket *socket = static_cast<QTcpSocket *>(sender());
  QString data(QString::fromUtf8(socket->readAll()));
  QStringList tokens = data.split("\t");
  QString args = QString(data).remove(0, tokens[0].length() + 1);  // arguments except first one
  QString args2 = QString(args).remove(0, tokens[1].length() + 1); // arguments except two firsts

  if (tokens[0] == "SHOW_AGENT") {
    if (tokens[1] == "PLUGINS")
      emit showPluginsAgent();
    else if (tokens[1] == "PROJECTS")
      emit showProjectsAgent();
    else if (tokens[1] == "ABOUT")
      emit showAboutAgent();
  } else if (tokens[0] == "ERROR_MESSAGE")
    emit showErrorMessage(tokens[1], args2);
  else if (tokens[0] == "TRAY_MESSAGE")
    emit showTrayMessage(args);
  else if (tokens[0] == "OPEN_PROJECT") {
    emit openProject(args);
  } else if (tokens[0] == "OPEN_PROJECT_WITH") {
    emit openProjectWith(args2, tokens[1]);
  } else if (tokens[0] == "CREATE_PERSPECTIVE")
    emit openPerspective(args);
  else if (tokens[0] == "PROJECT_LOCATION") {
    QProcess *p = fromId(tokens[1].toUInt());

    if (p != nullptr) {
      PerspectiveProcessInfo info = _processInfo[p];
      info.projectPath = args2;
      _processInfo[p] = info;
    }
  }
}