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

#include "talipot/SceneConfigWidget.h"

#include <talipot/GlGraph.h>
#include <talipot/GlWidget.h>
#include <talipot/GraphPropertiesModel.h>

#include "ui_SceneConfigWidget.h"

using namespace tlp;

SceneConfigWidget::SceneConfigWidget(QWidget *parent)
    : QWidget(parent), _ui(new Ui::SceneConfigWidget), _glWidget(nullptr), _resetting(false) {
  _ui->setupUi(this);

  connect(_ui->dynamicFontSizeRB, &QAbstractButton::toggled, this,
          &SceneConfigWidget::dynamicFontRBToggled);
  connect(_ui->labelSizesRangeSlider, &RangeSlider::lowerValueChanged, this,
          &SceneConfigWidget::applySettings);
  connect(_ui->labelSizesRangeSlider, &RangeSlider::upperValueChanged, this,
          &SceneConfigWidget::applySettings);
  connect(_ui->labelsDensitySlider, &QSlider::valueChanged, this,
          &SceneConfigWidget::applySettings);
  _ui->labelSizesRangeSlider->setHandleMovementMode(RangeSlider::NoCrossing);
  _ui->selectionColorButton->setDialogTitle("Choose the color of selected nodes or edges");
  _ui->backgroundColorButton->setDialogTitle("Choose the background color");

  _ui->labelsDisabledLabel->installEventFilter(this);
  _ui->labelsNoOverlapLabel->installEventFilter(this);
  _ui->labelsShowAllLabel->installEventFilter(this);
}

SceneConfigWidget::~SceneConfigWidget() {
  delete _ui->labelsOrderingCombo->model();
  delete _ui;
}

void SceneConfigWidget::setGlWidget(tlp::GlWidget *glWidget) {
  if (_glWidget != nullptr) {
    disconnect(_glWidget, &GlWidget::graphChanged, this, &SceneConfigWidget::resetChanges);
    disconnect(_glWidget, &GlWidget::viewDrawn, this, &SceneConfigWidget::resetChanges);
  }

  _glWidget = glWidget;

  if (_glWidget != nullptr) {
    connect(_glWidget, &GlWidget::graphChanged, this, &SceneConfigWidget::resetChanges);
    // we assume that if an outside action causes an update of one
    // of the rendering parameters managed in this widget, necessarily
    // a call to _glWidget->draw will follow
    connect(_glWidget, &GlWidget::viewDrawn, this, &SceneConfigWidget::resetChanges);
  }

  resetChanges();
}

void SceneConfigWidget::resetChanges() {
  _resetting = true;

  _ui->scrollArea->setEnabled(_glWidget != nullptr);

  if (_glWidget == nullptr || _glWidget->scene()->glGraph() == nullptr ||
      _glWidget->scene()->glGraph()->graph() == nullptr) {
    return;
  }

  Graph *graph = _glWidget->scene()->glGraph()->graph();
  const GlGraphRenderingParameters &renderingParameters = _glWidget->renderingParameters();

  // NODES
  delete _ui->labelsOrderingCombo->model();
  auto *model = new GraphPropertiesModel<NumericProperty>("Disable ordering", graph);
  _ui->labelsOrderingCombo->setModel(model);

  if (renderingParameters.getElementOrderingProperty() == nullptr) {
    _ui->labelsOrderingCombo->setCurrentIndex(0);
  } else {
    _ui->labelsOrderingCombo->setCurrentIndex(
        model->rowOf(renderingParameters.getElementOrderingProperty()));
  }

  _ui->descendingCB->setChecked(renderingParameters.isElementOrderedDescending());

  _ui->labelsFitCheck->setChecked(renderingParameters.isLabelScaled());
  _ui->labelsBillboardedCheck->setChecked(renderingParameters.getLabelsAreBillboarded());
  _ui->fixedFontSizeRB->setChecked(renderingParameters.isLabelFixedFontSize());
  _ui->dynamicFontSizeRB->setChecked(!renderingParameters.isLabelFixedFontSize());
  _ui->labelsDensitySlider->setValue(renderingParameters.getLabelsDensity());
  _ui->labelSizesRangeSlider->setLowerValue(renderingParameters.getMinSizeOfLabel());
  _ui->labelSizesRangeSlider->setUpperValue(renderingParameters.getMaxSizeOfLabel());

  // EDGES
  _ui->edges3DCheck->setChecked(renderingParameters.isEdge3D());
  _ui->edgesArrowCheck->setChecked(renderingParameters.isViewArrow());
  _ui->edgesColorInterpolationCheck->setChecked(renderingParameters.isEdgeColorInterpolate());
  _ui->edgesSizeInterpolationCheck->setChecked(renderingParameters.isEdgeSizeInterpolate());
  _ui->edgesFrontCheck->setChecked(renderingParameters.isEdgeFrontDisplay());

  // COLORS
  _ui->backgroundColorButton->setColor(_glWidget->scene()->getBackgroundColor());
  _ui->selectionColorButton->setColor(renderingParameters.getSelectionColor());

  // PROJECTION
  if (_glWidget->scene()->isViewOrtho()) {
    _ui->orthoRadioButton->setChecked(true);
  } else {
    _ui->centralRadioButton->setChecked(true);
  }

  // GRAPH CHANGING
  if (_glWidget->keepScenePointOfViewOnSubgraphChanging()) {
    _ui->keepSceneRadioButton->setChecked(true);
  } else {
    _ui->centerSceneRadioButton->setChecked(true);
  }

  //  QApplication::processEvents();
  _resetting = false;
}

