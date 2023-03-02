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

#include <GL/glew.h>

#include <talipot/GlGraph.h>
#include <talipot/GlLayer.h>
#include <talipot/View.h>
#include <talipot/GlWidget.h>
#include <talipot/GlTools.h>
#include <talipot/ColorScale.h>
#include <talipot/GlShaderProgram.h>
#include <talipot/MouseInteractors.h>
#include <talipot/GlView.h>
#include <talipot/GlCPULODCalculator.h>
#include <talipot/OpenGlConfigManager.h>
#include <talipot/config.h>
#include <talipot/GlVertexArrayManager.h>
#include <talipot/GlNode.h>
#include <talipot/GlEdge.h>
#include <talipot/NodeLinkDiagramView.h>

#include <iostream>
#include <climits>

#include <QTime>
#include <QTimeLine>
#include <QTimeLine>
#include <QCursor>
#include <QEvent>

#include "../../utils/InteractorIcons.h"
#include "../../utils/PluginNames.h"
#include "GraphSplattingInteractor.h"
#include "quad.h"

using namespace tlp;
using namespace std;

const float sobelFilter3x3[6] = {-1.0, -2.0, -1.0, 1.0, 2.0, 1.0};

const float prewittFilter3x3[6] = {-1.0, -1.0, -1.0, 1.0, 1.0, 1.0};

const float sobelFilter5x5[20] = {-1.0f,  -4.0f, -6.0f, -4.0f, -1.0f, -2.0f, -8.0f,
                                  -12.0f, -8.0f, -2.0f, 2.0f,  8.0f,  12.0f, 8.0f,
                                  2.0f,   1.0f,  4.0f,  6.0f,  4.0f,  1.0f};

const float prewittFilter5x5[20] = {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -2.0f, -2.0f,
                                    -2.0f, -2.0f, -2.0f, 2.0f,  2.0f,  2.0f,  2.0f,
                                    2.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f};

const float filter9x9[72] = {
    -0.00324675, -0.00649351, -0.00974026, -0.012987,   -0.0162338,  -0.012987,   -0.00974026,
    -0.00649351, -0.00324675, -0.00649351, -0.00974026, -0.012987,   -0.0162338,  -0.0194805,
    -0.0162338,  -0.012987,   -0.00974026, -0.00649351, -0.00974026, -0.012987,   -0.0162338,
    -0.0194805,  -0.0227273,  -0.0194805,  -0.0162338,  -0.012987,   -0.00974026, -0.012987,
    -0.0162338,  -0.0194805,  -0.0227273,  -0.025974,   -0.0227273,  -0.0194805,  -0.0162338,
    -0.012987,   0.012987,    0.0162338,   0.0194805,   0.0227273,   0.025974,    0.0227273,
    0.0194805,   0.0162338,   0.012987,    0.00974026,  0.012987,    0.0162338,   0.0194805,
    0.0227273,   0.0194805,   0.0162338,   0.012987,    0.00974026,  0.00649351,  0.00974026,
    0.012987,    0.0162338,   0.0194805,   0.0162338,   0.012987,    0.00974026,  0.00649351,
    0.00324675,  0.00649351,  0.00974026,  0.012987,    0.0162338,   0.012987,    0.00974026,
    0.00649351,  0.00324675};

const unsigned int colorScaleTextureSize = 1024;

static float *generateGaussianKernel(const int radius, const float sigma) {
  float *gaussianKernel = nullptr;
  if (radius > 0) {
    int kernelSize = 2 * radius + 1;
    gaussianKernel = new float[kernelSize];
    float gaussianKernelFactor = 0;

    for (int kx = -radius; kx <= radius; ++kx) {
      float e = exp(-(kx * kx) / (2.0f * sigma * sigma));
      gaussianKernelFactor += e;
      gaussianKernel[kx + radius] = e;
    }

    for (int kx = 0; kx < kernelSize; ++kx) {
      float val = gaussianKernel[kx];
      gaussianKernel[kx] = val / gaussianKernelFactor;
    }
  }
  return gaussianKernel;
}

static string splattingColorMappingFragmentShaderSrc =
    "#version 120\n"
    "uniform sampler2D densityMap;"
    "uniform sampler1D colorScale;"
    "uniform float min;"
    "uniform float max;"
    "uniform bool logMapping;"
    "void main() {"
    "	float d = texture2D(densityMap, gl_TexCoord[0].st).r;"
    "	if (d == 0.0) discard;"
    "	if (logMapping) {"
    "		gl_FragColor = texture1D(colorScale, pow(log(d+1) / log(max+1), 1.0/3.0));"
    "	} else {"
    "		gl_FragColor = texture1D(colorScale, pow((d-min) / (max-min), 1.0/3.0));"
    "	}"
    "}";

static string generateGaussianKernelConvolutionFragmentShader(const int radius) {
  ostringstream oss;
  oss << "#version 120" << endl;
  oss << "uniform float gaussianKernel[" << (2 * radius + 1) << "];" << endl;
  oss << "uniform bool horizontalPass;" << endl;
  oss << "uniform sampler2D densityMap;" << endl;
  oss << "uniform float stepW;" << endl;
  oss << "uniform float stepH;" << endl;
  oss << "void main(void) {" << endl;
  oss << "   vec4 sum = vec4(0.0);" << endl;
  oss << "   int idx = 0;" << endl;
  oss << "   for (int i = " << -radius << " ; i <= " << radius << " ; ++i) {" << endl;
  oss << "		vec2 neighborTexel = vec2(0.0);" << endl;
  oss << "		if (horizontalPass) {" << endl;
  oss << "			neighborTexel = gl_TexCoord[0].st + vec2(stepW*float(i), 0.0);"
      << endl;
  oss << "		} else {" << endl;
  oss << "			neighborTexel = gl_TexCoord[0].st + vec2(0.0, stepH*float(i));"
      << endl;
  oss << "		}" << endl;
  oss << "		vec4 d = texture2D(densityMap, neighborTexel);" << endl;
  oss << "		if (d != vec4(0.0)) {" << endl;
  oss << "			sum += (d * gaussianKernel[idx]);" << endl;
  oss << "		}" << endl;
  oss << "		++idx;" << endl;
  oss << "   }" << endl;
  oss << "   gl_FragColor = sum;" << endl;
  oss << "}" << endl;
  return oss.str();
}

static const string colorSplattingFragmentShaderSrc =
    "#version 120\n"
    "uniform sampler2D densityTexture;"
    "uniform sampler2D colorSumTexture;"
    "void main() {"
    "	float d = texture2D(densityTexture, gl_TexCoord[0].st).r;"
    "	if (d == 0.0) discard;"
    "	gl_FragColor = texture2D(colorSumTexture, gl_TexCoord[0].st) / ceil(d);"
    "}";

static const string reductionMinMaxVertexShaderSrc =
    "#version 120\n"
    "uniform float step;"
    "void main() {"
    "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
    "   gl_TexCoord[0] = (gl_MultiTexCoord0 - (step / 2.0)) * 2.0 + step / 2.0;"
    "}";

