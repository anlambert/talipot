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

#ifndef TALIPOT_STRINGS_LIST_SELECTION_WIDGET_INTERFACE_H
#define TALIPOT_STRINGS_LIST_SELECTION_WIDGET_INTERFACE_H

#include <talipot/config.h>

#include <vector>
#include <string>

namespace tlp {

class TLP_QT_SCOPE StringsListSelectionWidgetInterface {

  public:
    virtual ~StringsListSelectionWidgetInterface() = default;

    virtual void
    setUnselectedStringsList(const std::vector<std::string> &unselectedStringsList) = 0;

    virtual void setSelectedStringsList(const std::vector<std::string> &selectedStringsList) = 0;

    virtual void clearUnselectedStringsList() = 0;

    virtual void clearSelectedStringsList() = 0;

    virtual void setMaxSelectedStringsListSize(const uint maxSelectedStringsListSize) = 0;

    virtual std::vector<std::string> getSelectedStringsList() const = 0;

    virtual std::vector<std::string> getUnselectedStringsList() const = 0;

    virtual void selectAllStrings() = 0;

    virtual void unselectAllStrings() = 0;
};
}

#endif // TALIPOT_STRINGS_LIST_SELECTION_WIDGET_INTERFACE_H
