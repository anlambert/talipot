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

#include <talipot/TlpQtTools.h>

#include "talipot/PythonInterpreter.h"
#include "talipot/PythonCodeHighlighter.h"

using namespace tlp;

PythonCodeHighlighter::PythonCodeHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent), _shellMode(false) {
    buildHighlightingRules();
}

void PythonCodeHighlighter::buildHighlightingRules() {
    bool darkTheme = applicationHasDarkGuiTheme();

    _highlightingRules.clear();

    QTextCharFormat builtinFormat;
    builtinFormat.setForeground(darkTheme ? QColor("#8ab1b0") : QColor(0, 87, 187));

    HighlightingRule rule;

    _commentFormat.setForeground(darkTheme ? QColor("#6a9955") : QColor(Qt::darkGreen));
    _functionFormat.setFontWeight(QFont::Bold);
    _functionFormat.setForeground(darkTheme ? QColor("#dcdcaa") : QColor(Qt::darkCyan));
    _tlpApiFormat.setForeground(QColor(128, 128, 0));
    _classFormat.setFontWeight(QFont::Bold);
    _classFormat.setForeground(darkTheme ? QColor("#4ec9b0") : QColor(Qt::blue));

    rule.pattern = QRegularExpression("def [A-Za-z_][A-Za-z0-9_]+(?=\\()");
    rule.format = _functionFormat;
    _highlightingRules.append(rule);

    rule.pattern = QRegularExpression("class [A-Za-z_][A-Za-z0-9_]+");
    rule.format = _classFormat;
    _highlightingRules.append(rule);

    rule.pattern = QRegularExpression("tlp.*\\.[A-Za-z0-9_.]+");
    rule.format = _tlpApiFormat;
    _highlightingRules.append(rule);

    rule.pattern = QRegularExpression("^[ \t]*@.*$");
    rule.format = builtinFormat;
    _highlightingRules.append(rule);

    _keywordFormat.setForeground(darkTheme ? QColor("#c586c0") : QColor(Qt::darkBlue));
    _keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    int i = 0;

    while (PythonInterpreter::pythonKeywords[i]) {
        keywordPatterns << "\\b" + QString(PythonInterpreter::pythonKeywords[i++]) + "\\b";
    }

    QStringList specialCharsPatterns;
    specialCharsPatterns << "\\+" << "-" << "=" << "\\(" << "\\)" << "\\[" << "\\]" << "," << "!"
                         << "\\*" << "/" << "\\{" << "\\}" << ":" << "\\." << ">" << "<" << "%"
                         << "&" << "\\^" << "\\|";

    QString builtinModName = "__builtin__";

    if (PythonInterpreter::instance().getPythonVersion() >= 3.0) {
        builtinModName = "builtins";
    }

    if (PythonInterpreter::instance().runString(QString("import ") + builtinModName)) {
        QVector<QString> builtinDictContent =
            PythonInterpreter::instance().getObjectDictEntries(builtinModName);
        QStringList builtinPatterns;

        for (const auto &builtin : builtinDictContent) {
            builtinPatterns << "\\b" + builtin + "\\b";
        }

        builtinPatterns << "\\bself\\b";

        for (const QString &pattern : builtinPatterns) {
            rule.pattern = QRegularExpression(pattern);
            rule.format = builtinFormat;
            _highlightingRules.append(rule);
        }
    }

    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = _keywordFormat;
        _highlightingRules.append(rule);
    }

    QTextCharFormat format;
    format.setFontWeight(QFont::Bold);

    for (const QString &pattern : specialCharsPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = format;
        _highlightingRules.append(rule);
    }

    _numberFormat.setForeground(darkTheme ? QColor("#b5cea8") : QColor(Qt::darkCyan));
    rule.pattern = QRegularExpression("\\b[0-9]+[lL]?\\b");
    rule.format = _numberFormat;
    _highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\b0[xX][0-9A-Fa-f]+[lL]?\\b");
    _highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\b[0-9]+(?:\\.[0-9]+)?(?:[eE][+-]?[0-9]+)?\\b");
    _highlightingRules.append(rule);

    _quotationFormat.setForeground(darkTheme ? QColor("#ce9178") : QColor(Qt::darkMagenta));
}

