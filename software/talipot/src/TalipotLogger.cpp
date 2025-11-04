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

#include "TalipotLogger.h"

#include "ui_TalipotLogger.h"

#include <QKeyEvent>
#include <QClipboard>
#include <QMenu>

#include <talipot/Settings.h>
#include <talipot/FontIcon.h>
#include <talipot/MaterialDesignIcons.h>

using namespace tlp;

TalipotLogger::TalipotLogger(QWidget *parent)
    : QDialog(parent), _logType(QtDebugMsg), _ui(new Ui::TalipotLogger), _pythonOutput(false),
      _nbLog(0), _emptyIcon(16, 16) {
  _emptyIcon.fill(Qt::transparent);
  _ui->setupUi(this);
  _ui->listWidget->installEventFilter(this);
  _ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  _ui->copyButton->setIcon(FontIcon::icon(MaterialDesignIcons::ContentCopy, QColor(Qt::white)));
  _ui->clearButton->setIcon(FontIcon::icon(MaterialDesignIcons::Broom, QColor(Qt::white)));
  _ui->closeButton->setIcon(FontIcon::icon(MaterialDesignIcons::Close, QColor(Qt::white)));
  _ui->copyButton->setToolTip("Copy the selected lines into the clipboard");
  _ui->clearButton->setToolTip("Remove all messages");
  _ui->closeButton->setToolTip("Close this window");
  connect(_ui->listWidget, &QWidget::customContextMenuRequested, this,
          &TalipotLogger::showContextMenu);
  connect(_ui->copyButton, &QAbstractButton::clicked, this, &TalipotLogger::copy);
  connect(_ui->clearButton, &QAbstractButton::clicked, this, &TalipotLogger::clear);
  connect(_ui->closeButton, &QAbstractButton::clicked, this, &QWidget::hide);
  connect(_ui->anchoredCB, &QAbstractButton::toggled, this, &TalipotLogger::setAnchored);
  _ui->anchoredCB->setChecked(tlp::Settings::loggerAnchored());
}

TalipotLogger::~TalipotLogger() {
  delete _ui;
}

TalipotLogger::LogType TalipotLogger::getLastLogType() const {
  if (_pythonOutput) {
    return Python;
  }

  switch (_logType) {
  case QtDebugMsg:
    return Debug;
  case QtInfoMsg:
    return Info;

  case QtWarningMsg:
    return Warning;

  case QtCriticalMsg:
  case QtFatalMsg:
    return Error;
  }

  return Info;
}

int TalipotLogger::count() const {
  return _ui->listWidget->count();
}

int TalipotLogger::countByType(LogType logType) const {
  return _logCounts[logType];
}

void TalipotLogger::log(QtMsgType type, const QMessageLogContext &, const QString &msg) {
  logImpl(type, msg);
}

void TalipotLogger::logImpl(QtMsgType type, const QString &msg) {

  if (msg.isEmpty()) {
    return;
  }

  if (type == QtFatalMsg) {
    std::cerr << tlp::QStringToTlpString(msg) << std::endl;
    abort();
  }

  _logType = type;
  QString msgClean = msg;

  if (msg.startsWith("[Python")) {
    // remove quotes around message added by Qt
    msgClean = msg.mid(14).mid(2, msg.length() - 17);
    _pythonOutput = true;
  } else {
    _pythonOutput = false;
  }

  LogType lastLogType = getLastLogType();
  QBrush c = (_nbLog % 2 == 0) ? palette().base() : palette().alternateBase();
  auto lines = msgClean.split('\n');
  for (int i = 0; i < lines.count(); ++i) {
    QListWidgetItem *item = nullptr;
    if (i == 0) {
      item = new QListWidgetItem(QIcon(icon(lastLogType)), lines[i]);
    } else {
      item = new QListWidgetItem(QIcon(_emptyIcon), lines[i]);
    }
    item->setBackground(c);
    _ui->listWidget->addItem(item);
    _ui->listWidget->scrollToBottom();
  }
  _logCounts[lastLogType] += 1;
  _nbLog += 1;
}

QIcon TalipotLogger::icon(LogType logType) const {
  QIcon icon;
  switch (logType) {
  case Python:
    icon = FontIcon::icon(MaterialDesignIcons::LanguagePython, QColor(Qt::gray));
    break;

  case Info:
    icon = FontIcon::icon(MaterialDesignIcons::Information, QColor("#407fb2"));
    break;

  case Warning:
    icon = FontIcon::icon(MaterialDesignIcons::Alert, QColor("#e18d2b"));
    break;

  case Error:
    icon = FontIcon::icon(MaterialDesignIcons::MinusCircle, QColor("#c42730"));
    break;
  case Debug:
    icon = FontIcon::icon(MaterialDesignIcons::Bug, QColor("#55aa00"));
    break;
  }

  return icon;
}

