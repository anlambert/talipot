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

#include <QGraphicsProxyWidget>
#include <QGraphicsView>
#include <QApplication>
#include <QMenu>
#include <QComboBox>
#include <QPushButton>

#include <talipot/DoubleProperty.h>
#include <talipot/CaptionGraphicsItem.h>
#include <talipot/CaptionGraphicsSubItems.h>
#include <talipot/View.h>
#include <talipot/TlpQtTools.h>

using namespace std;

namespace tlp {

CaptionGraphicsItem::CaptionGraphicsItem(View *view) : _view(view) {

    _rondedRectItem = new CaptionGraphicsBackgroundItem(QRect(QPoint(0, 0), QSize(130, 260)));
    _rondedRectItem->setBrush(QBrush(QColor(255, 255, 255, 180)));
    connect(_rondedRectItem, &CaptionGraphicsBackgroundItem::filterChanged, this,
            &CaptionGraphicsItem::filterChangedSlot);

    _confPropertySelectionWidget = new QPushButton();
    _confPropertySelectionWidget->resize(QSize(120, 25));
    _confPropertySelectionItem = new QGraphicsProxyWidget(_rondedRectItem);
    _confPropertySelectionItem->setWidget(_confPropertySelectionWidget);
    _confPropertySelectionItem->setPos(5, 230);
    _confPropertySelectionItem->setZValue(2);
    _nodesEdgesTextItem = new QGraphicsSimpleTextItem(_rondedRectItem);
    connect(_confPropertySelectionWidget, &QAbstractButton::clicked, this,
            &CaptionGraphicsItem::selectPropertyButtonClicked);
}

void CaptionGraphicsItem::loadConfiguration() {
    constructConfigWidget();
}

void CaptionGraphicsItem::setType(uint type) {
    if (type == 1 || type == 2) {
        _nodesEdgesTextItem->setText("on nodes");
    } else {
        _nodesEdgesTextItem->setText("on edges");
    }

    _nodesEdgesTextItem->setPos(35, 210);
}

void CaptionGraphicsItem::generateColorCaption(const QGradient &activeGradient,
                                               const QGradient &hideGradient,
                                               const string &propertyName, double minValue,
                                               double maxValue) {
    _rondedRectItem->generateColorCaption(activeGradient, hideGradient, propertyName, minValue,
                                          maxValue);
}

void CaptionGraphicsItem::generateSizeCaption(
    const vector<pair<double, float>> &metricToSizeFilteredList, const string &propertyName,
    double minValue, double maxValue) {
    _rondedRectItem->generateSizeCaption(metricToSizeFilteredList, propertyName, minValue,
                                         maxValue);
}

void CaptionGraphicsItem::constructConfigWidget() {

    if (_view->graph() == nullptr) {
        _confPropertySelectionWidget->setText("");
        _confPropertySelectionWidget->setToolTip("");
        return;
    }

    QString oldName = _confPropertySelectionWidget->text();
    QString selectedProp;
    bool findViewMetric = false;
    QString firstDoubleMetricName;
    for (const string &piName : _view->graph()->getProperties()) {
        if (_view->graph()->getProperty(piName)->getTypename() != "double") {
            continue;
        }

        if (firstDoubleMetricName.isEmpty()) {
            firstDoubleMetricName = piName.c_str();
        }

        if (oldName == piName.c_str()) {
            selectedProp = piName.c_str();
        }

        if (piName == "viewMetric") {
            findViewMetric = true;
        }
    }

    if (selectedProp.isEmpty()) {
        if (findViewMetric) {
            selectedProp = "viewMetric";
        } else {
            selectedProp = firstDoubleMetricName;
        }
    }

    _confPropertySelectionWidget->setText(wrappedPropName(selectedProp));
    _confPropertySelectionWidget->setToolTip(selectedProp);
}

string CaptionGraphicsItem::usedProperty() {
    return QStringToTlpString(_confPropertySelectionWidget->toolTip());
}

CaptionGraphicsItem::~CaptionGraphicsItem() = default;

void CaptionGraphicsItem::filterChangedSlot(float begin, float end) {
    emit filterChanged(begin, end);
}

void CaptionGraphicsItem::selectPropertyButtonClicked() {
    if (_view->graph() == nullptr) {
        return;
    }

    QMenu menu;
    for (const string &piName : _view->graph()->getProperties()) {
        if (_view->graph()->getProperty(piName)->getTypename() != "double") {
            continue;
        }

        QAction *action =
            menu.addAction(piName.c_str(), this, &CaptionGraphicsItem::propertySelectedSlot);

        if (_confPropertySelectionWidget->text() == QString(piName.c_str())) {
            menu.setActiveAction(action);
        }
    }
    // set a combo like stylesheet
    QPalette palette = QComboBox().palette();
    menu.setStyleSheet(
        QString("QMenu::item {color: %1; background-color: %2;} QMenu::item:selected "
                "{color: %3; background-color: %4}")
            .arg(palette.color(QPalette::Active, QPalette::Text).name())
            .arg(palette.color(QPalette::Active, QPalette::Base).name())
            .arg(palette.color(QPalette::Active, QPalette::HighlightedText).name())
            .arg(palette.color(QPalette::Active, QPalette::Highlight).name()));

    // compute a combo like position
    // to popup the menu
    QWidget *pViewport = QApplication::widgetAt(QCursor::pos());
    QWidget *pView = pViewport->parentWidget();
    auto *pGraphicsView = qobject_cast<QGraphicsView *>(pView);
    QPoint popupPos = pGraphicsView->mapToGlobal(
        pGraphicsView->mapFromScene(_confPropertySelectionItem->mapToScene(
            _confPropertySelectionItem->subWidgetRect(_confPropertySelectionWidget).bottomLeft())));

    menu.exec(popupPos);
}

void CaptionGraphicsItem::propertySelectedSlot() {
    auto *action = static_cast<QAction *>(sender());
    _confPropertySelectionWidget->setText(wrappedPropName(action->text()));
    _confPropertySelectionWidget->setToolTip(action->text());
    emit selectedPropertyChanged(QStringToTlpString(action->text()));
}

QString CaptionGraphicsItem::wrappedPropName(const QString &originalName) const {
    return originalName;
}
}
