/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <QEventLoop>
#include <QNetworkReply>

#include <talipot/TlpQtTools.h>
#include <talipot/YajlFacade.h>

#include "NominatimGeocoder.h"

using namespace std;
using namespace tlp;

class NominatimResultsParser : public YajlParseFacade {

public:
  void parseString(const string &value) override {
    if (_currentKey == "display_name") {
      addresses.push_back(value);
    } else if (_currentKey == "lat") {
      _lat = stod(value);
    } else if (_currentKey == "lon") {
      _lng = stod(value);
      latLngs.push_back(make_pair(_lat, _lng));
    }
  }

  void parseMapKey(const string &value) override {
    _currentKey = value;
  }

  vector<string> addresses;
  vector<pair<double, double>> latLngs;

private:
  string _currentKey;
  double _lat;
  double _lng;
};

NominatimGeocoder::NominatimGeocoder() {
  _networkAccessManager = new QNetworkAccessManager();
}

NominatimGeocoder::~NominatimGeocoder() {
  delete _networkAccessManager;
}

vector<NominatimGeocoderResult> NominatimGeocoder::getLatLngForAddress(const string &address) {

  QUrl nominatimSearchUrl;
  nominatimSearchUrl.setScheme("https");
  nominatimSearchUrl.setHost("nominatim.openstreetmap.org");
  nominatimSearchUrl.setPath("/search");
  nominatimSearchUrl.setQuery("q=" + tlpStringToQString(address) +
                              "&format=json&dedupe=1&limit=20");

  QNetworkRequest request;
  request.setUrl(nominatimSearchUrl);

  QNetworkReply *reply = _networkAccessManager->get(request);
  QEventLoop loop;
  QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();
  QByteArray jsonData = reply->readAll();

  NominatimResultsParser nominatimParser;
  nominatimParser.parse(jsonData.constData(), jsonData.size());

  uint nbResults = nominatimParser.addresses.size();

  vector<NominatimGeocoderResult> ret;

  for (uint i = 0; i < nbResults; ++i) {
    NominatimGeocoderResult result;
    result.address = nominatimParser.addresses[i];
    result.latLng = nominatimParser.latLngs[i];
    ret.push_back(result);
  }

  return ret;
}
