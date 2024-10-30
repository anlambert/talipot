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

#include <GL/glew.h>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include <stb_image_resize2.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include <talipot/GlTextureManager.h>
#include <talipot/OpenGlConfigManager.h>
#include <talipot/TlpTools.h>

//====================================================
tlp::GlTextureLoader *tlp::GlTextureManager::loader = nullptr;
tlp::GlTextureManager::TextureMap tlp::GlTextureManager::texturesMap;
std::set<std::string> tlp::GlTextureManager::texturesWithError;

using namespace std;

namespace tlp {

struct TextureInfo {
  uint width;
  uint height;
  unsigned char *data;
};

static bool generateTexture(const TextureInfo &texti, GlTexture &glTexture) {

  uint width = texti.width;
  uint height = texti.height;

  bool canUseMipmaps = OpenGlConfigManager::isExtensionSupported("GL_ARB_framebuffer_object") ||
                       OpenGlConfigManager::isExtensionSupported("GL_EXT_framebuffer_object");

  glTexture.width = width;
  glTexture.height = height;

  glGenTextures(1, &glTexture.id);

  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, glTexture.id);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texti.data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (canUseMipmaps) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }

  glDisable(GL_TEXTURE_2D);

  return true;
}
//====================================================================
static unsigned char *invertImage(int pitch, int height, unsigned char *imagePixels) {
  auto *tempRow = new unsigned char[pitch];
  auto heightDiv2 = static_cast<uint>(height * .5);
  for (uint index = 0; index < heightDiv2; ++index) {
    memcpy(tempRow, imagePixels + pitch * index, pitch);
    memcpy(imagePixels + pitch * index, imagePixels + pitch * (height - index - 1), pitch);
    memcpy(imagePixels + pitch * (height - index - 1), tempRow, pitch);
  }
  delete[] tempRow;
  return imagePixels;
}
//====================================================================
static uint nearestPOT(uint x) {
  return pow(2, ceil(log(x) / log(2)));
}
//====================================================================
bool GlTextureLoader::loadTexture(const string &filename, GlTexture &texture) {

  TextureInfo texti;

  if (!tlp::pathExists(filename)) {
    tlp::error() << "Image file " << filename << " does not exist." << std::endl;
    return false;
  }
  int w, h, n;
  static const uint nbBytesPerPixel = 4;
  unsigned char *pixels = stbi_load(filename.c_str(), &w, &h, &n, nbBytesPerPixel);
  if (pixels) {
    int nearestpotW = nearestPOT(w);
    int nearestpotH = nearestPOT(h);
    if (nearestpotW != w || nearestpotH != h) {
      auto *newPixels = new unsigned char[nearestpotW * nearestpotH * nbBytesPerPixel];
      stbir_resize_uint8_linear(pixels, w, h, 0, newPixels, nearestpotW, nearestpotH, 0,
                                static_cast<stbir_pixel_layout>(nbBytesPerPixel));
      delete[] pixels;
      pixels = newPixels;
      w = nearestpotW;
      h = nearestpotH;
    }
    texti.width = w;
    texti.height = h;
    texti.data = invertImage(w * nbBytesPerPixel, h, pixels);
  } else {
    tlp::error() << "Unable to load image file " << filename << std::endl;
    return false;
  }

  bool result = generateTexture(texti, texture);

  delete[] texti.data;

  return result;
}
//====================================================================
GlTexture GlTextureManager::getTextureInfo(const string &filename) {
  if (texturesMap.contains(filename)) {
    return (texturesMap)[filename];
  } else {
    return GlTexture();
  }
}
//====================================================================
bool GlTextureManager::existsTexture(const string &filename) {
  return (texturesMap.contains(filename));
}
//====================================================================
bool GlTextureManager::loadTexture(const string &filename) {
  glEnable(GL_TEXTURE_2D);

  if (texturesMap.contains(filename)) {
    return true;
  }

  GlTexture texture;

  if (!getTextureLoader()->loadTexture(filename, texture)) {
    return false;
  }

  (texturesMap)[filename] = texture;
  return true;
}

void GlTextureManager::registerExternalTexture(const std::string &textureName,
                                               const GLuint textureId) {
  GlTexture texture;
  texture.id = textureId;
  (texturesMap)[textureName] = texture;
}

//====================================================================
static void deleteGlTexture(const GlTexture &texture) {
  glDeleteTextures(1, &texture.id);
}

void GlTextureManager::deleteTexture(const string &name) {
  auto it = texturesMap.find(name);
  if (it != texturesMap.end()) {
    deleteGlTexture(it->second);
    texturesMap.erase(it);
  }
}
//====================================================================
void GlTextureManager::beginNewTexture(const string &) {
  GLuint textureNum;
  glGenTextures(1, &textureNum);
  glBindTexture(GL_TEXTURE_2D, textureNum);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}
//====================================================================
bool GlTextureManager::activateTexture(const string &filename, int textureUnit) {
  if (texturesWithError.count(filename) != 0) {
    return false;
  }

  bool loadOk = true;

  glActiveTexture(GL_TEXTURE0 + textureUnit);

  if (!texturesMap.contains(filename)) {
    loadOk = loadTexture(filename);
  } else {
    glEnable(GL_TEXTURE_2D);
  }

  if (!loadOk) {
    texturesWithError.insert(filename);
    glDisable(GL_TEXTURE_2D);
    return false;
  }

  glBindTexture(GL_TEXTURE_2D, (texturesMap)[filename].id);
  return true;
}
//====================================================================
void GlTextureManager::deactivateTexture(int textureUnit) {
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
}
//====================================================================
void GlTextureManager::deleteAllTextures() {
  for (const auto &[name, texture] : texturesMap) {
    deleteGlTexture(texture);
  }
}

}
