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

#ifndef NOMINATIM_GEOCODER_H
#define NOMINATIM_GEOCODER_H

#include <string>
#include <vector>

class QNetworkAccessManager;

namespace tlp {

struct NominatimGeocoderResult {
    std::string address;
    std::pair<double, double> latLng;
};

class NominatimGeocoder {

  public:
    NominatimGeocoder();
    ~NominatimGeocoder();

    std::vector<NominatimGeocoderResult> getLatLngForAddress(const std::string &address);

  private:
    QNetworkAccessManager *_networkAccessManager;
};

}

#endif // NOMINATIM_GEOCODER_H
