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

#include "talipot/QuickAccessBar.h"

#include <QFontDatabase>
#include <QGraphicsView>

#include <talipot/GraphModel.h>
#include <talipot/FontDialog.h>
#include <talipot/GlGraph.h>
#include <talipot/GlView.h>
#include <talipot/SnapshotDialog.h>
#include <talipot/ItemDelegate.h>
#include <talipot/CaptionGraphicsSubItems.h>

using namespace tlp;

QuickAccessBar::QuickAccessBar(QWidget *parent) : QWidget(parent), _mainView(nullptr) {}

#include "ui_QuickAccessBar.h"

QPushButton *QuickAccessBarImpl::showNodesButton() {
  return _ui->showNodesToggle;
}

QPushButton *QuickAccessBarImpl::showEdgesButton() {
  return _ui->showEdgesToggle;
}

QPushButton *QuickAccessBarImpl::showLabelsButton() {
  return _ui->showLabelsToggle;
}

QPushButton *QuickAccessBarImpl::showLabelScaled() {
  return _ui->labelsScaledToggle;
}

ColorButton *QuickAccessBarImpl::backgroundColorButton() {
  return _ui->backgroundColorButton;
}

QPushButton *QuickAccessBarImpl::showColorInterpolation() {
  return _ui->colorInterpolationToggle;
}

