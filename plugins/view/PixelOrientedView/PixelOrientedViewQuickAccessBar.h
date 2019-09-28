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

#ifndef PIXELORIENTEDVIEWQUICKACCESSBAR_H
#define PIXELORIENTEDVIEWQUICKACCESSBAR_H

#include <tulip/QuickAccessBar.h>

namespace tlp {

class PixelOrientedOptionsWidget;

class PixelOrientedViewQuickAccessBar : public tlp::QuickAccessBarImpl {
  Q_OBJECT

  PixelOrientedOptionsWidget *_optionsWidget;

public:
  PixelOrientedViewQuickAccessBar(PixelOrientedOptionsWidget *opt, QWidget *parent = nullptr);

public slots:
  void reset() override;
  void setBackgroundColor(const QColor &) override;
};
} // namespace tlp
#endif
