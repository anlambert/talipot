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

#include "ThresholdInteractor.h"
#include "SOMView.h"
#include "GlLabelledColorScale.h"
#include <SOMMap.h>

#include <talipot/GlBoundingBoxSceneVisitor.h>
#include <talipot/GlTextureManager.h>
#include <talipot/ColorScale.h>
#include <talipot/GlQuad.h>
#include <talipot/GlRect.h>

#include <QMouseEvent>

using namespace tlp;
using namespace std;

void drawComposite(GlComposite *composite, float lod, Camera *camera) {
  for (const auto &entity : composite->getGlEntities()) {
    entity.second->draw(lod, camera);
  }
}

ColorScaleSlider::ColorScaleSlider(SliderWay way, Size size, GlLabelledColorScale *colorScale,
                                   const string &textureName)
    : way(way), size(size), linkedSlider(nullptr), linkedScale(colorScale), currentShift(0) {

  buildComposite(textureName);
  linkedScale->getGlColorScale()->getColorScale()->addObserver(this);
}

double ColorScaleSlider::getValue() {
  return linkedScale->getMinValue() +
         currentShift * (linkedScale->getMaxValue() - linkedScale->getMinValue());
}

ColorScaleSlider::~ColorScaleSlider() {
  reset(true);
}

void ColorScaleSlider::buildComposite(const std::string &textureName) {
  ostringstream oss;
  Coord colorScaleCoord = linkedScale->getGlColorScale()->getBaseCoord();
  float Ypos = colorScaleCoord.getY() - linkedScale->getGlColorScale()->getThickness() * .5;

  if (way == ToLeft) {
    oss << linkedScale->getMaxValue();
    position.set(colorScaleCoord.getX() + linkedScale->getGlColorScale()->getLength(), Ypos,
                 colorScaleCoord.getZ());
    currentShift = 1;
  } else {
    oss << linkedScale->getMinValue();
    position.set(colorScaleCoord.getX(), Ypos, colorScaleCoord.getZ());
    currentShift = 0;
  }

  float arrowLen = size.getW() * 0.25;

  Size labelSize = {size.getW(), size.getH()};
  vector<Color> fillColors;
  fillColors.insert(fillColors.begin(), 3, linkedScale->getGlColorScale()->getColorAtPos(position));

  vector<Coord> points;
  points.push_back(position);
  points.push_back(Coord(position.getX() - (size.getW() * 0.5), position.getY() - arrowLen));
  points.push_back(Coord(position.getX() + (size.getW() * 0.5), position.getY() - arrowLen));

  Coord p1 = Coord(points[2].getX(), position.getY() - size.getH(), 0);
  Coord p2 = Coord(points[1].getX(), position.getY() - size.getH(), 0);

  rect = new GlQuad(p1, p2, points[1], points[2], Color(255, 255, 255));
  Coord labelPosition = {position.getX(), p1.getY() + (points[1].getY() - p1.getY()) * 0.5f};
  rect->setTextureName(textureName);
  arrow = new GlPolygon(points, fillColors, fillColors, true, false);
  addGlEntity(arrow, "arrow");
  addGlEntity(rect, "frame");
  label = new GlLabel(labelPosition, labelSize, Color(0, 0, 0));
  addGlEntity(label, "label");
  label->setText(oss.str());

  computeBoundingBox();
}

void ColorScaleSlider::setLinkedSlider(ColorScaleSlider *linkedSlider) {
  if (!linkedSlider) {
    this->linkedSlider = nullptr;
  } else {
    if (way == ToLeft) {
      if (linkedSlider->getBasePosition().getX() <= position.getX()) {
        this->linkedSlider = linkedSlider;
      } else {
        this->linkedSlider = nullptr;
        std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << " "
                  << "Invalid linked slider bad coordinates" << std::endl;
      }
    } else {
      if (linkedSlider->getBasePosition().getX() >= position.getX()) {
        this->linkedSlider = linkedSlider;
      } else {
        this->linkedSlider = nullptr;
        std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << " "
                  << "Invalid linked slider bad coordinates" << std::endl;
      }
    }
  }
}

void ColorScaleSlider::computeBoundingBox() {
  tlp::GlBoundingBoxSceneVisitor visitor(nullptr);
  acceptVisitor(&visitor);
  boundingBox = visitor.getBoundingBox();
}

void ColorScaleSlider::setColor(Color c) {
  arrow->setFillColor(c);
}

