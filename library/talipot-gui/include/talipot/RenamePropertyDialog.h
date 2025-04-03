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

#ifndef TALIPOT_RENAME_PROPERTY_DIALOG_H
#define TALIPOT_RENAME_PROPERTY_DIALOG_H

#include <talipot/config.h>

class QWidget;

namespace tlp {
class PropertyInterface;

/**
 * @brief Provide a dialog that allow user to rename a property.
 *
 * The easiest way to use this class is to use the renameProperty static function.
 *
 **/

class TLP_QT_SCOPE RenamePropertyDialog {
  public:
    /**
     * @brief This is a convenience static function to rename a property using a dialog. If the user
     *presses Cancel or an error occurs, it returns false.
     *
     * The function creates a modal property rename dialog with the given property, and parent
     *widget.
     *
     * @param toRenameProp The property to rename.
     **/
    static bool renameProperty(tlp::PropertyInterface *toRenameProp, QWidget *parent = nullptr);
};
}
#endif // TALIPOT_RENAME_PROPERTY_DIALOG_H
