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

#include "ui_ElementInformationWidget.h"

#include <QPropertyAnimation>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>

#include <talipot/GraphElementModel.h>
#include <talipot/ItemDelegate.h>
#include <talipot/GlView.h>
#include <talipot/MouseShowElementInfo.h>

using namespace std;
using namespace tlp;

MouseShowElementInfo::MouseShowElementInfo(const bool showVisualPropButton)
    : _ui(new Ui::ElementInformationWidget), _informationWidget(new QWidget()),
      _informationWidgetItem(new QGraphicsProxyWidget()), glWidget(nullptr), _show(true) {
  _informationWidget->installEventFilter(this);
  _ui->setupUi(_informationWidget);
  tableView()->setItemDelegate(new ItemDelegate(tableView()));
  _informationWidgetItem->setWidget(_informationWidget);
  _informationWidgetItem->setVisible(false);

  if (showVisualPropButton) {
    connect(_ui->displayTalipotProp, &QAbstractButton::toggled, this,
            &MouseShowElementInfo::showVisualProp);
  } else {
    _ui->displayTalipotProp->hide();
  }
  connect(_ui->closeButton, &QAbstractButton::clicked, this, &MouseShowElementInfo::hideInfos);
}

MouseShowElementInfo::~MouseShowElementInfo() {
  delete _informationWidget;
  delete _ui;
}

void MouseShowElementInfo::showVisualProp(bool show) {
  QString regexp;
  if (!show) {
    // filter out properties whose name starts with "view"
    regexp = "^(?!view[A-Z]).?";
  }
#if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
  _model->setFilterRegExp(regexp);
#else
  _model->setFilterRegularExpression(regexp);
#endif
  _show = show;
}

void MouseShowElementInfo::hideInfos() {
  tableView()->setModel(nullptr);
  clear();
}

void MouseShowElementInfo::clear() {
  _informationWidgetItem->setVisible(false);

  if (glWidget) {
    glWidget->setCursor(QCursor());
  }
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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
      cursorPos = static_cast<QWheelEvent *>(e)->position().toPoint();
#else
      cursorPos = static_cast<QWheelEvent *>(e)->pos();
#endif
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

  auto *qMouseEv = dynamic_cast<QMouseEvent *>(e);

  if (qMouseEv != nullptr) {
    if (glWidget == nullptr) {
      glWidget = dynamic_cast<GlWidget *>(widget);
    }

    assert(glWidget);

    SelectedEntity selectedEntity;

    if (e->type() == QEvent::MouseMove) {
      if (pick(qMouseEv->pos().x(), qMouseEv->pos().y(), selectedEntity)) {
        glWidget->setCursor(Qt::WhatsThisCursor);
      } else {
        glWidget->setCursor(QCursor());
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

            auto *title = _informationWidget->findChild<QLabel *>();

            ElementType eltType = selectedEntity.getEntityType() == SelectedEntity::NODE_SELECTED
                                      ? ElementType::NODE
                                      : ElementType::EDGE;

            // set the table view as the parent of the models as it
            // takes ownership of them in that case (and thus
            _model = new QSortFilterProxyModel(tableView());
            _model->setFilterRole(GraphEdgeElementModel::PropertyNameRole);
            _model->setSourceModel(
                buildModel(eltType, selectedEntity.getGraphElementId(), tableView()));
            showVisualProp(_show);
            tableView()->setModel(_model);
            title->setText(elementName(eltType, selectedEntity.getGraphElementId()));

            QPoint position = qMouseEv->pos();

            if (position.x() + _informationWidgetItem->rect().width() >
                _view->graphicsView()->sceneRect().width() - 5) {
              position.setX(_view->graphicsView()->sceneRect().width() -
                            _informationWidgetItem->rect().width() - 5);
            }

            if (position.y() + _informationWidgetItem->rect().height() >
                _view->graphicsView()->sceneRect().height() - 5) {
              position.setY(_view->graphicsView()->sceneRect().height() -
                            _informationWidgetItem->rect().height() - 5);
            }

            _informationWidgetItem->setPos(position);
            _informationWidgetItem->setVisible(true);
            auto *animation = new QPropertyAnimation(_informationWidgetItem, "opacity");
            connect(animation, &QAbstractAnimation::finished, animation, &QObject::deleteLater);
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
  assert(glWidget);
  return glWidget->pickNodesEdges(x, y, selectedEntity);
}

void MouseShowElementInfo::viewChanged(View *view) {
  if (view == nullptr) {
    _view = nullptr;
    return;
  }

  auto *viewWidget = dynamic_cast<ViewWidget *>(view);
  assert(viewWidget);
  _view = viewWidget;
  connect(_view, &View::graphSet, _informationWidgetItem, &QGraphicsWidget::close);
  _view->graphicsView()->scene()->addItem(_informationWidgetItem);
}

QAbstractItemModel *MouseShowElementInfo::buildModel(ElementType elementType, uint elementId,
                                                     QObject *parent) const {
  if (elementType == ElementType::NODE) {
    return new GraphNodeElementModel(view()->graph(), elementId, parent);
  } else {
    return new GraphEdgeElementModel(view()->graph(), elementId, parent);
  }
}

QString MouseShowElementInfo::elementName(ElementType elementType, uint elementId) const {
  QString elementTypeLabel = elementType == ElementType::NODE ? QString("Node") : QString("Edge");
  return elementTypeLabel + " #" + QString::number(elementId);
}
