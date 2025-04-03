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

#ifndef TALIPOT_SIMPLE_STRINGS_LIST_SELECTION_WIDGET_H
#define TALIPOT_SIMPLE_STRINGS_LIST_SELECTION_WIDGET_H

#include <QWidget>

#include "StringsListSelectionWidgetInterface.h"

namespace Ui {
class SimpleStringsListSelectionWidget;
}

class QListWidgetItem;

namespace tlp {

class TLP_QT_SCOPE SimpleStringsListSelectionWidget : public QWidget,
                                                      public StringsListSelectionWidgetInterface {

    Q_OBJECT

    Ui::SimpleStringsListSelectionWidget *_ui;

  public:
    SimpleStringsListSelectionWidget(QWidget *parent = nullptr,
                                     const uint maxSelectedStringsListSize = 0);
    ~SimpleStringsListSelectionWidget() override;

    void setUnselectedStringsList(const std::vector<std::string> &unselectedStringsList) override;

    void setSelectedStringsList(const std::vector<std::string> &selectedStringsList) override;

    void clearUnselectedStringsList() override;

    void clearSelectedStringsList() override;

    void setMaxSelectedStringsListSize(const uint maxSelectedStringsListSize) override;

    std::vector<std::string> getSelectedStringsList() const override;

    std::vector<std::string> getUnselectedStringsList() const override;

    void selectAllStrings() override;

    void unselectAllStrings() override;

  private slots:

    void pressButtonSelectAll();
    void pressButtonUnselectAll();
    void listItemClicked(QListWidgetItem *item) const;
    void pressButtonUp();
    void pressButtonDown();

  private:
    void qtWidgetsConnection();

    uint maxSelectedStringsListSize;
};
}

#endif // TALIPOT_SIMPLE_STRINGS_LIST_SELECTION_WIDGET_H
