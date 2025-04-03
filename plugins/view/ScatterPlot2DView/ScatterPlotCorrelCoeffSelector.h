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

#ifndef SCATTER_PLOT_CORREL_COEFF_SELECTOR_H
#define SCATTER_PLOT_CORREL_COEFF_SELECTOR_H

#include <talipot/GlEntity.h>
#include <talipot/GlCircle.h>
#include <talipot/GLInteractor.h>

namespace tlp {

class ScatterPlotCorrelCoeffSelectorOptionsWidget;
class ScatterPlot2DView;

class GlEditableComplexPolygon : public GlEntity {

  public:
    GlEditableComplexPolygon(std::vector<Coord> polygonPoints, const Color &color);

    void translate(const Coord &move) override;
    void draw(float lod, Camera *camera) override;
    BoundingBox getBoundingBox() override;
    void getXML(std::string &) override {}
    void setWithXML(const std::string &, uint &) override {}

    uint getNumberOfVertex() const {
        return polygonPoints.size();
    }

    bool pointInsidePolygon(const Coord &point);
    Coord *getPolygonVertexUnderPointerIfAny(const Coord &pointerScreenCoord, Camera *camera);
    std::pair<Coord, Coord> *getPolygonSegmentUnderPointerIfAny(const Coord &pointerSceneCoord);

    void addPolygonVertex(std::pair<Coord, Coord> polygonSegment, const Coord &newVertex);
    void removePolygonVertex(const Coord &vertex);
    void movePolygonVertexToPoint(const Coord &polygonVertex, const Coord &targetPoint);

    void setColor(const Color &color) {
        this->color = color;
    }
    Color getColor() const {
        return color;
    }
    void setSelected(const bool selected) {
        this->selected = selected;
    }
    bool isSelected() const {
        return selected;
    }

    const std::vector<Coord> &getPolygonVertices() const {
        return polygonPoints;
    }

  private:
    std::vector<Coord> polygonPoints;
    Color color;
    GlCircle basicCircle;
    bool selected;
};

class ScatterPlotCorrelCoeffSelector : public GLInteractorComponent {

  public:
    ScatterPlotCorrelCoeffSelector(ScatterPlotCorrelCoeffSelectorOptionsWidget *optionsWidget);
    ScatterPlotCorrelCoeffSelector(
        const ScatterPlotCorrelCoeffSelector &scatterPlotCorrelCoeffSelector);
    ~ScatterPlotCorrelCoeffSelector() override;

    bool eventFilter(QObject *, QEvent *) override;
    bool draw(GlWidget *glWidget) override;
    bool compute(GlWidget *glWidget) override;
    void viewChanged(View *view) override;

  private:
    void getPolygonAndPointUnderPointerIfAny(const Coord &pointerSceneCoord, Camera *camera);
    void mapPolygonColorToCorrelCoeffOfData(GlEditableComplexPolygon *polygon, GlWidget *glWidget);

    ScatterPlotCorrelCoeffSelectorOptionsWidget *optionsWidget;
    ScatterPlot2DView *scatterView;
    Coord currentPointerSceneCoords;
    std::vector<Coord> polygonEdit;
    std::vector<GlEditableComplexPolygon *> polygons;
    GlCircle basicCircle;
    GlEditableComplexPolygon *selectedPolygon;
    Coord *selectedPolygonPoint;
    bool dragStarted;
    int x, y;
    std::map<GlEditableComplexPolygon *, std::pair<std::vector<node>, double>>
        polygonsToNodesSubsetAndCorrelCoeff;
};
}

#endif // SCATTER_PLOT_CORREL_COEFF_SELECTOR_H
