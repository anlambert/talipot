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

#include "GeographicViewShowElementInfo.h"
#include "GeographicViewGraphicsView.h"

#include "ui_ElementInformationWidget.h"
#include "GeographicViewInteractors.h"
#include "../../utils/StandardInteractorPriority.h"
#include "../../utils/InteractorIcons.h"

#include <talipot/ItemDelegate.h>
#include <talipot/GraphElementModel.h>
#include <talipot/GlEntityItemModel.h>
#include <talipot/GlComplexPolygon.h>

#include <QPropertyAnimation>

using namespace std;
using namespace tlp;

// this class is needed to allow interactive settings
// of some GlComplexPolygon rendering properties
class tlp::GlComplexPolygonItemEditor : public GlEntityItemEditor {
public:
  GlComplexPolygonItemEditor(GlComplexPolygon *poly) : GlEntityItemEditor(poly) {}

  GlComplexPolygon *getGlComplexPolygon() const {
    return static_cast<GlComplexPolygon *>(entity);
  }

  // redefined inherited methods from GlEntityItemEditor
  QStringList propertiesNames() const override {
    return {"fillColor", "outlineColor"};
  }

  QVariantList propertiesQVariant() const override {
    return {QVariant::fromValue<Color>(getGlComplexPolygon()->getFillColor()),
            QVariant::fromValue<Color>(getGlComplexPolygon()->getOutlineColor())};
  }

  void setProperty(const QString &name, const QVariant &value) override {
    if (name == "fillColor") {
      getGlComplexPolygon()->setFillColor(value.value<Color>());
    } else if (name == "outlineColor") {
      getGlComplexPolygon()->setOutlineColor(value.value<Color>());
    }
  }
};

class GeographicViewInteractorGetInformation : public NodeLinkDiagramViewInteractor {

public:
  PLUGININFORMATION("GeographicViewInteractorGetInformation", "Tulip Team", "06/2012",
                    "Geographic View Get Information Interactor", "1.0", "Information")
  /**
   * Default constructor
   */
  GeographicViewInteractorGetInformation(const tlp::PluginContext *)
      : NodeLinkDiagramViewInteractor(interactorIcon(InteractorType::GetInformation),
                                      "Get information on nodes/edges",
                                      StandardInteractorPriority::GetInformation) {}

  /**
   * Construct chain of responsibility
   */
  void construct() override {
    setConfigurationWidgetText(QString("<h3>Get information interactor</h3>") +
                               "<b>Mouse left</b> click on an element to display its properties");
    push_back(new GeographicViewNavigator);
    push_back(new GeographicViewShowElementInfo);
  }

  bool isCompatible(const string &viewName) const override {
    return (viewName == ViewName::GeographicViewName);
  }
};

PLUGIN(GeographicViewInteractorGetInformation)

GeographicViewShowElementInfo::GeographicViewShowElementInfo() : _editor(nullptr) {
  Ui::ElementInformationWidget ui;
  _informationWidget = new QWidget();
  _informationWidget->installEventFilter(this);
  ui.setupUi(_informationWidget);
  ui.displayTalipotProp->hide();
  connect(ui.closeButton, &QAbstractButton::clicked, this,
          &GeographicViewShowElementInfo::hideInfos);
  tableView()->setItemDelegate(new ItemDelegate(tableView()));
  _informationWidgetItem = new QGraphicsProxyWidget();
  _informationWidgetItem->setWidget(_informationWidget);
  _informationWidgetItem->setVisible(false);
}

GeographicViewShowElementInfo::~GeographicViewShowElementInfo() {
  delete _informationWidgetItem;
}

void GeographicViewShowElementInfo::clear() {
  static_cast<GeographicView *>(view())->getGeographicViewGraphicsView()->glWidget()->setCursor(
      QCursor());
  _informationWidgetItem->setVisible(false);
}

void GeographicViewShowElementInfo::hideInfos() {
  tableView()->setModel(nullptr);
  clear();
}

QTableView *GeographicViewShowElementInfo::tableView() const {
  return _informationWidget->findChild<QTableView *>();
}

