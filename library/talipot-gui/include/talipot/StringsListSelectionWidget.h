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

#ifndef TALIPOT_STRINGS_LIST_SELECTION_WIDGET_H
#define TALIPOT_STRINGS_LIST_SELECTION_WIDGET_H

#include <vector>

#include <talipot/StringsListSelectionWidgetInterface.h>

#include <QWidget>

namespace tlp {

/** \brief A widget for selecting a set of strings
 *
 * This widget allow to select a subset of strings from an initial input strings list.
 * The look of the widget can be set via the ListType parameter :
 *    -> SIMPLE_LIST : the widget contains only one strings list, the selection of strings is done
 * via the checkboxes located on the left of the items list
 *    -> DOUBLE_LIST : the widget contains two lists, the left one contains the unselected string
 * list and the right one the selected strings list. To select
 *                     a string (resp. deselect a string), it has to be moved from the list on the
 * left to the list on the right (resp. from the list on the right to
 *                     the list on the left) via the buttons located between the two lists or by
 * drag'n drop.
 */
class TLP_QT_SCOPE StringsListSelectionWidget : public QWidget,
                                                public StringsListSelectionWidgetInterface {

public:
  enum ListType { SIMPLE_LIST, DOUBLE_LIST };

  /**
   * Default constructor (for qt designer)
   * \param parent the widget's parent
   * \param listType this parameter defines the widget's look (see class description)
   * \param maxSelectedStringsListSize the maximum number of strings that can be selected (if 0, no
   * size restriction)
   */
  StringsListSelectionWidget(QWidget *parent = nullptr, const ListType listType = DOUBLE_LIST,
                             const uint maxSelectedStringsListSize = 0);

  /**
   * This constructor creates the widget and initializes the unselected strings list
   * \param unselectedStringsList a vector containing the set of strings that can be selected
   * \param parent the widget's parent
   * \param listType this parameter defines the widget's look (see class description)
   * \param maxSelectedStringsListSize the maximum number of strings that can be selected (if 0, no
   * size restriction)
   */
  StringsListSelectionWidget(const std::vector<std::string> &unselectedStringsList,
                             QWidget *parent = nullptr, const ListType listType = DOUBLE_LIST,
                             const uint maxSelectedStringsListSize = 0);

  /**
   * Method which sets the look of the widget
   * \param listType this parameter defines the widget's look (see class description)
   */
  void setListType(const ListType listType);

  /**
   * Method which sets the unselected strings list
   * \param unselectedStringsList a vector containing a set of strings to be unselected
   */
  void setUnselectedStringsList(const std::vector<std::string> &unselectedStringsList) override;

  /**
   * Method which sets the selected strings list
   * \param selectedStringsList a vector containing a set of strings to be selected
   */
  void setSelectedStringsList(const std::vector<std::string> &selectedStringsList) override;

  /**
   * Method which empty the unselected strings list
   */
  void clearUnselectedStringsList() override;

  /**
   * Method which empty the selected strings list
   */
  void clearSelectedStringsList() override;

  /**
   * Method which sets the label text value of the unselected strings list
   * (this method does nothing if listType = SIMPLE_LIST)
   */
  void setUnselectedStringsListLabel(const std::string &unselectedStringsListLabel);

  /**
   * Method which sets the label text value of the selected strings list
   * (this method does nothing if listType = SIMPLE_LIST)
   */
  void setSelectedStringsListLabel(const std::string &selectedStringsListLabel);

  /**
   * Method which sets the maximum size of the selected strings list
   */
  void setMaxSelectedStringsListSize(const uint maxSelectedStringsListSize) override;

  /**
   * Method which returns the selected strings as a vector
   */
  std::vector<std::string> getSelectedStringsList() const override;

  /**
   * Method which returns the unselected strings as a vector
   */
  std::vector<std::string> getUnselectedStringsList() const override;

  /**
   * Method which returns both of the selected and unselected strings as a vector
   */
  std::vector<std::string> getCompleteStringsList() const;

  /**
   * Method which selects all strings
   */
  void selectAllStrings() override;

  /**
   * Method which deselect all strings
   */
  void unselectAllStrings() override;

private:
  ListType listType;
  StringsListSelectionWidgetInterface *stringsListSelectionWidget;
};
}

#endif // TALIPOT_STRINGS_LIST_SELECTION_WIDGET_H
