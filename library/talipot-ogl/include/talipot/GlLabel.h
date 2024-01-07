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

#ifndef TALIPOT_GL_LABEL_H
#define TALIPOT_GL_LABEL_H

#include <talipot/Coord.h>
#include <talipot/Color.h>
#include <talipot/Size.h>
#include <talipot/Camera.h>
#include <talipot/GlEntity.h>
#include <talipot/Font.h>

class FTPolygonFont;
class FTOutlineFont;

namespace tlp {

struct OcclusionTest;

/**
 * @ingroup OpenGL
 * @brief Create a label
 *
 * The shortest way to create a label is :
 * @code
 * GlLabel *label=new GlLabel(Coord(0,0,0), Size (1,1,1), Color(1,1,1));
 * label->setText("example");
 * @endcode
 */
class TLP_GL_SCOPE GlLabel final : public GlEntity {
  /**
   * @brief Set default parameters of GlLabel
   *
   */
  void init();
  void initFont();

public:
  /**
   * @brief Constructor
   * @warning Don't use this constructor : see other constructor
   */
  GlLabel();

  /**
   * @brief Constructor
   * @param centerPosition position of the label
   * @param size size of the label
   * @param fontColor color of the label
   * @param leftAlign true if you want a left align label
   */
  GlLabel(Coord centerPosition, Size size, Color fontColor, bool leftAlign = false);

  /**
   * @brief Destructor
   */
  ~GlLabel() override;

  /**
   * @brief Set the text of the label
   */
  void setText(const std::string &text);

  /**
   * @brief getText gets the text of this label.
   * @return The text of this label.
   */
  const std::string &getText() const {
    return text;
  }

  /**
   * @brief Set the position used to render the label
   */
  void setPosition(const Coord &position) {
    centerPosition = position;
  }

  /**
   * @brief Set the position used to render the label
   */
  void setPosition(float x, float y, float z) {
    centerPosition.set(x, y, z);
  }

  /**
   * @brief Return the position of the label
   */
  const Coord &getPosition() const {
    return centerPosition;
  }

  /**
   * @brief Set the translation used after rotation of the label
   */
  void setTranslationAfterRotation(const Coord &translation) {
    translationAfterRotation = translation;
  }

  /**
   * @brief return the translation used after rotation of the label
   */
  const Coord &getTranslationAfterRotation() const {
    return translationAfterRotation;
  }

  /**
   * @brief Set the alignment of the label : LabelPosition::Center, LabelPosition::Top,
   * LabelPosition::Bottom, LabelPosition::Left, LabelPosition::Right
   * This function is useful when you have an entity : you specify the size of the position of this
   * entity and you tell that you want a label outside this entity
   * @see LabelPosition
   * @see setSizeOfOutAlign
   */
  void setAlignment(int alignment) {
    this->alignment = alignment;
  }

  /**
   * @brief Return the bounding box of the label
   */
  BoundingBox getBoundingBox() override;

  /**
   * @brief Return the bounding box of the text of the label after transformations
   */
  const BoundingBox &getTextBoundingBox() {
    return textBoundingBox;
  }

  /**
   * @brief Set the size of the label
   */
  void setSize(const Size &size) {
    this->sizeForOutAlign = this->size = size;
  }

  /**
   * @brief return the size of the text
   */
  const Size &getSize() const {
    return size;
  }

  /**
   * @brief Set the size for alignment outside (left/right/top/bottom)
   *
   * You can have a specific size when you want a label outside
   * @see setAlignment
   *
   * @Warning : this size is reinit when you call setSize
   */
  void setSizeForOutAlign(const Size &size) {
    sizeForOutAlign = size;
  }

  /**
   * @brief return the size for alignment outside (left/right/top/bottom)
   * @see setAlignment
   */
  const Size &getSizeForOutAlign() const {
    return sizeForOutAlign;
  }

  /**
   * @brief Set color of label
   */
  void setColor(const Color &color) {
    this->color = color;
  }

  /**
   * @brief Get color use to render the label
   */
  const Color &getColor() const {
    return color;
  }

  /**
   * @brief Enable/disable the OpenGL depth test for the label (default depth test is enable)
   */
  void enableDepthTest(bool state) {
    depthTestEnabled = state;
  }

  /**
   * @brief Enable/disable if label is scaled to size
   */
  void setScaleToSize(bool state) {
    scaleToSize = state;
  }

  /**
   * @brief Set the stencil and draw the Label, this function is useful when we directly call draw
   * without talipot engine
   */
  void drawWithStencil(float lod, Camera *camera = nullptr);

  /**
   * @brief Return the height of the label after scaling in  size box
   */
  float getHeightAfterScale();

  /**
   * @brief Draw the Label
   */
  void draw(float, Camera *camera = nullptr) override;

  /**
   * @brief Translate entity
   */
  void translate(const Coord &move) override;

  /**
   * @brief Rotate Label
   */
  void rotate(float xRot, float yRot, float zRot);

  /**
   * @brief Function to export data in outString (in XML format)
   */
  void getXML(std::string &outString) override;