static const string reductionMinMaxFragmentShaderSrc =
    "#version 120\n"
    "uniform sampler2D input;"
    "uniform float step;"

    "void main() {"
    "   float min1 = texture2D(input, gl_TexCoord[0].st).r;"
    "   float min2 = texture2D(input, gl_TexCoord[0].st + vec2(step, 0.0)).r;"
    "   float min3 = texture2D(input, gl_TexCoord[0].st + vec2(0.0, step)).r;"
    "   float min4 = texture2D(input, gl_TexCoord[0].st + vec2(step, step)).r;"
    "   float max1 = texture2D(input, gl_TexCoord[0].st).g;"
    "   float max2 = texture2D(input, gl_TexCoord[0].st + vec2(step, 0.0)).g;"
    "   float max3 = texture2D(input, gl_TexCoord[0].st + vec2(0.0, step)).g;"
    "   float max4 = texture2D(input, gl_TexCoord[0].st + vec2(step, step)).g;"
    "   gl_FragColor = vec4(min(min1, min(min2 , min(min3, min4))), max(max1, max(max2 , max(max3, "
    "max4))), 0.0, 0.0);"
    "}";

static string generateNormalMapCreationFragmentShaderSourceCode(int filterRadius) {
  ostringstream oss;
  oss << "#version 120" << endl;
  oss << "uniform sampler2D splatTexture;" << endl;
  oss << "uniform float stepWidth;" << endl;
  oss << "uniform float stepHeight;" << endl;
  oss << "uniform float scaleFactor;" << endl;
  oss << "uniform float filter[" << (2 * filterRadius + 1) * (2 * filterRadius) << "];" << endl;
  oss << "float computeAverageRGB(vec3 rgbColor) {" << endl;
  oss << "	return (0.3 * rgbColor.r + 0.59 * rgbColor.g + 0.11 * rgbColor.b);" << endl;
  oss << "}" << endl;
  oss << "void main(void) {" << endl;
  oss << "	vec3 sum = vec3(0.0);" << endl;
  oss << "	vec3 texelColor = vec3(0.0);" << endl;
  oss << "	float height = 0.0;" << endl;
  oss << "	int idx = 0;" << endl;
  oss << "	for (int i = " << -filterRadius << "; i <= " << filterRadius << "; ++i) {" << endl;
  oss << "		if (i != 0) {" << endl;
  oss << "			for (int j = " << -filterRadius << " ; j <= " << filterRadius
      << " ; ++j) {" << endl;
  oss << "				vec2 du = vec2(stepWidth*i, -stepHeight*j);" << endl;
  oss << "				texelColor = texture2D(splatTexture, gl_TexCoord[0].st + "
         "du).rgb;"
      << endl;
  oss << "				height = computeAverageRGB(texelColor);" << endl;
  oss << "				sum.x += height * filter[idx];" << endl;
  oss << "				++idx;" << endl;
  oss << "			}" << endl;
  oss << "		}" << endl;
  oss << "	}" << endl;
  oss << "	idx = 0;" << endl;
  oss << "	for (int i = " << -filterRadius << "; i <= " << filterRadius << "; ++i) {" << endl;
  oss << "		if (i != 0) {" << endl;
  oss << "			for (int j = " << -filterRadius << " ; j <= " << filterRadius
      << " ; ++j) {" << endl;
  oss << "				vec2 dv = vec2(stepWidth*j, -stepHeight*i);" << endl;
  oss << "				texelColor = texture2D(splatTexture, gl_TexCoord[0].st + "
         "dv).rgb;"
      << endl;
  oss << "				height = computeAverageRGB(texelColor);" << endl;
  oss << "				sum.y += height * filter[filter.length - 1 - idx];" << endl;
  oss << "				++idx;" << endl;
  oss << "			}" << endl;
  oss << "		}" << endl;
  oss << "	}" << endl;
  oss << "	texelColor = texture2D(splatTexture, gl_TexCoord[0].st).rgb;" << endl;
  oss << "	height = computeAverageRGB(texelColor);" << endl;
  oss << "	sum *= -scaleFactor;" << endl;
  oss << "	sum.z = 1.0;" << endl;
  oss << "	sum = normalize(sum);" << endl;
  oss << "	gl_FragColor = vec4((sum + 1.0) * 0.5, height);" << endl;
  oss << "}" << endl;
  return oss.str();
}

static string bumpmappingVertexShader =
    "#version 120\n"
    "uniform vec3 dirToEye;"
    "uniform vec3 dirToLight;"

    "varying vec3 tbnDirToLight;"
    "varying vec3 tbnHalfVector;"
    "varying vec3 tbnDirToEye;"

    "void main(void) {"
    "	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
    "	gl_TexCoord[0] = gl_MultiTexCoord0;"
    "	vec3 tangent = gl_NormalMatrix * gl_MultiTexCoord3.xyz;"
    "	vec3 binormal = gl_NormalMatrix * gl_MultiTexCoord4.xyz;"
    "	vec3 normal = gl_NormalMatrix * gl_Normal;"
    "	vec3 vertex = vec3(gl_ModelViewMatrix * gl_Vertex);"
    "	tbnDirToEye.x = dot(dirToEye, tangent);"
    "	tbnDirToEye.y = dot(dirToEye, binormal);"
    "	tbnDirToEye.z = dot(dirToEye, normal);"
    "	tbnDirToLight.x = dot(dirToLight, tangent);"
    "	tbnDirToLight.y = dot(dirToLight, binormal);"
    "	tbnDirToLight.z = dot(dirToLight, normal);"
    "	tbnHalfVector = (tbnDirToEye + tbnDirToLight);"
    "}";

static string bumpmappingFragmentShader =
    "#version 120\n"
    "uniform sampler2D normalMap;"
    "uniform sampler2D diffuseMap;"
    "uniform vec4 lightAmbientColor;"
    "uniform vec4 lightDiffuseColor;"
    "uniform vec4 lightSpecularColor;"
    "uniform bool enableSpecular;"
    "uniform float specularExponent;"
    "uniform bool heightToAlpha;"

    "varying vec3 tbnDirToLight;"
    "varying vec3 tbnHalfVector;"
    "varying vec3 tbnDirToEye;"

    "void main(void) {"
    "	vec3 texCoord = vec3(gl_TexCoord[0].st, 0.0);"
    "	vec3 h = normalize(tbnHalfVector);"
    "	vec3 l = normalize(tbnDirToLight);"
    "	vec4 diffuseColor = texture2D(diffuseMap, texCoord.st);"
    "	vec3 normal = normalize(texture2D(normalMap, texCoord.st).rgb * 2.0 - 1.0);"
    "	vec4 ambient = lightAmbientColor * diffuseColor;"
    "	vec4 diffuse = vec4(0.0);"
    "	vec4 specular = vec4(0.0);"
    "	float diffuseIntensity = max(dot(normal, l), 0.0);"
    "	diffuse = lightDiffuseColor * diffuseColor * diffuseIntensity;"
    "	float height = texture2D(normalMap, texCoord.st).a;"
    "	if (enableSpecular) {"
    "		float specularModifier = max(dot(normal, h), 0.0);"
    "		specular = lightSpecularColor * pow(specularModifier, specularExponent);"
    "	}"
    "	float alpha = diffuseColor.a;"
    "	if (heightToAlpha) alpha = mix(0.0, 1.0, height);"
    "	gl_FragColor = vec4(clamp(ambient.rgb + diffuse.rgb + specular.rgb, 0.0, 1.0), alpha);"
    "}";