void PythonCodeHighlighter::highlightBlock(const QString &text) {

    if (_shellMode) {
        if (currentBlock().blockNumber() > 2 && !text.startsWith(">>>") &&
            !text.startsWith("...")) {
            return;
        }
    }

    for (const HighlightingRule &rule : _highlightingRules) {
        QRegularExpressionMatch match;
        int index = text.indexOf(rule.pattern, 0, &match);

        while (index >= 0) {
            int length = match.capturedLength();
            setFormat(index, length, rule.format);
            index = text.indexOf(rule.pattern, index + length, &match);
        }
    }

    int quoteStartPos = -1;

    for (int i = 0; i < text.length(); ++i) {
        if (text[i] == '"' && (i == 0 || text[i - 1] != '\\')) {
            // don't treat multiline strings here (enclosed in """)
            if ((i + 1) < text.length() && (i + 2) < text.length() && text[i + 1] == '"' &&
                text[i + 2] == '"') {
                continue;
            }

            if ((i - 1) > 0 && (i + 1) < text.length() && text[i - 1] == '"' &&
                text[i + 1] == '"') {
                continue;
            }

            if ((i - 1) > 0 && (i - 2) > 0 && text[i - 1] == '"' && text[i - 2] == '"') {
                continue;
            }

            if (quoteStartPos == -1) {
                quoteStartPos = i;
                setFormat(quoteStartPos, 1, _quotationFormat);
            } else {
                setFormat(quoteStartPos, i - quoteStartPos + 1, _quotationFormat);
                quoteStartPos = -1;
            }
        } else if (quoteStartPos != -1) {
            setFormat(quoteStartPos, i - quoteStartPos + 1, _quotationFormat);
        }
    }

    quoteStartPos = -1;

    for (int i = 0; i < text.length(); ++i) {
        if (text[i] == '\'' && (i == 0 || text[i - 1] != '\\')) {
            // don't treat multiline strings here (enclosed in ''')
            if ((i + 1) < text.length() && (i + 2) < text.length() && text[i + 1] == '\'' &&
                text[i + 2] == '\'') {
                continue;
            }

            if ((i - 1) > 0 && (i + 1) < text.length() && text[i - 1] == '\'' &&
                text[i + 1] == '\'') {
                continue;
            }

            if ((i - 1) > 0 && (i - 2) > 0 && text[i - 1] == '\'' && text[i - 2] == '\'') {
                continue;
            }

            if (quoteStartPos == -1) {
                quoteStartPos = i;
                setFormat(quoteStartPos, 1, _quotationFormat);
            } else {
                setFormat(quoteStartPos, i - quoteStartPos + 1, _quotationFormat);
                quoteStartPos = -1;
            }
        } else if (quoteStartPos != -1) {
            setFormat(quoteStartPos, i - quoteStartPos + 1, _quotationFormat);
        }
    }

    setCurrentBlockState(0);

    static QRegularExpression triSingleQuote("'''");
    static QRegularExpression triDoubleQuote("\"\"\"");

    // highlight multi-line strings
    bool isInMultilne = highlightMultilineString(text, triSingleQuote, 1, _quotationFormat);

    if (!isInMultilne) {
        highlightMultilineString(text, triDoubleQuote, 2, _quotationFormat);
    }

    static QRegularExpression commentRegexp("#[^\n]*");
    QRegularExpressionMatch match;

    int index = text.indexOf(commentRegexp, 0, &match);

    while (index >= 0 && currentBlockState() == 0) {
        int nbQuotes = 0;

        for (int j = index - 1; j >= 0; --j) {
            if (text[j] == '\'') {
                ++nbQuotes;
            }
        }

        int nbDblQuotes = 0;

        for (int j = index - 1; j >= 0; --j) {
            if (text[j] == '"') {
                ++nbDblQuotes;
            }
        }

        int length = match.capturedLength();

        if (nbQuotes % 2 == 0 && nbDblQuotes % 2 == 0) {
            if (previousBlockState() <= 0 ||
                (previousBlockState() == 1 && text.indexOf(triSingleQuote) < index) ||
                (previousBlockState() == 2 && text.indexOf(triDoubleQuote) < index)) {
                setFormat(index, length, _commentFormat);
            }
        }

        index = text.indexOf(commentRegexp, index + length, &match);
    }
}

bool PythonCodeHighlighter::highlightMultilineString(const QString &text,
                                                     const QRegularExpression &delimiter,
                                                     const int inState,
                                                     const QTextCharFormat &style) {
    int start = -1;
    int add = -1;
    int commentPos = -1;

    QRegularExpressionMatch match;

    if (previousBlockState() == inState) {
        start = 0;
        add = 0;
    } else {
        start = text.indexOf(delimiter, 0, &match);
        add = match.capturedLength();
        commentPos = text.indexOf('#');
    }

    if (commentPos < 0 || commentPos > start) {

        while (start >= 0) {
            int end = text.indexOf(delimiter, start + add, &match);
            int length;

            if (end >= add) {
                length = end - start + add + match.capturedLength();
                setCurrentBlockState(0);
            } else {
                setCurrentBlockState(inState);
                length = text.length() - start + add;
            }

            setFormat(start, length, style);
            start = text.indexOf(delimiter, start + length, &match);
            add = match.capturedLength();
        }
    }

    return (currentBlockState() == inState);
}
