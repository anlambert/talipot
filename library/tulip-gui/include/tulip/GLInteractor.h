/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */
#ifndef GLINTERACTOR_H
#define GLINTERACTOR_H

#include <tulip/InteractorComposite.h>

#include <QIcon>

namespace tlp {

class GlMainWidget;

/**
 * @brief The GLInteractorComponent class is an InteractorComponent that can do OpenGL rendering on
 * a GlMainWidget.
 *
 * @warning Only GLInteractorComponent that are stored into a GLInteractorComposite will be able to
 * do OpenGL rendering. Storing them into an InteractorComposite will have no effect.
 *
 * GLInteractorComponent are meant to be stored into a GLInteractorComposite
 * When installed on a GlMainWidget, this interactor will have two new methods getting called:
 * @list
 * @li draw: Draws a visual feedback into the OpenGL scene. This is called after the scene was
 * rendered
 * @li compute: Allows the interactor to do some pre-initialisation steps before the OpenGL scene is
 * rendered.
 * @endlist
 */
class TLP_QT_SCOPE GLInteractorComponent : public tlp::InteractorComponent {
  Q_OBJECT
public slots:

  /**
   * @brief Draws an OpenGL visual feedback for the interactor into a given GlMainWidget.
   * This method is called after the scene was rendered.
   * @return true if the rendering completed sucessfully
   */
  virtual bool draw(tlp::GlMainWidget *) {
    return false;
  }

  /**
   * @brief Initializes the interactor before the scene in the given GlMainWidget is rendered
   */
  virtual bool compute(tlp::GlMainWidget *) {
    return false;
  }
};

/**
 * @brief The GLInteractorComposite class behaves like a InteractorComposite but is meant to store
 * GLInteractorComponent.
 * @warning Only GLInteractorComponent that are stored into a GLInteractorComposite will be able to
 * do OpenGL rendering. Storing them into an InteractorComposite will have no effect.
 */
class TLP_QT_SCOPE GLInteractorComposite : public tlp::InteractorComposite {
  Q_OBJECT

public:
  GLInteractorComposite(const QIcon &icon, const QString &text = "");

public slots:
  /**
   * @brief Calls the compute method on every sub-components
   * @note You can subclass this method to add custom behavior before or after components are
   * called.
   */
  virtual void compute(tlp::GlMainWidget *);

  /**
   * @brief Calls the draw method on every sub-components
   * @note You can subclass this method to add custom behavior before or after components are
   * called.
   */
  virtual void draw(tlp::GlMainWidget *);
};
} // namespace tlp

#endif // GLINTERACTOR_H
