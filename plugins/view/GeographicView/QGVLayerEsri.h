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

#ifndef QGVLAYERESRI_H
#define QGVLAYERESRI_H

#include <QGeoView/QGVLayerTilesOnline.h>

enum class EsriMapType {
  Satellite,
  Terrain,
  GrayCanvas,
};

class QGVLayerEsri : public QGVLayerTilesOnline {

public:
  explicit QGVLayerEsri(EsriMapType type = EsriMapType::Satellite);

private:
  int minZoomlevel() const override;
  int maxZoomlevel() const override;
  QString tilePosToUrl(const QGV::GeoTilePos &tilePos) const override;

private:
  EsriMapType type;
};

#endif // QGVLAYERESRI_H