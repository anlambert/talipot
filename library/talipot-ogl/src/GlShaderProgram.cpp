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

#include <GL/glew.h>

#include <algorithm>
#include <memory>

#include <talipot/GlShaderProgram.h>
#include <talipot/OpenGlConfigManager.h>

using namespace std;

enum ObjectType { SHADER, PROGRAM };

static void getInfoLog(GLuint obj, ObjectType objectType, string &logStr) {
  GLint infoLogLength = 0;
  GLint charsWritten = 0;
  GLchar *infoLog;

  if (objectType == SHADER) {
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);
  } else {
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);
  }

  if (infoLogLength > 1) {
    infoLog = new GLchar[infoLogLength + 1];

    if (objectType == SHADER) {
      glGetShaderInfoLog(obj, infoLogLength, &charsWritten, infoLog);
    } else {
      glGetProgramInfoLog(obj, infoLogLength, &charsWritten, infoLog);
    }

    infoLog[infoLogLength] = '\0';
    logStr = infoLog;
    delete[] infoLog;
  }
}

static string readShaderSourceFile(const string &vertexShaderSourceFilePath) {
  unique_ptr<istream> ifs(tlp::getInputFileStream(vertexShaderSourceFilePath));

  string shaderCode;

  if (!ifs->good()) {
    tlp::warning() << "Error opening file : " << vertexShaderSourceFilePath << endl;
    return shaderCode;
  }

  ifs->seekg(0, ios::end);
  shaderCode.reserve(ifs->tellg());
  ifs->seekg(0, ios::beg);

  shaderCode.assign((std::istreambuf_iterator<char>(*ifs)), std::istreambuf_iterator<char>());
  return shaderCode;
}

