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

#include "QGVCustomTilesLayer.h"

QGVCustomTilesLayer::QGVCustomTilesLayer() {
  setName("Custom Tiles Layer");
}

int QGVCustomTilesLayer::minZoomlevel() const {
  return 0;
}

int QGVCustomTilesLayer::maxZoomlevel() const {
  return 21;
}

QString QGVCustomTilesLayer::tilePosToUrl(const QGV::GeoTilePos &tilePos) const {
  QString url = _tilesUrl;
  url.replace("{z}", QString::number(tilePos.zoom()));
  url.replace("{x}", QString::number(tilePos.pos().x()));
  url.replace("{y}", QString::number(tilePos.pos().y()));
  return url;
}