float ColorScaleSlider::getLeftBound() {
  if (way == ToRight || linkedSlider == nullptr) {
    return 0;
  } else {
    return linkedSlider->getCurrentShift();
  }
}
float ColorScaleSlider::getRightBound() {
  if (way == ToLeft || linkedSlider == nullptr) {
    return 1;
  } else {
    return linkedSlider->getCurrentShift();
  }
}

void ColorScaleSlider::setValue(double value) {
  if (value >= linkedScale->getMinValue() && value <= linkedScale->getMaxValue()) {
    currentShift = (value - linkedScale->getMinValue()) /
                   (linkedScale->getMaxValue() - linkedScale->getMinValue());
    updatePosition();
  }
}

void ColorScaleSlider::shift(float shift) {
  currentShift += shift;

  if (currentShift < getLeftBound()) {
    currentShift = getLeftBound();
  }

  if (currentShift > getRightBound()) {
    currentShift = getRightBound();
  }

  updatePosition();
}

void ColorScaleSlider::updatePosition() {
  float xPos = linkedScale->getPosition().getX() + currentShift * linkedScale->getSize().getW();
  float xShift = xPos - position.getX();

  if (xShift != 0) {
    Coord move = {xShift, 0};
    arrow->translate(move);
    label->translate(move);
    rect->translate(move);
    setColor(linkedScale->getGlColorScale()->getColorAtPos(Coord(xPos, 0, 0)));
    ostringstream oss;
    oss << getValue();
    label->setText(oss.str());
    position.setX(xPos);
  }
}

void ColorScaleSlider::beginShift() {}

void ColorScaleSlider::endShift() {}
void ColorScaleSlider::draw(float lod, Camera *camera) {
  arrow->draw(lod, camera);
  rect->draw(lod, camera);
  label->draw(lod, camera);
}

void ColorScaleSlider::treatEvents(const std::vector<Event> &) {
  float xPos = linkedScale->getPosition().getX() + currentShift * linkedScale->getSize().getW();
  setColor(linkedScale->getGlColorScale()->getColorAtPos(Coord(xPos, 0, 0)));
}

SliderBar::SliderBar(ColorScaleSlider *left, ColorScaleSlider *right, const string &textureName)
    : GlEntity(), left(left), right(right), texture(textureName), isVisible(false) {}

SliderBar::~SliderBar() = default;

float SliderBar::getLeftBound() {
  return left->getLeftBound();
}

float SliderBar::getRightBound() {
  return right->getRightBound();
}

void SliderBar::beginShift() {
  isVisible = true;
  right->beginShift();
  left->beginShift();
}

void SliderBar::shift(float shift) {
  float combinedShift = shift;

  if (left->getCurrentShift() + shift < left->getLeftBound()) {
    combinedShift = left->getLeftBound() - left->getCurrentShift();
  }

  if (right->getCurrentShift() + shift > right->getRightBound()) {
    combinedShift = right->getRightBound() - right->getCurrentShift();
  }

  right->shift(combinedShift);
  left->shift(combinedShift);
}

void SliderBar::endShift() {
  right->endShift();
  left->endShift();
  isVisible = false;
}

void SliderBar::draw(float lod, tlp::Camera *camera) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Coord lPos = left->getBasePosition();
  Size lSize = left->getSize();
  Coord rPos = right->getBasePosition();
  Size rSize = right->getSize();

  Coord topLeft = {lPos.getX() + lSize.getW() * 0.5f, lPos.getY() - lSize.getH(), lPos.getZ()};
  Coord bottomRight = {rPos.getX() - rSize.getW() * 0.5f, rPos.getY() - rSize.getH() * 0.25f,
                       rPos.getZ()};

  // Sliders are overlapping don't draw the slider bar
  if (bottomRight.getX() - topLeft.getX() > 0) {
    GlRect rect(topLeft, bottomRight, Color(255, 255, 255), Color(255, 255, 255));

    if (isVisible) {
      rect.setTextureName(texture);
    } else {
      rect.setTopLeftColor(Color(255, 255, 255, 0));
      rect.setBottomRightColor(Color(255, 255, 255, 0));
    }

    rect.draw(lod, camera);
  }

  boundingBox = BoundingBox();
  boundingBox.expand(topLeft);
  boundingBox.expand(bottomRight);

  glDisable(GL_BLEND);
}

ThresholdInteractor::ThresholdInteractor()
    : layer(new GlLayer("Threshold")), movingSlider(nullptr), rSlider(nullptr), lSlider(nullptr),
      bar(nullptr), startDrag(false), XPosCursor(0), textureName(":/sliderTexture.png") {}

