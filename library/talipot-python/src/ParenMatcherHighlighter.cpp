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
    : QSyntaxHighlighter(parent) {}

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

  for (char paren : _parensToMatch) {
    int pos = modifiedText.indexOf(paren);

    while (pos != -1) {
      ParenInfo info;
      info.character = paren;
      info.position = currentBlock().position() + pos;
      data->insert(info);
      pos = modifiedText.indexOf(paren, pos + 1);
    }
  }

  data->sortParenInfo();
  setCurrentBlockUserData(data);
}