namespace tlp {

GlShader::GlShader(ShaderType shaderType)
    : shaderType(shaderType), shaderObjectId(0), shaderCompiled(false), anonymousCreation(false) {
  if (shaderType == Vertex) {
    shaderObjectId = glCreateShader(GL_VERTEX_SHADER);
  } else if (shaderType == Fragment) {
    shaderObjectId = glCreateShader(GL_FRAGMENT_SHADER);
  }
}

GlShader::GlShader(GLenum inputPrimitiveType, GLenum outputPrimitiveType)
    : shaderType(Geometry), shaderObjectId(0), inputPrimitiveType(inputPrimitiveType),
      outputPrimitiveType(outputPrimitiveType), shaderCompiled(false), anonymousCreation(false) {
  shaderObjectId = glCreateShader(GL_GEOMETRY_SHADER_EXT);
}

GlShader::~GlShader() {
  if (shaderObjectId != 0) {
    glDeleteShader(shaderObjectId);
  }
}

void GlShader::compileFromSourceCode(const char *shaderSrc) {
  compileShaderObject(shaderSrc);
}

void GlShader::compileFromSourceCode(const std::string &shaderSrc) {
  compileShaderObject(shaderSrc.c_str());
}

void GlShader::compileFromSourceFile(const std::string &shaderSrcFilename) {
  string shaderSrcCode = readShaderSourceFile(shaderSrcFilename);

  if (!shaderSrcCode.empty()) {
    compileShaderObject(shaderSrcCode.c_str());
  }
}

void GlShader::compileShaderObject(const char *shaderSrc) {
  glShaderSource(shaderObjectId, 1, &shaderSrc, nullptr);
  glCompileShader(shaderObjectId);
  GLint compileStatus;
  glGetShaderiv(shaderObjectId, GL_COMPILE_STATUS, &compileStatus);
  shaderCompiled = compileStatus > 0;
  getInfoLog(shaderObjectId, SHADER, compilationLog);
}

GlShaderProgram *GlShaderProgram::currentActiveShaderProgram(nullptr);

GlShaderProgram::GlShaderProgram(const std::string &name)
    : programName(name), programObjectId(0), programLinked(false),
      maxGeometryShaderOutputVertices(0) {
  programObjectId = glCreateProgram();
}

GlShaderProgram::~GlShaderProgram() {
  removeAllShaders();
  glDeleteProgram(programObjectId);
}

void GlShaderProgram::addShaderFromSourceCode(const ShaderType shaderType, const char *shaderSrc) {
  auto *shader = new GlShader(shaderType);
  shader->setAnonymousCreation(true);
  shader->compileFromSourceCode(shaderSrc);
  addShader(shader);
}

void GlShaderProgram::addShaderFromSourceCode(const ShaderType shaderType,
                                              const std::string &shaderSrc) {
  auto *shader = new GlShader(shaderType);
  shader->setAnonymousCreation(true);
  shader->compileFromSourceCode(shaderSrc);
  addShader(shader);
}

void GlShaderProgram::addShaderFromSourceFile(const ShaderType shaderType,
                                              const std::string &shaderSrcFilename) {
  auto *shader = new GlShader(shaderType);
  shader->setAnonymousCreation(true);
  shader->compileFromSourceFile(shaderSrcFilename);
  addShader(shader);
}

void GlShaderProgram::addGeometryShaderFromSourceCode(const char *geometryShaderSrc,
                                                      GLenum inputPrimitiveType,
                                                      GLenum outputPrimitiveType) {
  auto *shader = new GlShader(inputPrimitiveType, outputPrimitiveType);
  shader->setAnonymousCreation(true);
  shader->compileFromSourceCode(geometryShaderSrc);
  addShader(shader);
}

void GlShaderProgram::addGeometryShaderFromSourceCode(const std::string &geometryShaderSrc,
                                                      GLenum inputPrimitiveType,
                                                      GLenum outputPrimitiveType) {
  auto *shader = new GlShader(inputPrimitiveType, outputPrimitiveType);
  shader->setAnonymousCreation(true);
  shader->compileFromSourceCode(geometryShaderSrc);
  addShader(shader);
}

void GlShaderProgram::addGeometryShaderFromSourceFile(const std::string &geometryShaderSrcFilename,
                                                      GLenum inputPrimitiveType,
                                                      GLenum outputPrimitiveType) {
  auto *shader = new GlShader(inputPrimitiveType, outputPrimitiveType);
  shader->setAnonymousCreation(true);
  shader->compileFromSourceFile(geometryShaderSrcFilename);
  addShader(shader);
}

void GlShaderProgram::addShader(GlShader *shader) {
  if (std::find(attachedShaders.begin(), attachedShaders.end(), shader) == attachedShaders.end()) {
    if (shader->isCompiled()) {
      glAttachShader(programObjectId, shader->getShaderId());
    }

    attachedShaders.push_back(shader);
    programLinked = false;
  }
}

void GlShaderProgram::removeShader(GlShader *shader) {
  if (std::find(attachedShaders.begin(), attachedShaders.end(), shader) != attachedShaders.end()) {
    if (shader->isCompiled()) {
      glDetachShader(programObjectId, shader->getShaderId());
    }

    attachedShaders.erase(remove(attachedShaders.begin(), attachedShaders.end(), shader),
                          attachedShaders.end());
    programLinked = false;
  }
}

void GlShaderProgram::removeAllShaders() {
  for (auto *attachedShader : attachedShaders) {
    removeShader(attachedShader);

    if (attachedShader->anonymouslyCreated()) {
      delete attachedShader;
    }
  }
}

void GlShaderProgram::link() {
  bool allShaderCompiled = true;

  for (auto *attachedShader : attachedShaders) {
    if (!attachedShader->isCompiled()) {
      allShaderCompiled = false;
    }

    if (attachedShader->getShaderType() == Geometry) {
      glProgramParameteriEXT(programObjectId, GL_GEOMETRY_INPUT_TYPE_EXT,
                             attachedShader->getInputPrimitiveType());
      glProgramParameteriEXT(programObjectId, GL_GEOMETRY_OUTPUT_TYPE_EXT,
                             attachedShader->getOutputPrimitiveType());

      GLint maxOutputVertices = maxGeometryShaderOutputVertices;

      if (maxOutputVertices == 0) {
        glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, &maxOutputVertices);
      }

      glProgramParameteriEXT(programObjectId, GL_GEOMETRY_VERTICES_OUT_EXT, maxOutputVertices);
    }
  }

  glLinkProgram(programObjectId);

  getInfoLog(programObjectId, PROGRAM, programLinkLog);

  GLint linked;
  glGetProgramiv(programObjectId, GL_LINK_STATUS, &linked);
  programLinked = allShaderCompiled && linked > 0;
}