const int vsize = 16 * sizeof(float);
static GLuint quadVerticesVboId = 0;
static GLuint quadIndicesVboId = 0;

void drawTexturedQuad(float width, float height, GLuint texId) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texId);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0);
  glVertex2f(0.0, 0.0);
  glTexCoord2f(1.0, 0.0);
  glVertex2f(width, 0.0);
  glTexCoord2f(1.0, 1.0);
  glVertex2f(width, height);
  glTexCoord2f(0.0, 1.0);
  glVertex2f(0.0, height);
  glEnd();
  glDisable(GL_TEXTURE_2D);
}

void drawSquare(float size, float texCoord) {
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0);
  glVertex2f(0.0, 0.0);
  glTexCoord2f(texCoord, 0.0);
  glVertex2f(size, 0.0);
  glTexCoord2f(texCoord, texCoord);
  glVertex2f(size, size);
  glTexCoord2f(0.0, texCoord);
  glVertex2f(0.0, size);
  glEnd();
}

namespace tlp {

bool GraphSplattingInteractor::isCompatible(const string &viewName) const {
  return ((viewName == NodeLinkDiagramView::viewName) ||
          (viewName == ViewName::ScatterPlot2DViewName));
}

GraphSplattingInteractorComponent::GraphSplattingInteractorComponent(
    GraphSplattingInteractorConfigWidget *configWidget)
    : configWidget(configWidget), splattingColorMappingFragmentShader(nullptr),
      splattingFragmentShader(nullptr), normalMapGen3x3FragmentShader(nullptr),
      normalMapGen5x5FragmentShader(nullptr), normalMapGen9x9FragmentShader(nullptr),
      bumpmappingShader(nullptr), reductionMinMaxShader(nullptr), colorSplattingShader(nullptr),
      nbColors(0), splattingRadius(0), min(FLT_MAX), max(-FLT_MAX), glWidget(nullptr),
      fboDensityAndSplatField(nullptr), fboDiffuseHeightAndNormalMap(nullptr),
      fboReduction(nullptr), fboColorSplatting(nullptr), colorScaleTextureId(0),
      grayScaleTextureId(0), densityTexId(0), splatFieldFirstPassTexId(0), splatFieldTexId(0),
      diffuseMapTexId(0), heightMapTexId(0), normalMapTexId(0), reductionTex1Id(0),
      reductionTex2Id(0), edgesRenderingTexId(0), colorSplattingFirstPassTexId(0),
      colorSplattingTexId(0), colorSumTexId(0), confModified(true), splattingInputData(nullptr),
      viewColorTmp(nullptr) {}

GraphSplattingInteractorComponent::GraphSplattingInteractorComponent(
    const GraphSplattingInteractorComponent &edgeSplattingInteractorComponent)
    : splattingColorMappingFragmentShader(nullptr), splattingFragmentShader(nullptr),
      normalMapGen3x3FragmentShader(nullptr), normalMapGen5x5FragmentShader(nullptr),
      normalMapGen9x9FragmentShader(nullptr), bumpmappingShader(nullptr),
      reductionMinMaxShader(nullptr), colorSplattingShader(nullptr), nbColors(0),
      splattingRadius(0), min(FLT_MAX), max(-FLT_MAX), glWidget(nullptr),
      fboDensityAndSplatField(nullptr), fboDiffuseHeightAndNormalMap(nullptr),
      fboReduction(nullptr), fboColorSplatting(nullptr), colorScaleTextureId(0),
      grayScaleTextureId(0), densityTexId(0), splatFieldFirstPassTexId(0), splatFieldTexId(0),
      diffuseMapTexId(0), heightMapTexId(0), normalMapTexId(0), reductionTex1Id(0),
      reductionTex2Id(0), edgesRenderingTexId(0), colorSplattingFirstPassTexId(0), colorSumTexId(0),
      confModified(true), splattingInputData(nullptr), viewColorTmp(nullptr) {
  configWidget = edgeSplattingInteractorComponent.configWidget;
}

GraphSplattingInteractorComponent::~GraphSplattingInteractorComponent() {
  if (splattingColorMappingFragmentShader != nullptr) {
    delete splattingColorMappingFragmentShader;
  }
  if (splattingFragmentShader != nullptr) {
    delete splattingFragmentShader;
  }
  if (normalMapGen3x3FragmentShader != nullptr) {
    delete normalMapGen3x3FragmentShader;
  }
  if (normalMapGen5x5FragmentShader != nullptr) {
    delete normalMapGen5x5FragmentShader;
  }
  if (normalMapGen9x9FragmentShader != nullptr) {
    delete normalMapGen9x9FragmentShader;
  }
  if (bumpmappingShader != nullptr) {
    delete bumpmappingShader;
  }
  if (reductionMinMaxShader != nullptr) {
    delete reductionMinMaxShader;
  }
  if (colorSplattingShader != nullptr) {
    delete colorSplattingShader;
  }

  destroyFrameBuffers();
  glDeleteTextures(1, &colorScaleTextureId);
  glDeleteTextures(1, &grayScaleTextureId);

  delete splattingInputData;
  delete viewColorTmp;
}

static bool canDraw = false;

bool GraphSplattingInteractorComponent::compute(GlWidget *) {
  if (!canDraw)
    startTimer(500);
  return false;
}

void GraphSplattingInteractorComponent::createFrameBuffers(int width, int height) {

  glWidget->makeCurrent();

  fboDensityAndSplatField = new QOpenGLFramebufferObject(
      width, height, QOpenGLFramebufferObject::NoAttachment, GL_TEXTURE_2D, GL_RGBA32F_ARB);
  fboDiffuseHeightAndNormalMap =
      new QOpenGLFramebufferObject(width, height, QOpenGLFramebufferObject::NoAttachment);

  float maxLogSize = std::max(log(width) / log(2.0), log(height) / log(2.0));
  double fractpart, intpart;
  fractpart = modf(maxLogSize, &intpart);
  float exponent = intpart;
  if (fractpart != 0.0f)
    exponent += 1.0f;
  int fboReductionInitSize = int(pow(2, exponent));

  fboReduction = new QOpenGLFramebufferObject(fboReductionInitSize, fboReductionInitSize,
                                              QOpenGLFramebufferObject::NoAttachment, GL_TEXTURE_2D,
                                              GL_RGBA32F_ARB);

  fboColorSplatting = new QOpenGLFramebufferObject(
      width, height, QOpenGLFramebufferObject::CombinedDepthStencil, GL_TEXTURE_2D);

  glPushAttrib(GL_ALL_ATTRIB_BITS);

  reductionTex1Id = fboReduction->texture();
  fboReduction->bind();
  glGenTextures(1, &reductionTex2Id);
  glBindTexture(GL_TEXTURE_2D, reductionTex2Id);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, fboReductionInitSize, fboReductionInitSize, 0,
               GL_RGBA, GL_FLOAT, nullptr);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D,
                            reductionTex2Id, 0);
  fboReduction->release();

  densityTexId = fboDensityAndSplatField->texture();
  fboDensityAndSplatField->bind();
  glGenTextures(1, &splatFieldFirstPassTexId);
  glBindTexture(GL_TEXTURE_2D, splatFieldFirstPassTexId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D,
                            splatFieldFirstPassTexId, 0);

  glGenTextures(1, &splatFieldTexId);
  glBindTexture(GL_TEXTURE_2D, splatFieldTexId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D,
                            splatFieldTexId, 0);

  glGenTextures(1, &colorSumTexId);
  glBindTexture(GL_TEXTURE_2D, colorSumTexId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT3_EXT, GL_TEXTURE_2D,
                            colorSumTexId, 0);
  fboDensityAndSplatField->release();

  diffuseMapTexId = fboDiffuseHeightAndNormalMap->texture();
  fboDiffuseHeightAndNormalMap->bind();
  glGenTextures(1, &heightMapTexId);
  glBindTexture(GL_TEXTURE_2D, heightMapTexId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D,
                            heightMapTexId, 0);

  glGenTextures(1, &normalMapTexId);
  glBindTexture(GL_TEXTURE_2D, normalMapTexId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D,
                            normalMapTexId, 0);
  fboDiffuseHeightAndNormalMap->release();

  edgesRenderingTexId = fboColorSplatting->texture();
  fboColorSplatting->bind();
  glGenTextures(1, &colorSplattingFirstPassTexId);
  glBindTexture(GL_TEXTURE_2D, colorSplattingFirstPassTexId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D,
                            colorSplattingFirstPassTexId, 0);

  glGenTextures(1, &colorSplattingTexId);
  glBindTexture(GL_TEXTURE_2D, colorSplattingTexId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D,
                            colorSplattingTexId, 0);
  fboColorSplatting->release();

  glPopAttrib();
}

