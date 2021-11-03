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

#include <talipot/GlTextureManager.h>
#include <talipot/Gl2DRect.h>
#include <talipot/GlProgressBar.h>
#include <talipot/GlOffscreenRenderer.h>
#include <talipot/GlGraph.h>

#include "PixelOrientedOverview.h"

using namespace std;

template <typename T>
std::string getStringFromNumber(T number) {
  std::ostringstream oss;
  oss.precision(5);
  oss << number;
  return oss.str();
}

static void setGraphView(tlp::GlGraph *glGraph) {
  tlp::GlGraphRenderingParameters &param = glGraph->getRenderingParameters();
  param.setAntialiasing(true);
  param.setViewNodeLabel(false);
  param.setFontsType(2);
  param.setSelectedNodesStencil(1);
  param.setNodesStencil(0xFFFF);
  param.setNodesLabelStencil(0xFFFF);
  param.setDisplayEdges(false);
  param.setDisplayNodes(true);
}

namespace tlp {

int PixelOrientedOverview::overviewCpt(0);

PixelOrientedOverview::PixelOrientedOverview(GraphDimension *data,
                                             PixelOrientedMediator *pixelOrientedMediator,
                                             Coord blCornerPos, const std::string &dimName,
                                             const Color &backgroundColor, const Color &textColor)
    : data(data), pixelOrientedMediator(pixelOrientedMediator), blCornerPos(blCornerPos),
      dimName(dimName), frame(nullptr), frame2(nullptr), overviewGen(false),
      backgroundColor(backgroundColor), textColor(textColor) {

  if (this->dimName.empty()) {
    this->dimName = data->getDimensionName();
  }

  overviewId = overviewCpt++;

  textureName = dimName + " texture " + getStringFromNumber(overviewId);

  uint width = pixelOrientedMediator->getImageWidth();
  uint height = pixelOrientedMediator->getImageHeight();

  uint labelHeight = height / 4;

  Graph *graph = data->getGraph();
  pixelLayout = new LayoutProperty(graph);
  pixelSize = new SizeProperty(graph);
  glGraph = new GlGraph(graph);
  setGraphView(glGraph);
  GlGraphInputData *glGraphInputData = glGraph->getInputData();
  glGraphInputData->setLayout(pixelLayout);
  glGraphInputData->setSizes(pixelSize);

  frame = new GlRect(Coord(blCornerPos.getX() - 3, blCornerPos.getY() + height + 3),
                     Coord(blCornerPos.getX() + width + 3, blCornerPos.getY() - 3), Color(0, 0, 0),
                     Color(0, 0, 0), false, true);
  addGlEntity(frame, dimName + "frame");
  frame2 = new GlRect(Coord(blCornerPos.getX() - 4, blCornerPos.getY() + height + 4),
                      Coord(blCornerPos.getX() + width + 4, blCornerPos.getY() - 4), Color(0, 0, 0),
                      Color(0, 0, 0), false, true);
  addGlEntity(frame2, dimName + "frame 2");

  backgroundRect = new GlRect(Coord(blCornerPos.getX(), blCornerPos.getY() + height),
                              Coord(blCornerPos.getX() + width, blCornerPos.getY()),
                              Color(255, 255, 255), Color(255, 255, 255), true, false);
  addGlEntity(backgroundRect, "background rect");
  clickLabel = new GlLabel(Coord(blCornerPos.getX() + width / 2, blCornerPos.getY() + height / 2),
                           Size(width, height / 4), Color(0, 0, 0));
  clickLabel->setText("Double Click to generate overview");
  addGlEntity(clickLabel, "label");

  computeBoundingBox();

  overviewLabel =
      new GlLabel(Coord(blCornerPos.getX() + width / 2, blCornerPos.getY() - labelHeight / 2),
                  Size(width, labelHeight), textColor);
  overviewLabel->setText(dimName);
  addGlEntity(overviewLabel, "overview label");
}

PixelOrientedOverview::~PixelOrientedOverview() {
  GlTextureManager::deleteTexture(textureName);
  reset(true);
}

struct NodeCoordXOrdering {
  bool operator()(pair<node, Coord> p1, pair<node, Coord> p2) {
    return p1.second.getX() < p2.second.getX();
  }
};

struct NodeCoordYOrdering {
  bool operator()(pair<node, Coord> p1, pair<node, Coord> p2) {
    return p1.second.getY() > p2.second.getY();
  }
};

void PixelOrientedOverview::computePixelView(GlWidget *glWidget) {

  reset(false);

  if (clickLabel != nullptr) {
    delete clickLabel;
    clickLabel = nullptr;
  }

  if (backgroundRect != nullptr) {
    delete backgroundRect;
    backgroundRect = nullptr;
  }

  if (frame != nullptr) {
    delete frame;
    frame = nullptr;
  }

  if (frame2 != nullptr) {
    delete frame2;
    frame2 = nullptr;
  }

  Graph *graph = data->getGraph();

  uint width = pixelOrientedMediator->getImageWidth();
  uint height = pixelOrientedMediator->getImageHeight();

  GlProgressBar *glProgressBar = nullptr;

  if (glWidget != nullptr) {
    glProgressBar =
        new GlProgressBar(Coord(blCornerPos.getX() + width / 2, blCornerPos.getY() + height / 2),
                          width, height, Color(0, 0, 255));
    glProgressBar->setComment("Generating overview ...");
    addGlEntity(glProgressBar, "progress bar");
  }

  uint currentStep = 0;
  uint maxStep = graph->numberOfNodes();
  uint drawStep = maxStep / 10;

  set<int> xCoordSet;

  for (uint i = 0; i < graph->numberOfNodes(); ++i) {
    node n = node(data->getItemIdAtRank(i));
    Vec2i pos = pixelOrientedMediator->getPixelPosForRank(i);
    Coord nodeCoord = Coord(pos[0], pos[1], 0);
    xCoordSet.insert(pos[0]);
    pixelLayout->setNodeValue(n, nodeCoord);
    ++currentStep;

    if (glWidget != nullptr && currentStep % drawStep == 0) {
      glProgressBar->progress(currentStep, maxStep);
      glWidget->draw();
    }
  }

  if (xCoordSet.size() < 2) {
    return;
  }

  auto it = xCoordSet.begin();
  int x1 = *(it++);
  int x2 = *it;
  int size = x2 - x1;

  pixelSize->setAllNodeValue(Size(size, size, size));

  overviewLabel->setColor(textColor);

  GlOffscreenRenderer &glOffscreenRenderer = GlOffscreenRenderer::instance();

  glOffscreenRenderer.setViewPortSize(width, height);
  glOffscreenRenderer.clearScene();
  glOffscreenRenderer.setSceneBackgroundColor(backgroundColor);
  glOffscreenRenderer.addGlGraphToScene(glGraph);
  glOffscreenRenderer.renderScene(true);

  if (glWidget != nullptr) {
    glProgressBar->progress(maxStep, maxStep);
    glWidget->draw();

    deleteGlEntity(glProgressBar);
    delete glProgressBar;
  }

  GLuint textureId = glOffscreenRenderer.getGLTexture(true);
  GlTextureManager::deleteTexture(textureName);
  GlTextureManager::registerExternalTexture(textureName, textureId);

  if (findGlEntity(dimName) == nullptr) {
    addGlEntity(new Gl2DRect(blCornerPos.getY() + pixelOrientedMediator->getImageHeight(),
                             blCornerPos.getY(), blCornerPos.getX(),
                             blCornerPos.getX() + pixelOrientedMediator->getImageWidth(),
                             textureName),
                dimName);
    addGlEntity(overviewLabel, "overview label");
    computeBoundingBox();
  }

  overviewGen = true;
}

void PixelOrientedOverview::setBLCorner(const Coord &blCorner) {
  GlComposite::translate(blCorner - blCornerPos);
  blCornerPos = blCorner;
  computeBoundingBox();
}
}
