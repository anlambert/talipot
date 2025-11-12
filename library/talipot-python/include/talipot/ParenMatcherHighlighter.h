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

#ifndef TALIPOT_PAREN_MATCHER_HIGHLIGHTER_H
#define TALIPOT_PAREN_MATCHER_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QVector>

class QTextDocument;

struct ParenInfo {
  char character;
  int position;

  bool operator<(const ParenInfo &info) const {
    return position < info.position;
  }
};

class ParenInfoTextBlockData : public QTextBlockUserData {

  QVector<ParenInfo> _parenInfo;

public:
  ParenInfoTextBlockData();

  QVector<ParenInfo> parens();
  void insert(const ParenInfo &parenInfo);
  void sortParenInfo();
};

class ParenMatcherHighlighter : public QSyntaxHighlighter {

  const QVector<char> _parensToMatch = {'(', '[', '{', ')', ']', '}'};

public:
  explicit ParenMatcherHighlighter(QTextDocument *parent = nullptr);

protected:
  void highlightBlock(const QString &text) override;
};

#endif // TALIPOT_PAREN_MATCHER_HIGHLIGHTER_H