void GlShaderProgram::printInfoLog() {
  for (auto *attachedShader : attachedShaders) {
    string shaderCompilationlog = attachedShader->getCompilationLog();

    if (!shaderCompilationlog.empty()) {
      tlp::debug() << shaderCompilationlog << endl;
    }
  }

  if (!programLinkLog.empty()) {
    tlp::debug() << programLinkLog << endl;
  }
}

void GlShaderProgram::activate() {
  if (!programLinked) {
    link();
  }

  if (programLinked) {
    glUseProgram(programObjectId);
    currentActiveShaderProgram = this;
  }
}

void GlShaderProgram::deactivate() {
  glUseProgram(0);
  currentActiveShaderProgram = nullptr;
}

bool GlShaderProgram::shaderProgramsSupported() {

  static bool vertexShaderExtOk = OpenGlConfigManager::isExtensionSupported("GL_ARB_vertex_shader");
  static bool fragmentShaderExtOk =
      OpenGlConfigManager::isExtensionSupported("GL_ARB_fragment_shader");
  return (vertexShaderExtOk && fragmentShaderExtOk);
}

bool GlShaderProgram::geometryShaderSupported() {
  static bool geometryShaderExtOk =
      OpenGlConfigManager::isExtensionSupported("GL_EXT_geometry_shader4");
  return geometryShaderExtOk;
}

GlShaderProgram *GlShaderProgram::getCurrentActiveShader() {
  return currentActiveShaderProgram;
}

GLint GlShaderProgram::getUniformVariableLocation(const std::string &variableName) const {
  return glGetUniformLocation(programObjectId, variableName.c_str());
}

void GlShaderProgram::setUniformFloat(const std::string &variableName, const float f) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform1f(loc, f);
}

void GlShaderProgram::setUniformVec2Float(const std::string &variableName,
                                          const Vector<float, 2> &vec2f) {
  setUniformVec2FloatArray(variableName, 1, reinterpret_cast<const float *>(&vec2f));
}

void GlShaderProgram::setUniformVec2Float(const std::string &variableName, const float f1,
                                          const float f2) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform2f(loc, f1, f2);
}

void GlShaderProgram::setUniformVec3Float(const std::string &variableName, const Vec3f &vec3f) {
  setUniformVec3FloatArray(variableName, 1, reinterpret_cast<const float *>(&vec3f));
}

void GlShaderProgram::setUniformVec3Float(const std::string &variableName, const float f1,
                                          const float f2, const float f3) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform3f(loc, f1, f2, f3);
}

void GlShaderProgram::setUniformVec4Float(const std::string &variableName, const Vec4f &vec4f) {
  setUniformVec4FloatArray(variableName, 1, reinterpret_cast<const float *>(&vec4f));
}

void GlShaderProgram::setUniformVec4Float(const std::string &variableName, const float f1,
                                          const float f2, const float f3, const float f4) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform4f(loc, f1, f2, f3, f4);
}

template <size_t SIZE>
float *getMatrixData(const Matrix<float, SIZE> &matrix) {
  auto *matrixData = new float[SIZE * SIZE];

  for (size_t i = 0; i < SIZE; ++i) {
    for (size_t j = 0; j < SIZE; ++j) {
      matrixData[i * SIZE + j] = matrix[i][j];
    }
  }

  return matrixData;
}

void GlShaderProgram::setUniformMat2Float(const std::string &variableName,
                                          const Matrix<float, 2> &mat2f, const bool transpose) {
  float *matrixData = getMatrixData(mat2f);
  setUniformMat2Float(variableName, matrixData, transpose);
  delete[] matrixData;
}

void GlShaderProgram::setUniformMat2Float(const std::string &variableName, const float *f,
                                          const bool transpose) {
  setUniformMat2FloatArray(variableName, 1, f, transpose);
}

void GlShaderProgram::setUniformMat3Float(const std::string &variableName,
                                          const Matrix<float, 3> &mat3f, const bool transpose) {
  float *matrixData = getMatrixData(mat3f);
  setUniformMat3Float(variableName, matrixData, transpose);
  delete[] matrixData;
}

void GlShaderProgram::setUniformMat3Float(const std::string &variableName, const float *f,
                                          const bool transpose) {
  setUniformMat3FloatArray(variableName, 1, f, transpose);
}

