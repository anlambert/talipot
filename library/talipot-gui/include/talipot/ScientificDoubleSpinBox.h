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

// original code is written in Python and is taken from
// https://jdreaver.com/posts/2014-07-28-scientific-notation-spin-box-pyside.html
// see also https://gist.github.com/jdreaver/0be2e44981159d0854f5

#ifndef TALIPOT_SCIENTIFIC_DOUBLE_SPIN_BOX_H
#define TALIPOT_SCIENTIFIC_DOUBLE_SPIN_BOX_H

#include <talipot/config.h>

#include <QDoubleSpinBox>
#include <QValidator>

namespace tlp {

/**
 * @brief Utility class used to validate that a QString contains a valid representation of a
 * floating point number
 * including scientific notation
 *
 */
class TLP_QT_SCOPE FloatValidator : public QValidator {

  public:
    State validate(QString &input, int &pos) const override;

    void fixup(QString &text) const override;
};

/**
 * @brief Utility class implementing a QDoubleSpinBox supporting scientific notation
 *
 */
class TLP_QT_SCOPE ScientificDoubleSpinBox : public QDoubleSpinBox {

  public:
    ScientificDoubleSpinBox(QWidget *parent = nullptr);

    QValidator::State validate(QString &input, int &pos) const override;

    void fixup(QString &input) const override;

    double valueFromText(const QString &text) const override;

    QString textFromValue(double value) const override;

    void stepBy(int steps) override;
};
}

#endif // TALIPOT_SCIENTIFIC_DOUBLE_SPIN_BOX_H
