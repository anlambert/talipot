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

#include <talipot/GlyphRenderer.h>
#include <talipot/GlyphManager.h>
#include <talipot/GlOffscreenRenderer.h>
#include <talipot/GlGraph.h>
#include <talipot/ViewSettings.h>
#include <talipot/EdgeExtremityGlyph.h>
#include <talipot/TlpQtTools.h>

using namespace tlp;
using namespace std;

QPixmap GlyphRenderer::render(int glyphId, const QColor &backgroundColor) {
  QString glyphKey = QString::number(glyphId) + backgroundColor.name();
  static QMap<QString, QPixmap> previews;
  if (!previews.contains(glyphKey)) {
    // Init graph parameters.
    GlGraphRenderingParameters parameters;
    auto *graph = newGraph();
    auto node = graph->addNode();
    {
      // need a block to ensure inputData
      // will be destroyed before graph
      GlGraphInputData inputData(graph, &parameters);
      inputData.sizes()->setAllNodeValue(Size(1, 1, 1));
      inputData.colors()->setAllNodeValue(Color(192, 192, 192));
      inputData.borderColors()->setAllNodeValue(QColorToColor(textColor()));
      inputData.borderWidths()->setAllNodeValue(1);

      GlOffscreenRenderer &renderer = GlOffscreenRenderer::instance();
      renderer.setViewPortSize(16, 16);
      renderer.clearScene();
      renderer.setSceneBackgroundColor(QColorToColor(backgroundColor));
      renderer.addGraphToScene(graph);
      renderer.scene()->centerScene();
      renderer.scene()->graphCamera().setZoomFactor(0.9);
      // init previews
      for (const string &glyphName : PluginsManager::availablePlugins<Glyph>()) {
        auto glId = GlyphManager::glyphId(glyphName);
        // Create the glyph preview
        graph->getIntegerProperty("viewShape")->setNodeValue(node, glId);
        renderer.renderScene(false, true);
        QString glKey = QString::number(glId) + backgroundColor.name();
        previews[glKey] = QPixmap::fromImage(renderer.getImage());
      }
      renderer.clearScene(true);
    }
    // graph is no longer needed
    delete graph;
  }
  return previews[glyphKey];
}

QPixmap EdgeExtremityGlyphRenderer::render(int glyphId, const QColor &backgroundColor) {
  static QMap<QString, QPixmap> previews;
  QString glyphKey = QString::number(glyphId) + backgroundColor.name();
  if (!previews.contains(glyphKey)) {
    previews[QString::number(EdgeExtremityShape::None) + backgroundColor.name()] = QPixmap();
    // Init graph parameters.
    GlGraphRenderingParameters parameters;
    auto *graph = newGraph();
    node n1 = graph->addNode();
    node n2 = graph->addNode();
    edge e = graph->addEdge(n1, n2);
    {
      // need a block to ensure inputData
      // will be destroyed before graph
      GlGraphInputData inputData(graph, &parameters);
      inputData.sizes()->setAllNodeValue(Size(0.01f, 0.2f, 0.1f));
      inputData.sizes()->setAllEdgeValue(Size(0.125f, 0.125f, 0.125f));
      inputData.colors()->setAllNodeValue(QColorToColor(tlp::backgroundColor()));
      inputData.borderColors()->setAllNodeValue(QColorToColor(tlp::backgroundColor()));
      inputData.colors()->setAllEdgeValue(Color(192, 192, 192));
      inputData.borderColors()->setAllEdgeValue(QColorToColor(textColor()));
      inputData.layout()->setNodeValue(n1, Coord(0, 0, 0));
      inputData.layout()->setNodeValue(n2, Coord(0.3f, 0, 0));
      vector<Coord> bends;
      bends.push_back(Coord(0.01f, 0, 0));
      inputData.layout()->setAllEdgeValue(bends);

      inputData.srcAnchorShapes()->setAllEdgeValue(EdgeExtremityShape::None);
      inputData.tgtAnchorSizes()->setAllEdgeValue(Size(2, 2, 1));

      GlOffscreenRenderer &renderer = GlOffscreenRenderer::instance();
      renderer.setViewPortSize(16, 16);
      renderer.clearScene();
      renderer.setSceneBackgroundColor(QColorToColor(backgroundColor));
      renderer.addGraphToScene(graph);
      GlGraphRenderingParameters &renderingParamerters =
          renderer.scene()->glGraph()->getRenderingParameters();
      renderingParamerters.setEdgeColorInterpolate(false);
      renderingParamerters.setEdgeSizeInterpolate(false);
      renderingParamerters.setViewArrow(true);

      // init previews
      for (const string &glyphName : PluginsManager::availablePlugins<EdgeExtremityGlyph>()) {
        const Plugin &info = PluginsManager::pluginInformation(glyphName);
        int glId = info.id();
        // Create the glyph preview
        graph->getIntegerProperty("viewTgtAnchorShape")->setEdgeValue(e, glId);
        renderer.renderScene(true);
        QString glKey = QString::number(glId) + backgroundColor.name();
        previews[glKey] = QPixmap::fromImage(renderer.getImage());
      }
      renderer.clearScene(true);
    }
    // graph is no longer needed
    delete graph;
  }

  return previews[glyphKey];
}
