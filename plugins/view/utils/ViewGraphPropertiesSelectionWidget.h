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

#ifndef VIEW_GRAPH_PROPERTIES_SELECTION_WIDGET_H
#define VIEW_GRAPH_PROPERTIES_SELECTION_WIDGET_H

#include <vector>
#include <string>

#include <QWidget>

#include <talipot/Graph.h>

namespace Ui {
class ViewGraphPropertiesSelectionWidget;
}

namespace tlp {

class ViewGraphPropertiesSelectionWidget : public QWidget, public Observable {

    Q_OBJECT

  public:
    ViewGraphPropertiesSelectionWidget(QWidget *parent = nullptr);
    ~ViewGraphPropertiesSelectionWidget() override;

    void setWidgetParameters(Graph *graph, std::vector<std::string> graphPropertiesTypesFilter);
    std::vector<std::string> getSelectedGraphProperties();

    void setWidgetEnabled(const bool enabled);
    void setSelectedProperties(std::vector<std::string> selectedProperties);

    ElementType getDataLocation() const;
    void setDataLocation(const ElementType location);
    void enableEdgesButton(const bool enable);
    void treatEvent(const Event &) override;

    bool configurationChanged();

  private:
    Ui::ViewGraphPropertiesSelectionWidget *_ui;
    Graph *graph;
    std::vector<std::string> graphPropertiesTypesFilter;
    std::vector<std::string> lastSelectedProperties;
    ElementType lastDataLocation;
};
}

#endif // VIEW_GRAPH_PROPERTIES_SELECTION_WIDGET_H