QuickAccessBarImpl::QuickAccessBarImpl(QGraphicsItem *quickAccessBarItem,
                                       QuickAccessButtons buttons, QWidget *parent)
    : QuickAccessBar(parent), _ui(new Ui::QuickAccessBar), _quickAccessBarItem(quickAccessBarItem),
      delegate(new ItemDelegate(this)), _oldFontScale(1), _oldNodeScale(1),
      _captionsInitialized(false) {
  _ui->setupUi(this);
  _ui->screenshotButton->setIcon(
      FontIconManager::icon(MaterialDesignIcons::Camera, darkColor, 1.0, 0.0, QPointF(0, 1)));
  _ui->backgroundColorButton->setDialogTitle("Choose the background color");
  _ui->nodeColorButton->setDialogTitle("Choose the node's default color");
  _ui->edgeColorButton->setDialogTitle("Choose the edge's default color");
  _ui->nodeBorderColorButton->setDialogTitle("Choose the default color for the border of nodes");
  _ui->edgeBorderColorButton->setDialogTitle("Choose the default color for the border of edges");
  _ui->labelColorButton->setDialogTitle("Choose the default color for the label of nodes or edges");

  connect(_ui->backgroundColorButton, &ColorButton::colorChanged, this,
          &QuickAccessBarImpl::setBackgroundColor);
  connect(_ui->colorInterpolationToggle, &QPushButton::clicked, this,
          &QuickAccessBarImpl::setColorInterpolation);
  connect(_ui->sizeInterpolationToggle, &QPushButton::clicked, this,
          &QuickAccessBarImpl::setSizeInterpolation);
  connect(_ui->labelColorButton, &ColorButton::colorChanged, this,
          &QuickAccessBarImpl::setLabelColor);
  connect(_ui->nodesColorCaptionButton, &QPushButton::clicked, this,
          &QuickAccessBarImpl::showHideNodesColorCaption);
  connect(_ui->nodesSizeCaptionButton, &QPushButton::clicked, this,
          &QuickAccessBarImpl::showHideNodesSizeCaption);
  connect(_ui->edgesColorCaptionButton, &QPushButton::clicked, this,
          &QuickAccessBarImpl::showHideEdgesColorCaption);
  connect(_ui->edgesSizeCaptionButton, &QPushButton::clicked, this,
          &QuickAccessBarImpl::showHideEdgesSizeCaption);
  connect(_ui->showEdgesToggle, &QPushButton::clicked, this, &QuickAccessBarImpl::setEdgesVisible);
  connect(_ui->showLabelsToggle, &QPushButton::clicked, this,
          &QuickAccessBarImpl::setLabelsVisible);
  connect(_ui->labelsScaledToggle, &QPushButton::clicked, this,
          &QuickAccessBarImpl::setLabelsScaled);
  connect(_ui->fontButton, &QPushButton::clicked, this, &QuickAccessBarImpl::selectFont);
  connect(_ui->screenshotButton, &QPushButton::clicked, this, &QuickAccessBarImpl::takeSnapshot);
  connect(_ui->nodeColorButton, &ColorButton::colorChanged, this,
          &QuickAccessBarImpl::setNodeColor);
  connect(_ui->edgeColorButton, &ColorButton::colorChanged, this,
          &QuickAccessBarImpl::setEdgeColor);
  connect(_ui->nodeBorderColorButton, &ColorButton::colorChanged, this,
          &QuickAccessBarImpl::setNodeBorderColor);
  connect(_ui->edgeBorderColorButton, &ColorButton::colorChanged, this,
          &QuickAccessBarImpl::setEdgeBorderColor);
  connect(_ui->nodeShapeButton, &QPushButton::clicked, this, &QuickAccessBarImpl::setNodeShape);
  connect(_ui->edgeShapeButton, &QPushButton::clicked, this, &QuickAccessBarImpl::setEdgeShape);
  connect(_ui->nodeSizeButton, &QPushButton::clicked, this, &QuickAccessBarImpl::setNodeSize);
  connect(_ui->edgeSizeButton, &QPushButton::clicked, this, &QuickAccessBarImpl::setEdgeSize);
  connect(_ui->labelPositionButton, &QPushButton::clicked, this,
          QOverload<>::of(&QuickAccessBarImpl::setNodeLabelPosition));
  connect(_ui->showNodesToggle, &QPushButton::clicked, this, &QuickAccessBarImpl::setNodesVisible);

  if (buttons.testFlag(ALLBUTTONS)) {
    return;
  }

  if (!buttons.testFlag(NODESCOLORCAPTION)) {
    _ui->nodesColorCaptionButton->hide();
  }

  if (!buttons.testFlag(NODESSIZECAPTION)) {
    _ui->nodesSizeCaptionButton->hide();
  }

  if (!buttons.testFlag(EDGESCOLORCAPTION)) {
    _ui->edgesColorCaptionButton->hide();
  }

  if (!buttons.testFlag(EDGESIZECAPTION)) {
    _ui->edgesSizeCaptionButton->hide();
  }

  if (!buttons.testFlag(SCREENSHOT)) {
    _ui->screenshotButton->hide();
  }

  if (!buttons.testFlag(BACKGROUNDCOLOR)) {
    _ui->backgroundColorButton->hide();
  }

  if (!buttons.testFlag(NODECOLOR)) {
    _ui->nodeColorButton->hide();
  }

  if (!buttons.testFlag(EDGECOLOR)) {
    _ui->edgeColorButton->hide();
  }

  if (!buttons.testFlag(NODEBORDERCOLOR)) {
    _ui->nodeBorderColorButton->hide();
  }

  if (!buttons.testFlag(EDGEBORDERCOLOR)) {
    _ui->edgeBorderColorButton->hide();
  }

  if (!buttons.testFlag(LABELCOLOR)) {
    _ui->labelColorButton->hide();
  }

  if (!buttons.testFlag(COLORINTERPOLATION)) {
    _ui->colorInterpolationToggle->hide();
  }

  if (!buttons.testFlag(SIZEINTERPOLATION)) {
    _ui->sizeInterpolationToggle->hide();
  }

  if (!buttons.testFlag(SHOWEDGES)) {
    _ui->showEdgesToggle->hide();
  }

  if (!buttons.testFlag(SHOWLABELS)) {
    _ui->showLabelsToggle->hide();
  }

  if (!buttons.testFlag(LABELSSCALED)) {
    _ui->labelsScaledToggle->hide();
  }

  if (!buttons.testFlag(NODESHAPE)) {
    _ui->nodeShapeButton->hide();
  }

  if (!buttons.testFlag(EDGESHAPE)) {
    _ui->edgeShapeButton->hide();
  }

  if (!buttons.testFlag(NODESIZE)) {
    _ui->nodeSizeButton->hide();
  }

  if (!buttons.testFlag(EDGESIZE)) {
    _ui->edgeSizeButton->hide();
  }

  if (!buttons.testFlag(NODELABELPOSITION)) {
    _ui->labelPositionButton->hide();
  }

  if (!buttons.testFlag(SELECTFONT)) {
    _ui->fontButton->hide();
  }

  if (!buttons.testFlag(SHOWNODES)) {
    _ui->showNodesToggle->hide();
  }
}

