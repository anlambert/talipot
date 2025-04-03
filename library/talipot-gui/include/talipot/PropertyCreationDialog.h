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

#ifndef TALIPOT_PROPERTY_CREATION_DIALOG_H
#define TALIPOT_PROPERTY_CREATION_DIALOG_H

#include <QDialog>

#include <talipot/config.h>

namespace Ui {
class PropertyCreationDialog;
}

namespace tlp {

class PropertyInterface;
class Graph;

/**
 * @brief Provide a dialog that allow user to create a new property.
 *
 * The easiest way to use this class is to use the static function.
 * @code
 * Graph* g;
 * QWidget* parent;
 * PropertyCreationDialog::createNewProperty(g,parent);
 * @endcode
 *
 *
 **/
class TLP_QT_SCOPE PropertyCreationDialog : public QDialog {
    Q_OBJECT

  public:
    /**
     * @brief Constructs a property creation dialog with the given parent.
     **/
    explicit PropertyCreationDialog(QWidget *parent = nullptr);
    /**
     * @brief Constructs a property creation dialog with the given parent graph and parent widget.
     **/
    explicit PropertyCreationDialog(tlp::Graph *graph, QWidget *parent = nullptr,
                                    const std::string &selectedType = "");
    ~PropertyCreationDialog() override;

    /**
     * @brief Try to create a new property from the givent parameters.
     * To get the created property use the createdProperty() function.
     **/
    void accept() override;

    /**
     * @brief Change the graph to use as parent for the properties to create.
     **/
    void setGraph(tlp::Graph *graph);

    tlp::Graph *getGraph() const {
        return _graph;
    }

    /**
     * @brief Return the property created. You need to call this function after the accept()
     *function.
     *
     * @return The last created property or nullptr if no property there is an error during the
     *property
     *creation.
     **/
    tlp::PropertyInterface *createdProperty() const {
        return _createdProperty;
    }

    /**
     * @brief This is a convenience static function that create a new property using user
     *parameters. If the user presses Cancel or an error occur, it returns a null pointer.
     *
     * The function creates a modal property creation dialog with the given graph and parent widget.
     *
     * @param graph The graph to use as parent for the properties to create.
     **/
    static PropertyInterface *createNewProperty(tlp::Graph *graph, QWidget *parent = nullptr,
                                                const std::string &selectedType = "");
  private slots:
    void checkValidity();

  private:
    void initGui();

    Ui::PropertyCreationDialog *ui;
    QPushButton *_createPropertyButton;
    Graph *_graph;
    PropertyInterface *_createdProperty;
};
}

#endif // TALIPOT_PROPERTY_CREATION_DIALOG_H