void GlShaderProgram::setUniformMat4Float(const std::string &variableName,
                                          const Matrix<float, 4> &mat4f, const bool transpose) {
  float *matrixData = getMatrixData(mat4f);
  setUniformMat4Float(variableName, matrixData, transpose);
  delete[] matrixData;
}

void GlShaderProgram::setUniformMat4Float(const std::string &variableName, const float *f,
                                          const bool transpose) {
  setUniformMat4FloatArray(variableName, 1, f, transpose);
}

void GlShaderProgram::setUniformInt(const std::string &variableName, const int i) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform1i(loc, i);
}

void GlShaderProgram::setUniformVec2Int(const std::string &variableName, const Vec2i &vec2i) {
  setUniformVec2IntArray(variableName, 1, reinterpret_cast<const int *>(&vec2i));
}

void GlShaderProgram::setUniformVec2Int(const std::string &variableName, const int i1,
                                        const int i2) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform2i(loc, i1, i2);
}

void GlShaderProgram::setUniformVec3Int(const std::string &variableName,
                                        const Vector<int, 3> &vec3i) {
  setUniformVec3IntArray(variableName, 1, reinterpret_cast<const int *>(&vec3i));
}

void GlShaderProgram::setUniformVec3Int(const std::string &variableName, const int i1, const int i2,
                                        const int i3) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform3i(loc, i1, i2, i3);
}

void GlShaderProgram::setUniformVec4Int(const std::string &variableName, const Vec4i &vec4i) {
  setUniformVec4IntArray(variableName, 1, reinterpret_cast<const int *>(&vec4i));
}

void GlShaderProgram::setUniformVec4Int(const std::string &variableName, const int i1, const int i2,
                                        const int i3, const int i4) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform4i(loc, i1, i2, i3, i4);
}

void GlShaderProgram::setUniformBool(const std::string &variableName, const bool b) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform1i(loc, b);
}

void GlShaderProgram::setUniformVec2Bool(const std::string &variableName,
                                         const Array<bool, 2> &vec2b) {
  setUniformVec2IntArray(variableName, 1, reinterpret_cast<const int *>(&vec2b));
}

void GlShaderProgram::setUniformVec2Bool(const std::string &variableName, const bool b1,
                                         const bool b2) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform2i(loc, b1, b2);
}

void GlShaderProgram::setUniformVec3Bool(const std::string &variableName,
                                         const Array<bool, 3> &vec3b) {
  setUniformVec3IntArray(variableName, 1, reinterpret_cast<const int *>(&vec3b));
}

void GlShaderProgram::setUniformVec3Bool(const std::string &variableName, const bool b1,
                                         const bool b2, const bool b3) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform3i(loc, b1, b2, b3);
}

void GlShaderProgram::setUniformVec4Bool(const std::string &variableName,
                                         const Array<bool, 4> &vec4b) {
  setUniformVec4IntArray(variableName, 1, reinterpret_cast<const int *>(&vec4b));
}

void GlShaderProgram::setUniformVec4Bool(const std::string &variableName, const bool b1,
                                         const bool b2, const bool b3, const bool b4) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform4i(loc, b1, b2, b3, b4);
}

GLint GlShaderProgram::getAttributeVariableLocation(const std::string &variableName) const {
  return glGetAttribLocation(programObjectId, variableName.c_str());
}

void GlShaderProgram::setAttributeFloat(const std::string &variableName, const float f) {
  GLint loc = getAttributeVariableLocation(variableName);
  glVertexAttrib1f(loc, f);
}

void GlShaderProgram::setAttributeVec2Float(const std::string &variableName,
                                            const Vector<float, 2> &vec2f) {
  setAttributeVec2Float(variableName, vec2f[0], vec2f[1]);
}

void GlShaderProgram::setAttributeVec2Float(const std::string &variableName, const float f1,
                                            const float f2) {
  GLint loc = getAttributeVariableLocation(variableName);
  glVertexAttrib2f(loc, f1, f2);
}

void GlShaderProgram::setAttributeVec3Float(const std::string &variableName, const Vec3f &vec3f) {
  setAttributeVec3Float(variableName, vec3f[0], vec3f[1], vec3f[2]);
}

