/**
 *
 * Copyright (C) 2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef EDGESPLATTINGINTERACTOR_H_
#define EDGESPLATTINGINTERACTOR_H_

#include <talipot/GLInteractor.h>
#include <talipot/GlWidget.h>

#include <QOpenGLFramebufferObject>

#include <string>

#include "GraphSplattingInteractorConfigWidget.h"

namespace tlp {

class ColorProperty;
class GlShaderProgram;

class GraphSplattingInteractorComponent : public GLInteractorComponent {

  Q_OBJECT

public:
  GraphSplattingInteractorComponent(GraphSplattingInteractorConfigWidget *configWidget);

  GraphSplattingInteractorComponent(
      const GraphSplattingInteractorComponent &edgeSplattingInteractorComponent);

  ~GraphSplattingInteractorComponent();

  bool compute(GlWidget *glMainWidget);

  bool draw(GlWidget *glMainWidget);

  void viewChanged(View *view);

protected:
  void timerEvent(QTimerEvent *event);

protected slots:

  void configurationModified();

private:
  void createShaders();
  void setupInteractor();
  void createFrameBuffers(int width, int height);
  void destroyFrameBuffers();
  void generateDensityMap();
  void computeSplatField();
  void computeSplatFieldMinMaxWithGPUReduction();
  void generateDiffuseMap();
  void generateNormalMap();
  void renderSplatFieldWithBumpMapping();
  void renderSplatFieldWithColorMapping();

  GraphSplattingInteractorConfigWidget *configWidget;
  Graph *graph;
  std::string splattingColorMappingFragmentShaderId, splattingFragmentShaderId;
  GlShaderProgram *splattingColorMappingFragmentShader, *splattingFragmentShader;
  GlShaderProgram *normalMapGen3x3FragmentShader, *normalMapGen5x5FragmentShader,
      *normalMapGen9x9FragmentShader, *bumpmappingShader;
  GlShaderProgram *reductionMinMaxShader, *colorSplattingShader;

  unsigned int nbColors;
  int splattingRadius;
  int width, height;
  float min, max;
  float *minVar, *maxVar;
  float minLoc, maxLoc;
  GlWidget *glWidget;
  QOpenGLFramebufferObject *fboDensityAndSplatField, *fboDiffuseHeightAndNormalMap;
  QOpenGLFramebufferObject *fboReduction;
  QOpenGLFramebufferObject *fboColorSplatting;

  GLuint colorMappingInputTexId, colorScaleTextureId, grayScaleTextureId;
  Camera *camera;
  Camera *camera2D;
  GLuint densityTexId, splatFieldFirstPassTexId, splatFieldTexId;
  GLuint diffuseMapTexId, heightMapTexId, normalMapTexId;
  GLuint reductionTex1Id, reductionTex2Id;
  GLuint edgesRenderingTexId, colorSplattingFirstPassTexId, colorSplattingTexId;
  GLuint colorSumTexId;

  bool confModified;

  GlGraphInputData *splattingInputData;
  ColorProperty *viewColorTmp;
};

/** \file
 *  \brief  Tulip Graph Splatting Interactor

 * This interactor plugin allow to visualize nodes and edges density in a graph.
 * The amount of overdraw for nodes or edges is computed for each pixel of the graph
 * visualization, these values are then diffused by convoluting them with a Gaussian kernel and
 * mapped on screen with a colorscale. To produce a more visually appealing result, a bump mapping
 * based rendering is performed making dense areas emerge. Note that edge splatting is usefull
 * principally for edge bundled graph where edges have been rerouted and merged.
 *
 */
class GraphSplattingInteractor : public GLInteractorComposite {

public:
  PLUGININFORMATION("GraphSplattingInteractor", "Tulip Team", "21/10/2009", "Graph Splatting",
                    "1.1", "Visualization")

  GraphSplattingInteractor(const PluginContext *);
  ~GraphSplattingInteractor();

  void construct() override;

  QWidget *configurationWidget() const override {
    return configWidget;
  }

  unsigned int priority() const override {
    return 0;
  }

  virtual bool isCompatible(const std::string &viewName) const override;

private:
  GraphSplattingInteractorConfigWidget *configWidget;
};

}

#endif /* EDGESPLATTINGINTERACTOR_H_ */
