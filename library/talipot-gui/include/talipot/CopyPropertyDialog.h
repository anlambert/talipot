/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_COPY_PROPERTY_DIALOG_H
#define TALIPOT_COPY_PROPERTY_DIALOG_H

#include <talipot/config.h>

#include <QDialog>

namespace Ui {
class CopyPropertyDialog;
}

namespace tlp {
class Graph;
class PropertyInterface;

/**
 * @brief Provide a dialog that allow user to copy a property in an existing property or in a new
 *one.
 *
 * The easiest way to use this class is to use the copyProperty static function.
 * @code
 * Graph* g;
 * PropertyInterface* source = g->getLocalBooleanProperty("viewSelection");
 * PropertyInterface* clonedProperty = CopyPropertyDialog::copyProperty(g, source);
 * @endcode
 *
 *
 **/

class TLP_QT_SCOPE CopyPropertyDialog : public QDialog {
  Q_OBJECT
public:
  CopyPropertyDialog(QWidget *parent = nullptr);
  ~CopyPropertyDialog() override;

  /**
   * @brief Init dialog with required parameters. To make the copy this dialog need to have a valid
   *source property and destination graph.
   **/
  void init(tlp::Graph *graph, tlp::PropertyInterface *toCopy);

  /**
   * @brief Perform the copy of the property in function of the parameters given by user. If
   *parameters are invalid return a null pointer and fill the errorMsg with the description of the
   *error.
   *
   * This function don't hold observers during the copy process. It's up to user to call
   *Observable::holdObserver and Observable::unholdObserver before and after calling this function.
   **/
  tlp::PropertyInterface *copyProperty(QString &errorMsg);

  /**
   * @brief Get the name of the destination property.
   **/
  QString destinationPropertyName() const;

  enum PropertyScope { NEW, LOCAL, INHERITED };

  /**
   * @brief Return the scope where the new property will be created.
   **/
  PropertyScope destinationPropertyScope() const;

  /**
   * @brief This is a convenience static function that copy property in function of user
   *parameters. If the user presses Cancel or an error occur, it returns a null pointer.
   *
   * The function creates a modal property copy dialog with the given source property, graph and
   *parent widget.
   *
   * @param graph The graph to use as parent for the properties to create.
   * @param source The property to copy.
   * @param askBeforePropertyOverwriting If set to true and user try to create a new property with
   *the same name than another existing ask user before overwriting them.
   **/
  static PropertyInterface *copyProperty(tlp::Graph *graph, tlp::PropertyInterface *source,
                                         bool askBeforePropertyOverwriting = false,
                                         QWidget *parent = nullptr);

private:
  Ui::CopyPropertyDialog *ui;
  tlp::Graph *_graph;
  tlp::PropertyInterface *_source;

private slots:
  void checkValidity();
};
}
#endif // TALIPOT_COPY_PROPERTY_DIALOG_H
