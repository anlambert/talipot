/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "talipot/CaptionItem.h"

#include <talipot/DoubleProperty.h>
#include <talipot/ColorProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/CaptionGraphicsItem.h>
#include <talipot/View.h>

#include <QtGui/QLinearGradient>

using namespace std;

namespace tlp {

CaptionItem::CaptionItem(View *view)
    : view(view), _graph(nullptr), _metricProperty(nullptr), _colorProperty(nullptr),
      _sizeProperty(nullptr), _backupColorProperty(nullptr), _backupBorderColorProperty(nullptr) {
  _captionGraphicsItem = new CaptionGraphicsItem(view);
  connect(_captionGraphicsItem, &CaptionGraphicsItem::filterChanged, this,
          &CaptionItem::applyNewFilter);
  connect(_captionGraphicsItem, &CaptionGraphicsItem::selectedPropertyChanged, this,
          &CaptionItem::selectedPropertyChanged);
}

CaptionItem::~CaptionItem() {
  clearObservers();
}

void CaptionItem::create(CaptionType captionType) {
  _captionType = captionType;
  _captionGraphicsItem->setType(captionType);
  initCaption();

  if (captionType == NodesColorCaption || captionType == EdgesColorCaption) {
    generateColorCaption(captionType);
  } else {
    generateSizeCaption(captionType);
  }

  if (_backupColorProperty) {
    delete _backupColorProperty;
    delete _backupBorderColorProperty;
  }

  _backupColorProperty = new ColorProperty(_graph);
  *_backupColorProperty = *_colorProperty;
  _backupBorderColorProperty = nullptr;
}

void CaptionItem::initCaption() {
  _captionGraphicsItem->loadConfiguration();

  if (_metricProperty) {
    _metricProperty->removeObserver(this);
  }

  _metricProperty = nullptr;

  if (_colorProperty) {
    _colorProperty->removeObserver(this);
  }

  _colorProperty = nullptr;

  if (_sizeProperty) {
    _sizeProperty->removeObserver(this);
  }

  _sizeProperty = nullptr;
}

void CaptionItem::clearObservers() {

  if (_graph != view->graph() && _graph) {
    _graph->removeObserver(this);
  }

  _graph = view->graph();

  if (_graph == nullptr) {
    _metricProperty = nullptr;
    _colorProperty = nullptr;
    _sizeProperty = nullptr;
    return;
  }

  if (_metricProperty) {
    _metricProperty->removeObserver(this);
  }

  if (!_captionGraphicsItem->usedProperty().empty()) {
    _metricProperty = view->graph()->getDoubleProperty(_captionGraphicsItem->usedProperty());
    _metricProperty->addObserver(this);
  } else {
    _metricProperty = nullptr;
  }

  if (_captionType == NodesColorCaption || _captionType == EdgesColorCaption) {
    if (_colorProperty) {
      _colorProperty->removeObserver(this);
    }
  } else {
    if (_sizeProperty) {
      _sizeProperty->removeObserver(this);
    }

    _sizeProperty = view->graph()->getSizeProperty("viewSize");
    _sizeProperty->addObserver(this);
  }

  _colorProperty = view->graph()->getColorProperty("viewColor");

  if (_captionType == NodesColorCaption || _captionType == EdgesColorCaption) {
    _colorProperty->addObserver(this);
  }

  if (_graph) {
    _graph->removeObserver(this);
    _graph->addObserver(this);
  }
}

void CaptionItem::generateColorCaption(CaptionType captionType) {

  clearObservers();

  vector<pair<double, Color>> metricToColorFiltered;
  double minProp = 0;
  double maxProp = 1;
  QLinearGradient activeGradient(QPointF(0, 0), QPointF(0, 160.));
  QLinearGradient hideGradient(QPointF(0, 0), QPointF(0, 160.));

  string propertyName = "empty";

  if (_metricProperty == nullptr) {
    metricToColorFiltered.push_back(pair<double, Color>(0., Color(255, 255, 255, 255)));
    metricToColorFiltered.push_back(pair<double, Color>(1., Color(255, 255, 255, 255)));
  } else {

    map<double, Color> metricToColorMap;

    if (captionType == NodesColorCaption) {
      minProp = _metricProperty->getNodeMin();
      maxProp = _metricProperty->getNodeMax();

      for (auto n : view->graph()->nodes()) {
        metricToColorMap[_metricProperty->getNodeValue(n)] = _colorProperty->getNodeValue(n);
      }

    } else {
      minProp = _metricProperty->getEdgeMin();
      maxProp = _metricProperty->getEdgeMax();

      for (auto e : view->graph()->edges()) {
        metricToColorMap[_metricProperty->getEdgeValue(e)] = _colorProperty->getEdgeValue(e);
      }
    }

    double intervale = (maxProp - minProp) / 50.;
    double nextValue = minProp;

    for (const auto &it : metricToColorMap) {
      if (it.first >= nextValue) {
        metricToColorFiltered.push_back(it);
        nextValue += intervale;
      }
    }

    propertyName = _captionGraphicsItem->usedProperty();
  }

  if (metricToColorFiltered.size() < 2) {
    metricToColorFiltered.push_back(pair<double, Color>(0., Color(255, 255, 255, 255)));
    metricToColorFiltered.push_back(pair<double, Color>(1., Color(255, 255, 255, 255)));
  }

  generateGradients(metricToColorFiltered, activeGradient, hideGradient);
  _captionGraphicsItem->generateColorCaption(activeGradient, hideGradient, propertyName, minProp,
                                             maxProp);
}

void CaptionItem::generateSizeCaption(CaptionType captionType) {

  clearObservers();

  if (!_metricProperty) {
    vector<pair<double, float>> metricToSizeFiltered;
    metricToSizeFiltered.push_back(make_pair(0., 1.));
    metricToSizeFiltered.push_back(make_pair(1., 1.));
    _captionGraphicsItem->generateSizeCaption(metricToSizeFiltered, "empty", 0., 1.);
    return;
  }

  double minProp = _metricProperty->getNodeMin();
  double maxProp = _metricProperty->getNodeMax();
  float maxSize = 0;

  map<double, float> metricToSizeMap;
  vector<pair<double, float>> metricToSizeFiltered;

  if (captionType == NodesSizeCaption) {

    for (auto n : view->graph()->nodes()) {
      metricToSizeMap[_metricProperty->getNodeValue(n)] = _sizeProperty->getNodeValue(n)[0];

      if (maxSize < _sizeProperty->getNodeValue(n)[0]) {
        maxSize = _sizeProperty->getNodeValue(n)[0];
      }
    }

  } else {

    for (auto e : view->graph()->edges()) {
      metricToSizeMap[_metricProperty->getEdgeValue(e)] = _sizeProperty->getEdgeValue(e)[0];

      if (maxSize < _sizeProperty->getEdgeValue(e)[0]) {
        maxSize = _sizeProperty->getEdgeValue(e)[0];
      }
    }
  }

  double intervale = (maxProp - minProp) / 50.;
  double nextValue = minProp;

  for (const auto &it : metricToSizeMap) {
    if (it.first >= nextValue) {
      metricToSizeFiltered.push_back({it.first, it.second / maxSize});
      nextValue += intervale;
    }
  }

  if (metricToSizeFiltered.empty()) {
    metricToSizeFiltered.push_back({minProp, 0});
    metricToSizeFiltered.push_back({maxProp, 0});
  }

  if (metricToSizeFiltered.size() == 1) {
    metricToSizeFiltered.push_back(metricToSizeFiltered[0]);
  }

  if (metricToSizeFiltered.size() < 2) {
    metricToSizeFiltered.clear();
    metricToSizeFiltered.push_back({0., 1.});
    metricToSizeFiltered.push_back({1., 1.});
    _captionGraphicsItem->generateSizeCaption(metricToSizeFiltered, "empty", 0., 1.);
    return;
  }

  _captionGraphicsItem->generateSizeCaption(metricToSizeFiltered,
                                            _captionGraphicsItem->usedProperty(), minProp, maxProp);
}

void CaptionItem::generateGradients(const vector<pair<double, Color>> &metricToColorFiltered,
                                    QGradient &activeGradient, QGradient &hideGradient) {
  double minProp = (*metricToColorFiltered.begin()).first;
  double maxProp = (*(--metricToColorFiltered.end())).first;

  Color color;

  for (const auto &it : metricToColorFiltered) {
    float position = (maxProp - minProp) ? (1. - (it.first - minProp) / (maxProp - minProp)) : 0.0;
    color = it.second;
    activeGradient.setColorAt(position, QColor(color[0], color[1], color[2], 255));
    hideGradient.setColorAt(position, QColor(color[0], color[1], color[2], 100));
  }
}

CaptionGraphicsBackgroundItem *CaptionItem::captionGraphicsItem() {
  return _captionGraphicsItem->getCaptionItem();
}

void CaptionItem::removeObservation(bool remove) {
  if (!remove) {
    _graph->addObserver(this);
    _metricProperty->addObserver(this);

    if (_captionType == NodesColorCaption || _captionType == EdgesColorCaption) {
      _colorProperty->addObserver(this);
    } else {
      _sizeProperty->addObserver(this);
    }
  } else {
    _graph->removeObserver(this);

    if (_metricProperty) {
      _metricProperty->removeObserver(this);
    }

    if (_captionType == NodesColorCaption || _captionType == EdgesColorCaption) {
      _colorProperty->removeObserver(this);
    } else {
      _sizeProperty->removeObserver(this);
    }
  }
}

void CaptionItem::applyNewFilter(float begin, float end) {
  if (_metricProperty == nullptr) {
    return;
  }

  emit filtering(true);
  _graph->removeObserver(this);
  _metricProperty->removeObserver(this);

  if (_captionType == NodesColorCaption || _captionType == EdgesColorCaption) {
    _colorProperty->removeObserver(this);
  } else {
    _sizeProperty->removeObserver(this);
  }

  Observable::holdObservers();

  ColorProperty *borderColorProperty = _graph->getColorProperty("viewBorderColor");

  if (!_backupBorderColorProperty) {
    _backupBorderColorProperty = new ColorProperty(_graph);
    *_backupBorderColorProperty = *borderColorProperty;
  } else {
    *borderColorProperty = *_backupBorderColorProperty;
  }

  *_colorProperty = *_backupColorProperty;

  Color tmp;
  Color borderTmp;

  if (_captionType == NodesColorCaption || _captionType == NodesSizeCaption) {
    double minProp = _metricProperty->getNodeMin();
    double maxProp = _metricProperty->getNodeMax();

    double beginMetric = minProp + (begin * (maxProp - minProp));
    double endMetric = minProp + (end * (maxProp - minProp));

    for (auto nit : view->graph()->nodes()) {
      tmp = Color(_backupColorProperty->getNodeValue(nit));
      borderTmp = Color(_backupBorderColorProperty->getNodeValue(nit));

      if (_metricProperty->getNodeValue(nit) < beginMetric ||
          _metricProperty->getNodeValue(nit) > endMetric) {
        tmp[3] = 25;
        borderTmp[3] = 25;
        _colorProperty->setNodeValue(nit, tmp);
        borderColorProperty->setNodeValue(nit, borderTmp);
      } else {
        tmp[3] = 255;
        borderTmp[3] = 255;
        _colorProperty->setNodeValue(nit, tmp);
        borderColorProperty->setNodeValue(nit, borderTmp);
      }
    }

  } else {
    double minProp = _metricProperty->getEdgeMin();
    double maxProp = _metricProperty->getEdgeMax();

    double beginMetric = minProp + (begin * (maxProp - minProp));
    double endMetric = minProp + (end * (maxProp - minProp));

    for (auto e : view->graph()->edges()) {

      tmp = Color(_backupColorProperty->getEdgeValue(e));
      borderTmp = Color(_backupBorderColorProperty->getEdgeValue(e));

      if (_metricProperty->getEdgeValue(e) < beginMetric ||
          _metricProperty->getEdgeValue(e) > endMetric) {
        tmp[3] = 25;
        borderTmp[3] = 25;
        _colorProperty->setEdgeValue(e, tmp);
        borderColorProperty->setEdgeValue(e, borderTmp);
      } else {
        tmp[3] = 255;
        borderTmp[3] = 255;
        _colorProperty->setEdgeValue(e, tmp);
        borderColorProperty->setEdgeValue(e, borderTmp);
      }
    }
  }

  Observable::unholdObservers();

  _graph->addObserver(this);
  _metricProperty->addObserver(this);

  if (_captionType == NodesColorCaption || _captionType == EdgesColorCaption) {
    _colorProperty->addObserver(this);
  } else {
    _sizeProperty->addObserver(this);
  }

  emit filtering(false);
}

void CaptionItem::treatEvents(const vector<Event> &ev) {

  bool deleteEvent = false;
  bool propertyEvent = false;
  bool graphEvent = false;

  for (const auto &e : ev) {

    auto *prop = dynamic_cast<PropertyInterface *>(e.sender());
    auto *graph = dynamic_cast<Graph *>(e.sender());

    if (e.type() == EventType::TLP_DELETE) {
      deleteEvent = true;
    }

    if (prop) {
      propertyEvent = true;
    }

    if (graph) {
      graphEvent = true;
    }
  }

  if (deleteEvent) {
    create(_captionType);
  }

  if (propertyEvent) {
    if (_captionType == NodesColorCaption || _captionType == EdgesColorCaption) {
      generateColorCaption(_captionType);
    } else {
      generateSizeCaption(_captionType);
    }

    if (_backupColorProperty) {
      delete _backupColorProperty;
    }

    _backupColorProperty = new ColorProperty(_graph);
    *_backupColorProperty = *_colorProperty;
  }

  if (graphEvent) {
    create(_captionType);
  }
}

void CaptionItem::selectedPropertyChanged(string /*propertyName*/) {
  if (_captionType == NodesColorCaption || _captionType == EdgesColorCaption) {
    generateColorCaption(_captionType);
  } else {
    generateSizeCaption(_captionType);
  }

  if (_backupColorProperty) {
    delete _backupColorProperty;
  }

  _backupColorProperty = new ColorProperty(_graph);
  *_backupColorProperty = *_colorProperty;
}
}
