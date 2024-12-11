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

#include "QGVLayerGeoPortail.h"

namespace {

const QMap<GeoPortailMapType, QString> URLTemplates = {
    {GeoPortailMapType::Plan, "https://wmts.geopf.fr/wmts?REQUEST=GetTile&SERVICE=WMTS"
                              "&VERSION=1.0.0&STYLE=normal&TILEMATRIXSET=PM&FORMAT=image/"
                              "png&LAYER=GEOGRAPHICALGRIDSYSTEMS.PLANIGNV2&"
                              "TILEMATRIX={z}&TILEROW={y}&TILECOL={x}"},

    {GeoPortailMapType::Satellite, "https://wmts.geopf.fr/wmts?REQUEST=GetTile&SERVICE=WMTS"
                                   "&VERSION=1.0.0&STYLE=normal&TILEMATRIXSET=PM&FORMAT=image/"
                                   "jpeg&LAYER=ORTHOIMAGERY.ORTHOPHOTOS"
                                   "&TILEMATRIX={z}&TILEROW={y}&TILECOL={x}"},

};

}

QGVLayerGeoPortail::QGVLayerGeoPortail(GeoPortailMapType type) : type(type) {
  switch (type) {
  case GeoPortailMapType::Plan:
    setName("Géoportail France Plan");
    setDescription(
        "<a target=\"_blank\" href=\"https://www.geoportail.gouv.fr/\">Géoportail France</a>");

    break;
  case GeoPortailMapType::Satellite:
    setName("Géoportail France Satellite");
    setDescription(
        "<a target=\"_blank\" href=\"https://www.geoportail.gouv.fr/\">Géoportail France</a>");
    break;
  }
}

int QGVLayerGeoPortail::minZoomlevel() const {
  return 2;
}

int QGVLayerGeoPortail::maxZoomlevel() const {
  if (type == GeoPortailMapType::Plan) {
    return 18;
  } else {
    return 19;
  }
}

QString QGVLayerGeoPortail::tilePosToUrl(const QGV::GeoTilePos &tilePos) const {
  QString url = URLTemplates[type];
  url.replace("{z}", QString::number(tilePos.zoom()));
  url.replace("{x}", QString::number(tilePos.pos().x()));
  url.replace("{y}", QString::number(tilePos.pos().y()));
  return url;
}