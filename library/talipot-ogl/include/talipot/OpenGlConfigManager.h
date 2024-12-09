/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_OPEN_GL_CONFIG_MANAGER_H
#define TALIPOT_OPEN_GL_CONFIG_MANAGER_H

#include <talipot/config.h>

#include <string>
#include <talipot/hash.h>

#define BUFFER_OFFSET(bytes) (reinterpret_cast<GLubyte *>(bytes))

namespace tlp {

/**
 *
 * Used to manage OpenGl configuration
 */
class TLP_GL_SCOPE OpenGlConfigManager {

public:
  /**
   * Returns the OpenGL version number supported by the host system as a string.
   *
   */
  static std::string getOpenGLVersionString();

  /**
   * Returns the OpenGL version number supported by the host system as a number.
   */
  static double getOpenGLVersion();

  /**
   * Return the vendor name of the OpenGL driver installed on the host system.
   */
  static std::string getOpenGLVendor();

  static void initExtensions();

  /**
   * Checks if an OpenGL extension is supported by the driver installed on the host system.
   * \param extensionName the name of the OpenGL extension to check in the form "GL_.*" (for
   * instance "GL_ARB_vertex_buffer_object")
   */
  static bool isExtensionSupported(const std::string &extensionName);

  /**
   * Returns if vertex buffer objects can be used on the host system.
   */
  static bool hasVertexBufferObject();

  /**
   * Enables / disables anti-aliasing rendering.
   */
  static void setAntiAliasing(const bool antialiasing) {
    _antialiased = antialiasing;
  }

  /**
   * Returns the anti-aliasing state
   */
  static bool antiAliasing() {
    return _antialiased;
  }

  /**
   * Activates anti-aliasing
   * This method has no effect if anti-aliasing has been disabled by a call to
   * setAntiAliasing(false).
   */
  static void activateAntiAliasing();

  /**
   * deactivates anti-aliasing
   * This method has no effect if anti-aliasing has been disabled by a call to
   * setAntiAliasing(false).
   */
  static void deactivateAntiAliasing();

  /**
   * Returns a maximum number of samples for anti-aliasing based on graphics hardware capability
   *
   */
  static int maxNumberOfSamples();

private:
  static bool _glewIsInit;
  static bool _antialiased;
  static flat_hash_map<std::string, bool> _checkedExtensions;
};
}

#endif // TALIPOT_OPEN_GL_CONFIG_MANAGER_H
