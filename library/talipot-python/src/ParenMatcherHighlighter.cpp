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

#include "talipot/ParenMatcherHighlighter.h"

#include <QRegularExpression>

ParenInfoTextBlockData::ParenInfoTextBlockData() = default;

QVector<ParenInfo> ParenInfoTextBlockData::parens() {
    return _parenInfo;
}

void ParenInfoTextBlockData::insert(const ParenInfo &parenInfo) {
    _parenInfo.append(parenInfo);
}

void ParenInfoTextBlockData::sortParenInfo() {
    std::sort(_parenInfo.begin(), _parenInfo.end());
}

ParenMatcherHighlighter::ParenMatcherHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {
    _leftParensToMatch.append('(');
    _leftParensToMatch.append('[');
    _leftParensToMatch.append('{');
    _rightParensToMatch.append(')');
    _rightParensToMatch.append(']');
    _rightParensToMatch.append('}');
}

void ParenMatcherHighlighter::highlightBlock(const QString &text) {
    auto *data = new ParenInfoTextBlockData;

    QString modifiedText = text;
    QRegularExpression dblQuotesRegexp("\"[^\"]*\"");
    QRegularExpression simpleQuotesRegexp("'[^']*'");

    QRegularExpressionMatch match;

    int pos = modifiedText.indexOf(dblQuotesRegexp, 0, &match);

    while (pos != -1) {
        for (int i = pos; i < pos + match.capturedLength(); ++i) {
            modifiedText[i] = ' ';
        }

        pos = modifiedText.indexOf(dblQuotesRegexp, pos + match.capturedLength(), &match);
    }

    pos = modifiedText.indexOf(simpleQuotesRegexp, 0, &match);

    while (pos != -1) {
        for (int i = pos; i < pos + match.capturedLength(); ++i) {
            modifiedText[i] = ' ';
        }

        pos = modifiedText.indexOf(simpleQuotesRegexp, pos + match.capturedLength(), &match);
    }

    for (char c : _leftParensToMatch) {
        int leftPos = modifiedText.indexOf(c);

        while (leftPos != -1) {
            ParenInfo info;
            info.character = c;
            info.position = currentBlock().position() + leftPos;
            data->insert(info);
            leftPos = modifiedText.indexOf(c, leftPos + 1);
        }
    }

    for (char c : _rightParensToMatch) {
        int rightPos = modifiedText.indexOf(c);

        while (rightPos != -1) {
            ParenInfo info;
            info.character = c;
            info.position = currentBlock().position() + rightPos;
            data->insert(info);
            rightPos = modifiedText.indexOf(c, rightPos + 1);
        }
    }

    data->sortParenInfo();
    setCurrentBlockUserData(data);
}