void GraphSplattingInteractorComponent::destroyFrameBuffers() {
  glDeleteTextures(1, &splatFieldFirstPassTexId);
  glDeleteTextures(1, &splatFieldTexId);
  glDeleteTextures(1, &colorSumTexId);
  if (fboDensityAndSplatField) {
    delete fboDensityAndSplatField;
    fboDensityAndSplatField = nullptr;
  }
  glDeleteTextures(1, &heightMapTexId);
  glDeleteTextures(1, &normalMapTexId);
  if (fboDiffuseHeightAndNormalMap) {
    delete fboDiffuseHeightAndNormalMap;
    fboDiffuseHeightAndNormalMap = nullptr;
  }
  glDeleteTextures(1, &reductionTex2Id);
  if (fboReduction) {
    delete fboReduction;
    fboReduction = nullptr;
  }

  glDeleteTextures(1, &colorSplattingFirstPassTexId);
  glDeleteTextures(1, &colorSplattingTexId);
  if (fboColorSplatting) {
    delete fboColorSplatting;
    fboColorSplatting = nullptr;
  }
}

void GraphSplattingInteractorComponent::generateDensityMap() {
  GlGraphInputData *inputData = glWidget->scene()->glGraph()->inputData();

  SizeProperty *currentViewSize = splattingInputData->sizes();

  fboDensityAndSplatField->bind();
  glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
  camera->initGl();
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  GlScene *glScene = glWidget->scene();
  GlGraph *glGraph = glScene->glGraph();

  GlGraphRenderingParameters renderingParameters = glGraph->renderingParameters();
  GlGraphRenderingParameters oriRenderingParameters(renderingParameters);

  GlCPULODCalculator lodCalculator;
  lodCalculator.setInputData(splattingInputData);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_POLYGON_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  if (configWidget->edgeSplatting()) {
    lodCalculator.setRenderingEntitiesFlag(RenderingEdges);

    renderingParameters.setDisplayEdges(true);
    renderingParameters.setDisplayNodes(false);
    glGraph->setRenderingParameters(renderingParameters);

    splattingInputData->setRenderingParameters(&renderingParameters);
    // splattingInputData->getGlVertexArrayManager()->beginRendering();

    GlLayer fakeLayer("fakeLayer", camera);
    fakeLayer.acceptVisitor(&lodCalculator);
    glScene->glGraph()->acceptVisitorOnGraph(&lodCalculator);
    lodCalculator.compute(glScene->getViewport(), glScene->getViewport());
    LayersLODVector &layersLODVector = lodCalculator.getResult();
    vector<GraphElementLODUnit> &edgesVector = layersLODVector[0].edgesLODVector;
    GlEdge glEdge(0);
    for (auto &it : edgesVector) {
      bool drawEdge = true;
      edge e(it.id);
      if (configWidget->edgeSplattingRestriction()) {
        drawEdge = false;
        node src = graph->source(e);
        node tgt = graph->target(e);
        GlNode srcGl(src.id);
        GlNode tgtGl(tgt.id);
        BoundingBox srcGlBB(srcGl.getBoundingBox(splattingInputData));
        BoundingBox tgtGlBB(tgtGl.getBoundingBox(splattingInputData));
        Coord srcGlBBBLScr(camera->worldTo2DScreen(Coord(srcGlBB[0])));
        Coord srcGlBBTRScr(camera->worldTo2DScreen(Coord(srcGlBB[1])));
        if ((srcGlBBBLScr.getX() >= 0 && srcGlBBBLScr.getX() <= width && srcGlBBBLScr.getY() >= 0 &&
             srcGlBBBLScr.getY() <= height) ||
            (srcGlBBTRScr.getX() >= 0 && srcGlBBTRScr.getX() <= width && srcGlBBTRScr.getY() >= 0 &&
             srcGlBBTRScr.getY() <= height)) {
          drawEdge = true;
        }
        if (!drawEdge) {
          Coord tgtGlBBBLScr(camera->worldTo2DScreen(Coord(tgtGlBB[0])));
          Coord tgtGlBBTRScr(camera->worldTo2DScreen(Coord(tgtGlBB[1])));
          if ((tgtGlBBBLScr.getX() >= 0 && tgtGlBBBLScr.getX() <= width &&
               tgtGlBBBLScr.getY() >= 0 && tgtGlBBBLScr.getY() <= height) ||
              (tgtGlBBTRScr.getX() >= 0 && tgtGlBBTRScr.getX() <= width &&
               tgtGlBBTRScr.getY() >= 0 && tgtGlBBTRScr.getY() <= height)) {
            drawEdge = true;
          }
        }
      }
      if (drawEdge) {
        glEdge.e = it.id;
        glEdge.draw(it.lod, splattingInputData, camera);
      }
    }

    // splattingInputData->getGlVertexArrayManager()->endRendering();

  } else {
    lodCalculator.setRenderingEntitiesFlag(RenderingNodes);
    renderingParameters.setDisplayNodes(true);
    renderingParameters.setDisplayEdges(false);
    glGraph->setRenderingParameters(renderingParameters);
    GlLayer fakeLayer("fakeLayer", camera);
    fakeLayer.acceptVisitor(&lodCalculator);
    glScene->glGraph()->acceptVisitorOnGraph(&lodCalculator);
    LayersLODVector &layersLODVector = lodCalculator.getResult();
    vector<GraphElementLODUnit> &nodesVector = layersLODVector[0].nodesLODVector;
    GlNode glNode(0);
    for (auto &it : nodesVector) {
      glNode.n = node(it.id);
      glNode.draw(100, splattingInputData, camera);
    }
  }
  glDisable(GL_BLEND);
  fboDensityAndSplatField->release();

  colorMappingInputTexId = densityTexId;

  if (configWidget->edgeSplatting() && configWidget->bumpmapSplatting() &&
      (configWidget->useGraphColorsForDiffuseMap() ||
       configWidget->useMeanGraphColorsForDiffuseMap())) {

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (configWidget->useMeanGraphColorsForDiffuseMap()) {
      fboDensityAndSplatField->bind();
      glDrawBuffer(GL_COLOR_ATTACHMENT3_EXT);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LEQUAL);
      glClear(GL_DEPTH_BUFFER_BIT);
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);
      glClearColor(0.0, 0.0, 0.0, 0.0);
    } else {
      fboColorSplatting->bind();
      glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LEQUAL);
      glClear(GL_DEPTH_BUFFER_BIT);
      glClearColor(0.0, 0.0, 0.0, 0.0);
    }

    glClear(GL_COLOR_BUFFER_BIT);

    renderingParameters.setEdgeSizeInterpolate(false);
    renderingParameters.setEdgesMaxSizeToNodesSize(false);
    glGraph->setRenderingParameters(renderingParameters);

    SizeProperty *viewSizeTmp = new SizeProperty(graph);
    for (auto e : graph->edges()) {
      viewSizeTmp->setEdgeValue(e, currentViewSize->getEdgeValue(e) *
                                       (configWidget->getSplattingRadius() / 5.0f) *
                                       (configWidget->getSplattingSigma() / 3.0f) * 20.0f);
    }
    for (auto n : graph->nodes()) {
      viewSizeTmp->setNodeValue(n, currentViewSize->getNodeValue(n));
    }

    inputData->setSizes(viewSizeTmp);

    camera->initGl();

    LayersLODVector &layersLODVector = lodCalculator.getResult();
    vector<GraphElementLODUnit> &edgesVector = layersLODVector[0].edgesLODVector;
    GlEdge glEdge(0);
    for (auto &it : edgesVector) {
      bool drawEdge = true;
      edge e(it.id);
      if (configWidget->edgeSplattingRestriction()) {
        drawEdge = false;
        node src = graph->source(e);
        node tgt = graph->target(e);
        GlNode srcGl(src.id);
        GlNode tgtGl(tgt.id);
        BoundingBox srcGlBB(srcGl.getBoundingBox(inputData));
        BoundingBox tgtGlBB(tgtGl.getBoundingBox(inputData));
        Coord srcGlBBBLScr(camera->worldTo2DScreen(Coord(srcGlBB[0])));
        Coord srcGlBBTRScr(camera->worldTo2DScreen(Coord(srcGlBB[1])));
        if ((srcGlBBBLScr.getX() >= 0 && srcGlBBBLScr.getX() <= width && srcGlBBBLScr.getY() >= 0 &&
             srcGlBBBLScr.getY() <= height) ||
            (srcGlBBTRScr.getX() >= 0 && srcGlBBTRScr.getX() <= width && srcGlBBTRScr.getY() >= 0 &&
             srcGlBBTRScr.getY() <= height)) {
          drawEdge = true;
        }
        if (!drawEdge) {
          Coord tgtGlBBBLScr(camera->worldTo2DScreen(Coord(tgtGlBB[0])));
          Coord tgtGlBBTRScr(camera->worldTo2DScreen(Coord(tgtGlBB[1])));
          if ((tgtGlBBBLScr.getX() >= 0 && tgtGlBBBLScr.getX() <= width &&
               tgtGlBBBLScr.getY() >= 0 && tgtGlBBBLScr.getY() <= height) ||
              (tgtGlBBTRScr.getX() >= 0 && tgtGlBBTRScr.getX() <= width &&
               tgtGlBBTRScr.getY() >= 0 && tgtGlBBTRScr.getY() <= height)) {
            drawEdge = true;
          }
        }
      }
      if (drawEdge) {
        glEdge.e = e;
        glEdge.draw(it.lod, inputData, camera);
      }
    }

    inputData->setSizes(currentViewSize);
    delete viewSizeTmp;

    if (configWidget->useMeanGraphColorsForDiffuseMap()) {
      fboDensityAndSplatField->release();
    } else {
      fboColorSplatting->release();
    }

    glPopAttrib();
  }

  glGraph->setRenderingParameters(oriRenderingParameters);
  splattingInputData->setRenderingParameters(&glGraph->renderingParameters());
}