bool GeographicViewShowElementInfo::eventFilter(QObject *widget, QEvent *e) {
  if (widget == _informationWidget &&
      (e->type() == QEvent::Wheel || e->type() == QEvent::MouseButtonPress)) {
    return true;
  }

  if (_informationWidget->isVisible() && e->type() == QEvent::Wheel) {
    _informationWidgetItem->setVisible(false);
    return false;
  }

  auto *qMouseEv = dynamic_cast<QMouseEvent *>(e);

  if (qMouseEv != nullptr) {
    auto *geoView = static_cast<GeographicView *>(view());
    SelectedEntity selectedEntity;

    if (e->type() == QEvent::MouseMove) {
      if (pick(qMouseEv->pos().x(), qMouseEv->pos().y(), selectedEntity)) {
        geoView->getGeographicViewGraphicsView()->glWidget()->setCursor(Qt::WhatsThisCursor);
      } else {
        geoView->getGeographicViewGraphicsView()->glWidget()->setCursor(QCursor());
      }

      return false;
    } else if (e->type() == QEvent::MouseButtonPress && qMouseEv->button() == Qt::LeftButton) {
      if (_informationWidgetItem->isVisible()) {
        // Hide widget if we click outside it
        _informationWidgetItem->setVisible(false);
      }

      if (!_informationWidgetItem->isVisible()) {

        // Show widget if we click on node or edge
        if (pick(qMouseEv->pos().x(), qMouseEv->pos().y(), selectedEntity)) {
          if (selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED ||
              selectedEntity.getEntityType() == SelectedEntity::EDGE_SELECTED) {
            _informationWidgetItem->setVisible(true);

            auto *title = _informationWidget->findChild<QLabel *>();

            if (selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED) {
              title->setText("Node");
              tableView()->setModel(new GraphNodeElementModel(
                  _view->graph(), selectedEntity.getGraphElementId(), _informationWidget));
            } else {
              title->setText("Edge");
              tableView()->setModel(new GraphEdgeElementModel(
                  _view->graph(), selectedEntity.getGraphElementId(), _informationWidget));
            }

            title->setText(title->text() + " #" +
                           QString::number(selectedEntity.getGraphElementId()));

            QPoint position = qMouseEv->pos();

            if (position.x() + _informationWidgetItem->rect().width() >
                _view->graphicsView()->sceneRect().width() - 5) {
              position.setX(_view->graphicsView()->sceneRect().width() -
                            _informationWidgetItem->rect().width() - 5);
            }

            if (position.y() + _informationWidgetItem->rect().height() >
                _view->graphicsView()->sceneRect().height()) {
              position.setY(_view->graphicsView()->sceneRect().height() -
                            _informationWidgetItem->rect().height() - 5);
            }

            _informationWidgetItem->setPos(position);
            auto *animation = new QPropertyAnimation(_informationWidgetItem, "opacity");
            animation->setDuration(100);
            animation->setStartValue(0.);
            animation->setEndValue(1.);
            animation->start();

            return true;
          } else if (selectedEntity.getEntityType() == SelectedEntity::SIMPLE_ENTITY_SELECTED) {

            auto *polygon = dynamic_cast<GlComplexPolygon *>(selectedEntity.getEntity());

            if (!polygon) {
              return false;
            }

            _informationWidgetItem->setVisible(true);
            auto *title = _informationWidget->findChild<QLabel *>();
            title->setText(selectedEntity.getEntity()
                               ->getParent()
                               ->findKey(selectedEntity.getEntity())
                               .c_str());

            delete _editor;

            _editor = new GlComplexPolygonItemEditor(polygon);

            tableView()->setModel(new GlEntityItemModel(_editor, _informationWidget));
            int size = title->height() + _informationWidget->layout()->spacing() +
                       tableView()->rowHeight(0) + tableView()->rowHeight(1) + 10;
            _informationWidget->setMaximumHeight(size);

            QPoint position = qMouseEv->pos();

            if (position.x() + _informationWidgetItem->rect().width() >
                _view->graphicsView()->sceneRect().width()) {
              position.setX(qMouseEv->pos().x() - _informationWidgetItem->rect().width());
            }

            if (position.y() + _informationWidgetItem->rect().height() >
                _view->graphicsView()->sceneRect().height()) {
              position.setY(qMouseEv->pos().y() - _informationWidgetItem->rect().height());
            }

            _informationWidgetItem->setPos(position);
            auto *animation = new QPropertyAnimation(_informationWidgetItem, "opacity");
            animation->setDuration(100);
            animation->setStartValue(0.);
            animation->setEndValue(1.);
            animation->start();
          } else {
            return false;
          }
        }
      }
    }
  }

  return false;
}

bool GeographicViewShowElementInfo::pick(int x, int y, SelectedEntity &selectedEntity) {
  auto *geoView = static_cast<GeographicView *>(view());

  if (geoView->getGeographicViewGraphicsView()->glWidget()->pickNodesEdges(x, y, selectedEntity)) {
    return true;
  }

  vector<SelectedEntity> selectedEntities;

  if (geoView->getGeographicViewGraphicsView()->glWidget()->pickGlEntities(x, y,
                                                                           selectedEntities)) {
    selectedEntity = selectedEntities[0];
    return true;
  }

  return false;
}

void GeographicViewShowElementInfo::viewChanged(View *view) {
  if (view == nullptr) {
    _view = nullptr;
    return;
  }

  auto *geoView = static_cast<GeographicView *>(view);
  _view = geoView;
  connect(_view, &View::graphSet, _informationWidgetItem, &QGraphicsWidget::close);
  _view->getGeographicViewGraphicsView()->scene()->addItem(_informationWidgetItem);
}