ThresholdInteractor::~ThresholdInteractor() {
  layer->getComposite()->reset(true);
  delete layer;
}

void ThresholdInteractor::setView(View *view) {
  EditColorScaleInteractor::setView(view);

  if (currentProperty) {
    buildSliders(static_cast<SOMView *>(view));
  }

  view->refresh();
}

bool ThresholdInteractor::draw(GlWidget *glWidget) {
  EditColorScaleInteractor::draw(glWidget);

  if (layer->isVisible()) {
    glWidget->scene()->graphCamera().initGl();
    Camera camera2D(glWidget->scene(), false);
    camera2D.setScene(glWidget->scene());
    camera2D.initGl();
    drawComposite(layer->getComposite(), 0, &camera2D);
  }

  return true;
}

bool ThresholdInteractor::eventFilter(QObject *widget, QEvent *event) {

  auto *glWidget = static_cast<GlWidget *>(widget);
  auto *somView = static_cast<SOMView *>(view());

  auto *me = static_cast<QMouseEvent *>(event);

  if (event->type() == QEvent::MouseButtonPress && me->button() == Qt::LeftButton) {

    vector<SelectedEntity> selectedEntities;

    // Update Camera for selection
    layer->set2DMode();
    glWidget->scene()->addExistingLayer(layer);
    glWidget->scene()->selectEntities(RenderingEntities, me->pos().x(), me->pos().y(), 0, 0, layer,
                                      selectedEntities);
    glWidget->scene()->removeLayer(layer, false);

    if (!selectedEntities.empty()) {

      for (const auto &itPE : selectedEntities) {
        for (const auto &itDisplay : layer->getGlEntities()) {
          auto *composite = dynamic_cast<GlComposite *>(itDisplay.second);

          if (composite && !composite->findKey(itPE.getEntity()).empty()) {

            auto *slider = dynamic_cast<Slider *>(composite);

            if (slider) {
              movingSlider = slider;
            }

            break;
          } else {
            if (itDisplay.second == (itPE.getEntity())) {
              auto *slider = dynamic_cast<Slider *>(itDisplay.second);

              if (slider) {
                movingSlider = slider;
              }
            }
          }
        }
      }

      if (!startDrag && movingSlider) {
        glWidget->setMouseTracking(true);
        startDrag = true;
        movingSlider->beginShift();
        XPosCursor = me->pos().x();
        glWidget->scene()->graphCamera().initGl();

        layer->setVisible(false);
        colorScale->setVisible(false);
        somView->drawMapWidget();
        colorScale->setVisible(true);
        layer->setVisible(true);
        somView->refresh();
      }
    }

    return true;
  }

  if (event->type() == QEvent::MouseMove) {
    if (startDrag) {
      float xShift = me->pos().x() - XPosCursor;
      XPosCursor = me->pos().x();

      if (xShift == 0) {
        return true;
      }

      movingSlider->shift(xShift / (colorScale->getGlColorScale()->getLength()));
      somView->refresh();
    }

    return true;
  }

  if (event->type() == QEvent::MouseButtonRelease && startDrag && movingSlider) {
    SOMMap *som = somView->getSOM();
    glWidget->setMouseTracking(false);
    startDrag = false;
    movingSlider->endShift();
    movingSlider = nullptr;
    Qt::KeyboardModifiers systMod;
#if defined(__APPLE__)
    systMod = Qt::AltModifier;
#else
    systMod = Qt::ControlModifier;
#endif

    if (me->modifiers() == systMod) {
      if (somView->getMask()) {
        performSelection(somView, somView->getMask()->getNodesEqualTo(true, som));
      } else {
        performSelection(somView, som->getNodes());
      }
    } else {
      performSelection(somView, som->getNodes());
    }

    return true;
  }

  EditColorScaleInteractor::eventFilter(widget, event);
  return false;
}

