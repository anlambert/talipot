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

#ifndef TALIPOT_GL_TEXTURE_MANAGER_H
#define TALIPOT_GL_TEXTURE_MANAGER_H

#include <talipot/config.h>

#include <talipot/OpenGlIncludes.h>

#include <set>
#include <talipot/hash.h>
#include <string>

namespace tlp {

struct GlTexture {
  GLuint id = 0;
  int height = 0;
  int width = 0;
};

/**
 *  \brief Class to load textures
 */
class TLP_GL_SCOPE GlTextureLoader {
public:
  /**
   * Load a texture from a file
   * in the default implementation only bmp, jpeg and png files
   * can be loaded.
   * Return false if an error occurs
   */
  virtual bool loadTexture(const std::string &filename, GlTexture &texture);

  virtual ~GlTextureLoader() = default;
};

/**
 * class to load/store textures need by OpenGL rendering
 */
class TLP_GL_SCOPE GlTextureManager {

  typedef flat_hash_map<std::string, GlTexture> TextureMap;

public:
  /**
   * Return texture info (id, width and height) for the given name
   */
  static GlTexture getTextureInfo(const std::string &);

  /**
   * Check if a texture for the given name exists in the current context
   */
  static bool existsTexture(const std::string &filename);
  /**
   * Load texture with given name
   */
  static bool loadTexture(const std::string &);
  /**
   * Remove texture with given name
   */
  static void deleteTexture(const std::string &);
  /**
   * Begin a new texture with given name
   */
  static void beginNewTexture(const std::string &);

  /**
   * Activate a texture with given name
   */
  static bool activateTexture(const std::string &, int textureUnit = 0);
  /**
   * Disable texture with given name
   */
  static void deactivateTexture(int textureUnit = 0);

  /**
   * Clear vector of textures with error
   */
  static void clearErrorVector() {
    texturesWithError.clear();
  }
  /**
   * Remove an entry of vector of textures with error
   */
  static void removeEntryOfErrorVector(const std::string &name) {
    texturesWithError.erase(name);
  }

  /**
   * Register an external texture is GlTextureManager
   */
  static void registerExternalTexture(const std::string &textureName, const GLuint textureId);

  /**
   * Get Texture loader
   */
  static GlTextureLoader *getTextureLoader() {
    return loader ? loader : (loader = new GlTextureLoader());
  }

  /**
   * Set Texture loader
   */
  static void setTextureLoader(GlTextureLoader *texLoader) {
    if (loader) {
      delete loader;
    }

    loader = texLoader;
  }

  static void deleteAllTextures();

private:
  static GlTextureLoader *loader;

  static TextureMap texturesMap;
  static std::set<std::string> texturesWithError;
};
}

#endif // TALIPOT_GL_TEXTURE_MANAGER_H
