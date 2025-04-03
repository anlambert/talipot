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

#include "GeolocationConfigWidget.h"
#include "ui_GeolocationConfigWidget.h"

#include <talipot/Graph.h>
#include <talipot/TlpQtTools.h>

using namespace std;
using namespace tlp;

const string viewPropertiesName[] = {
    "viewBorderColor",   "viewBorderWidth", "viewColor",     "viewFont",     "viewLabelColor",
    "viewLabelPosition", "viewLayout",      "viewMetaGraph", "viewRotation", "viewSelection",
    "viewShape",         "viewSize",        "viewTexture",   "viewMetric"};

const uint nbViewProperties = sizeof(viewPropertiesName) / sizeof(string);

const vector<string> graphViewProperties(viewPropertiesName, viewPropertiesName + nbViewProperties);

static vector<string> getGraphPropertiesListAccordingToType(Graph *graph, const string &typeName) {
    vector<string> ret;

    for (const string &propertyName : graph->getProperties()) {
        if (std::find(graphViewProperties.begin(), graphViewProperties.end(), propertyName) ==
            graphViewProperties.end()) {
            if (graph->getProperty(propertyName)->getTypename() == typeName) {
                ret.push_back(propertyName);
            }
        }
    }

    return ret;
}

GeolocationConfigWidget::GeolocationConfigWidget(QWidget *parent)
    : QWidget(parent), _ui(new Ui::GeolocationConfigWidget) {
    _ui->setupUi(this);
    connect(_ui->addressLocRB, &QAbstractButton::toggled, this,
            &GeolocationConfigWidget::enableDisableComboBoxes);
    connect(_ui->latLngRB, &QAbstractButton::toggled, this,
            &GeolocationConfigWidget::enableDisableComboBoxes);
    connect(_ui->genLayoutButton, &QAbstractButton::clicked, this,
            &GeolocationConfigWidget::computeGeoLayout);
    connect(_ui->createLatLngPropsCB, &QAbstractButton::clicked, _ui->resetLatLngValuesCB,
            &QWidget::setEnabled);
}

GeolocationConfigWidget::~GeolocationConfigWidget() {
    delete _ui;
}

void GeolocationConfigWidget::setGraph(Graph *graph) {
    _ui->addressPropCB->clear();
    vector<string> stringProperties = getGraphPropertiesListAccordingToType(graph, "string");

    for (size_t i = 0; i < stringProperties.size(); ++i) {
        _ui->addressPropCB->addItem(tlpStringToQString(stringProperties[i]));

        // set viewLabel as the default address property
        if (stringProperties[i] == "viewLabel") {
            _ui->addressPropCB->setCurrentIndex(i);
        }
    }

    _ui->latPropCB->clear();
    _ui->lngPropCB->clear();
    vector<string> doubleProperties = getGraphPropertiesListAccordingToType(graph, "double");

    for (const auto &prop : doubleProperties) {
        _ui->latPropCB->addItem(tlpStringToQString(prop));
        _ui->lngPropCB->addItem(tlpStringToQString(prop));
    }

    _ui->edgesPathsPropertyCB->clear();
    vector<string> doubleVectorProperties =
        getGraphPropertiesListAccordingToType(graph, "vector<double>");

    for (const auto &prop : doubleVectorProperties) {
        _ui->edgesPathsPropertyCB->addItem(tlpStringToQString(prop));
    }
}

void GeolocationConfigWidget::setLatLngGeoLocMethod(const std::string &latitudePropertyName,
                                                    const std::string &longitudePropertyName) {
    _ui->latLngRB->setChecked(true);
    int latPropIndex = _ui->latPropCB->findText(tlpStringToQString(latitudePropertyName));
    int lngPropIndex = _ui->lngPropCB->findText(tlpStringToQString(longitudePropertyName));

    if (latPropIndex != -1 && lngPropIndex != -1) {
        _ui->latPropCB->setCurrentIndex(latPropIndex);
        _ui->lngPropCB->setCurrentIndex(lngPropIndex);
    }
}

void GeolocationConfigWidget::setEdgesPathsPropertyName(const std::string &edgesPathsPropertyName) {
    int edgesPathsPropertyIndex =
        _ui->edgesPathsPropertyCB->findText(tlpStringToQString(edgesPathsPropertyName));

    if (edgesPathsPropertyIndex != -1) {
        _ui->edgesControlPointsGB->setChecked(true);
        _ui->edgesPathsPropertyCB->setCurrentIndex(edgesPathsPropertyIndex);
    }
}

bool GeolocationConfigWidget::geolocateByAddress() const {
    return _ui->addressLocRB->isChecked();
}

string GeolocationConfigWidget::getAddressGraphPropertyName() const {
    return QStringToTlpString(_ui->addressPropCB->currentText());
}

string GeolocationConfigWidget::getLatitudeGraphPropertyName() const {
    return QStringToTlpString(_ui->latPropCB->currentText());
}

string GeolocationConfigWidget::getLongitudeGraphPropertyName() const {
    return QStringToTlpString(_ui->lngPropCB->currentText());
}

bool GeolocationConfigWidget::createLatAndLngProperties() const {
    return _ui->createLatLngPropsCB->isChecked();
}

bool GeolocationConfigWidget::resetLatAndLngValues() const {
    return _ui->resetLatLngValuesCB->isChecked();
}

void GeolocationConfigWidget::enableDisableComboBoxes() {
    if (_ui->addressLocRB->isChecked()) {
        _ui->addressPropCB->setEnabled(true);
        _ui->createLatLngPropsCB->setEnabled(true);
        _ui->resetLatLngValuesCB->setEnabled(true);
        _ui->latPropCB->setEnabled(false);
        _ui->lngPropCB->setEnabled(false);
        _ui->edgesControlPointsGB->setEnabled(false);
    } else {
        _ui->addressPropCB->setEnabled(false);
        _ui->createLatLngPropsCB->setEnabled(false);
        _ui->resetLatLngValuesCB->setEnabled(false);
        _ui->latPropCB->setEnabled(true);
        _ui->lngPropCB->setEnabled(true);
        _ui->edgesControlPointsGB->setEnabled(true);
    }
}

bool GeolocationConfigWidget::useEdgesPaths() const {
    return _ui->edgesControlPointsGB->isChecked();
}

std::string GeolocationConfigWidget::getEdgesPathsPropertyName() const {
    return QStringToTlpString(_ui->edgesPathsPropertyCB->currentText());
}
