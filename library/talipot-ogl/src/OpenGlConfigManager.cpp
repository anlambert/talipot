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

#include <GL/glew.h>

#include <talipot/OpenGlConfigManager.h>
#include <talipot/ParallelTools.h>

#include <sstream>

//====================================================

using namespace std;

#ifndef NDEBUG
static void glDebugOutput(GLenum source, GLenum type, GLuint, GLenum severity, GLsizei,
                          const GLchar *message, const void *) {

  string sourceStr = "Unknown";

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    sourceStr = "API";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    sourceStr = "Window System";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    sourceStr = "Shader Compiler";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    sourceStr = "Third Party";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    sourceStr = "Application";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    sourceStr = "Other";
    break;
  }

  string typeStr;

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    typeStr = "Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    typeStr = "Deprecated Behaviour";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    typeStr = "Undefined Behaviour";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    typeStr = "Portability";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    typeStr = "Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    typeStr = "Marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    typeStr = "Push Group";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    typeStr = "Pop Group";
    break;
  case GL_DEBUG_TYPE_OTHER:
    typeStr = "Other";
    break;
  }

  string severityStr;

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    severityStr = "High";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    severityStr = "Medium";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    severityStr = "Low";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    severityStr = "Notification";
    break;
  }

  tlp::debug() << "[OPENGL DEBUG] "
               << "Type: " << typeStr << ", Severity: " << severityStr << ", Source: " << sourceStr
               << ", Message: " << message << std::endl;
}
#endif

namespace tlp {

bool OpenGlConfigManager::_glewIsInit = false;
bool OpenGlConfigManager::_antialiased = true;
flat_hash_map<std::string, bool> OpenGlConfigManager::_checkedExtensions;

void OpenGlConfigManager::initExtensions() {
  if (!_glewIsInit) {
    glewExperimental = true;
    _glewIsInit = (glewInit() == GLEW_OK);
#ifndef NDEBUG
    if (_glewIsInit) {
      bool canUseDebugOutput = isExtensionSupported("GL_ARB_debug_output") ||
                               isExtensionSupported("GL_AMD_debug_output");
      if (canUseDebugOutput) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr,
                              GL_TRUE);
      }
    }
#endif
  }
}

string OpenGlConfigManager::getOpenGLVersionString() {
  return reinterpret_cast<const char *>(glGetString(GL_VERSION));
}

double OpenGlConfigManager::getOpenGLVersion() {
  double ret = 0;
  std::istringstream iss(getOpenGLVersionString());
  iss >> ret;
  return ret;
}

string OpenGlConfigManager::getOpenGLVendor() {
  return string(reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
}

bool OpenGlConfigManager::isExtensionSupported(const string &extensionName) {
  if (!_glewIsInit) {
    return false;
  }

  bool supported = false;
  TLP_LOCK_SECTION(OpenGlConfigManagerExtensionSupported) {
    auto it = _checkedExtensions.find(extensionName);
    if (it == _checkedExtensions.end()) {
      supported = _checkedExtensions[extensionName] =
          (glewIsSupported(extensionName.c_str()) == GL_TRUE &&
           glewGetExtension(extensionName.c_str()) == GL_TRUE);
    } else {
      supported = it->second;
    }
  }
  TLP_UNLOCK_SECTION(OpenGlConfigManagerExtensionSupported);
  return supported;
}

bool OpenGlConfigManager::hasVertexBufferObject() {
  return isExtensionSupported("GL_ARB_vertex_buffer_object");
}

void OpenGlConfigManager::activateAntiAliasing() {
  if (_antialiased) {
    glEnable(GL_MULTISAMPLE);
  }
}

void OpenGlConfigManager::deactivateAntiAliasing() {
  glDisable(GL_MULTISAMPLE);
}

int OpenGlConfigManager::maxNumberOfSamples() {
  static int maxSamples = -1;

  if (maxSamples < 0) {
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    maxSamples = std::min(maxSamples, 8);
  }

  return maxSamples;
}
}
