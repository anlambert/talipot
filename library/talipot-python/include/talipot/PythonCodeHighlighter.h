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

#ifndef TALIPOT_PYTHON_CODE_HIGHLIGHTER_H
#define TALIPOT_PYTHON_CODE_HIGHLIGHTER_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

class QTextDocument;

class PythonCodeHighlighter : public QSyntaxHighlighter {

    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> _highlightingRules;
    QTextCharFormat _keywordFormat;
    QTextCharFormat _classFormat;
    QTextCharFormat _commentFormat;
    QTextCharFormat _quotationFormat;
    QTextCharFormat _functionFormat;
    QTextCharFormat _numberFormat;
    QTextCharFormat _tlpApiFormat;
    QTextCharFormat _qtApiFormat;

    bool _shellMode;

  public:
    explicit PythonCodeHighlighter(QTextDocument *parent = nullptr);

    void setShellMode(const bool shellMode) {
        _shellMode = shellMode;
    }

    void buildHighlightingRules();

  protected:
    void highlightBlock(const QString &text) override;

    bool highlightMultilineString(const QString &text, const QRegularExpression &delimiter,
                                  const int inState, const QTextCharFormat &style);
};

#endif // TALIPOT_PYTHON_CODE_HIGHLIGHTER_H