void ThresholdInteractor::performSelection(SOMView *view, tlp::Iterator<node> *it) {
  BooleanProperty *selection = view->graph()->getBooleanProperty("viewSelection");
  set<node> mask;
  flat_hash_map<node, set<node>> &mappingTab = view->getMappingTab();
  Observable::holdObservers();
  selection->setAllNodeValue(false);

  // If we are using normalized values we display unnormalized values, to compare them we need to
  // renormalize them
  InputSample &inputSample = view->getInputSample();
  uint propertyIndex = inputSample.findIndexForProperty(view->getSelectedProperty());
  double rightSliderRealValue = inputSample.isUsingNormalizedValues()
                                    ? inputSample.normalize(rSlider->getValue(), propertyIndex)
                                    : rSlider->getValue();
  double leftSliderRealValue = inputSample.isUsingNormalizedValues()
                                   ? inputSample.normalize(lSlider->getValue(), propertyIndex)
                                   : lSlider->getValue();

  for (auto n : it) {
    double nodeValue = currentProperty->getNodeDoubleValue(n);

    if (nodeValue <= rightSliderRealValue && nodeValue >= leftSliderRealValue) {
      if (mappingTab.contains(n)) {
        for (auto v : mappingTab[n]) {
          selection->setNodeValue(v, true);
        }
      }

      mask.insert(n);
    }
  }
  view->setMask(mask);
  Observable::unholdObservers();
}

bool ThresholdInteractor::screenSizeChanged(SOMView *somView) {
  if (EditColorScaleInteractor::screenSizeChanged(somView)) {
    clearSliders();

    if (currentProperty) {
      buildSliders(somView);
    }

    return true;
  }

  return false;
}

void ThresholdInteractor::propertyChanged(SOMView *somView, const string &propertyName,
                                          NumericProperty *newProperty) {
  EditColorScaleInteractor::propertyChanged(somView, propertyName, newProperty);

  if (newProperty) {
    clearSliders();
    buildSliders(somView);
    layer->setVisible(true);
  } else {
    layer->setVisible(false);
  }
}

void ThresholdInteractor::buildSliders(SOMView *somView) {
  BooleanProperty *mask = somView->getMask();
  SOMMap *som = somView->getSOM();
  Size sliderSize = {colorScale->getSize().getH(), colorScale->getSize().getH()};

  double minValue, maxValue, intervalMinValue, intervalMaxValue;
  // Get the minimum and the maximum values.
  minValue = currentProperty->getNodeDoubleMin(somView->getSOM());
  maxValue = currentProperty->getNodeDoubleMax(somView->getSOM());

  if (mask) {
    intervalMinValue = maxValue;
    intervalMaxValue = minValue;
    for (auto n : mask->getNodesEqualTo(true, som)) {
      double nodeValue = currentProperty->getNodeDoubleValue(n);

      if (nodeValue < intervalMinValue) {
        intervalMinValue = nodeValue;
      }

      if (nodeValue > intervalMaxValue) {
        intervalMaxValue = nodeValue;
      }
    }
  } else {
    intervalMinValue = minValue;
    intervalMaxValue = maxValue;
  }

  InputSample &inputSample = somView->getInputSample();
  uint propertyIndex = inputSample.findIndexForProperty(somView->getSelectedProperty());

  if (textureName.empty()) {
    generateSliderTexture();
  }

  lSlider = new ColorScaleSlider(ColorScaleSlider::ToRight, sliderSize, colorScale, textureName);

  if (intervalMinValue != minValue) {
    // If we use normalized values we need to translate them to unnormalized for user comprehension
    double intervalMinDisplayValue = inputSample.isUsingNormalizedValues()
                                         ? inputSample.unnormalize(intervalMinValue, propertyIndex)
                                         : intervalMinValue;
    lSlider->setValue(intervalMinDisplayValue);
  }

  layer->addGlEntity(lSlider, "Left");

  rSlider = new ColorScaleSlider(ColorScaleSlider::ToLeft, sliderSize, colorScale, textureName);

  if (intervalMaxValue != maxValue) {
    // If we use normalized values we need to translate them to unnormalized for user comprehension
    double intervalMaxDisplayValue = inputSample.isUsingNormalizedValues()
                                         ? inputSample.unnormalize(intervalMaxValue, propertyIndex)
                                         : intervalMaxValue;
    rSlider->setValue(intervalMaxDisplayValue);
  }

  layer->addGlEntity(rSlider, "Right");

  lSlider->setLinkedSlider(rSlider);
  rSlider->setLinkedSlider(lSlider);

  bar = new SliderBar(lSlider, rSlider, textureName);
  layer->addGlEntity(bar, "sliderBar");
}
void ThresholdInteractor::clearSliders() {
  if (layer) {
    layer->getComposite()->reset(true);
  }

  rSlider = nullptr;
  lSlider = nullptr;
  bar = nullptr;
}

void ThresholdInteractor::generateSliderTexture() {
  GlTextureManager::loadTexture(textureName);
}
