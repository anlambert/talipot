/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */
#include "ExpandableGroupBox.h"

#include <QVBoxLayout>
#include <QDebug>

ExpandableGroupBox::ExpandableGroupBox(QWidget *parent, const QString &title)
    : QGroupBox(title, parent), _expanded(true), _widget(nullptr) {
  setCheckable(true);
  setChecked(true);
  setExpanded(_expanded);
  connect(this, SIGNAL(toggled(bool)), this, SLOT(setExpanded(bool)));

  QVBoxLayout *lyt = new QVBoxLayout;
  lyt->setContentsMargins(0, 0, 0, 0);
  lyt->setMargin(0);
  lyt->setSpacing(0);
  setLayout(lyt);
}

ExpandableGroupBox::~ExpandableGroupBox() {}

void ExpandableGroupBox::setExpanded(bool e) {
  _expanded = e;
  // As told in Qt stylesheet reference, this force recomputation of stylesheet based on dynamic
  // properties
  setStyleSheet(styleSheet());

  if (_widget) {
    _widget->setVisible(e);
  }
}

void ExpandableGroupBox::setWidget(QWidget *w) {
  delete _widget;
  _widget = w;

  if (_widget)
    layout()->addWidget(_widget);
}

QWidget *ExpandableGroupBox::takeWidget() {
  QWidget *result = _widget;
  _widget = nullptr;

  if (result)
    layout()->removeWidget(result);

  return result;
}
