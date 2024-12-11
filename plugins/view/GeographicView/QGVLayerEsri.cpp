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

#include "QGVLayerEsri.h"

namespace {

const QString esriBaseUrl = "https://server.arcgisonline.com/ArcGIS/rest/services/";

const QMap<EsriMapType, QString> URLTemplates = {
    {EsriMapType::Satellite, esriBaseUrl + "World_Imagery/MapServer/tile/{z}/{y}/{x}"},
    {EsriMapType::Terrain, esriBaseUrl + "World_Topo_Map/MapServer/tile/{z}/{y}/{x}"},
    {EsriMapType::GrayCanvas,
     esriBaseUrl + "Canvas/World_Light_Gray_Base/MapServer/tile/{z}/{y}/{x}"}};
}

QGVLayerEsri::QGVLayerEsri(EsriMapType type) : type(type) {
  switch (type) {
  case EsriMapType::Satellite:
    setName("Esri Satellite");
    setDescription("Tiles &copy; Esri &mdash; Source: Esri, i-cubed, USDA, USGS, AEX, GeoEye, "
                   "Getmapping, Aerogrid, IGN, IGP, UPR-EGP, and the GIS User Community");

    break;
  case EsriMapType::Terrain:
    setName("Esri Terrain");
    setDescription("Tiles &copy; Esri &mdash; Esri, DeLorme, NAVTEQ, TomTom, Intermap, iPC, "
                   "USGS, FAO, NPS, NRCAN, GeoBase, Kadaster NL, Ordnance Survey, Esri Japan, "
                   "METI, Esri China (Hong Kong), and the GIS User Community");
    break;
  case EsriMapType::GrayCanvas:
    setName("Esri Gray Canvas");
    setDescription("Tiles &copy; Esri &mdash; Esri, DeLorme, NAVTEQ");
    break;
  }
}

int QGVLayerEsri::minZoomlevel() const {
  return 0;
}

int QGVLayerEsri::maxZoomlevel() const {
  if (type == EsriMapType::GrayCanvas) {
    return 16;
  } else {
    return 21;
  }
}

QString QGVLayerEsri::tilePosToUrl(const QGV::GeoTilePos &tilePos) const {
  QString url = URLTemplates[type];
  url.replace("{z}", QString::number(tilePos.zoom()));
  url.replace("{x}", QString::number(tilePos.pos().x()));
  url.replace("{y}", QString::number(tilePos.pos().y()));
  return url;
}