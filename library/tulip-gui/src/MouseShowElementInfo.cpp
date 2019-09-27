/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */
#include "ui_ElementInformationWidget.h"

#include <QPropertyAnimation>
#include <QGraphicsView>
#include <QHeaderView>
#include <QMouseEvent>
#include <QGraphicsProxyWidget>
#include <QSortFilterProxyModel>

#include <tulip/GraphElementModel.h>
#include <tulip/TulipItemDelegate.h>
#include <tulip/GlMainView.h>
#include <tulip/GlMainWidget.h>
#include <tulip/GlScene.h>
#include <tulip/MouseShowElementInfo.h>
#include <tulip/TulipMetaTypes.h>

using namespace std;
using namespace tlp;

MouseShowElementInfo::MouseShowElementInfo(const bool showVisualPropButton)
    : _ui(new Ui::ElementInformationWidget), _informationWidget(new QWidget()),
      _informationWidgetItem(new QGraphicsProxyWidget()), glMainWidget(nullptr), _show(true) {
  _informationWidget->installEventFilter(this);
  _ui->setupUi(_informationWidget);
// workaround to get rid of Qt5 warnings : QMacCGContext:: Unsupported painter devtype type 1
// see https://bugreports.qt.io/browse/QTBUG-32639
#if defined(__APPLE__)
  _informationWidget->setWindowOpacity(0.99);
#endif
  tableView()->setItemDelegate(new TulipItemDelegate(tableView()));
  _informationWidgetItem->setWidget(_informationWidget);
  _informationWidgetItem->setVisible(false);

  if (showVisualPropButton)
    connect(_ui->displayTulipProp, SIGNAL(toggled(bool)), this, SLOT(showVisualProp(bool)));
  else
    _ui->displayTulipProp->hide();
  connect(_ui->closeButton, SIGNAL(clicked()), this, SLOT(hideInfos()));
}

MouseShowElementInfo::~MouseShowElementInfo() {
  delete _informationWidget;
  delete _ui;
}

void MouseShowElementInfo::showVisualProp(bool show) {
  if (show) {
    _model->setFilterRegExp("");
  } else {
    // filter out properties whose name starts with "view"
    _model->setFilterRegExp("^(?!view[A-Z]).?");
  }
  _show = show;
}

void MouseShowElementInfo::hideInfos() {
  tableView()->setModel(nullptr);
  clear();
}

void MouseShowElementInfo::clear() {
  _informationWidgetItem->setVisible(false);

  if (glMainWidget)
    glMainWidget->setCursor(QCursor());
}

QTableView *MouseShowElementInfo::tableView() const {
  return _informationWidget->findChild<QTableView *>();
}

bool MouseShowElementInfo::eventFilter(QObject *widget, QEvent *e) {

  if (widget == _informationWidget &&
      (e->type() == QEvent::Wheel || e->type() == QEvent::MouseButtonPress)) {
    return true;
  }

  // ensure the info window stays visible while using the wheel or clicking in it
  if (_informationWidget->isVisible() &&
      (e->type() == QEvent::Wheel || e->type() == QEvent::MouseButtonPress)) {
    QRect widgetRect = _informationWidget->geometry();
    QPoint cursorPos;
    if (e->type() == QEvent::Wheel) {
      cursorPos = static_cast<QWheelEvent *>(e)->pos();
    } else {
      cursorPos = static_cast<QMouseEvent *>(e)->pos();
    }

    if (!widgetRect.contains(cursorPos)) {
      _informationWidgetItem->setVisible(false);
      return false;
    } else {
      return true;
    }
  }

  QMouseEvent *qMouseEv = dynamic_cast<QMouseEvent *>(e);

  if (qMouseEv != nullptr) {
    if (glMainWidget == nullptr)
      glMainWidget = dynamic_cast<GlMainWidget *>(widget);

    assert(glMainWidget);

    SelectedEntity selectedEntity;

    if (e->type() == QEvent::MouseMove) {
      if (pick(qMouseEv->x(), qMouseEv->y(), selectedEntity)) {
        glMainWidget->setCursor(Qt::WhatsThisCursor);
      } else {
        glMainWidget->setCursor(QCursor());
      }

      return false;
    } else if (e->type() == QEvent::MouseButtonPress && qMouseEv->button() == Qt::LeftButton) {
      if (_informationWidgetItem->isVisible()) {
        // Hide widget if we click outside it
        _informationWidgetItem->setVisible(false);
      }

      if (!_informationWidgetItem->isVisible()) {

        // Show widget if we click on node or edge
        if (pick(qMouseEv->x(), qMouseEv->y(), selectedEntity)) {
          if (selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED ||
              selectedEntity.getEntityType() == SelectedEntity::EDGE_SELECTED) {

            QLabel *title = _informationWidget->findChild<QLabel *>();

            ElementType eltType =
                selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED ? NODE : EDGE;

            // set the table view as the parent of the models as it
            // takes ownership of them in that case (and thus
            _model = new QSortFilterProxyModel(tableView());
            _model->setFilterRole(GraphEdgeElementModel::PropertyNameRole);
            _model->setSourceModel(
                buildModel(eltType, selectedEntity.getComplexEntityId(), tableView()));
            showVisualProp(_show);
            tableView()->setModel(_model);
            title->setText(elementName(eltType, selectedEntity.getComplexEntityId()));

            QPoint position = qMouseEv->pos();

            if (position.x() + _informationWidgetItem->rect().width() >
                _view->graphicsView()->sceneRect().width() - 5)
              position.setX(_view->graphicsView()->sceneRect().width() -
                            _informationWidgetItem->rect().width() - 5);

            if (position.y() + _informationWidgetItem->rect().height() >
                _view->graphicsView()->sceneRect().height() - 5)
              position.setY(_view->graphicsView()->sceneRect().height() -
                            _informationWidgetItem->rect().height() - 5);

            _informationWidgetItem->setPos(position);
            _informationWidgetItem->setVisible(true);
            QPropertyAnimation *animation =
                new QPropertyAnimation(_informationWidgetItem, "opacity");
            connect(animation, SIGNAL(finished()), animation, SLOT(deleteLater()));
            animation->setDuration(100);
            animation->setStartValue(0.);
            animation->setEndValue(1);
            animation->start();

            return true;
          } else {
            return false;
          }
        }
      }
    }
  }

  return false;
}

bool MouseShowElementInfo::pick(int x, int y, SelectedEntity &selectedEntity) {
  assert(glMainWidget);
  return glMainWidget->pickNodesEdges(x, y, selectedEntity);
}

void MouseShowElementInfo::viewChanged(View *view) {
  if (view == nullptr) {
    _view = nullptr;
    return;
  }

  ViewWidget *viewWidget = dynamic_cast<ViewWidget *>(view);
  assert(viewWidget);
  _view = viewWidget;
  connect(_view, SIGNAL(graphSet(tlp::Graph *)), _informationWidgetItem, SLOT(close()));
  _view->graphicsView()->scene()->addItem(_informationWidgetItem);
}

QAbstractItemModel *MouseShowElementInfo::buildModel(ElementType elementType,
                                                     unsigned int elementId,
                                                     QObject *parent) const {
  if (elementType == NODE) {
    return new GraphNodeElementModel(view()->graph(), elementId, parent);
  } else {
    return new GraphEdgeElementModel(view()->graph(), elementId, parent);
  }
}

QString MouseShowElementInfo::elementName(ElementType elementType, unsigned int elementId) const {
  QString elementTypeLabel = elementType == NODE ? QString("Node") : QString("Edge");
  return elementTypeLabel + " #" + QString::number(elementId);
}
