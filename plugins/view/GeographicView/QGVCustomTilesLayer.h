/**
 *
 * Copyright (C) 2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef QGVCUSTOMTILESLAYER_H
#define QGVCUSTOMTILESLAYER_H

#include <QGeoView/QGVLayerTilesOnline.h>

class QGVCustomTilesLayer : public QGVLayerTilesOnline {

public:
  explicit QGVCustomTilesLayer();
  void setTilesUrl(const QString &tilesUrl) {
    _tilesUrl = tilesUrl;
  }

private:
  int minZoomlevel() const override;
  int maxZoomlevel() const override;
  QString tilePosToUrl(const QGV::GeoTilePos &tilePos) const override;

private:
  QString _tilesUrl;
};

#endif // QGVCUSTOMTILESLAYER_H