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

#include <talipot/StringsListSelectionDialog.h>
#include "ui_StringsListSelectionDialog.h"

using namespace std;

namespace tlp {
StringsListSelectionDialog::StringsListSelectionDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::StringsListSelectionDialog) {
    ui->setupUi(this);
}

/**
 * set the look of the widget
 * \param listType this parameter defines the widget's look (see class description)
 */
void StringsListSelectionDialog::setListType(const StringsListSelectionWidget::ListType listType) {
    ui->stringsListSelectionWidget->setListType(listType);
}

/**
 * sets the maximum size of the selected strings list
 */
void StringsListSelectionDialog::setMaxSelectedStringsListSize(const uint maxSize) {
    ui->stringsListSelectionWidget->setMaxSelectedStringsListSize(maxSize);
}

/**
 * sets the strings list and the already selected strings
 */
void StringsListSelectionDialog::setStringsList(const std::vector<std::string> &strList,
                                                std::vector<std::string> &selList) {
    ui->stringsListSelectionWidget->setUnselectedStringsList(strList);
    ui->stringsListSelectionWidget->setSelectedStringsList(selList);
}

/**
 * @brief Constructs a strings list selection dialog with the given parent.
 * \param parent the widget's parent
 * \param listType this parameter defines the widget's look (see class description)
 * \param maxSelectedStringsListSize the maximum number of strings that can be selected (if 0, no
 *size restriction)
 **/
StringsListSelectionDialog::StringsListSelectionDialog(
    QString title, QWidget *parent, const StringsListSelectionWidget::ListType listType,
    const uint maxSize)
    : QDialog(parent), ui(new Ui::StringsListSelectionDialog) {
    ui->setupUi(this);
    setWindowTitle(title);
    setListType(listType);
    setMaxSelectedStringsListSize(maxSize);
}

StringsListSelectionDialog::~StringsListSelectionDialog() {
    if (ui) {
        delete ui;
    }
}

/**
 * @brief This is a convenience static function that create a dialog
 * to select some strings in a list.
 * The function creates a modal dialog with the given title and parent widget.
 *
 * @param title the string to display in the dialog title bar,
 * @param strList the list of strings to choose,
 * @param selList on input it indicates the already selected strings, then on output the user
 *selected strings,
 * @param listType the type of display for the strings to choose,
 * @param maxSize the maximum number of strings to select. 0 means this number is not limited.
 **/
bool StringsListSelectionDialog::choose(QString title, const std::vector<std::string> &strList,
                                        std::vector<std::string> &selList, QWidget *parent,
                                        const StringsListSelectionWidget::ListType listType,
                                        const uint maxSize) {
    StringsListSelectionDialog dialog(title, parent, listType, maxSize);
    dialog.setStringsList(strList, selList);
    int result = dialog.exec();

    if (result == QDialog::Accepted) {
        selList = dialog.ui->stringsListSelectionWidget->getSelectedStringsList();
        return true;
    }

    return false;
}
}
