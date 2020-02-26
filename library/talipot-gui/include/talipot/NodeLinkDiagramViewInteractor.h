/**
 *
 * Copyright (C) 2019-2020  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_NODE_LINK_DIAGRAM_VIEW_INTERACTOR_H
#define TALIPOT_NODE_LINK_DIAGRAM_VIEW_INTERACTOR_H

#include <talipot/GLInteractor.h>

class QLabel;

namespace tlp {

/** @brief Interactor abstract class for NodeLinkDiagramView
 *
 */
class TLP_QT_SCOPE NodeLinkDiagramViewInteractor : public GLInteractorComposite {
  QLabel *_label;
  unsigned int _priority;

public:
  NodeLinkDiagramViewInteractor(const QString &iconPath, const QString &text,
                                unsigned int priority = 0);

  ~NodeLinkDiagramViewInteractor() override;

  void setConfigurationWidgetText(const QString &text);

  QLabel *configurationDocWidget() const override;

  unsigned int priority() const override;
};
}

#endif // TALIPOT_NODE_LINK_DIAGRAM_VIEW_INTERACTOR_H