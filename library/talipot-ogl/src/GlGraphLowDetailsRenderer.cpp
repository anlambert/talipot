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

#include <talipot/GlGraphLowDetailsRenderer.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/OpenGlConfigManager.h>

using namespace std;

namespace tlp {

GlGraphLowDetailsRenderer::GlGraphLowDetailsRenderer(const GlGraphInputData *inputData)
    : GlGraphRenderer(inputData), fakeScene(new GlScene), buildVBO(true) {
  fakeScene->createLayer("fakeLayer");
  addObservers();
}

GlGraphLowDetailsRenderer::~GlGraphLowDetailsRenderer() {
  delete fakeScene;
  removeObservers();
}
//====================================================
void GlGraphLowDetailsRenderer::initEdgesArray() {
  Graph *graph = inputData->graph();
  LayoutProperty *layout = inputData->layout();
  ColorProperty *color = inputData->colors();

  size_t nbEdges = graph->numberOfEdges();
  size_t nbBends = 0;
  for (auto e : graph->edges()) {
    nbBends += layout->getEdgeValue(e).size();
  }
  points.resize(nbEdges * 2 + nbBends); // todo: should be #|V| !!!
  indices.resize(nbEdges * 2 + nbBends * 2);
  colors.resize(nbEdges * 2 + nbBends);
  // tlp::debug() << "nb lines = " << indices.size()/2 << endl;
  size_t i_point = 0;
  size_t i_indices = 0;
  size_t i_col = 0;
  for (auto e : graph->edges()) {
    const auto &[src, tgt] = graph->ends(e);
    Color a = color->getEdgeValue(e);
    Color b = color->getEdgeValue(e);
    Vec4f ca, cb;

    for (size_t i = 0; i < 4; ++i) {
      ca[i] = a[i];
      cb[i] = b[i];
    }

    indices[i_indices++] = i_point;
    colors[i_col++] = a;
    const auto &srcCoord = layout->getNodeValue(src);
    points[i_point][0] = srcCoord[0];
    points[i_point++][1] = srcCoord[1];

    const auto &bends = layout->getEdgeValue(e);

    for (size_t j = 0; j < bends.size(); ++j) {
      Vec4f tmp(ca - cb);
      tmp *= 1. / (bends.size() + 2);
      tmp *= j + 1;
      tmp += ca;
      colors[i_col++].set(uchar(tmp[0]), uchar(tmp[1]), uchar(tmp[2]), uchar(tmp[3]));
      indices[i_indices++] = i_point;
      indices[i_indices++] = i_point;
      points[i_point][0] = bends[j][0];
      points[i_point++][1] = bends[j][1];
    }

    indices[i_indices++] = i_point;
    colors[i_col++] = b;
    const auto &tgtCoord = layout->getNodeValue(tgt);
    points[i_point][0] = tgtCoord[0];
    points[i_point++][1] = tgtCoord[1];
  }
}
//====================================================
void GlGraphLowDetailsRenderer::initNodesArray() {
  Graph *graph = inputData->graph();
  LayoutProperty *layout = inputData->layout();
  ColorProperty *color = inputData->colors();
  SizeProperty *size = inputData->sizes();

  size_t nbNodes = graph->numberOfNodes();
  quad_points.resize(nbNodes * 4);
  quad_indices.resize(nbNodes * 4);
  quad_colors.resize(nbNodes * 4);
  // i % x  i%3
  float tab1[4] = {-1, 1, 1, -1};
  float tab2[4] = {-1, -1, 1, 1};

  size_t i_point = 0;
  size_t i_col = 0;
  size_t i_indices = 0;

  for (auto n : graph->nodes()) {
    const Coord &p = layout->getNodeValue(n);
    const Size &s = size->getNodeValue(n) / 2.f;
    const Color &c = color->getNodeValue(n);

    for (int i = 0; i < 4; ++i) {
      Vec3f a = p;
      a[0] += s[0] * tab1[i]; // s[0] * i%2 ;  // 0 1 1 0
      a[1] += s[1] * tab2[i]; // s[1] * i/2 %2;  // 0 0 1 1

      quad_colors[i_col++] = c;
      quad_indices[i_indices++] = i_point;
      quad_points[i_point][0] = a[0];
      quad_points[i_point++][1] = a[1];
    }
  }
}
//===================================================================
void GlGraphLowDetailsRenderer::draw(float, Camera *) {

  if (!inputData->renderingParameters()->isAntialiased()) {
    OpenGlConfigManager::deactivateAntiAliasing();
  }

  if (buildVBO) {
    initEdgesArray();
    initNodesArray();
    buildVBO = false;
  }

  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glVertexPointer(2, GL_FLOAT, 2 * sizeof(GLfloat), &points[0]);
  glColorPointer(4, GL_UNSIGNED_BYTE, 4 * sizeof(GLubyte), &colors[0]);
  size_t cur = 0;

  while (cur < indices.size()) {
    if (indices.size() - cur > 64000) {
      glDrawElements(GL_LINES, 64000, GL_UNSIGNED_INT, &indices[cur]);
    } else {
      glDrawElements(GL_LINES, indices.size() - cur, GL_UNSIGNED_INT, &indices[cur]);
    }

    cur += 64000;
  }

  glDisable(GL_BLEND);

  glVertexPointer(2, GL_FLOAT, 2 * sizeof(GLfloat), &quad_points[0]);
  glColorPointer(4, GL_UNSIGNED_BYTE, 4 * sizeof(GLubyte), &quad_colors[0]);
  cur = 0;

  while (cur < quad_indices.size()) {
    if (quad_indices.size() - cur > 64000) {
      glDrawElements(GL_QUADS, 64000, GL_UNSIGNED_INT, &quad_indices[cur]);
    } else {
      glDrawElements(GL_QUADS, quad_indices.size() - cur, GL_UNSIGNED_INT, &quad_indices[cur]);
    }

    cur += 64000;
  }

  // glDrawElements(GL_QUADS, quad_indices.size(), GL_UNSIGNED_INT, &quad_indices[0]);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);

  OpenGlConfigManager::activateAntiAliasing();
}

void GlGraphLowDetailsRenderer::addObservers() {
  observedGraph = inputData->graph();
  observedGraph->addListener(this);
  observedLayoutProperty = inputData->layout();
  observedLayoutProperty->addListener(this);
  observedSizeProperty = inputData->sizes();
  observedSizeProperty->addListener(this);
  observedSelectionProperty = inputData->selection();
  observedSelectionProperty->addListener(this);
  observedColorProperty = inputData->colors();
  observedColorProperty->addListener(this);
}

void GlGraphLowDetailsRenderer::removeObservers() {
  observedGraph->removeListener(this);
  observedLayoutProperty->removeListener(this);
  observedSizeProperty->removeListener(this);
  observedSelectionProperty->removeListener(this);
  observedColorProperty->removeListener(this);
}

void GlGraphLowDetailsRenderer::updateObservers() {
  removeObservers();
  addObservers();
}

void GlGraphLowDetailsRenderer::treatEvent(const Event &ev) {
  if (typeid(ev) == typeid(GraphEvent)) {
    const auto *graphEvent = dynamic_cast<const GraphEvent *>(&ev);

    switch (graphEvent->getType()) {
    case GraphEvent::TLP_ADD_NODE:
    case GraphEvent::TLP_ADD_EDGE:
    case GraphEvent::TLP_DEL_NODE:
    case GraphEvent::TLP_DEL_EDGE:
      buildVBO = true;
      break;

    case GraphEvent::TLP_ADD_LOCAL_PROPERTY:
    case GraphEvent::TLP_BEFORE_DEL_LOCAL_PROPERTY: {
      const PropertyInterface *property =
          inputData->graph()->getProperty(graphEvent->getPropertyName());

      if (property == inputData->layout() || property == inputData->sizes() ||
          property == inputData->colors() || property == inputData->selection()) {
        buildVBO = true;
        updateObservers();
      }

      break;
    }

    default:
      break;
    }
  } else if (typeid(ev) == typeid(PropertyEvent)) {
    const auto *propertyEvent = dynamic_cast<const PropertyEvent *>(&ev);

    switch (propertyEvent->getType()) {
    case PropertyEvent::TLP_BEFORE_SET_ALL_NODE_VALUE:
    case PropertyEvent::TLP_BEFORE_SET_NODE_VALUE:
    case PropertyEvent::TLP_BEFORE_SET_ALL_EDGE_VALUE:
    case PropertyEvent::TLP_BEFORE_SET_EDGE_VALUE:
      buildVBO = true;
      break;

    default:
      break;
    }
  } else if (ev.type() == Event::TLP_DELETE) {

    if (dynamic_cast<tlp::Graph *>(ev.sender())) {
      removeObservers();
    }
  }
}
}