void TalipotLogger::clear() {
  _ui->listWidget->clear();
  _logType = QtDebugMsg;
  emit cleared();
  _logCounts[Info] = 0;
  _logCounts[Warning] = 0;
  _logCounts[Error] = 0;
  _logCounts[Python] = 0;
  _logCounts[Debug] = 0;
  _nbLog = 0;
}

void TalipotLogger::copy() {
  QStringList strings;

  for (auto *item : _ui->listWidget->selectedItems()) {
    strings << item->text();
  }

  if (!strings.isEmpty()) {
    QApplication::clipboard()->setText(strings.join("\n"));
  }
}

void TalipotLogger::showContextMenu(const QPoint &pos) {
  QMenu m;
  QAction *clear = m.addAction("Clear content", this, &TalipotLogger::clear);
#if QT_VERSION < QT_VERSION_CHECK(6, 3, 0)
  QAction *copy = m.addAction("Copy", this, &TalipotLogger::copy, QKeySequence::Copy);
  m.addAction("Close", this, &QWidget::close, QKeySequence::Close);
#else
  QAction *copy = m.addAction("Copy", QKeySequence::Copy, this, &TalipotLogger::copy);
  m.addAction("Close", QKeySequence::Close, this, &QWidget::close);
#endif
  copy->setEnabled(_ui->listWidget->count() != 0);
  clear->setEnabled(_ui->listWidget->count() != 0);
  m.exec(_ui->listWidget->mapToGlobal(pos));
}

// catch the copy to clipboard event of the QListWidget and reimplement
// its behaviour in order to be able to copy the text of all the selected rows
// (only the text of the current item is copied otherwise)
bool TalipotLogger::eventFilter(QObject *, QEvent *event) {
  auto *ke = dynamic_cast<QKeyEvent *>(event);

  if (ke && ke->matches(QKeySequence::Copy)) {
    copy();
    return true;
  }

  return false;
}

void TalipotLogger::showEvent(QShowEvent *evt) {
  QDialog::showEvent(evt);

  if (!_windowGeometry.isNull()) {
    restoreGeometry(_windowGeometry);
  }
}

void TalipotLogger::hideEvent(QHideEvent *evt) {

  _windowGeometry = saveGeometry();
  QDialog::hideEvent(evt);
}

void TalipotLogger::setGeometry(int x, int y, int w, int h) {
  setMinimumSize(QSize(0, 0));
  setMaximumSize(QSize(16777215, 16777215));
  QDialog::setGeometry(x, y, w, h);
  _windowGeometry = saveGeometry();

  if (_anchored) {
    setMinimumSize(size());
    setMaximumSize(size());
  }
}

void TalipotLogger::setAnchored(bool anchored) {
  _anchored = anchored;
  bool visible = isVisible();
  QIcon icon;
  QString tooltip;

  if (_anchored) {
    icon = FontIcon::icon(MaterialDesignIcons::WindowRestore, QColor(Qt::white));
    tooltip = "Display the logger in a separate window";
    setAttribute(Qt::WA_X11NetWmWindowTypeDialog, false);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setMinimumSize(size());
    setMaximumSize(size());
    emit resetLoggerPosition();
  } else {
    icon = FontIcon::icon(MaterialDesignIcons::DockBottom, QColor(Qt::white));
    tooltip = "Dock the logger to the bottom of Talipot window";
    setAttribute(Qt::WA_X11NetWmWindowTypeDialog, true);
    setWindowFlags(Qt::Dialog);
    setMinimumSize(QSize(0, 0));
    setMaximumSize(QSize(16777215, 16777215));
  }

  _ui->anchoredCB->setIcon(icon);
  _ui->anchoredCB->setToolTip(tooltip);

  tlp::Settings::setLoggerAnchored(anchored);

  // force the update of the window after modifying its flags
  if (visible) {
    show();
  }
}

void TalipotLogger::updateLogItemsBackgroundColor() {
  for (int i = 0; i < _nbLog; ++i) {
    QBrush c = (i % 2 == 0) ? palette().base() : palette().alternateBase();
    _ui->listWidget->item(i)->setBackground(c);
  }
}