void GraphSplattingInteractorComponent::computeSplatField() {

  int diffuseRadius = configWidget->getSplattingRadius();

  float *gaussianKernel = generateGaussianKernel(diffuseRadius, configWidget->getSplattingSigma());

  fboDensityAndSplatField->bind();
  camera2D->initGl();
  splattingFragmentShader->activate();
  splattingFragmentShader->setUniformFloatArray("gaussianKernel", (2 * diffuseRadius + 1),
                                                gaussianKernel);
  splattingFragmentShader->setUniformBool("horizontalPass", true);
  splattingFragmentShader->setUniformTextureSampler("densityMap", 0);
  splattingFragmentShader->setUniformFloat("stepW", 1.0f / width);
  splattingFragmentShader->setUniformFloat("stepH", 1.0f / height);

  glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);
  drawTexturedQuad(width, height, densityTexId);

  glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);
  splattingFragmentShader->setUniformBool("horizontalPass", false);
  drawTexturedQuad(width, height, splatFieldFirstPassTexId);

  splattingFragmentShader->deactivate();
  fboDensityAndSplatField->release();

  delete[] gaussianKernel;

  colorMappingInputTexId = splatFieldTexId;
}

void GraphSplattingInteractorComponent::computeSplatFieldMinMaxWithGPUReduction() {

  glPushAttrib(GL_ALL_ATTRIB_BITS);

  fboReduction->bind();
  glViewport(0, 0, fboReduction->width(), fboReduction->width());
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, fboReduction->width(), 0, fboReduction->width(), -1, 1);
  glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
  glClearColor(FLT_MAX, -FLT_MAX, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  setMaterial(Color(255, 255, 255));
  drawTexturedQuad(width, height, colorMappingInputTexId);

  unsigned int currentInputSize = fboReduction->width();
  unsigned int currentOutputSize = currentInputSize / 2;
  float currentTexCoord = 0.5f;

  GLuint texToBind = reductionTex1Id;
  GLenum drawBufferToBind = GL_COLOR_ATTACHMENT1_EXT;
  GLenum lastDrawBufferBound = GL_COLOR_ATTACHMENT0_EXT;

  unsigned int nbPasses = log(fboReduction->width()) / log(2.0) - 1;

  reductionMinMaxShader->activate();
  reductionMinMaxShader->setUniformTextureSampler("input", 0);
  reductionMinMaxShader->setUniformFloat("step", (1.0f / currentInputSize));

  glEnable(GL_TEXTURE_2D);
  for (unsigned int i = 0; i < nbPasses; ++i) {
    glDrawBuffer(drawBufferToBind);
    glBindTexture(GL_TEXTURE_2D, texToBind);
    drawSquare(currentOutputSize, currentTexCoord);
    if (texToBind == reductionTex1Id) {
      texToBind = reductionTex2Id;
      drawBufferToBind = GL_COLOR_ATTACHMENT0_EXT;
      lastDrawBufferBound = GL_COLOR_ATTACHMENT1_EXT;
    } else {
      texToBind = reductionTex1Id;
      drawBufferToBind = GL_COLOR_ATTACHMENT1_EXT;
      lastDrawBufferBound = GL_COLOR_ATTACHMENT0_EXT;
    }
    currentInputSize /= 2;
    currentOutputSize /= 2;
    currentTexCoord /= 2.0f;
  }

  glDisable(GL_TEXTURE_2D);
  reductionMinMaxShader->deactivate();

  float gpuResult[4];
  glReadBuffer(lastDrawBufferBound);
  glReadPixels(0, 0, 1, 1, GL_RGBA, GL_FLOAT, gpuResult);
  fboReduction->release();

  if (gpuResult[0] < *minVar) {
    *minVar = gpuResult[0];
  }

  if (gpuResult[1] > *maxVar) {
    *maxVar = gpuResult[1];
  }

  glPopAttrib();
}

