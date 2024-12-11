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

#ifndef QGVLAYERGEOPORTAIL_H
#define QGVLAYERGEOPORTAIL_H

#include <QGeoView/QGVLayerTilesOnline.h>

enum class GeoPortailMapType {
  Plan,
  Satellite,
};

class QGVLayerGeoPortail : public QGVLayerTilesOnline {

public:
  explicit QGVLayerGeoPortail(GeoPortailMapType type = GeoPortailMapType::Plan);

private:
  int minZoomlevel() const override;
  int maxZoomlevel() const override;
  QString tilePosToUrl(const QGV::GeoTilePos &tilePos) const override;

private:
  GeoPortailMapType type;
};

#endif // QGVLAYERGEOPORTAIL_H