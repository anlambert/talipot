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

#ifndef TALIPOT_FILE_DOWNLOADER_H
#define TALIPOT_FILE_DOWNLOADER_H

#include <talipot/config.h>

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

namespace tlp {

/**
 * @brief Helper class to download the content of a file referenced by an url
 **/
class TLP_QT_SCOPE FileDownloader : public QObject {

    Q_OBJECT

  public:
    FileDownloader();

    /**
     * Synchronously download the content of a file referenced by an url
     * and returns it as a raw byte array.
     *
     */
    const QByteArray &download(const QUrl &url);

  signals:

    void downloaded();

  private slots:

    void fileDownloaded(QNetworkReply *pReply);

  private:
    QNetworkAccessManager _webCtrl;
    QByteArray _downloadedData;
};
}

#endif // TALIPOT_FILE_DOWNLOADER_H