void QuickAccessBarImpl::addButtonAtEnd(QAbstractButton *button) {
  QLayoutItem *spacer = _ui->horizontalLayout->itemAt(_ui->horizontalLayout->count() - 1);
  _ui->horizontalLayout->removeItem(spacer);
  _ui->horizontalLayout->addWidget(button);
  _ui->horizontalLayout->addItem(spacer);
}

void QuickAccessBarImpl::addButtonsAtEnd(const QVector<QAbstractButton *> &buttonvect) {
  QLayoutItem *spacer = _ui->horizontalLayout->itemAt(_ui->horizontalLayout->count() - 1);
  _ui->horizontalLayout->removeItem(spacer);

  for (auto *b : buttonvect) {
    _ui->horizontalLayout->addWidget(b);
  }

  _ui->horizontalLayout->addItem(spacer);
}

void QuickAccessBarImpl::addSeparator() {
  auto *sep = new QFrame(this);
  sep->setFrameShape(QFrame::VLine);
  sep->setFrameShadow(QFrame::Sunken);
  QLayoutItem *spacer = _ui->horizontalLayout->itemAt(_ui->horizontalLayout->count() - 1);
  _ui->horizontalLayout->removeItem(spacer);
  _ui->horizontalLayout->addWidget(sep);
  _ui->horizontalLayout->addItem(spacer);
}

QuickAccessBarImpl::~QuickAccessBarImpl() {
  if (_captionsInitialized) {
    delete _captions[0];
    delete _captions[1];
    delete _captions[2];
    delete _captions[3];
  }

  delete _ui;
}

void QuickAccessBar::setGlView(GlView *v) {
  _mainView = v;
  reset();
}

void QuickAccessBarImpl::reset() {
  _resetting = true;

  _ui->backgroundColorButton->setColor(scene()->getBackgroundColor());
  _ui->colorInterpolationToggle->setChecked(renderingParameters()->isEdgeColorInterpolate());
  _ui->sizeInterpolationToggle->setChecked(renderingParameters()->isEdgeSizeInterpolate());
  _ui->showEdgesToggle->setChecked(renderingParameters()->isDisplayEdges());
  _ui->showNodesToggle->setChecked(renderingParameters()->isDisplayNodes());
  _ui->showLabelsToggle->setChecked(renderingParameters()->isViewNodeLabel());
  _ui->labelsScaledToggle->setChecked(renderingParameters()->isLabelScaled());
  updateFontButtonStyle();
  _resetting = false;
}

void QuickAccessBarImpl::showHideNodesColorCaption(bool val) {
  showHideCaption(CaptionItem::NodesColorCaption);
  _ui->nodesColorCaptionButton->setChecked(val);
}

void QuickAccessBarImpl::showHideNodesSizeCaption(bool val) {
  showHideCaption(CaptionItem::NodesSizeCaption);
  _ui->nodesSizeCaptionButton->setChecked(val);
}

void QuickAccessBarImpl::showHideEdgesColorCaption(bool val) {
  showHideCaption(CaptionItem::EdgesColorCaption);
  _ui->edgesColorCaptionButton->setChecked(val);
}

void QuickAccessBarImpl::showHideEdgesSizeCaption(bool val) {
  showHideCaption(CaptionItem::EdgesSizeCaption);
  _ui->edgesSizeCaptionButton->setChecked(val);
}