void GlShaderProgram::setAttributeVec3Float(const std::string &variableName, const float f1,
                                            const float f2, const float f3) {
  GLint loc = getAttributeVariableLocation(variableName);
  glVertexAttrib3f(loc, f1, f2, f3);
}

void GlShaderProgram::setAttributeVec4Float(const std::string &variableName, const Vec4f &vec4f) {
  setAttributeVec4Float(variableName, vec4f[0], vec4f[1], vec4f[2], vec4f[3]);
}

void GlShaderProgram::setAttributeVec4Float(const std::string &variableName, const float f1,
                                            const float f2, const float f3, const float f4) {
  GLint loc = getAttributeVariableLocation(variableName);
  glVertexAttrib4f(loc, f1, f2, f3, f4);
}

void GlShaderProgram::setAttributeInt(const std::string &variableName, const int i) {
  GLint loc = getAttributeVariableLocation(variableName);
  glVertexAttrib1s(loc, i);
}

void GlShaderProgram::setAttributeVec2Int(const std::string &variableName, const Vec2i &vec2i) {
  setAttributeVec2Int(variableName, vec2i[0], vec2i[1]);
}

void GlShaderProgram::setAttributeVec2Int(const std::string &variableName, const int i1,
                                          const int i2) {
  GLint loc = getAttributeVariableLocation(variableName);
  glVertexAttrib2s(loc, i1, i2);
}

void GlShaderProgram::setAttributeVec3Int(const std::string &variableName,
                                          const Vector<int, 3> &vec3i) {
  setAttributeVec3Int(variableName, vec3i[0], vec3i[1], vec3i[2]);
}

void GlShaderProgram::setAttributeVec3Int(const std::string &variableName, const int i1,
                                          const int i2, const int i3) {
  GLint loc = getAttributeVariableLocation(variableName);
  glVertexAttrib3s(loc, i1, i2, i3);
}

void GlShaderProgram::setAttributeVec4Int(const std::string &variableName, const Vec4i &vec4i) {
  setAttributeVec4Int(variableName, vec4i[0], vec4i[1], vec4i[2], vec4i[3]);
}

void GlShaderProgram::setAttributeVec4Int(const std::string &variableName, const int i1,
                                          const int i2, const int i3, const int i4) {
  GLint loc = getAttributeVariableLocation(variableName);
  glVertexAttrib4s(loc, i1, i2, i3, i4);
}

void GlShaderProgram::setAttributeBool(const std::string &variableName, const bool b) {
  setAttributeInt(variableName, b);
}

void GlShaderProgram::setAttributeVec2Bool(const std::string &variableName,
                                           const Array<bool, 2> &vec2b) {
  setAttributeVec2Bool(variableName, vec2b[0], vec2b[1]);
}

void GlShaderProgram::setAttributeVec2Bool(const std::string &variableName, const bool b1,
                                           const bool b2) {
  setAttributeVec2Int(variableName, b1, b2);
}

void GlShaderProgram::setAttributeVec3Bool(const std::string &variableName,
                                           const Array<bool, 3> &vec3b) {
  setAttributeVec3Bool(variableName, vec3b[0], vec3b[1], vec3b[2]);
}

void GlShaderProgram::setAttributeVec3Bool(const std::string &variableName, const bool b1,
                                           const bool b2, const bool b3) {
  setAttributeVec3Int(variableName, b1, b2, b3);
}

void GlShaderProgram::setAttributeVec4Bool(const std::string &variableName,
                                           const Array<bool, 4> &vec4b) {
  setAttributeVec4Bool(variableName, vec4b[0], vec4b[1], vec4b[2], vec4b[3]);
}

void GlShaderProgram::setAttributeVec4Bool(const std::string &variableName, const bool b1,
                                           const bool b2, const bool b3, const bool b4) {
  setAttributeVec4Int(variableName, b1, b2, b3, b4);
}

void GlShaderProgram::setUniformTextureSampler(const std::string &samplerVariableName,
                                               const int samplerId) {
  setUniformInt(samplerVariableName, samplerId);
}

void GlShaderProgram::setUniformColor(const std::string &variableName, const Color &color) {
  float *glColor = color.getGL();
  setUniformVec4Float(variableName, glColor[0], glColor[1], glColor[2], glColor[3]);
  delete[] glColor;
}

