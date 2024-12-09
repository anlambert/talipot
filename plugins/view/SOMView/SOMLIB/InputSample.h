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

#ifndef INPUT_SAMPLE_H
#define INPUT_SAMPLE_H

#include <talipot/hash.h>
#include <vector>
#include <talipot/Graph.h>
#include <talipot/Observable.h>
#include <talipot/NumericProperty.h>
#include "DynamicVector.h"

/**
 * Handle all the the samples data used to train the SOM. Take a graph and a vector of
 *NumericProperties to use it as sample data.
 * This object can handle standard or normalized values.
 **/
namespace tlp {
class InputSample : public tlp::Observable {
public:
  InputSample(tlp::Graph *graph = nullptr);
  InputSample(tlp::Graph *graph, const std::vector<std::string> &propertiesToListen);

  void setGraph(tlp::Graph *graph);
  void setGraph(tlp::Graph *graph, const std::vector<std::string> &propertiesToListen);

  void setPropertiesToListen(const std::vector<std::string> &propertiesToListen);
  std::vector<std::string> getListenedProperties();
  ~InputSample() override;

  const DynamicVector<double> &getWeight(tlp::node n);

  tlp::Iterator<tlp::node> *getNodes();
  tlp::Iterator<tlp::node> *getRandomNodeOrder();

  unsigned int getSampleSize() const {
    assert(rootGraph);
    return rootGraph->numberOfNodes();
  }
  unsigned int getDimensionOfSample() const {
    return propertiesList.size();
  }
  tlp::Graph *getGraph() {
    return rootGraph;
  }

  void treatEvents(const std::vector<Event> &events) override;

  void addNode(tlp::Graph *, const tlp::node);
  void delNode(tlp::Graph *, const tlp::node);
  void delLocalProperty(tlp::Graph *, const std::string &);

  double getMeanProperty(const std::string &propertyName);
  double getSDProperty(const std::string &propertyName);

  bool isUsingNormalizedValues() const;
  void setUsingNormalizedValues(bool);

  /**
   * Search the index of the property with the given name.
   **/
  unsigned findIndexForProperty(const std::string &propertyName) const;

  // recover normalized value for val according to meanProperties[propNum] and sdProperties[propNum]
  double normalize(double val, unsigned propNum);
  // convenience function
  double normalize(double val, const std::string &propertyName) {
    return normalize(val, findIndexForProperty(propertyName));
  }
  // recover unnormalized value for val according to meanProperties[propNum] and
  // sdProperties[propNum]
  double unnormalize(double val, unsigned propNum);
  double unnormalize(double val, const std::string &propertyName) {
    return unnormalize(val, findIndexForProperty(propertyName));
  }

protected:
  void initGraphObs();
  void clearGraphObs();

  void initPropertiesObs();
  void clearPropertiesObs();

  void buildPropertyVector(const std::vector<std::string> &propertiesToListen);

  // Compute the mean value of propertiesList[i] and store it in meanProperties[i]
  void updateMeanValue(unsigned int);
  // Compute the standard deviation value of propertiesList[i] and store it in sdProperties[i]
  void updateSDValue(unsigned int);
  // Compute the mean values of all propertiesList and store them in meanProperties
  void updateAllMeanValues();
  // Compute the standard deviation values of all propertiesList and store them in sdProperties
  void updateAllSDValues();

  // void buildNodeVector(unsigned int i);
  void buildNodeVector(tlp::node n);

  tlp::node getNodeNumber(unsigned int i);
  unsigned int getNumberForNode(tlp::node n);

  // Data sources
  tlp::Graph *rootGraph;

  // Cache for data avoid to create a somVector each time we need
  // tlp::MutableContainer<DynamicVector<double> > mWeightTab;
  flat_hash_map<unsigned int, DynamicVector<double>> mWeightTab;

  // Container used to store random list
  std::vector<tlp::node> randomVector;

  // The number of properties to listen
  std::vector<std::string> propertiesNameList;
  std::vector<tlp::NumericProperty *> propertiesList;

  // The mean of each property
  std::vector<double> meanProperties;
  // The standard deviation of each property
  std::vector<double> sdProperties;

  // Indicates whether or not normalized values should be used
  bool usingNormalizedValues;
};
}
#endif // INPUT_SAMPLE_H
