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

#include <QNetworkAccessManager>
#include <QUrl>
#include <QMap>

#include <talipot/config.h>
#include <talipot/Singleton.h>
#include <talipot/TlpQtTools.h>

class QNetworkReply;

namespace tlp {

class DownloadManager;
DECLARE_DLL_TEMPLATE_INSTANCE(Singleton<DownloadManager>, TLP_QT_TEMPLATE_DECLARE_SCOPE)

class TLP_QT_SCOPE DownloadManager : public QNetworkAccessManager,
                                     public Singleton<DownloadManager> {
    Q_OBJECT

    friend class Singleton<DownloadManager>;

    QList<QNetworkReply *> currentDownloads;
    QMap<QUrl, QString> downloadDestinations;

    DownloadManager();

  public:
    QNetworkReply *downloadPlugin(const QUrl &url, const QString &destination);
    bool saveToDisk(const QString &filename, QIODevice *data);

  public slots:
    void downloadFinished(QNetworkReply *reply);
};
}
