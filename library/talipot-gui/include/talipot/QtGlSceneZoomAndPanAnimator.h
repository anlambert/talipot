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

#ifndef TALIPOT_QT_GL_SCENE_ZOOM_AND_PAN_ANIMATOR_H
#define TALIPOT_QT_GL_SCENE_ZOOM_AND_PAN_ANIMATOR_H

#include <QObject>

#include <talipot/GlSceneZoomAndPan.h>

namespace tlp {

class GlWidget;

/**
 * \brief A convenient class to perform a Zoom and Pan animation on a GlWidget
 *
 * This class allows to perform a Zoom and Pan animation on the Talipot OpenGL scene associated with
 * a GlWidget The algorithm used to perform this task is the one published in : Jarke J. van
 * Wijk and Wim A.A. Nuij, "Smooth and efficient zooming and panning" For more details, the paper
 * can be downloaded at the following url : www.win.tue.nl/~vanwijk/zoompan.pdf
 */
class TLP_QT_SCOPE QtGlSceneZoomAndPanAnimator : public QObject, public GlSceneZoomAndPan {

    Q_OBJECT

  public:
    /**
     * QtGlSceneZoomAndPanAnimator constructor
     *
     * \param glWidget The Talipot OpenGL widget on which to perform the animation
     * * \param boundingBox the bounding box in scene coordinates on which the Talipot OpenGL scene
     * has to be zoomed and panned. At the end of the animation, the viewport will be zoomed and
     * centered on the content of that bounding box. \param optimalPath if true zooming and panning
     * will be combined at each step of the animation, if false the scene will be zoomed out/in,
     * panned then zoomed in/out \param layerName name of the layer zoom animation should be done on
     * \param velocity animation speed parameter, adjust it according to your needs
     * \param p zoom/pan trade-off parameter, adjust it according to your needs
     */
    QtGlSceneZoomAndPanAnimator(GlWidget *glWidget, const BoundingBox &boundingBox,
                                const double duration = 1000.,
                                const std::string &layerName = "Main",
                                const bool optimalPath = true, const double velocity = 1.1,
                                const double p = sqrt(1.6));

    /**
     * Method to call to start the Zoom and Pan animation
     */
    void animateZoomAndPan();

  protected slots:

    virtual void zoomAndPanAnimStepSlot(int animationStep);

  protected:
    GlWidget *glWidget;
    double animationDurationMsec;
};
}

#endif // TALIPOT_QT_GL_SCENE_ZOOM_AND_PAN_ANIMATOR_H