  /**
   * @brief Function to set data with inString (in XML format)
   */
  void setWithXML(const std::string &inString, uint &currentPosition) override;

  /**
   * @brief Change font name
   */
  void setFontName(const std::string &name);

  /**
   * @brief Change font name, size and color of the text
   */
  void setFontNameSizeAndColor(const std::string &name, const int &size, const Color &color);

  /**
   * @brief This parameters is not used
   */
  void setRenderingMode(int mode) {
    renderingMode = mode;
  }

  /**
   * @brief Set the occlusion tester
   * If occlusionTester is nullptr : deactivate occlusion test
   */
  void setOcclusionTester(OcclusionTest *tester) {
    occlusionTester = tester;
  }

  /**
   * @brief Set if the label is optimized with the lod
   */
  void setUseLODOptimisation(bool state, const BoundingBox &bb = BoundingBox()) {
    useLOD = state;
    lodBoundingBox = bb;
  }

  /**
   * @brief Return if the label is otimized with the lod
   */
  bool getUseLODOptimisation() const {
    return useLOD;
  }

  /**
   * @brief Set labels density of occlusion test
   * This density must be in interval -100 100
   *
   * - If density is equal to -100 : we don't have occlusion test
   * - If density is equal to 0 : GlLabels don't overlap
   * - If density > 0 : GlLabels don't overlap and have space wetween us
   */
  void setLabelsDensity(int density) {
    if (density < -100) {
      labelsDensity = -100;
    } else if (density > 100) {
      labelsDensity = 100;
    } else {
      labelsDensity = density;
    }
  }

  /**
   * @brief Return label density of occlusion test
   * This density will be in interval -100 100
   */
  int getLabelDensity() const {
    return labelsDensity;
  }

  /**
   * @brief Set min screen size (in pixel) of the label : this size is used in not scaled mode
   * @see setUseMinMaxSize
   */
  void setMinSize(int size) {
    minSize = size;
  }

  /**
   * @brief Get min screen size (in pixel) of the label : this size is used in not scaled mode
   * @see setUseMinMaxSize
   */
  int getMinSize() const {
    return minSize;
  }

  /**
   * @brief Set max screen size (in pixel) of the label : this size is used in not scaled mode
   * @see setUseMinMaxSize
   */
  void setMaxSize(int size) {
    maxSize = size;
  }

  /**
   * @brief Get max screen size (in pixel) of the label : this size is used in not scaled mode
   * @see setUseMinMaxSize
   */
  int getMaxSize() const {
    return maxSize;
  }

  /**
   * @brief Set if the label use min/max screen size in not scaled mode
   */
  void setUseMinMaxSize(bool state) {
    useMinMaxSize = state;
  }

  /**
   * @brief Return if the label using min/max screen size in not scaled mode
   */
  bool isUsingMinMaxSize() const {
    return useMinMaxSize;
  }

  /**
   * @brief Return the font size
   */
  int getFontSize() const {
    return fontSize;
  }

  /**
   * @brief Sets the font size used when rendering the label.
   */
  void setFontSize(int size) {
    fontSize = size;
  }

  /**
   * @brief Return the outline color
   */
  const Color &getOutlineColor() const {
    return outlineColor;
  }

  /**
   * @brief Sets the outline color used when rendering the label.
   */
  void setOutlineColor(const Color &color) {
    outlineColor = color;
  }

  /**
   * @brief Return the outline size
   */
  float getOutlineSize() const {
    return outlineSize;
  }

  /**
   * @brief Sets the outline size used when rendering the label.
   */
  void setOutlineSize(float size) {
    outlineSize = size;
  }

  /**
   * @brief Return the texture name used to render the label
   */
  const std::string &getTextureName() const {
    return textureName;
  }

  /**
   * @brief Sets the texture name used when rendering the label.
   */
  void setTextureName(const std::string &name) {
    textureName = name;
  }

  /**
   * @brief Return if the label is billboarded
   */
  bool isBillboarded() const {
    return billboarded;
  }

  /**
   * @brief Set if the label is billboarded
   */
  void setBillboarded(bool billboarded) {
    this->billboarded = billboarded;
  }

private:
  std::string text;
  Font font;
  int fontSize;
  int renderingMode;
  FTPolygonFont *ftglPolygonFont;
  FTOutlineFont *ftglOutlineFont;
  Coord centerPosition;
  Coord translationAfterRotation;
  Size size;
  Size sizeForOutAlign;
  Color color;
  Color outlineColor;
  float outlineSize;
  std::string textureName;
  int alignment;
  bool scaleToSize;
  bool useMinMaxSize;
  int minSize;
  int maxSize;
  bool depthTestEnabled;
  bool leftAlign;
  bool billboarded;
  float xRot;
  float yRot;
  float zRot;
  bool useLOD;
  BoundingBox lodBoundingBox;
  int labelsDensity;
  OcclusionTest *occlusionTester;

  Camera oldCamera;
  float oldLod;
  Vec4i oldViewport;

  std::vector<std::string> textVector;
  std::vector<float> textWidthVector;
  BoundingBox textBoundingBox;
};
}
#endif // TALIPOT_GL_LABEL_H