void QuickAccessBarImpl::showHideCaption(CaptionItem::CaptionType captionType) {
  if (!_captionsInitialized) {
    _captionsInitialized = true;

    _captions[0] = new CaptionItem(_mainView);
    _captions[0]->create(CaptionItem::NodesColorCaption);
    _captions[0]->captionGraphicsItem()->setParentItem(_quickAccessBarItem);
    _captions[0]->captionGraphicsItem()->setVisible(false);

    _captions[1] = new CaptionItem(_mainView);
    _captions[1]->create(CaptionItem::NodesSizeCaption);
    _captions[1]->captionGraphicsItem()->setParentItem(_quickAccessBarItem);
    _captions[1]->captionGraphicsItem()->setVisible(false);

    _captions[2] = new CaptionItem(_mainView);
    _captions[2]->create(CaptionItem::EdgesColorCaption);
    _captions[2]->captionGraphicsItem()->setParentItem(_quickAccessBarItem);
    _captions[2]->captionGraphicsItem()->setVisible(false);

    _captions[3] = new CaptionItem(_mainView);
    _captions[3]->create(CaptionItem::EdgesSizeCaption);
    _captions[3]->captionGraphicsItem()->setParentItem(_quickAccessBarItem);
    _captions[3]->captionGraphicsItem()->setVisible(false);

    for (size_t i = 0; i < 4; i++) {
      connect(_captions[i]->captionGraphicsItem(),
              &CaptionGraphicsBackgroundItem::interactionsActivated,
              _captions[(i + 1) % 4]->captionGraphicsItem(),
              &CaptionGraphicsBackgroundItem::removeInteractions);
      connect(_captions[i]->captionGraphicsItem(),
              &CaptionGraphicsBackgroundItem::interactionsActivated,
              _captions[(i + 2) % 4]->captionGraphicsItem(),
              &CaptionGraphicsBackgroundItem::removeInteractions);
      connect(_captions[i]->captionGraphicsItem(),
              &CaptionGraphicsBackgroundItem::interactionsActivated,
              _captions[(i + 3) % 4]->captionGraphicsItem(),
              &CaptionGraphicsBackgroundItem::removeInteractions);
      connect(_captions[i], &CaptionItem::filtering, _captions[(i + 1) % 4],
              &CaptionItem::removeObservation);
      connect(_captions[i], &CaptionItem::filtering, _captions[(i + 2) % 4],
              &CaptionItem::removeObservation);
      connect(_captions[i], &CaptionItem::filtering, _captions[(i + 3) % 4],
              &CaptionItem::removeObservation);
    }
  }

  size_t captionIndice = 0;

  if (captionType == CaptionItem::NodesSizeCaption) {
    captionIndice = 1;
  } else if (captionType == CaptionItem::EdgesColorCaption) {
    captionIndice = 2;
  } else if (captionType == CaptionItem::EdgesSizeCaption) {
    captionIndice = 3;
  }

  _captions[captionIndice]->captionGraphicsItem()->setVisible(
      !_captions[captionIndice]->captionGraphicsItem()->isVisible());

  uint numberVisible = 0;

  for (auto *caption : _captions) {
    if (caption->captionGraphicsItem()->isVisible()) {
      caption->captionGraphicsItem()->setPos(QPoint(numberVisible * 130, -260));
      numberVisible++;
    }
  }
}

void QuickAccessBarImpl::takeSnapshot() {
  SnapshotDialog dlg(_mainView, _mainView->graphicsView()->window());
  dlg.exec();
}

void QuickAccessBarImpl::setBackgroundColor(const QColor &c) {
  if (scene()->getBackgroundColor() != QColorToColor(c)) {
    scene()->setBackgroundColor(QColorToColor(c));
    _mainView->emitDrawNeededSignal();
    emit settingsChanged();
  }
}

void QuickAccessBarImpl::setColorInterpolation(bool f) {
  if (renderingParameters()->isEdgeColorInterpolate() != f) {
    renderingParameters()->setEdgeColorInterpolate(f);
    _mainView->emitDrawNeededSignal();
    emit settingsChanged();
  }
}