void GlShaderProgram::setAttributeColor(const std::string &variableName, const Color &color) {
  float *glColor = color.getGL();
  setAttributeVec4Float(variableName, glColor[0], glColor[1], glColor[2], glColor[3]);
  delete[] glColor;
}

template <uint SIZE>
void GlShaderProgram::setUniformFloatArray(const std::string &variableName,
                                           const Vector<float, SIZE> &vecf) {
  setUniformFloatArray(variableName, SIZE, reinterpret_cast<const float *>(vecf));
}

void GlShaderProgram::setUniformFloatArray(const std::string &variableName, const uint fCount,
                                           const float *f) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform1fv(loc, fCount, f);
}

template <typename T, uint SIZE, uint SIZE2>
T *getVectorOfVectorData(const Array<Vector<T, SIZE>, SIZE2> &vv) {
  T *vvData = new T[SIZE * SIZE2];

  for (uint i = 0; i < SIZE2; ++i) {
    for (uint j = 0; j < SIZE; ++j) {
      vvData[i * SIZE + j] = vv[i][j];
    }
  }

  return vvData;
}

template <uint SIZE>
void GlShaderProgram::setUniformVec2FloatArray(const std::string &variableName,
                                               const Array<Vector<float, 2>, SIZE> &vecvec2f) {
  float *vvData = getVectorOfVectorData(vecvec2f);
  setUniformVec2FloatArray(variableName, SIZE, vvData);
  delete[] vvData;
}

void GlShaderProgram::setUniformVec2FloatArray(const std::string &variableName,
                                               const uint vec2fCount, const float *f) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform2fv(loc, vec2fCount, f);
}

template <uint SIZE>
void GlShaderProgram::setUniformVec3FloatArray(const std::string &variableName,
                                               const Array<Vec3f, SIZE> &vecvec3f) {
  float *vvData = getVectorOfVectorData(vecvec3f);
  setUniformVec3FloatArray(variableName, SIZE, vvData);
  delete[] vvData;
}

void GlShaderProgram::setUniformVec3FloatArray(const std::string &variableName,
                                               const uint vec3fCount, const float *f) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform3fv(loc, vec3fCount, f);
}

template <uint SIZE>
void GlShaderProgram::setUniformVec4FloatArray(const std::string &variableName,
                                               const Array<Vec4f, SIZE> &vecvec4f) {
  float *vvData = getVectorOfVectorData(vecvec4f);
  setUniformVec4FloatArray(variableName, SIZE, vvData);
  delete[] vvData;
}

void GlShaderProgram::setUniformVec4FloatArray(const std::string &variableName,
                                               const uint vec4fCount, const float *f) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform4fv(loc, vec4fCount, f);
}

template <uint SIZE, uint SIZE2>
float *getVectorOfMatrixData(const Vector<Matrix<float, SIZE>, SIZE2> &vm) {
  auto *vmData = new float[SIZE * SIZE * SIZE2];

  for (uint i = 0; i < SIZE2; ++i) {
    for (uint j = 0; j < SIZE; ++j) {
      for (uint k = 0; k < SIZE; ++k) {
        vmData[i * (SIZE * SIZE) + j * SIZE + k] = vm[i][j][k];
      }
    }
  }

  return vmData;
}

template <uint SIZE>
void GlShaderProgram::setUniformMat2FloatArray(const std::string &variableName,
                                               const Vector<Matrix<float, 2>, SIZE> &vecmat2f,
                                               const bool transpose) {
  float *vmData = getVectorOfMatrixData(vecmat2f);
  setUniformMat2FloatArray(variableName, SIZE, vmData, transpose);
  delete[] vmData;
}

void GlShaderProgram::setUniformMat2FloatArray(const std::string &variableName,
                                               const uint mat2fCount, const float *f,
                                               const bool transpose) {
  GLint loc = getUniformVariableLocation(variableName);
  GLboolean transposeGL = transpose ? GL_TRUE : GL_FALSE;
  glUniformMatrix2fv(loc, mat2fCount, transposeGL, f);
}