void GraphSplattingInteractorComponent::generateDiffuseMap() {
  fboDiffuseHeightAndNormalMap->bind();
  glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
  camera2D->initGl();
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_1D);
  glBindTexture(GL_TEXTURE_1D, colorScaleTextureId);
  glActiveTexture(GL_TEXTURE0);
  splattingColorMappingFragmentShader->activate();
  splattingColorMappingFragmentShader->setUniformTextureSampler("colorScale", 1);
  splattingColorMappingFragmentShader->setUniformTextureSampler("densityMap", 0);
  splattingColorMappingFragmentShader->setUniformFloat("min", *minVar);
  splattingColorMappingFragmentShader->setUniformFloat("max", *maxVar);
  splattingColorMappingFragmentShader->setUniformBool(
      "logMapping", configWidget->getMappingType() == LOGARITHMIC);

  drawTexturedQuad(width, height, colorMappingInputTexId);
  splattingColorMappingFragmentShader->deactivate();
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, 0);
  glDisable(GL_TEXTURE_1D);
  glActiveTexture(GL_TEXTURE0);
  fboDiffuseHeightAndNormalMap->release();

  if (configWidget->edgeSplatting() && configWidget->bumpmapSplatting() &&
      (configWidget->useGraphColorsForDiffuseMap() ||
       configWidget->useMeanGraphColorsForDiffuseMap())) {

    fboColorSplatting->bind();
    camera2D->initGl();
    if (configWidget->useMeanGraphColorsForDiffuseMap()) {
      glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
      glClearColor(0.0, 0.0, 0.0, 0.0);
      glClear(GL_COLOR_BUFFER_BIT);

      glActiveTexture(GL_TEXTURE1);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, colorSumTexId);
      glActiveTexture(GL_TEXTURE0);
      colorSplattingShader->activate();
      colorSplattingShader->setUniformTextureSampler("densityTexture", 0);
      colorSplattingShader->setUniformTextureSampler("colorSumTexture", 1);
      drawTexturedQuad(width, height, densityTexId);
      colorSplattingShader->deactivate();
      glActiveTexture(GL_TEXTURE1);
      glDisable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, 0);
      glActiveTexture(GL_TEXTURE0);
    }

    int diffuseRadius = configWidget->getSplattingRadius();
    float *gaussianKernel =
        generateGaussianKernel(diffuseRadius, configWidget->getSplattingSigma());

    splattingFragmentShader->activate();
    splattingFragmentShader->setUniformFloatArray("gaussianKernel", (2 * diffuseRadius + 1),
                                                  gaussianKernel);
    splattingFragmentShader->setUniformBool("horizontalPass", true);
    splattingFragmentShader->setUniformTextureSampler("densityMap", 0);
    splattingFragmentShader->setUniformFloat("stepW", 1.0f / width);
    splattingFragmentShader->setUniformFloat("stepH", 1.0f / height);

    glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    drawTexturedQuad(width, height, edgesRenderingTexId);

    glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    splattingFragmentShader->setUniformBool("horizontalPass", false);
    drawTexturedQuad(width, height, colorSplattingFirstPassTexId);

    splattingFragmentShader->deactivate();
    fboColorSplatting->release();

    delete[] gaussianKernel;
  }
}

void GraphSplattingInteractorComponent::generateNormalMap() {

  fboDiffuseHeightAndNormalMap->bind();
  glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
  camera2D->initGl();
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_1D);
  glBindTexture(GL_TEXTURE_1D, grayScaleTextureId);
  glActiveTexture(GL_TEXTURE0);
  splattingColorMappingFragmentShader->activate();
  splattingColorMappingFragmentShader->setUniformTextureSampler("colorScale", 1);
  splattingColorMappingFragmentShader->setUniformTextureSampler("densityMap", 0);
  splattingColorMappingFragmentShader->setUniformFloat("min", *minVar);
  splattingColorMappingFragmentShader->setUniformFloat("max", *maxVar);
  drawTexturedQuad(width, height, colorMappingInputTexId);
  splattingColorMappingFragmentShader->deactivate();
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, 0);
  glDisable(GL_TEXTURE_1D);
  glActiveTexture(GL_TEXTURE0);

  string normalMapFilter = configWidget->getNormalMapFilterName();

  glDrawBuffer(GL_COLOR_ATTACHMENT2_EXT);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);
  GlShaderProgram *normalMapGenerationShader = nullptr;
  if (normalMapFilter == SOBEL_3X3 || normalMapFilter == PREWITT_3X3) {
    normalMapGenerationShader = normalMapGen3x3FragmentShader;
  } else if (normalMapFilter == SOBEL_5X5 || normalMapFilter == PREWITT_5X5) {
    normalMapGenerationShader = normalMapGen5x5FragmentShader;
  } else {
    normalMapGenerationShader = normalMapGen9x9FragmentShader;
  }
  normalMapGenerationShader->activate();
  normalMapGenerationShader->setUniformTextureSampler("splatTexture", 0);
  normalMapGenerationShader->setUniformFloat("stepWidth", 1.0 / (width - 1));
  normalMapGenerationShader->setUniformFloat("stepHeight", 1.0 / (height - 1));
  if (normalMapFilter == SOBEL_3X3) {
    normalMapGenerationShader->setUniformFloatArray("filter", 6, sobelFilter3x3);
  } else if (normalMapFilter == PREWITT_3X3) {
    normalMapGenerationShader->setUniformFloatArray("filter", 6, prewittFilter3x3);
  } else if (normalMapFilter == SOBEL_5X5) {
    normalMapGenerationShader->setUniformFloatArray("filter", 20, sobelFilter5x5);
  } else if (normalMapFilter == PREWITT_5X5) {
    normalMapGenerationShader->setUniformFloatArray("filter", 20, prewittFilter5x5);
  } else {
    normalMapGenerationShader->setUniformFloatArray("filter", 72, filter9x9);
  }
  normalMapGenerationShader->setUniformFloat("scaleFactor",
                                             configWidget->getBumpmappingScaleFactor());
  drawTexturedQuad(width, height, heightMapTexId);
  normalMapGenerationShader->deactivate();
  fboDiffuseHeightAndNormalMap->release();
}