bool SceneConfigWidget::eventFilter(QObject *obj, QEvent *ev) {
  if (ev->type() == QEvent::MouseButtonPress) {
    if (obj == _ui->labelsDisabledLabel) {
      _ui->labelsDensitySlider->setValue(-100);
      applySettings();
    } else if (obj == _ui->labelsNoOverlapLabel) {
      _ui->labelsDensitySlider->setValue(0);
      applySettings();
    } else if (obj == _ui->labelsShowAllLabel) {
      _ui->labelsDensitySlider->setValue(100);
      applySettings();
    }

    return true;
  }

  return false;
}

void SceneConfigWidget::applySettings() {
  if (_resetting || !_glWidget->scene()->glGraph()) {
    return;
  }

  GlGraphRenderingParameters &renderingParameters = _glWidget->renderingParameters();

  // NODES
  if (_ui->labelsOrderingCombo->currentIndex() == 0) {
    renderingParameters.setElementOrderingProperty(nullptr);
  } else {
    auto *model =
        static_cast<GraphPropertiesModel<NumericProperty> *>(_ui->labelsOrderingCombo->model());
    renderingParameters.setElementOrderingProperty(
        dynamic_cast<NumericProperty *>(model->index(_ui->labelsOrderingCombo->currentIndex(), 0)
                                            .data(Model::PropertyRole)
                                            .value<PropertyInterface *>()));
  }

  renderingParameters.setElementOrderedDescending(_ui->descendingCB->isChecked());

  renderingParameters.setLabelScaled(_ui->labelsFitCheck->isChecked());
  renderingParameters.setLabelsAreBillboarded(_ui->labelsBillboardedCheck->isChecked());
  renderingParameters.setLabelFixedFontSize(_ui->fixedFontSizeRB->isChecked());
  renderingParameters.setLabelsDensity(_ui->labelsDensitySlider->value());
  renderingParameters.setMinSizeOfLabel(_ui->labelSizesRangeSlider->lowerValue());
  renderingParameters.setMaxSizeOfLabel(_ui->labelSizesRangeSlider->upperValue());

  // EDGES
  renderingParameters.setEdge3D(_ui->edges3DCheck->isChecked());
  renderingParameters.setViewArrow(_ui->edgesArrowCheck->isChecked());
  renderingParameters.setEdgeColorInterpolate(_ui->edgesColorInterpolationCheck->isChecked());
  renderingParameters.setEdgeSizeInterpolate(_ui->edgesSizeInterpolationCheck->isChecked());
  renderingParameters.setEdgeFrontDisplay(_ui->edgesFrontCheck->isChecked());

  // COLORS
  renderingParameters.setSelectionColor(_ui->selectionColorButton->talipotColor());
  _glWidget->scene()->setBackgroundColor(_ui->backgroundColorButton->talipotColor());

  // PROJECTION
  _glWidget->scene()->setViewOrtho(_ui->orthoRadioButton->isChecked());

  // GRAPH CHANGINS
  _glWidget->setKeepScenePointOfViewOnSubgraphChanging(_ui->keepSceneRadioButton->isChecked());

  _glWidget->draw();
  emit settingsApplied();
}

void SceneConfigWidget::dynamicFontRBToggled(bool state) {
  _ui->sizeLimitsGB->setEnabled(state);
}
