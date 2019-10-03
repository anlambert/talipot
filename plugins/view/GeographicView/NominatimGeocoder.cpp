/**
 *
 * Copyright (C) 2019  The Talipot developers
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
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

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
  nominatimSearchUrl.setPath("/search/" + tlpStringToQString(address));
  nominatimSearchUrl.setQuery("format=json&dedupe=1&limit=20");

  QNetworkRequest request;
  request.setUrl(nominatimSearchUrl);

  QNetworkReply *reply = _networkAccessManager->get(request);
  QEventLoop loop;
  QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
  loop.exec();
  QByteArray jsonData = reply->readAll();

  NominatimResultsParser nominatimParser;
  nominatimParser.parse(reinterpret_cast<const unsigned char *>(jsonData.constData()),
                        jsonData.size());

  unsigned int nbResults = nominatimParser.addresses.size();

  vector<NominatimGeocoderResult> ret;

  for (unsigned int i = 0; i < nbResults; ++i) {
    NominatimGeocoderResult result;
    result.address = nominatimParser.addresses[i];
    result.latLng = nominatimParser.latLngs[i];
    ret.push_back(result);
  }

  return ret;
}