template <uint SIZE>
void GlShaderProgram::setUniformMat3FloatArray(const std::string &variableName,
                                               const Vector<Matrix<float, 3>, SIZE> &vecmat3f,
                                               const bool transpose) {
  float *vmData = getVectorOfMatrixData(vecmat3f);
  setUniformMat3FloatArray(variableName, SIZE, vmData, transpose);
  delete[] vmData;
}

void GlShaderProgram::setUniformMat3FloatArray(const std::string &variableName,
                                               const uint mat3fCount, const float *f,
                                               const bool transpose) {
  GLint loc = getUniformVariableLocation(variableName);
  GLboolean transposeGL = transpose ? GL_TRUE : GL_FALSE;
  glUniformMatrix3fv(loc, mat3fCount, transposeGL, f);
}

template <uint SIZE>
void GlShaderProgram::setUniformMat4FloatArray(const std::string &variableName,
                                               const Vector<Matrix<float, 4>, SIZE> &vecmat4f,
                                               const bool transpose) {
  float *vmData = getVectorOfMatrixData(vecmat4f);
  setUniformMat4FloatArray(variableName, SIZE, vmData, transpose);
  delete[] vmData;
}

void GlShaderProgram::setUniformMat4FloatArray(const std::string &variableName,
                                               const uint mat4fCount, const float *f,
                                               const bool transpose) {
  GLint loc = getUniformVariableLocation(variableName);
  GLboolean transposeGL = transpose ? GL_TRUE : GL_FALSE;
  glUniformMatrix4fv(loc, mat4fCount, transposeGL, f);
}

template <uint SIZE>
void GlShaderProgram::setUniformIntArray(const std::string &variableName,
                                         const Vector<int, SIZE> &veci) {
  setUniformIntArray(variableName, SIZE, reinterpret_cast<const int *>(&veci));
}

void GlShaderProgram::setUniformIntArray(const std::string &variableName, const uint iCount,
                                         const int *i) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform1iv(loc, iCount, static_cast<const GLint *>(i));
}

template <uint SIZE>
void GlShaderProgram::setUniformVec2IntArray(const std::string &variableName,
                                             const Array<Vec2i, SIZE> &vecvec2i) {
  int *vvData = getVectorOfVectorData(vecvec2i);
  setUniformVec2IntArray(variableName, SIZE, vvData);
  delete[] vvData;
}

void GlShaderProgram::setUniformVec2IntArray(const std::string &variableName, const uint vec2iCount,
                                             const int *i) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform2iv(loc, vec2iCount, static_cast<const GLint *>(i));
}

template <uint SIZE>
void GlShaderProgram::setUniformVec3IntArray(const std::string &variableName,
                                             const Array<Vector<int, 3>, SIZE> &vecvec3i) {
  int *vvData = getVectorOfVectorData(vecvec3i);
  setUniformVec3IntArray(variableName, SIZE, vvData);
  delete[] vvData;
}

void GlShaderProgram::setUniformVec3IntArray(const std::string &variableName, const uint vec3iCount,
                                             const int *i) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform3iv(loc, vec3iCount, static_cast<const GLint *>(i));
}

template <uint SIZE>
void GlShaderProgram::setUniformVec4IntArray(const std::string &variableName,
                                             const Array<Vec4i, SIZE> &vecvec4i) {
  int *vvData = getVectorOfVectorData(vecvec4i);
  setUniformVec4IntArray(variableName, SIZE, vvData);
  delete[] vvData;
}

void GlShaderProgram::setUniformVec4IntArray(const std::string &variableName, const uint vec4iCount,
                                             const int *i) {
  GLint loc = getUniformVariableLocation(variableName);
  glUniform4iv(loc, vec4iCount, static_cast<const GLint *>(i));
}

template <uint SIZE>
void GlShaderProgram::setUniformBoolArray(const std::string &variableName,
                                          const Array<bool, SIZE> &vecb) {
  setUniformIntArray(variableName, SIZE, reinterpret_cast<const int *>(&vecb));
}

void GlShaderProgram::setUniformBoolArray(const std::string &variableName, const uint bCount,
                                          const bool *b) {
  setUniformIntArray(variableName, bCount, reinterpret_cast<const int *>(b));
}

template <uint SIZE>
void GlShaderProgram::setUniformVec2BoolArray(const std::string &variableName,
                                              const Array<Array<bool, 2>, SIZE> &vecvec2b) {
  bool *vvData = getVectorOfVectorData(vecvec2b);
  setUniformVec2IntArray(variableName, SIZE, reinterpret_cast<const int *>(vvData));
  delete[] vvData;
}

