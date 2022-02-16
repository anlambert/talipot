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

#ifndef PARALLEL_COORDINATES_GRAPH_PROXY_H
#define PARALLEL_COORDINATES_GRAPH_PROXY_H

#include <talipot/GraphDecorator.h>
#include <talipot/Size.h>
#include <talipot/StableIterator.h>
#include <talipot/StoredType.h>
#include <talipot/TypeInterface.h>

namespace tlp {

class ColorProperty;

class ParallelCoordinatesGraphProxy : public GraphDecorator {

public:
  ParallelCoordinatesGraphProxy(Graph *graph, const ElementType location = ElementType::NODE);
  ~ParallelCoordinatesGraphProxy() override;

  uint getNumberOfSelectedProperties() const;
  bool selectedPropertiesisEmpty() const;
  std::vector<std::string> getAllProperties();
  std::vector<std::string> getSelectedProperties();
  void setSelectedProperties(const std::vector<std::string> &properties);
  void removePropertyFromSelection(const std::string &propertyName);

  ElementType getDataLocation() const;
  void setDataLocation(const ElementType location) {
    dataLocation = location;
  }

  uint getDataCount() const;
  Iterator<uint> *getDataIterator();
  Iterator<uint> *getSelectedDataIterator();
  Iterator<uint> *getUnselectedDataIterator();
  Color getDataColor(const uint dataId);
  std::string getDataTexture(const uint dataId);
  Size getDataViewSize(const uint dataId);
  std::string getDataLabel(const uint dataId);

  bool isDataSelected(const uint dataId);
  void setDataSelected(const uint dataId, const bool dataSelected);
  void resetSelection();

  void deleteData(const uint dataId);

  Graph *getGraph() const {
    return graph_component;
  }

  void setUnhighlightedEltsColorAlphaValue(const uint alpha) {
    unhighlightedEltsColorAlphaValue = alpha;
  }
  uint getUnhighlightedEltsColorAlphaValue() const {
    return unhighlightedEltsColorAlphaValue;
  }
  void addOrRemoveEltToHighlight(const uint eltId);
  void unsetHighlightedElts();
  bool highlightedEltsSet() const;
  bool isDataHighlighted(const uint dataId);
  void selectHighlightedElements();
  void setSelectHighlightedElements(bool val);
  const std::set<uint> &getHighlightedElts() const {
    return highlightedElts;
  }
  void resetHighlightedElts(const std::set<uint> &highlightedData);
  void removeHighlightedElement(const uint dataId);
  bool graphColorsModified() const {
    return graphColorsChanged;
  }
  void colorDataAccordingToHighlightedElts();

  void treatEvents(const std::vector<Event> &) override;

  template <typename Proxytype>
  Proxytype *getLocalProperty(const std::string &name) {
    return graph_component->getLocalProperty<Proxytype>(name);
  }

  template <typename Proxytype>
  Proxytype *getProperty(const std::string &name) {
    return graph_component->getProperty<Proxytype>(name);
  }

  PropertyInterface *getProperty(const std::string &name) const override {
    return graph_component->getProperty(name);
  }

  template <typename PROPERTY, typename PROPERTYTYPE>
  TYPE_CONST_REFERENCE(PROPERTYTYPE)
  getPropertyValueForData(const std::string &propertyName, const uint dataId) {
    if (getDataLocation() == ElementType::NODE) {
      return getProperty<PROPERTY>(propertyName)->getNodeValue(node(dataId));
    } else {
      return getProperty<PROPERTY>(propertyName)->getEdgeValue(edge(dataId));
    }
  }

  template <typename PROPERTY, typename PROPERTYTYPE>
  void setPropertyValueForData(const std::string &propertyName, const uint dataId,
                               const REAL_TYPE(PROPERTYTYPE) & propertyValue) {
    if (getDataLocation() == ElementType::NODE) {
      getProperty<PROPERTY>(propertyName)->setNodeValue(node(dataId), propertyValue);
    } else {
      getProperty<PROPERTY>(propertyName)->setEdgeValue(edge(dataId), propertyValue);
    }
  }

  template <typename PROPERTY, typename PROPERTYTYPE>
  void setPropertyValueForAllData(const std::string &propertyName,
                                  const REAL_TYPE(PROPERTYTYPE) & propertyValue) {
    if (getDataLocation() == ElementType::NODE) {
      getProperty<PROPERTY>(propertyName)->setAllNodeValue(propertyValue);
    } else {
      getProperty<PROPERTY>(propertyName)->setAllEdgeValue(propertyValue);
    }
  }

  template <typename PROPERTY, typename PROPERTYTYPE>
  REAL_TYPE(PROPERTYTYPE)
  getPropertyMinValue(const std::string &propertyName) {
    if (getDataLocation() == ElementType::NODE) {
      return getProperty<PROPERTY>(propertyName)->getNodeMin(graph_component);
    } else {
      return getProperty<PROPERTY>(propertyName)->getEdgeMin(graph_component);
    }
  }

  template <typename PROPERTY, typename PROPERTYTYPE>
  REAL_TYPE(PROPERTYTYPE)
  getPropertyMaxValue(const std::string &propertyName) {
    if (getDataLocation() == ElementType::NODE) {
      return getProperty<PROPERTY>(propertyName)->getNodeMax(graph_component);
    } else {
      return getProperty<PROPERTY>(propertyName)->getEdgeMax(graph_component);
    }
  }

private:
  Color getOriginalDataColor(const uint dataId);

  bool graphColorsChanged;
  ColorProperty *dataColors;
  ColorProperty *originalDataColors;
  std::set<uint> highlightedElts;
  std::vector<std::string> selectedProperties;
  ElementType dataLocation;
  uint unhighlightedEltsColorAlphaValue;
};

}

#endif // PARALLEL_COORDINATES_GRAPH_PROXY_H
