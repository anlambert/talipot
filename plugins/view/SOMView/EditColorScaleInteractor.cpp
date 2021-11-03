/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "EditColorScaleInteractor.h"
#include "GlLabelledColorScale.h"

#include <talipot/ColorScaleConfigDialog.h>

#include "SOMView.h"
#include <SOMMap.h>

#include <QMouseEvent>

using namespace tlp;
using namespace std;

EditColorScaleInteractor::EditColorScaleInteractor()
    : currentProperty(nullptr), colorScale(nullptr), widthPercent(.80f), heightPercent(.1f),
      heightPosition(.1f), glWidgetWidth(0), glWidgetHeight(0),
      selectionLayer(new GlLayer("SelectionLayer")) {}

EditColorScaleInteractor::~EditColorScaleInteractor() {
  selectionLayer->clear();
  delete selectionLayer;
  delete colorScale;
}

bool EditColorScaleInteractor::eventFilter(QObject *obj, QEvent *event) {
  auto *glWidget = static_cast<tlp::GlWidget *>(obj);

  if (event->type() == QEvent::MouseButtonDblClick) {
    auto *me = static_cast<QMouseEvent *>(event);

    glWidget->scene()->getGraphCamera().initGl();
    selectionLayer->set2DMode();
    glWidget->scene()->addExistingLayer(selectionLayer);
    selectionLayer->getCamera().initGl();
    selectionLayer->addGlEntity(colorScale, "colorScale");

    std::vector<SelectedEntity> entities;
    glWidget->scene()->selectEntities(RenderingEntities, me->pos().x(), me->pos().y(), 2, 2,
                                      selectionLayer, entities);
    bool foundGlColorScale = false;

    if (!entities.empty()) {
      for (const auto &entity : entities) {
        if (entity.getEntity() == colorScale->getGlColorScale()) {
          ColorScaleConfigDialog dialog(*colorScale->getGlColorScale()->getColorScale(), glWidget);
          foundGlColorScale = true;

          if (dialog.exec()) {
            auto *somView = static_cast<SOMView *>(view());
            // update shared color scale
            somView->getColorScale()->setColorMap(dialog.getColorScale().getColorMap());
            somView->updateDefaultColorProperty();
          }
        }
      }
    }

    // Empty layer without destructing objects.
    selectionLayer->deleteGlEntity(colorScale);
    glWidget->scene()->removeLayer(selectionLayer, false);

    return foundGlColorScale;
  }

  return false;
}
void EditColorScaleInteractor::viewChanged(View *view) {
  auto *somView = static_cast<SOMView *>(view);

  if (somView != nullptr) {
    assert(colorScale == nullptr);
    GlWidget *glWidget = somView->getMapWidget();
    Size screenSize = {glWidget->width() * widthPercent, glWidget->height() * heightPercent};
    Coord bottomLeftScreenCoord = {(glWidget->width() - screenSize.getW()) * .5f,
                                   glWidget->height() * .1f};
    colorScale = new GlLabelledColorScale(bottomLeftScreenCoord, screenSize,
                                          somView->getColorScale(), 0, 0, false);

    propertyChanged(somView, somView->getSelectedProperty(), somView->getSelectedPropertyValues());
  }
}

bool EditColorScaleInteractor::compute(GlWidget *) {
  auto *somView = static_cast<SOMView *>(view());
  assert(somView != nullptr);

  screenSizeChanged(somView);
  return true;
}

bool EditColorScaleInteractor::draw(GlWidget *glWidget) {
  auto *somView = static_cast<SOMView *>(view());
  assert(somView != nullptr);

  if (colorScale) {
    NumericProperty *newProperty = somView->getSelectedPropertyValues();

    if (currentProperty != newProperty) {
      propertyChanged(somView, somView->getSelectedProperty(), newProperty);
    }

    if (colorScale->isVisible()) {
      glWidget->scene()->getGraphCamera().initGl();
      Camera camera2D = Camera(glWidget->scene(), false);
      camera2D.setScene(glWidget->scene());
      camera2D.initGl();

      for (const auto &it : colorScale->getGlEntities()) {
        it.second->draw(0, &camera2D);
      }
    }
  }

  return true;
}

bool EditColorScaleInteractor::screenSizeChanged(SOMView *somView) {
  GlWidget *glWidget = somView->getMapWidget();

  if (glWidget->width() != glWidgetWidth || glWidget->height() != glWidgetHeight) {
    if (colorScale) {
      Size screenSize = {glWidget->width() * widthPercent, glWidget->height() * heightPercent};
      Coord bottomLeftScreenCoord = {(glWidget->width() - screenSize.getW()) * .5f,
                                     glWidget->height() * .1f};
      colorScale->setPosition(bottomLeftScreenCoord);
      colorScale->setSize(screenSize);
      glWidgetWidth = glWidget->width();
      glWidgetHeight = glWidget->height();
    }

    return true;
  }

  return false;
}

void EditColorScaleInteractor::propertyChanged(SOMView *somView, const string &propertyName,
                                               NumericProperty *propertyValues) {
  if (propertyValues) {
    colorScale->setVisible(true);
    // If the input samples are normalized we need to translate it to unormalized values before
    // displaying it.
    double minValue = propertyValues->getNodeDoubleMin(somView->getSOM());
    double maxValue = propertyValues->getNodeDoubleMax(somView->getSOM());
    InputSample &inputSample = somView->getInputSample();
    colorScale->setMinValue(inputSample.isUsingNormalizedValues()
                                ? inputSample.unnormalize(minValue, propertyName)
                                : minValue);
    colorScale->setMaxValue(inputSample.isUsingNormalizedValues()
                                ? inputSample.unnormalize(maxValue, propertyName)
                                : maxValue);
  } else {
    colorScale->setVisible(false);
  }

  currentProperty = propertyValues;
}
