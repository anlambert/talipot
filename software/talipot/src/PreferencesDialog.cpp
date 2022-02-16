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

#include <QMenu>
#include "PreferencesDialog.h"
#include "ThemeUtils.h"

#include "ui_PreferencesDialog.h"

#include <talipot/Settings.h>
#include <talipot/ItemDelegate.h>

using namespace tlp;

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent), _ui(new Ui::PreferencesDialog) {
  _ui->setupUi(this);
  _ui->graphDefaultsTable->setItemDelegate(new tlp::ItemDelegate(_ui->graphDefaultsTable));
  // disable edition for title items (in column 0)
  for (int i = 0; i < _ui->graphDefaultsTable->rowCount(); ++i) {
    QString sectionText =
        _ui->graphDefaultsTable->model()->headerData(i, Qt::Vertical, Qt::DisplayRole).toString();
    QString tooltip = "Click mouse right button to display a contextual menu allowing to "
                      "reset the default values of <b>" +
                      sectionText + "</b>.";
    _ui->graphDefaultsTable->model()->setHeaderData(i, Qt::Vertical, tooltip, Qt::ToolTipRole);
  }

  _ui->graphDefaultsTable->setContextMenuPolicy(Qt::CustomContextMenu);
  _ui->graphDefaultsTable->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(_ui->graphDefaultsTable, &QTableWidget::cellChanged, this,
          &PreferencesDialog::cellChanged);
  connect(_ui->graphDefaultsTable, &QWidget::customContextMenuRequested, []() {});
  connect(_ui->graphDefaultsTable->verticalHeader(), &QWidget::customContextMenuRequested, this,
          &PreferencesDialog::showGraphDefaultsContextMenu);
  connect(_ui->randomSeedCheck, &QCheckBox::stateChanged, this,
          &PreferencesDialog::randomSeedCheckChanged);
  connect(_ui->resetAllDrawingDefaultsButton, &QAbstractButton::released,
          [this] { resetToTalipotDefaults(); });
  connect(_ui->guiThemeCB, &QComboBox::currentTextChanged, this,
          &PreferencesDialog::guiThemeChanged);

  _ui->graphDefaultsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

PreferencesDialog::~PreferencesDialog() {
  delete _ui;
}

template <typename PROP, typename TYPE>
inline void setDefaultNodeValueInProperty(const std::string &propertyName, const TYPE &value,
                                          bool &graphPush) {
  for (Graph *root : getRootGraphs()) {
    if (root->existLocalProperty(propertyName)) {
      PROP *prop = dynamic_cast<PROP *>(root->getProperty(propertyName));

      if (prop) {
        if (graphPush) {
          root->push();
        }

        prop->setAllNodeValue(value);
      }
    }

    for (Graph *sg : root->getDescendantGraphs()) {
      if (sg->existLocalProperty(propertyName)) {
        PROP *prop = dynamic_cast<PROP *>(sg->getProperty(propertyName));

        if (prop) {
          prop->setAllNodeValue(value);
        }
      }
    }
  }
  graphPush = false;
}

template <typename PROP, typename TYPE>
inline void setDefaultEdgeValueInProperty(const std::string &propertyName, const TYPE &value,
                                          bool &graphPush) {
  for (Graph *root : getRootGraphs()) {
    if (root->existLocalProperty(propertyName)) {
      PROP *prop = dynamic_cast<PROP *>(root->getProperty(propertyName));

      if (prop) {
        if (graphPush) {
          root->push();
        }

        prop->setAllEdgeValue(value);
      }
    }

    for (Graph *sg : root->getDescendantGraphs()) {
      if (sg->existLocalProperty(propertyName)) {
        PROP *prop = dynamic_cast<PROP *>(sg->getProperty(propertyName));

        if (prop) {
          prop->setAllEdgeValue(value);
        }
      }
    }
  }
  graphPush = false;
}

void PreferencesDialog::writeSettings() {
  Settings::setProxyEnabled(_ui->proxyCheck->isChecked());

  switch (_ui->proxyType->currentIndex()) {
  case 0:
    Settings::setProxyType(QNetworkProxy::Socks5Proxy);
    break;

  case 1:
    Settings::setProxyType(QNetworkProxy::HttpProxy);
    break;

  case 2:
    Settings::setProxyType(QNetworkProxy::HttpCachingProxy);
    break;

  case 3:
    Settings::setProxyType(QNetworkProxy::FtpCachingProxy);
    break;

  default:
    break;
  }

  Settings::setProxyHost(_ui->proxyAddr->text());
  Settings::setProxyPort(_ui->proxyPort->value());
  Settings::setUseProxyAuthentification(_ui->proxyAuthCheck->isChecked());
  Settings::setProxyUsername(_ui->proxyUser->text());
  Settings::setProxyPassword(_ui->proxyPassword->text());

  QAbstractItemModel *model = _ui->graphDefaultsTable->model();
  bool applyDrawingDefaults = _ui->applyDrawingDefaultsCheck->isChecked();
  bool graphPush = true;

  if (Settings::defaultColor(tlp::ElementType::NODE) !=
      model->data(model->index(0, 0)).value<tlp::Color>()) {
    Settings::setDefaultColor(tlp::ElementType::NODE,
                              model->data(model->index(0, 0)).value<tlp::Color>());

    if (applyDrawingDefaults) {
      setDefaultNodeValueInProperty<ColorProperty>(
          "viewColor", Settings::defaultColor(tlp::ElementType::NODE), graphPush);
    }
  }

  if (Settings::defaultColor(tlp::ElementType::EDGE) !=
      model->data(model->index(0, 1)).value<tlp::Color>()) {
    Settings::setDefaultColor(tlp::ElementType::EDGE,
                              model->data(model->index(0, 1)).value<tlp::Color>());

    if (applyDrawingDefaults) {
      setDefaultEdgeValueInProperty<ColorProperty>(
          "viewColor", Settings::defaultColor(tlp::ElementType::EDGE), graphPush);
    }
  }

  if (Settings::defaultSize(tlp::ElementType::NODE) !=
      model->data(model->index(1, 0)).value<tlp::Size>()) {
    Settings::setDefaultSize(tlp::ElementType::NODE,
                             model->data(model->index(1, 0)).value<tlp::Size>());
    setDefaultNodeValueInProperty<SizeProperty>(
        "viewSize", Settings::defaultSize(tlp::ElementType::NODE), graphPush);
  }

  if (Settings::defaultSize(tlp::ElementType::EDGE) !=
      model->data(model->index(1, 1)).value<tlp::Size>()) {
    Settings::setDefaultSize(tlp::ElementType::EDGE,
                             model->data(model->index(1, 1)).value<tlp::Size>());

    if (applyDrawingDefaults) {
      setDefaultEdgeValueInProperty<SizeProperty>(
          "viewSize", Settings::defaultSize(tlp::ElementType::EDGE), graphPush);
    }
  }

  if (Settings::defaultShape(tlp::ElementType::NODE) !=
      model->data(model->index(2, 0)).value<NodeShape::NodeShapes>()) {
    Settings::setDefaultShape(tlp::ElementType::NODE,
                              model->data(model->index(2, 0)).value<NodeShape::NodeShapes>());

    if (applyDrawingDefaults) {
      setDefaultNodeValueInProperty<IntegerProperty>(
          "viewShape", Settings::defaultShape(tlp::ElementType::NODE), graphPush);
    }
  }

  if (Settings::defaultShape(tlp::ElementType::EDGE) !=
      int(model->data(model->index(2, 1)).value<EdgeShape::EdgeShapes>())) {
    Settings::setDefaultShape(tlp::ElementType::EDGE,
                              int(model->data(model->index(2, 1)).value<EdgeShape::EdgeShapes>()));
    setDefaultEdgeValueInProperty<IntegerProperty>(
        "viewShape", Settings::defaultShape(tlp::ElementType::EDGE), graphPush);
  }

  if (Settings::defaultLabelColor() != model->data(model->index(4, 0)).value<tlp::Color>()) {
    Settings::setDefaultLabelColor(model->data(model->index(4, 0)).value<tlp::Color>());

    if (applyDrawingDefaults) {
      setDefaultNodeValueInProperty<ColorProperty>("viewLabelColor", Settings::defaultLabelColor(),
                                                   graphPush);
      setDefaultEdgeValueInProperty<ColorProperty>("viewLabelColor", Settings::defaultLabelColor(),
                                                   graphPush);
    }
  }

  Settings::setDefaultSelectionColor(model->data(model->index(3, 0)).value<tlp::Color>());

  Settings::applyProxySettings();

  Settings::setDisplayDefaultViews(_ui->displayDefaultViews->isChecked());
  Settings::setAutomaticMapMetric(_ui->colorMappingCheck->isChecked());
  Settings::setAutomaticRatio(_ui->aspectRatioCheck->isChecked());
  Settings::setAutomaticCentering(_ui->centerViewCheck->isChecked());
  Settings::setViewOrtho(_ui->viewOrthoCheck->isChecked());
  Settings::setResultPropertyStored(_ui->resultPropertyStoredCheck->isChecked());
  Settings::setLogPluginCall(_ui->logCombo->currentIndex());
  Settings::setUseTlpbFileFormat(_ui->usetlpbformat->isChecked());

  if (_ui->randomSeedCheck->isChecked()) {
    bool ok = true;
    uint seed = _ui->randomSeedEdit->text().toUInt(&ok);
    tlp::setSeedOfRandomSequence(seed);
  } else {
    tlp::setSeedOfRandomSequence();
  }

  Settings::setSeedOfRandomSequence(tlp::getSeedOfRandomSequence());
  Settings::setGuiTheme(_ui->guiThemeCB->currentText());
}

void PreferencesDialog::readSettings() {
  _ui->proxyCheck->setChecked(Settings::isProxyEnabled());

  if (Settings::isProxyEnabled()) {
    _ui->networkFrame1->setEnabled(true);
    _ui->networkFrame2->setEnabled(true);
    _ui->networkFrame3->setEnabled(true);
  }

  switch (Settings::proxyType()) {
  case QNetworkProxy::Socks5Proxy:
    _ui->proxyType->setCurrentIndex(0);
    break;

  case QNetworkProxy::HttpProxy:
    _ui->proxyType->setCurrentIndex(1);
    break;

  case QNetworkProxy::HttpCachingProxy:
    _ui->proxyType->setCurrentIndex(2);
    break;

  case QNetworkProxy::FtpCachingProxy:
    _ui->proxyType->setCurrentIndex(3);
    break;

  default:
    break;
  }

  _ui->proxyAddr->setText(Settings::proxyHost());
  _ui->proxyPort->setValue(Settings::proxyPort());
  _ui->proxyAuthCheck->setChecked(Settings::isUseProxyAuthentification());

  if (Settings::isUseProxyAuthentification()) {
    _ui->proxyUser->setEnabled(true);
    _ui->proxyPassword->setEnabled(true);
  }

  _ui->proxyUser->setText(Settings::proxyUsername());
  _ui->proxyPassword->setText(Settings::proxyPassword());

  QAbstractItemModel *model = _ui->graphDefaultsTable->model();
  model->setData(model->index(0, 0),
                 QVariant::fromValue<tlp::Color>(Settings::defaultColor(tlp::ElementType::NODE)));
  model->setData(model->index(0, 1),
                 QVariant::fromValue<tlp::Color>(Settings::defaultColor(tlp::ElementType::EDGE)));
  model->setData(model->index(1, 0),
                 QVariant::fromValue<tlp::Size>(Settings::defaultSize(tlp::ElementType::NODE)));
  model->setData(model->index(1, 1),
                 QVariant::fromValue<tlp::Size>(Settings::defaultSize(tlp::ElementType::EDGE)));
  model->setData(model->index(2, 0),
                 QVariant::fromValue<NodeShape::NodeShapes>(static_cast<NodeShape::NodeShapes>(
                     Settings::defaultShape(tlp::ElementType::NODE))));
  model->setData(model->index(2, 1),
                 QVariant::fromValue<EdgeShape::EdgeShapes>(static_cast<EdgeShape::EdgeShapes>(
                     Settings::defaultShape(tlp::ElementType::EDGE))));
  model->setData(model->index(3, 0),
                 QVariant::fromValue<tlp::Color>(Settings::defaultSelectionColor()));
  model->setData(model->index(3, 1),
                 QVariant::fromValue<tlp::Color>(Settings::defaultSelectionColor()));
  model->setData(model->index(4, 0),
                 QVariant::fromValue<tlp::Color>(Settings::defaultLabelColor()));
  model->setData(model->index(4, 1),
                 QVariant::fromValue<tlp::Color>(Settings::defaultLabelColor()));
  // edges selection color is not editable
  //_ui->graphDefaultsTable->item(3,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  _ui->applyDrawingDefaultsCheck->setChecked(false);
  _ui->applyDrawingDefaultsCheck->setEnabled(!iteratorEmpty(tlp::getRootGraphs()));

  _ui->displayDefaultViews->setChecked(Settings::displayDefaultViews());
  _ui->aspectRatioCheck->setChecked(Settings::isAutomaticRatio());
  _ui->centerViewCheck->setChecked(Settings::isAutomaticCentering());
  _ui->viewOrthoCheck->setChecked(Settings::isViewOrtho());
  _ui->resultPropertyStoredCheck->setChecked(Settings::isResultPropertyStored());
  _ui->colorMappingCheck->setChecked(Settings::isAutomaticMapMetric());
  _ui->logCombo->setCurrentIndex(Settings::logPluginCall());

  if (Settings::isUseTlpbFileFormat()) {
    _ui->usetlpbformat->setChecked(true);
  }

  // initialize seed according to settings
  uint seed;
  tlp::setSeedOfRandomSequence(seed = Settings::seedOfRandomSequence());
  // UINT_MAX seed value means the seed is random
  bool isSeedRandom = (seed == UINT_MAX);
  _ui->randomSeedCheck->setChecked(!isSeedRandom);
  _ui->randomSeedEdit->setEnabled(!isSeedRandom);
  _ui->randomSeedEdit->setText(isSeedRandom ? QString() : QString::number(seed));

  _ui->guiThemeCB->setCurrentText(Settings::guiTheme());
}

void PreferencesDialog::cellChanged(int row, int column) {
  if (row >= 3) {
    // force selection color to be the same for nodes & edges
    QAbstractItemModel *model = _ui->graphDefaultsTable->model();
    model->setData(model->index(row, column == 0 ? 1 : 0), model->data(model->index(row, column)));
  }
}

void PreferencesDialog::randomSeedCheckChanged(int state) {
  if (state == Qt::Checked && _ui->randomSeedEdit->text().isEmpty()) {
    _ui->randomSeedEdit->setText("1");
  }
}

#define RESET_NODE 0
#define RESET_EDGE 1
#define RESET_BOTH 2
void PreferencesDialog::resetToTalipotDefaults(int row, int updateMode) {
  if (updateMode == RESET_BOTH) {
    resetToTalipotDefaults(row, RESET_NODE);
    resetToTalipotDefaults(row, RESET_EDGE);
    return;
  }

  if (row == -1) {
    for (row = 0; row < _ui->graphDefaultsTable->rowCount(); ++row) {
      resetToTalipotDefaults(row, RESET_BOTH);
    }

    return;
  }

  QAbstractItemModel *model = _ui->graphDefaultsTable->model();

  switch (row) {
  case 0: // default color
    if (updateMode == RESET_NODE) {
      model->setData(model->index(0, 0),
                     QVariant::fromValue<tlp::Color>(ViewSettings::ApplicationDefault::NodeColor));
    } else {
      model->setData(model->index(0, 1),
                     QVariant::fromValue<tlp::Color>(ViewSettings::ApplicationDefault::EdgeColor));
    }

    break;

  case 1: // default size
    if (updateMode == RESET_NODE) {
      model->setData(model->index(1, 0),
                     QVariant::fromValue<tlp::Size>(ViewSettings::ApplicationDefault::NodeSize));
    } else {
      model->setData(model->index(1, 1),
                     QVariant::fromValue<tlp::Size>(ViewSettings::ApplicationDefault::EdgeSize));
    }

    break;

  case 2: // default shape
    if (updateMode == RESET_NODE) {
      model->setData(model->index(2, 0),
                     QVariant::fromValue<NodeShape::NodeShapes>(static_cast<NodeShape::NodeShapes>(
                         ViewSettings::ApplicationDefault::NodeShape)));
    } else {
      model->setData(model->index(2, 1),
                     QVariant::fromValue<EdgeShape::EdgeShapes>(static_cast<EdgeShape::EdgeShapes>(
                         ViewSettings::ApplicationDefault::EdgeShape)));
    }

    break;

  case 3: // default selection color
    if (updateMode == RESET_NODE) {
      model->setData(model->index(3, 0), QVariant::fromValue<tlp::Color>(
                                             ViewSettings::ApplicationDefault::SelectionColor));
    } else {
      model->setData(model->index(3, 1), QVariant::fromValue<tlp::Color>(
                                             ViewSettings::ApplicationDefault::SelectionColor));
    }

    break;

  case 4: // default label color
    if (updateMode == RESET_NODE) {
      model->setData(model->index(4, 0),
                     QVariant::fromValue<tlp::Color>(ViewSettings::ApplicationDefault::LabelColor));
    } else {
      model->setData(model->index(4, 1),
                     QVariant::fromValue<tlp::Color>(ViewSettings::ApplicationDefault::LabelColor));
    }

  default:
    break;
  }
}

void PreferencesDialog::showGraphDefaultsContextMenu(const QPoint &p) {
  int row = _ui->graphDefaultsTable->verticalHeader()->logicalIndexAt(p);

  QMenu contextMenu;
  // the style sheet below allows to display disabled items
  // as "title" items in the "mainMenu"
  contextMenu.setStyleSheet("QMenu[mainMenu = \"true\"]::item:disabled {color: white; "
                            "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:, "
                            "y2:1, stop:0 rgb(75,75,75), stop:1 rgb(60, 60, 60))}");
  // so it is the "mainMenu"
  contextMenu.setProperty("mainMenu", true);
  contextMenu.setToolTipsVisible(true);

  QString defaultProp =
      _ui->graphDefaultsTable->model()->headerData(row, Qt::Vertical, Qt::DisplayRole).toString();
  QAction *action = contextMenu.addAction(defaultProp);
  action->setEnabled(false);
  contextMenu.addSeparator();

  if (row < 3) {
    QMenu *subMenu = contextMenu.addMenu(QString("Reset to Talipot predefined"));
    subMenu->setToolTip("Choose the type of elements for which the default value will be reset");
    action = subMenu->addAction("Node default value");
    action->setToolTip("Reset the node " + defaultProp + " to the Talipot predefined value");
    action->setData(QVariant(int(RESET_NODE)));
    action = subMenu->addAction("Edge default value");
    action->setToolTip("Reset the edge " + defaultProp + " to the Talipot predefined value");
    action->setData(QVariant(int(RESET_EDGE)));
    action = subMenu->addAction("Node/Edge default values");
    action->setToolTip("Reset the node/edge " + defaultProp + " to the Talipot predefined value");
    action->setData(QVariant(int(RESET_BOTH)));
  } else {
    action = contextMenu.addAction("Reset to Talipot predefined value");
    action->setData(QVariant(int(RESET_BOTH)));
    action->setToolTip("Reset " + defaultProp + " to the Talipot predefined value");
  }

  action = contextMenu.exec(QCursor::pos() - QPoint(5, 5));

  if (action) {
    resetToTalipotDefaults(row, action->data().toInt());
  }
}

void PreferencesDialog::guiThemeChanged(const QString &guiTheme) {
  setApplicationGuiTheme(guiTheme, true);
}

void PreferencesDialog::reject() {
  setApplicationGuiTheme(Settings::guiTheme(), true);
  QDialog::reject();
}