void GlShaderProgram::setUniformVec2BoolArray(const std::string &variableName,
                                              const uint vec2bCount, const bool *b) {
  setUniformVec2IntArray(variableName, vec2bCount, reinterpret_cast<const int *>(b));
}

template <uint SIZE>
void GlShaderProgram::setUniformVec3BoolArray(const std::string &variableName,
                                              const Array<Array<bool, 3>, SIZE> &vecvec3b) {
  bool *vvData = getVectorOfVectorData(vecvec3b);
  setUniformVec3IntArray(variableName, SIZE, reinterpret_cast<const int *>(vvData));
  delete[] vvData;
}

void GlShaderProgram::setUniformVec3BoolArray(const std::string &variableName,
                                              const uint vec3bCount, const bool *b) {
  setUniformVec3IntArray(variableName, vec3bCount, reinterpret_cast<const int *>(b));
}

template <uint SIZE>
void GlShaderProgram::setUniformVec4BoolArray(const std::string &variableName,
                                              const Array<Array<bool, 4>, SIZE> &vecvec4b) {
  bool *vvData = getVectorOfVectorData(vecvec4b);
  setUniformVec4IntArray(variableName, SIZE, reinterpret_cast<const int *>(vvData));
  delete[] vvData;
}

void GlShaderProgram::setUniformVec4BoolArray(const std::string &variableName,
                                              const uint vec4bCount, const bool *b) {
  setUniformVec4IntArray(variableName, vec4bCount, reinterpret_cast<const int *>(b));
}

void GlShaderProgram::getUniformFloatVariableValue(const std::string &variableName, float *value) {
  GLint loc = getUniformVariableLocation(variableName);
  glGetUniformfv(programObjectId, loc, value);
}

void GlShaderProgram::getUniformIntVariableValue(const std::string &variableName, int *value) {
  GLint loc = getUniformVariableLocation(variableName);
  glGetUniformiv(programObjectId, loc, static_cast<GLint *>(value));
}

void GlShaderProgram::getUniformBoolVariableValue(const std::string &variableName, bool *value) {
  int valueInt;
  getUniformIntVariableValue(variableName, &valueInt);
  *value = (valueInt > 0);
}

void GlShaderProgram::getUniformVec2BoolVariableValue(const std::string &variableName,
                                                      bool *value) {
  int valueInt[2];
  getUniformIntVariableValue(variableName, valueInt);

  for (uint i = 0; i < 2; ++i) {
    value[i] = (valueInt[i] > 0);
  }
}

void GlShaderProgram::getUniformVec3BoolVariableValue(const std::string &variableName,
                                                      bool *value) {
  int valueInt[3];
  getUniformIntVariableValue(variableName, valueInt);

  for (uint i = 0; i < 3; ++i) {
    value[i] = (valueInt[i] > 0);
  }
}

void GlShaderProgram::getUniformVec4BoolVariableValue(const std::string &variableName,
                                                      bool *value) {
  int valueInt[4];
  getUniformIntVariableValue(variableName, valueInt);

  for (uint i = 0; i < 4; ++i) {
    value[i] = (valueInt[i] > 0);
  }
}

void GlShaderProgram::setMaxGeometryShaderOutputVertices(const int maxOutputVertices) {
  maxGeometryShaderOutputVertices = maxOutputVertices;
}

void GlShaderProgram::setVertexAttribPointer(const std::string &variableName, GLint size,
                                             GLenum type, GLboolean normalized, GLsizei stride,
                                             const GLvoid *pointer) {
  GLint attributeIndex = getAttributeVariableLocation(variableName);
  if (attributeIndex >= 0) {
    activeAttributesArrays.push_back(attributeIndex);
    glVertexAttribPointer(attributeIndex, size, type, normalized, stride, pointer);
    glEnableVertexAttribArray(attributeIndex);
  }
}

void GlShaderProgram::disableAttributesArrays() {
  for (int activeAttributesArray : activeAttributesArrays) {
    glDisableVertexAttribArray(activeAttributesArray);
  }
  activeAttributesArrays.clear();
}
}
