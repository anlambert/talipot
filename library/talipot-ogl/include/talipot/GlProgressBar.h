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

#ifndef TALIPOT_GL_PROGRESS_BAR_H
#define TALIPOT_GL_PROGRESS_BAR_H

#include <talipot/GlComposite.h>
#include <talipot/SimplePluginProgress.h>
#include <talipot/Color.h>
#include <talipot/Coord.h>

const std::string SLIDER_TEXTURE_NAME = "cylinderTexture.png";

namespace tlp {
/**
 *  This class allows to display a progress bar in OpenGL
 */
class TLP_GL_SCOPE GlProgressBar : public GlComposite, public SimplePluginProgress {

  public:
    /**
     * GlProgressBar constructor
     * \param centerPosition the position of the center of the frame containing the progress bar
     * \param width the width of the frame containing the progress bar
     * \param height the height of the frame containing the progress bar
     * \param progressBarColor the color of the progress bar
     * \param commentColor the color of the comment and the frame
     */
    GlProgressBar(const Coord &centerPosition, const uint width, const uint height,
                  const Color &progressBarcolor, const Color &commentColor = Color(0, 0, 0));

    ~GlProgressBar() override;

    /**
     * This method allow to set a comment message above the progress bar
     * \param msg the text of the comment to display
     */
    void setComment(const std::string &msg) override {
        comment = msg;
    }

  protected:
    void progress_handler(int step, int max_step) override;

  private:
    Color progressBarColor, commentColor;
    float progressBarMaxWidth, progressBarHeight;
    float commentWidth, commentHeight;
    Coord progressBarTLCorner;
    Coord commentLabelCenter;

    uint currentPercent;
    std::string comment;
};
}
#endif // TALIPOT_GL_PROGRESS_BAR_H