void QuickAccessBarImpl::setSizeInterpolation(bool f) {
  if (renderingParameters()->isEdgeSizeInterpolate() != f) {
    renderingParameters()->setEdgeSizeInterpolate(f);
    _mainView->emitDrawNeededSignal();
    emit settingsChanged();
  }
}

void QuickAccessBarImpl::setLabelColor(const QColor &c) {

  BooleanProperty *selected = inputData()->selection();
  bool hasSelected = false;

  _mainView->graph()->push();

  Observable::holdObservers();
  ColorProperty *labelColors = inputData()->labelColors();
  ColorProperty *labelBorderColors = inputData()->labelBorderColors();

  Color color = QColorToColor(c);

  for (auto n : selected->getNonDefaultValuatedNodes(_mainView->graph())) {
    labelColors->setNodeValue(n, color);
    labelBorderColors->setNodeValue(n, color);
    hasSelected = true;
  }

  if (!hasSelected) {
    labelColors->setAllNodeValue(color, _mainView->graph());
    labelBorderColors->setAllNodeValue(color, _mainView->graph());
  }

  for (auto e : selected->getNonDefaultValuatedEdges(_mainView->graph())) {
    labelColors->setEdgeValue(e, color);
    labelBorderColors->setEdgeValue(e, color);
    hasSelected = true;
  }

  if (!hasSelected) {
    labelColors->setAllEdgeValue(color, _mainView->graph());
    labelBorderColors->setAllEdgeValue(color, _mainView->graph());
  }

  Observable::unholdObservers();
  _mainView->graph()->popIfNoUpdates();
  emit settingsChanged();
}

void QuickAccessBarImpl::setAllColorValues(uint eltType, ColorProperty *prop, const Color &color) {
  BooleanProperty *selected = inputData()->selection();
  bool hasSelected = false;

  _mainView->graph()->push();

  Observable::holdObservers();

  if (eltType == NODE) {
    for (auto n : selected->getNonDefaultValuatedNodes(_mainView->graph())) {
      prop->setNodeValue(n, color);
      hasSelected = true;
    }

    if (!hasSelected) {
      prop->setAllNodeValue(color, _mainView->graph());
    }
  } else {
    for (auto e : selected->getNonDefaultValuatedEdges(_mainView->graph())) {
      prop->setEdgeValue(e, color);
      hasSelected = true;
    }

    if (!hasSelected) {
      prop->setAllEdgeValue(color, _mainView->graph());
    }
  }

  Observable::unholdObservers();
  _mainView->graph()->popIfNoUpdates();
  emit settingsChanged();
}

void QuickAccessBarImpl::setNodeColor(const QColor &c) {
  setAllColorValues(NODE, inputData()->colors(), QColorToColor(c));
}

void QuickAccessBarImpl::setEdgeColor(const QColor &c) {
  setAllColorValues(EDGE, inputData()->colors(), QColorToColor(c));
}

void QuickAccessBarImpl::setNodeBorderColor(const QColor &c) {
  setAllColorValues(NODE, inputData()->borderColors(), QColorToColor(c));
}

void QuickAccessBarImpl::setEdgeBorderColor(const QColor &c) {
  setAllColorValues(EDGE, inputData()->borderColors(), QColorToColor(c));
}

void QuickAccessBarImpl::setAllValues(uint eltType, PropertyInterface *prop) {
  QVariant val = ItemDelegate::showEditorDialog(static_cast<tlp::ElementType>(eltType), prop,
                                                _mainView->graph(), delegate,
                                                _mainView->graphicsView()->window());

  // Check if edition has been cancelled
  if (!val.isValid()) {
    return;
  }

  BooleanProperty *selected = inputData()->selection();
  bool hasSelected = false;

  _mainView->graph()->push();

  Observable::holdObservers();

  if (eltType == NODE) {
    for (auto n : selected->getNonDefaultValuatedNodes(_mainView->graph())) {
      GraphModel::setNodeValue(n.id, prop, val);
      hasSelected = true;
    }

    if (!hasSelected) {
      GraphModel::setAllNodeValue(prop, val, _mainView->graph());
    }
  } else {
    for (auto e : selected->getNonDefaultValuatedEdges(_mainView->graph())) {
      GraphModel::setEdgeValue(e.id, prop, val);
      hasSelected = true;
    }

    if (!hasSelected) {
      GraphModel::setAllEdgeValue(prop, val, _mainView->graph());
    }
  }

  Observable::unholdObservers();
  _mainView->graph()->popIfNoUpdates();
  emit settingsChanged();
}

