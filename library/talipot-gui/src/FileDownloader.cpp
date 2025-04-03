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

#include "talipot/FileDownloader.h"

#include <QEventLoop>

using namespace tlp;

FileDownloader::FileDownloader() : QObject() {
    connect(&_webCtrl, &QNetworkAccessManager::finished, this, &FileDownloader::fileDownloaded);
}

void FileDownloader::fileDownloaded(QNetworkReply *pReply) {
    _downloadedData = pReply->readAll();
    pReply->deleteLater();
    emit downloaded();
}

const QByteArray &FileDownloader::download(const QUrl &url) {
    QNetworkRequest request(url);
    _webCtrl.get(request);
    QEventLoop loop;
    connect(this, &FileDownloader::downloaded, &loop, &QEventLoop::quit);
    loop.exec();
    return _downloadedData;
}
