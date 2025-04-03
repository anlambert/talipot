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

#ifndef TALIPOT_STRINGS_LIST_SELECTION_DIALOG_H
#define TALIPOT_STRINGS_LIST_SELECTION_DIALOG_H

#include <QDialog>

#include <talipot/config.h>
#include <talipot/StringsListSelectionWidget.h>

namespace Ui {
class StringsListSelectionDialog;
}

namespace tlp {

/**
 * @brief Provide a dialog that allow user to select a list of strings.
 *
 * The easiest way to use this class is to use the static function.
 * @code
 * QString title;
 * QWidget* parent;
 * vector<string> listOfStrings; // must be initialized
 * vector<string> selectedStrings;
 * bool chosen = StringsListSelectionDialog::choose(title, parent, listOfStrings,
 *selectedStrings);
 * @endcode
 *
 *
 **/
class TLP_QT_SCOPE StringsListSelectionDialog : public QDialog {
    Q_OBJECT

  public:
    /**
     * @brief Constructs a dialog with the given parent.
     **/
    explicit StringsListSelectionDialog(QWidget *parent = nullptr);

    /**
     * set the look of the widget
     * \param listType this parameter defines the widget's look (see class description)
     */
    void setListType(const StringsListSelectionWidget::ListType listType);

    /**
     * sets the maximum size of the selected strings list
     */
    void setMaxSelectedStringsListSize(const uint maxSize);

    /**
     * sets the strings list and the already selected strings
     */
    void setStringsList(const std::vector<std::string> &strList, std::vector<std::string> &selList);

    /**
     * @brief Constructs a strings list selection dialog with the given parent.
     * \param parent the widget's parent
     * \param listType this parameter defines the widget's look (see class description)
     * \param maxSelectedStringsListSize the maximum number of strings that can be selected (if 0,
     *no size restriction)
     **/
    explicit StringsListSelectionDialog(QString title, QWidget *parent = nullptr,
                                        const StringsListSelectionWidget::ListType listType =
                                            StringsListSelectionWidget::SIMPLE_LIST,
                                        const uint maxSelectedStringsListSize = 0);
    ~StringsListSelectionDialog() override;

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
    static bool choose(QString title, const std::vector<std::string> &strList,
                       std::vector<std::string> &selList, QWidget *parent = nullptr,
                       const StringsListSelectionWidget::ListType listType =
                           StringsListSelectionWidget::SIMPLE_LIST,
                       const uint maxSize = 0);

  private:
    Ui::StringsListSelectionDialog *ui;
};
}

#endif // TALIPOT_STRINGS_LIST_SELECTION_DIALOG_H
