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

#include <talipot/SimpleStringsListSelectionWidget.h>
#include <talipot/DoubleStringsListSelectionWidget.h>
#include <talipot/StringsListSelectionWidget.h>

#include <QLayout>

using namespace std;

namespace tlp {

StringsListSelectionWidget::StringsListSelectionWidget(QWidget *parent, const ListType listType,
                                                       const uint maxSelectedStringsListSize)
    : QWidget(parent), listType(listType), stringsListSelectionWidget(nullptr) {
    setListType(listType);
    stringsListSelectionWidget->setMaxSelectedStringsListSize(maxSelectedStringsListSize);
}

StringsListSelectionWidget::StringsListSelectionWidget(const vector<string> &unselectedStringsList,
                                                       QWidget *parent, const ListType listType,
                                                       const uint maxSelectedStringsListSize)
    : QWidget(parent), listType(listType), stringsListSelectionWidget(nullptr) {
    setListType(listType);
    stringsListSelectionWidget->setMaxSelectedStringsListSize(maxSelectedStringsListSize);
    stringsListSelectionWidget->setUnselectedStringsList(unselectedStringsList);
}

void StringsListSelectionWidget::setListType(const ListType listType) {
    QLayout *currentLayout = layout();
    delete currentLayout;

    if (stringsListSelectionWidget != nullptr) {
        auto *widget = dynamic_cast<QWidget *>(stringsListSelectionWidget);
        delete widget;
    }

    if (listType == DOUBLE_LIST) {
        stringsListSelectionWidget = new DoubleStringsListSelectionWidget();
    } else {
        stringsListSelectionWidget = new SimpleStringsListSelectionWidget();
    }

    auto *newLayout = new QVBoxLayout;
    newLayout->addWidget(dynamic_cast<QWidget *>(stringsListSelectionWidget));
    setLayout(newLayout);
}

void StringsListSelectionWidget::setUnselectedStringsList(
    const vector<string> &unselectedStringsList) {
    stringsListSelectionWidget->setUnselectedStringsList(unselectedStringsList);
}

void StringsListSelectionWidget::setSelectedStringsList(const vector<string> &selectedStringsList) {
    stringsListSelectionWidget->setSelectedStringsList(selectedStringsList);
}

void StringsListSelectionWidget::clearUnselectedStringsList() {
    stringsListSelectionWidget->clearUnselectedStringsList();
}

void StringsListSelectionWidget::clearSelectedStringsList() {
    stringsListSelectionWidget->clearSelectedStringsList();
}

void StringsListSelectionWidget::setUnselectedStringsListLabel(
    const std::string &unselectedStringsListLabel) {
    if (listType == DOUBLE_LIST) {
        static_cast<DoubleStringsListSelectionWidget *>(stringsListSelectionWidget)
            ->setUnselectedStringsListLabel(unselectedStringsListLabel);
    }
}

void StringsListSelectionWidget::setSelectedStringsListLabel(
    const std::string &selectedStringsListLabel) {
    if (listType == DOUBLE_LIST) {
        static_cast<DoubleStringsListSelectionWidget *>(stringsListSelectionWidget)
            ->setSelectedStringsListLabel(selectedStringsListLabel);
    }
}

void StringsListSelectionWidget::setMaxSelectedStringsListSize(
    const uint maxSelectedStringsListSize) {
    stringsListSelectionWidget->setMaxSelectedStringsListSize(maxSelectedStringsListSize);
}

vector<string> StringsListSelectionWidget::getSelectedStringsList() const {
    return stringsListSelectionWidget->getSelectedStringsList();
}

vector<string> StringsListSelectionWidget::getUnselectedStringsList() const {
    return stringsListSelectionWidget->getUnselectedStringsList();
}

std::vector<std::string> StringsListSelectionWidget::getCompleteStringsList() const {
    vector<string> completeList = stringsListSelectionWidget->getSelectedStringsList();
    vector<string> unselected = stringsListSelectionWidget->getUnselectedStringsList();
    completeList.insert(completeList.end(), unselected.begin(), unselected.end());
    return completeList;
}

void StringsListSelectionWidget::selectAllStrings() {
    stringsListSelectionWidget->selectAllStrings();
}

void StringsListSelectionWidget::unselectAllStrings() {
    stringsListSelectionWidget->unselectAllStrings();
}
}
