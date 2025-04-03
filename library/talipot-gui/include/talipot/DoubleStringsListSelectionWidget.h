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

#ifndef TALIPOT_DOUBLE_STRINGS_LIST_SELECTION_WIDGET_H
#define TALIPOT_DOUBLE_STRINGS_LIST_SELECTION_WIDGET_H

#include <QWidget>

#include "StringsListSelectionWidgetInterface.h"

namespace Ui {
class DoubleStringsListSelectionWidget;
}

namespace tlp {

class TLP_QT_SCOPE DoubleStringsListSelectionWidget : public QWidget,
                                                      public StringsListSelectionWidgetInterface {

    Q_OBJECT

    Ui::DoubleStringsListSelectionWidget *_ui;

  public:
    DoubleStringsListSelectionWidget(QWidget *parent = nullptr,
                                     const uint maxSelectedStringsListSize = 0);
    ~DoubleStringsListSelectionWidget() override;

    void setUnselectedStringsList(const std::vector<std::string> &unselectedStringsList) override;

    void setSelectedStringsList(const std::vector<std::string> &selectedStringsList) override;

    void clearUnselectedStringsList() override;

    void clearSelectedStringsList() override;

    void setMaxSelectedStringsListSize(const uint maxSelectedStringsListSize) override;

    std::vector<std::string> getSelectedStringsList() const override;

    std::vector<std::string> getUnselectedStringsList() const override;

    void selectAllStrings() override;

    void unselectAllStrings() override;

    void setUnselectedStringsListLabel(const std::string &unselectedStringsListLabel);

    void setSelectedStringsListLabel(const std::string &selectedStringsListLabel);

  private slots:

    void pressButtonSelectAll();
    void pressButtonUnselectAll();
    void pressButtonAdd();
    void pressButtonRem();
    void pressButtonUp();
    void pressButtonDown();

  private:
    void qtWidgetsConnection();
};
}

#endif // TALIPOT_DOUBLE_STRINGS_LIST_SELECTION_WIDGET_H