void QuickAccessBarImpl::setNodeShape() {
  setAllValues(NODE, inputData()->shapes());
}

void QuickAccessBarImpl::setEdgeShape() {
  setAllValues(EDGE, inputData()->shapes());
}

void QuickAccessBarImpl::setNodeSize() {
  setAllValues(NODE, inputData()->sizes());
}

void QuickAccessBarImpl::setEdgeSize() {
  setAllValues(EDGE, inputData()->sizes());
}

void QuickAccessBarImpl::setNodeLabelPosition() {
  setAllValues(NODE, inputData()->labelPositions());
}

void QuickAccessBarImpl::setEdgesVisible(bool v) {
  if (renderingParameters()->isDisplayEdges() != v) {
    renderingParameters()->setDisplayEdges(v);
    _mainView->emitDrawNeededSignal();
    emit settingsChanged();
  }
}

void QuickAccessBarImpl::setNodesVisible(bool v) {
  if (renderingParameters()->isDisplayNodes() != v) {
    renderingParameters()->setDisplayNodes(v);
    _mainView->emitDrawNeededSignal();
    emit settingsChanged();
  }
}

void QuickAccessBarImpl::setLabelsVisible(bool v) {
  if (renderingParameters()->isViewNodeLabel() != v) {
    renderingParameters()->setViewNodeLabel(v);
    _mainView->emitDrawNeededSignal();
    emit settingsChanged();
  }
}

void QuickAccessBarImpl::setLabelsScaled(bool v) {
  if (renderingParameters()->isLabelScaled() != v) {
    renderingParameters()->setLabelScaled(v);
    _mainView->emitDrawNeededSignal();
    emit settingsChanged();
  }
}

GlGraphRenderingParameters *QuickAccessBar::renderingParameters() const {
  return &scene()->getGlGraph()->getRenderingParameters();
}

GlGraphInputData *QuickAccessBar::inputData() const {
  return scene()->getGlGraph()->getInputData();
}

GlScene *QuickAccessBar::scene() const {
  return _mainView->getGlWidget()->scene();
}

void QuickAccessBarImpl::selectFont() {
  FontDialog dlg(_mainView->graphicsView()->window());
  dlg.selectFont(Font::fromName(inputData()->fonts()->getNodeDefaultValue()));

  if (dlg.exec() != QDialog::Accepted) {
    return;
  }

  _mainView->graph()->push();

  Observable::holdObservers();

  inputData()->fonts()->setAllNodeValue(dlg.font().fontName(), _mainView->graph());
  inputData()->fonts()->setAllEdgeValue(dlg.font().fontName(), _mainView->graph());
  inputData()->fontSizes()->setAllNodeValue(dlg.fontSize(), _mainView->graph());
  inputData()->fontSizes()->setAllEdgeValue(dlg.fontSize(), _mainView->graph());

  Observable::unholdObservers();
  _mainView->graph()->popIfNoUpdates();
  updateFontButtonStyle();
  emit settingsChanged();
}

void QuickAccessBarImpl::updateFontButtonStyle() {
  std::string fontName = inputData()->fonts()->getNodeDefaultValue();
  Font selectedFont = Font::fromName(fontName);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QFontDatabase fontDb;
  _ui->fontButton->setFont(fontDb.font(tlpStringToQString(selectedFont.fontFamily()),
#else
  _ui->fontButton->setFont(QFontDatabase::font(tlpStringToQString(selectedFont.fontFamily()),
#endif
                                       tlpStringToQString(selectedFont.fontStyle()), 10));
}
