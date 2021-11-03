/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/GlLine.h>
#include <talipot/GlQuantitativeAxis.h>

#include "ScatterPlotTrendLine.h"
#include "ScatterPlot2DView.h"
#include "ScatterPlot2D.h"

using namespace std;

namespace tlp {

// Computes the Linear regression function of a graph on two metrics (y = ax +b)
void computeLinearRegressionFunction(Graph *graph, DoubleProperty *xk, DoubleProperty *yk, float &a,
                                     float &b) {
  float sxk, syk, sxkxk, sxkyk;
  sxk = 0.0f;
  syk = 0.0f;
  sxkxk = 0.0f;
  sxkyk = 0.0f;

  // We compute the sum of xk, yk, xk² and xkyk for the whole set of nodes

  for (auto n : graph->nodes()) {
    float nodeValx = xk->getNodeValue(n), nodeValy = yk->getNodeValue(n);
    sxk += nodeValx;
    sxkxk += (nodeValx * nodeValx);
    syk += nodeValy;
    sxkyk += (nodeValx * nodeValy);
  }

  float n = graph->numberOfNodes();

  // Then we compute a and b :
  a = (sxkyk - ((sxk * syk) / n)) / (sxkxk - ((sxk * sxk) / n));
  b = (syk / n) - a * (sxk / n);
}

ScatterPlotTrendLine::ScatterPlotTrendLine() : scatterView(nullptr), a(0.0f), b(0.0f) {}

ScatterPlotTrendLine::~ScatterPlotTrendLine() = default;

bool ScatterPlotTrendLine::eventFilter(QObject *, QEvent *e) {
  if (e->type() == QEvent::MouseMove) {
    scatterView->refresh();
    return true;
  }

  return false;
}

bool ScatterPlotTrendLine::draw(GlWidget *glWidget) {

  ScatterPlot2D *currentScatterPlot = scatterView->getDetailedScatterPlot();

  if (currentScatterPlot == nullptr || (a == 0.0f && b == 0.0f)) {
    return false;
  }

  GlQuantitativeAxis *xAxis = currentScatterPlot->getXAxis();
  GlQuantitativeAxis *yAxis = currentScatterPlot->getYAxis();
  float xStart = xAxis->getAxisMinValue();
  float xEnd = xAxis->getAxisMaxValue();

  float yStart = a * xStart + b;
  float yEnd = a * xEnd + b;

  Camera &camera = glWidget->scene()->getLayer("Main")->getCamera();
  camera.initGl();

  GlLine trendLine;
  trendLine.addPoint(Coord(xAxis->getAxisBaseCoord().getX(),
                           yAxis->getAxisPointCoordForValue(yStart).getY(), 0.0f),
                     Color(0, 255, 0));
  trendLine.addPoint(Coord(xAxis->getAxisBaseCoord().getX() + xAxis->getAxisLength(),
                           yAxis->getAxisPointCoordForValue(yEnd).getY(), 0.0f),
                     Color(0, 255, 0));
  trendLine.setLineWidth(3);

  glDisable(GL_STENCIL_TEST);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  trendLine.draw(0, nullptr);
  glDisable(GL_BLEND);

  GlLabel lineEquationLabel(Coord(xAxis->getAxisBaseCoord().getX() + xAxis->getAxisLength() +
                                      xAxis->getAxisLength() / 8.0f,
                                  yAxis->getAxisPointCoordForValue(yEnd).getY(), 0.0f),
                            Size(xAxis->getAxisLength() / 4.0f, yAxis->getAxisLength() / 10.0f),
                            Color(0, 255, 0));
  ostringstream oss;
  oss << "y = " << a << " * x + " << b;
  lineEquationLabel.setText(oss.str());
  lineEquationLabel.draw(0, &glWidget->scene()->getLayer("Main")->getCamera());

  return true;
}

bool ScatterPlotTrendLine::compute(GlWidget *) {
  ScatterPlot2D *currentScatterPlot = scatterView->getDetailedScatterPlot();

  if (currentScatterPlot == nullptr) {
    return false;
  }

  Graph *graph = scatterView->getScatterPlotGraph();
  DoubleProperty *xDim, *yDim;
  string xDimName(currentScatterPlot->getXDim());
  string yDimName(currentScatterPlot->getYDim());
  string xDimType(graph->getProperty(xDimName)->getTypename());
  string yDimType(graph->getProperty(yDimName)->getTypename());

  if (xDimType == "double") {
    xDim = graph->getDoubleProperty(xDimName);
  } else {
    IntegerProperty *xDimInt = graph->getIntegerProperty(xDimName);
    xDim = new DoubleProperty(graph);
    for (auto n : graph->nodes()) {
      xDim->setNodeValue(n, double(xDimInt->getNodeValue(n)));
    }
  }

  if (yDimType == "double") {
    yDim = graph->getDoubleProperty(yDimName);
  } else {
    IntegerProperty *yDimInt = graph->getIntegerProperty(yDimName);
    yDim = new DoubleProperty(graph);
    for (auto n : graph->nodes()) {
      yDim->setNodeValue(n, double(yDimInt->getNodeValue(n)));
    }
  }

  computeLinearRegressionFunction(graph, xDim, yDim, a, b);

  if (xDimType == "int") {
    delete xDim;
  }

  if (yDimType == "int") {
    delete yDim;
  }

  return true;
}

void ScatterPlotTrendLine::viewChanged(View *view) {
  if (view == nullptr) {
    scatterView = nullptr;
    return;
  }

  scatterView = static_cast<ScatterPlot2DView *>(view);
  compute(nullptr);
  scatterView->refresh();
}
}
