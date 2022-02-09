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

#ifndef SOM_PROPERTIES_WIDGET_H
#define SOM_PROPERTIES_WIDGET_H

#include <talipot/Observable.h>
#include <talipot/DataSet.h>

#include <QWidget>
#include <QList>

#include "GradientManager.h"
#include "ComputeSOMWidget.h"

namespace Ui {
class SOMPropertiesWidget;
}

class QButtonGroup;
class QRadioButton;
class QPushButton;
class QAbstractButton;

namespace tlp {

class SOMView;
class ColorScale;
class Graph;
class ColorScalePreview;

class SOMPropertiesWidget : public QWidget, public tlp::Observable {
  Q_OBJECT

  Ui::SOMPropertiesWidget *_ui;

public:
  enum SizeMappingType { NoSizeMapping, RealNodeSizeMapping, DistanceMapping };
  SOMPropertiesWidget(SOMView *view, QWidget *parent = nullptr);
  ~SOMPropertiesWidget() override;

  uint getGridWidth() const;
  uint getGridHeight() const;
  QString getConnectivityLabel() const;
  bool getOppositeConnected() const;
  double getLearningRateValue() const;
  QString getDiffusionRateMethodLabel() const;
  uint getMaxDistanceValue() const;
  double getDiffusionRateValue() const;
  bool getAutoMapping() const;
  bool getLinkColor() const;

  ColorScale *getDefaultColorScale() const {
    return defaultScale;
  }

  ColorScale *getPropertyColorScale(const std::string &propertyName);
  void clearpropertiesConfigurationWidget();
  void addfilter(Graph *g, std::vector<std::string> &propertyFilterType);
  void graphChanged(Graph *graph);
  std::vector<std::string> getSelectedProperties() const;
  unsigned getIterationNumber() const;
  unsigned getConnectivityIndex() const;
  SizeMappingType getSizeMapping() const;
  QList<QWidget *> configurationWidgets() const;

  void treatEvents(const std::vector<Event> &) override;

  /**
   * @brief Return true if using namiation during switching to preview.
   */
  bool useAnimation() const;

  /**
   * @brief Store the configuration widget data properties.
   * Function called during getData function to store the state of the configuration widget.
   * @return The dataset containing the configuration state.
   */
  DataSet getData() const;

  /**
   * @brief Restore saved properties from data set.
   * @param data The previously saved dataset.
   */
  void setData(const DataSet &data);

  QButtonGroup *colorScalingButtonGroup;
  QRadioButton *singleColorScale;
  ColorScalePreview *singleColorScaleGradientPreview;
  QRadioButton *multipleColorScale;
  QPushButton *editGradients;

  QButtonGroup *sizeMappingButtonGroup;
  QRadioButton *noNodeSizeMappingRadioButton;
  QRadioButton *realNodeSizeMappingRadioButton;

  bool multiplePropertiesRepresentation;

protected:
  GradientManager gradientManager;
  ColorScale *defaultScale;
  SOMView *view;
  ComputeSOMWidget *dimensionConfigurationWidget;

protected slots:
  void diffusionMethodChange();
  void scalingMethodChange(QAbstractButton *) const;
};
}
#endif // SOM_PROPERTIES_WIDGET_H
