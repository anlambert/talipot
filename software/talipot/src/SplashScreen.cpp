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

#include "SplashScreen.h"

#include <QPainter>
#include <QApplication>
#include <QPropertyAnimation>
#include <QDir>

#include <talipot/Plugin.h>
#include <talipot/TlpQtTools.h>

using namespace tlp;

SplashScreen::SplashScreen(bool debugOutput)
    : PluginLoader(), QSplashScreen(), _fileCounter(0), _debugOutput(debugOutput) {
    setPixmap(QPixmap(QDir(QApplication::applicationDirPath())
                          .absoluteFilePath("../share/talipot/bitmaps/logo.png")));
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    auto *fadeInAnimation = new QPropertyAnimation(this, "windowOpacity");
    fadeInAnimation->setStartValue(0);
    fadeInAnimation->setEndValue(1);
    fadeInAnimation->setDuration(200);
    show();
    fadeInAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void SplashScreen::start(const std::string &path) {
    _title = "Loading...";
    _message = QString();
    repaint();
    if (_debugOutput) {
        tlp::debug() << "Entering " << path << std::endl;
    }
}

void SplashScreen::loading(const std::string &filename) {
    QApplication::processEvents();
    _fileCounter++;
    _message = filename.c_str();
    repaint();
    if (_debugOutput) {
        tlp::debug() << "Loading " << filename << std::endl;
    }
}

void SplashScreen::loaded(const Plugin *plugin, const std::list<Dependency> &) {
    _message = tlp::tlpStringToQString(plugin->name()) + " loaded.";
    if (_debugOutput) {
        tlp::debug() << "  - Plugin '" << plugin->name() << "' registered" << std::endl;
    }
}

void SplashScreen::aborted(const std::string &filename, const std::string &errorMsg) {
    _message = QString("Error loading ") + filename.c_str() + ": " + errorMsg.c_str();
    _errors[filename.c_str()] = errorMsg.c_str();
    tlp::error() << "[Error] Failed to load " << filename << ": " << errorMsg << std::endl;
}

void SplashScreen::finished(bool state, const std::string &msg) {
    _title = "Plugins loaded.";

    if (state) {
        if (_debugOutput) {
            tlp::debug() << "Plugins successfully loaded" << std::endl;
        }
    } else {
        if (_debugOutput) {
            tlp::debug() << msg << std::endl;
        }
    }

    repaint();
}

void SplashScreen::drawContents(QPainter *painter) {
    QSize size(pixmap().size());
    painter->drawPixmap(0, 0, pixmap());
    QRectF messageRect(0, size.height() * 2 / 3, size.width(), size.height() / 3);

    painter->setBrush(Qt::white);
    painter->setPen(Qt::white);
    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(messageRect.x(), messageRect.y() + messageRect.height() / 3,
                      messageRect.width(), messageRect.height() / 3, Qt::AlignHCenter, _title);
    font.setBold(false);
    painter->setFont(font);
    painter->drawText(messageRect.x(), messageRect.y() + messageRect.height() * 2 / 3,
                      messageRect.width(), messageRect.height() / 3, Qt::AlignHCenter, _message);

    QRectF progressRect(messageRect.x() + 10, messageRect.y() + messageRect.height() - 10,
                        messageRect.width() - 20, 5);
    painter->setBrush(Qt::transparent);
    painter->setPen(Qt::white);
    painter->drawRect(progressRect);
    qreal w = progressRect.width();

    if (_numberOfFiles > 0) {
        w = 1. * _fileCounter * progressRect.width() / _numberOfFiles;
    }

    painter->setBrush(Qt::white);
    painter->drawRect(progressRect.x(), progressRect.y(), w, progressRect.height());
}
