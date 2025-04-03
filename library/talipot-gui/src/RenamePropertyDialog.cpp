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

#include <QMessageBox>
#include <QInputDialog>

#include <talipot/RenamePropertyDialog.h>
#include <talipot/TlpQtTools.h>
#include <talipot/Graph.h>

bool tlp::RenamePropertyDialog::renameProperty(tlp::PropertyInterface *prop, QWidget *parent) {
    if (prop == nullptr) {
        QMessageBox::critical(parent, "Renaming error", "The property is nullptr");
        return false;
    }

    bool valid = false;

    while (!valid) {
        QString errorMsg;
        QString pName = QInputDialog::getText(
            parent, "Renaming property '" + tlp::tlpStringToQString(prop->getName()) + "'",
            "New name: ", QLineEdit::Normal, tlpStringToQString(prop->getName()), &valid);

        if (!valid) {
            return false;
        }

        // Check if parameters are valid.
        std::string propertyName = tlp::QStringToTlpString(pName);

        if (propertyName.empty()) {
            valid = false;
            errorMsg = "Cannot rename a property with an empty name";
        } else if (prop->getGraph()->existLocalProperty(propertyName)) {
            valid = false;
            errorMsg = "A local property named '" + tlpStringToQString(propertyName) +
                       "'\n already exists";
        }

        if (valid) {
            errorMsg = "Unable to rename the property";
            valid = prop->rename(propertyName);
        }

        if (!valid) {
            QMessageBox::critical(parent, "Error when renaming property", errorMsg);
        } else {
            return true;
        }
    }

    return false;
}
