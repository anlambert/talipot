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

#ifndef DOUBLE_STRINGS_LIST_RELATION_DIALOG_H
#define DOUBLE_STRINGS_LIST_RELATION_DIALOG_H

#include <QDialog>

#include <talipot/Color.h>

namespace Ui {
class DoubleStringsListRelationDialog;
}

namespace tlp {

class DoubleStringsListRelationDialog : public QDialog {

    Q_OBJECT

    Ui::DoubleStringsListRelationDialog *_ui;

  public:
    DoubleStringsListRelationDialog(const std::vector<std::string> &firstValues,
                                    const std::vector<Color> &secondValues,
                                    QWidget *parent = nullptr);
    ~DoubleStringsListRelationDialog() override;
    void getResult(std::vector<std::pair<std::string, Color>> &result);

  private slots:
    void upButtonClicked();
    void downButtonClicked();
    void upButtonColorClicked();
    void downButtonColorClicked();
    void scrollBarValueChanged(int value);
    void interpolateCheckBoxChange(int state);

  private:
    std::vector<Color> lastNonInterpolateValues;
};
}

#endif // DOUBLE_STRINGS_LIST_RELATION_DIALOG_H