void GraphSplattingInteractorComponent::renderSplatFieldWithBumpMapping() {

  if (quadVerticesVboId == 0) {
    glGenBuffers(1, &quadVerticesVboId);
    glBindBuffer(GL_ARRAY_BUFFER, quadVerticesVboId);
    glBufferData(GL_ARRAY_BUFFER, QUAD_NUM_VERTS * vsize, quad_verts, GL_STATIC_DRAW);
    glGenBuffers(1, &quadIndicesVboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndicesVboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, QUAD_NUM_INDICES * sizeof(float), quad_indices,
                 GL_STATIC_DRAW);
  }

  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glViewport(0, 0, width, height);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(width / 2.0f, height / 2.0f, 0);
  glScalef(width / 2.0f, height / 2.0f, 0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width, 0, height, -1, 1);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
  glEnable(GL_STENCIL_TEST);

  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_2D);
  if (configWidget->edgeSplatting() && configWidget->bumpmapSplatting() &&
      (configWidget->useGraphColorsForDiffuseMap() ||
       configWidget->useMeanGraphColorsForDiffuseMap())) {
    glBindTexture(GL_TEXTURE_2D, colorSplattingTexId);
  } else {
    glBindTexture(GL_TEXTURE_2D, diffuseMapTexId);
  }
  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, normalMapTexId);

  glBindBuffer(GL_ARRAY_BUFFER, quadVerticesVboId);

#define OFFSET(x) (reinterpret_cast<char *>((x) * sizeof(float)))

  glVertexPointer(4, GL_FLOAT, vsize, OFFSET(0));
  glNormalPointer(GL_FLOAT, vsize, OFFSET(12));
  glClientActiveTexture(GL_TEXTURE4);
  glTexCoordPointer(3, GL_FLOAT, vsize, OFFSET(9));
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glClientActiveTexture(GL_TEXTURE3);
  glTexCoordPointer(3, GL_FLOAT, vsize, OFFSET(6));
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glClientActiveTexture(GL_TEXTURE0);
  glTexCoordPointer(2, GL_FLOAT, vsize, OFFSET(4));
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  bumpmappingShader->activate();
  bumpmappingShader->setUniformVec3Float("dirToEye", 0.0f, 0.0f, 1.0f);
  bumpmappingShader->setUniformVec3Float("dirToLight", 0.0f, 0.0f, 1.0f);
  bumpmappingShader->setUniformTextureSampler("normalMap", 0);
  bumpmappingShader->setUniformTextureSampler("diffuseMap", 1);
  bumpmappingShader->setUniformColor("lightAmbientColor", configWidget->getAmbientColor());
  bumpmappingShader->setUniformColor("lightDiffuseColor", configWidget->getDiffuseColor());
  bumpmappingShader->setUniformColor("lightSpecularColor", configWidget->getSpecularColor());
  bumpmappingShader->setUniformTextureSampler("enableSpecular", configWidget->useSpecular());
  bumpmappingShader->setUniformFloat("specularExponent", configWidget->getSpecularExponent());
  bumpmappingShader->setUniformBool("heightToAlpha",
                                    (configWidget->useGraphColorsForDiffuseMap() ||
                                     configWidget->useMeanGraphColorsForDiffuseMap()));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndicesVboId);
  glDrawElements(GL_TRIANGLES, QUAD_NUM_INDICES, GL_UNSIGNED_SHORT, OFFSET(0));

  bumpmappingShader->deactivate();

#undef OFFSET

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glClientActiveTexture(GL_TEXTURE4);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glClientActiveTexture(GL_TEXTURE3);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glClientActiveTexture(GL_TEXTURE0);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glActiveTexture(GL_TEXTURE1);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glDisable(GL_TEXTURE_2D);

  glPopAttrib();
}

void GraphSplattingInteractorComponent::renderSplatFieldWithColorMapping() {
  camera2D->initGl();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_STENCIL_TEST);

  setMaterial(Color(255, 255, 255));
  drawTexturedQuad(width, height, diffuseMapTexId);
}

