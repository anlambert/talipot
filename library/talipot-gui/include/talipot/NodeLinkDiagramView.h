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

#ifndef TALIPOT_NODE_LINK_DIAGRAM_VIEW_H
#define TALIPOT_NODE_LINK_DIAGRAM_VIEW_H

#include <talipot/GlView.h>
#include <talipot/Camera.h>

namespace Ui {
class GridOptionsWidget;
}

class QDialog;
class QAction;

namespace tlp {
class GlGrid;
class GlCompositeHierarchyManager;
class GlGraphRenderingParameters;
class PropertyInterface;
class StringProperty;

class TLP_QT_SCOPE NodeLinkDiagramView : public tlp::GlView {
    Q_OBJECT

    GlGrid *_grid;
    QDialog *_gridOptions;
    GlCompositeHierarchyManager *manager;
    bool _hasHulls;

    void registerTriggers();
    void updateGrid();

    Ui::GridOptionsWidget *grid_ui;

  public:
    static const std::string viewName;
    PLUGININFORMATION(
        NodeLinkDiagramView::viewName, "Tulip Team", "16/04/2008",
        "The Node Link Diagram view is the standard representation of relational data, "
        "where entities are represented as nodes, and their relation as edges.<br>"
        "This view allows you to change the glyph used to represent nodes (e.g. "
        "square, round, cross, ...), as well as the shape of the arrows indicating the "
        "direction of the relationship.",
        "1.0", "relational")

    NodeLinkDiagramView(const tlp::PluginContext *context = nullptr);
    ~NodeLinkDiagramView() override;
    std::string icon() const override {
        return ":/talipot/gui/icons/32/node_link_diagram_view.png";
    }
    void setState(const tlp::DataSet &) override;
    tlp::DataSet state() const override;
    // default initialization of scene rendering parameters
    // can be used by other view
    static void initRenderingParameters(tlp::GlGraphRenderingParameters &);

  public slots:
    void draw() override;
    void requestChangeGraph(Graph *graph);
    const Camera &goInsideItem(node meta);

  protected slots:

    void deleteItem();
    void editColor();
    void editLabel();
    void editShape();
    void editSize();
    void goInsideItem();
    void ungroupItem();
    void setZOrdering(bool);
    void showGridControl();
    void fillContextMenu(QMenu *menu, const QPointF &point) override;

    void addRemoveItemToSelection(bool pushGraph = true, bool toggleSelection = true,
                                  bool selectValue = false, bool resetSelection = false);
    void addRemoveInNodesToSelection(bool pushGraph = true, bool toggleSelection = true,
                                     bool selectValue = false, bool resetSelection = false);
    void addRemoveOutNodesToSelection(bool pushGraph = true, bool toggleSelection = true,
                                      bool selectValue = false, bool resetSelection = false);
    void addRemoveInEdgesToSelection(bool pushGraph = true, bool toggleSelection = true,
                                     bool selectValue = false, bool resetSelection = false);
    void addRemoveOutEdgesToSelection(bool pushGraph = true, bool toggleSelection = true,
                                      bool selectValue = false, bool resetSelection = false);
    void addRemoveNodeAndAllNeighbourNodesAndEdges(bool toggleSelection = true,
                                                   bool selectValue = false,
                                                   bool resetSelection = false);
    void addRemoveExtremitiesToSelection(bool pushGraph = true, bool toggleSelection = true,
                                         bool selectValue = false, bool resetSelection = false);
    void addRemoveEdgeAndExtremitiesToSelection(bool toggleSelection = true,
                                                bool selectValue = false,
                                                bool resetSelection = false);

    void selectItem();
    void selectInNodes(bool pushGraph = true);
    void selectOutNodes(bool pushGraph = true);
    void selectInEdges(bool pushGraph = true);
    void selectOutEdges(bool pushGraph = true);
    void selectNodeAndAllNeighbourNodesAndEdges();
    void selectExtremities(bool pushGraph = true);
    void selectEdgeAndExtremities();

    void addItemToSelection();
    void addInNodesToSelection(bool pushGraph = true);
    void addOutNodesToSelection(bool pushGraph = true);
    void addInEdgesToSelection(bool pushGraph = true);
    void addOutEdgesToSelection(bool pushGraph = true);
    void addNodeAndAllNeighbourNodesAndEdgesToSelection();
    void addExtremitiesToSelection(bool pushGraph = true);
    void addEdgeAndExtremitiesToSelection();

    void removeItemFromSelection();
    void removeInNodesFromSelection(bool pushGraph = true);
    void removeOutNodesFromSelection(bool pushGraph = true);
    void removeInEdgesFromSelection(bool pushGraph = true);
    void removeOutEdgesFromSelection(bool pushGraph = true);
    void removeNodeAndAllNeighbourNodesAndEdgesFromSelection();
    void removeExtremitiesFromSelection(bool pushGraph = true);
    void removeEdgeAndExtremitiesFromSelection();

  protected:
    bool isNode;
    uint itemId;

    void graphChanged(tlp::Graph *) override;

    void createScene(Graph *graph, DataSet dataSet);
    DataSet sceneData() const;
    void loadGraphOnScene(Graph *graph);
    void useHulls(bool hasHulls);
    bool hasHulls() const;
    void editValue(PropertyInterface *pi);
};
}

#endif // TALIPOT_NODE_LINK_DIAGRAM_VIEW_H