bool GraphSplattingInteractorComponent::draw(GlWidget *glMainWidget) {

  if (!canDraw) {
    return false;
  }

  // glPushAttrib(GL_ALL_ATTRIB_BITS);

  // glMatrixMode(GL_PROJECTION);
  // glPushMatrix();

  // glMatrixMode(GL_MODELVIEW);
  // glPushMatrix();

  createShaders();
  setupInteractor();

  if (!splattingColorMappingFragmentShader) {
    return false;
  }

  width = glMainWidget->width();
  height = glMainWidget->height();

  bool createFbos = false;
  if (fboDiffuseHeightAndNormalMap == nullptr) {
    createFbos = true;
  } else if (fboDiffuseHeightAndNormalMap->width() != width ||
             fboDiffuseHeightAndNormalMap->height() != height) {
    destroyFrameBuffers();
    createFbos = true;
  }

  if (createFbos) {
    createFrameBuffers(width, height);
  }

  minLoc = FLT_MAX, maxLoc = -FLT_MAX;

  if (configWidget->adjustSplattingToZoom()) {
    minVar = &minLoc;
    maxVar = &maxLoc;
  } else {
    minVar = &min;
    maxVar = &max;
  }

  OpenGlConfigManager::setAntiAliasing(false);

  generateDensityMap();

  if (configWidget->splattingEnabled()) {
    computeSplatField();
  }

  // force all pending GL commands to be executed before running GPU reduction
  // Application crashes could occur without this fix
  glFinish();

  computeSplatFieldMinMaxWithGPUReduction();

  generateDiffuseMap();

  if (!configWidget->keepOriginalGraphImageInBackground()) {
    Color backgroundColor = glMainWidget->scene()->getBackgroundColor();
    glClearColor(backgroundColor.getRGL(), backgroundColor.getGGL(), backgroundColor.getBGL(), 0.0);
    glClearStencil(0xFFFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }

  if (configWidget->bumpmapSplatting()) {
    generateNormalMap();
    renderSplatFieldWithBumpMapping();
  } else {
    renderSplatFieldWithColorMapping();
  }

  confModified = false;

  // glFinish();

  // glMatrixMode(GL_MODELVIEW);
  // glPopMatrix();

  // glMatrixMode(GL_PROJECTION);
  // glPopMatrix();

  // glPopAttrib();

  return true;
}

void GraphSplattingInteractorComponent::createShaders() {
  if (splattingColorMappingFragmentShader == nullptr) {
    splattingColorMappingFragmentShader = new GlShaderProgram();
    splattingColorMappingFragmentShader->addShaderFromSourceCode(
        Fragment, splattingColorMappingFragmentShaderSrc);
  }
  if (normalMapGen3x3FragmentShader == nullptr) {
    normalMapGen3x3FragmentShader = new GlShaderProgram();
    normalMapGen3x3FragmentShader->addShaderFromSourceCode(
        Fragment, generateNormalMapCreationFragmentShaderSourceCode(1));
  }
  if (normalMapGen5x5FragmentShader == nullptr) {
    normalMapGen5x5FragmentShader = new GlShaderProgram();
    normalMapGen5x5FragmentShader->addShaderFromSourceCode(
        Fragment, generateNormalMapCreationFragmentShaderSourceCode(2));
  }
  if (normalMapGen9x9FragmentShader == nullptr) {
    normalMapGen9x9FragmentShader = new GlShaderProgram();
    normalMapGen9x9FragmentShader->addShaderFromSourceCode(
        Fragment, generateNormalMapCreationFragmentShaderSourceCode(4));
  }
  if (bumpmappingShader == nullptr) {
    bumpmappingShader = new GlShaderProgram();
    bumpmappingShader->addShaderFromSourceCode(Vertex, bumpmappingVertexShader);
    bumpmappingShader->addShaderFromSourceCode(Fragment, bumpmappingFragmentShader);
  }
  if (reductionMinMaxShader == nullptr) {
    reductionMinMaxShader = new GlShaderProgram();
    reductionMinMaxShader->addShaderFromSourceCode(Vertex, reductionMinMaxVertexShaderSrc);
    reductionMinMaxShader->addShaderFromSourceCode(Fragment, reductionMinMaxFragmentShaderSrc);
  }

  if (colorSplattingShader == nullptr) {
    colorSplattingShader = new GlShaderProgram();
    colorSplattingShader->addShaderFromSourceCode(Fragment, colorSplattingFragmentShaderSrc);
  }
}

void GraphSplattingInteractorComponent::viewChanged(View *view) {
  if (view == nullptr) {
    glWidget = nullptr;
    graph = nullptr;
    return;
  }

  glWidget = static_cast<GlView *>(view)->glWidget();
  graph = glWidget->scene()->glGraph()->graph();

  glWidget->makeCurrent();

  camera = &glWidget->scene()->getLayer("Main")->getCamera();
  camera2D = new Camera(glWidget->scene(), false);

  vector<Color> grayScaleColors;
  grayScaleColors.push_back(Color(0, 0, 0));
  grayScaleColors.push_back(Color(255, 255, 255));
  ColorScale grayScale;
  grayScale.setColorScale(grayScaleColors);
  unsigned char *grayScaleTextureData = new unsigned char[colorScaleTextureSize * 4];
  for (unsigned int i = 0; i < colorScaleTextureSize; ++i) {
    Color color = grayScale.getColorAtPos(i / float(colorScaleTextureSize - 1));
    grayScaleTextureData[4 * i] = color.getR();
    grayScaleTextureData[4 * i + 1] = color.getG();
    grayScaleTextureData[4 * i + 2] = color.getB();
    grayScaleTextureData[4 * i + 3] = color.getA();
  }
  glEnable(GL_TEXTURE_1D);
  glGenTextures(1, &grayScaleTextureId);
  glBindTexture(GL_TEXTURE_1D, grayScaleTextureId);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, colorScaleTextureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               grayScaleTextureData);
  glDisable(GL_TEXTURE_1D);

  delete[] grayScaleTextureData;

  GlGraph *glGraph = glWidget->scene()->glGraph();

  delete splattingInputData;
  splattingInputData = new GlGraphInputData(graph, &glGraph->renderingParameters());
  delete viewColorTmp;
  viewColorTmp = new ColorProperty(graph);
  viewColorTmp->setAllEdgeValue(Color(255, 255, 0.0, 0.0));
  viewColorTmp->setAllNodeValue(Color(255, 255, 0.0, 0.0));
  splattingInputData->setBorderColors(viewColorTmp);
  splattingInputData->setColors(viewColorTmp);

  connect(configWidget, SIGNAL(configModified()), this, SLOT(configurationModified()));

  canDraw = false;
}

void GraphSplattingInteractorComponent::configurationModified() {
  confModified = true;
  if (!configWidget->keepOriginalGraphImageInBackground()) {
    glWidget->redraw();
  } else {
    glWidget->draw();
  }
}

void GraphSplattingInteractorComponent::setupInteractor() {
  if (confModified) {
    const ColorScale &colorScale = configWidget->getColorScale();
    unsigned char *colorScaleTextureData = new unsigned char[colorScaleTextureSize * 4];
    for (unsigned int i = 0; i < colorScaleTextureSize; ++i) {
      Color color = colorScale.getColorAtPos(i / float(colorScaleTextureSize - 1));
      colorScaleTextureData[4 * i] = color.getR();
      colorScaleTextureData[4 * i + 1] = color.getG();
      colorScaleTextureData[4 * i + 2] = color.getB();
      colorScaleTextureData[4 * i + 3] = color.getA();
    }

    glEnable(GL_TEXTURE_1D);
    if (colorScaleTextureId == 0) {
      glGenTextures(1, &colorScaleTextureId);
    }
    glBindTexture(GL_TEXTURE_1D, colorScaleTextureId);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, colorScaleTextureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 colorScaleTextureData);
    glDisable(GL_TEXTURE_1D);

    delete[] colorScaleTextureData;

    if (configWidget->getSplattingRadius() != splattingRadius) {
      if (splattingFragmentShader) {
        delete splattingFragmentShader;
      }
      splattingFragmentShader = new GlShaderProgram();
      splattingFragmentShader->addShaderFromSourceCode(
          Fragment,
          generateGaussianKernelConvolutionFragmentShader(configWidget->getSplattingRadius()));
    }
    splattingRadius = configWidget->getSplattingRadius();
    min = FLT_MAX;
    max = -FLT_MAX;
  }
}

void GraphSplattingInteractorComponent::timerEvent(QTimerEvent *event) {
  canDraw = true;
  killTimer(event->timerId());
  view()->draw();
  GlGraph *graphComposite = glWidget->scene()->glGraph();
  graphComposite->inputData()->glVertexArrayManager()->activate(true);
}

GraphSplattingInteractor::GraphSplattingInteractor(const PluginContext *)
    : GLInteractorComposite(interactorIcon(InteractorType::GraphSplatting), "Graph Splatting"),
      configWidget(nullptr) {}

GraphSplattingInteractor::~GraphSplattingInteractor() {
  if (configWidget) {
    delete configWidget;
  }
}

void GraphSplattingInteractor::construct() {
  configWidget = new GraphSplattingInteractorConfigWidget;
  push_back(new MouseNKeysNavigator);
  push_back(new GraphSplattingInteractorComponent(configWidget));
}

PLUGIN(GraphSplattingInteractor)

}
