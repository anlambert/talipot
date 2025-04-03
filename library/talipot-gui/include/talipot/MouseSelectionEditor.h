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

#ifndef TALIPOT_MOUSE_SELECTION_EDITOR_H
#define TALIPOT_MOUSE_SELECTION_EDITOR_H

#include <talipot/GlCircle.h>
#include <talipot/GLInteractor.h>
#include <talipot/GlComplexPolygon.h>
#include <talipot/GlRect.h>

namespace tlp {

class LayoutProperty;
class BooleanProperty;
class DoubleProperty;
class SizeProperty;

class GlLayer;

/// This interactor allows to move/rotate/stretch the current selection layout
class TLP_QT_SCOPE MouseSelectionEditor : public GLInteractorComponent {

  public:
    MouseSelectionEditor();
    ~MouseSelectionEditor() override;
    void clear() override;
    bool compute(GlWidget *glWidget) override;
    bool draw(GlWidget *) override;
    bool eventFilter(QObject *, QEvent *) override;

  private:
    enum EditOperation {
        NONE = 0,
        ROTATE_Z,
        ROTATE_XY,
        STRETCH_X,
        STRETCH_Y,
        STRETCH_XY,
        TRANSLATE,
        ALIGN_TOP,
        ALIGN_BOTTOM,
        ALIGN_LEFT,
        ALIGN_RIGHT,
        ALIGN_VERTICALLY,
        ALIGN_HORIZONTALLY
    };
    enum OperationTarget { COORD = 0, SIZE, COORD_AND_SIZE };

    GlWidget *glWidget;
    DoubleProperty *_rotation;

    void initProxies(GlWidget *glWidget);
    void initEdition();
    void undoEdition();
    void stopEdition();

    Coord ffdCenter;

    GlLayer *layer;
    GlComposite *composite;
    GlCircle _controls[8];
    GlComplexPolygon _advControls[6];
    GlRect centerRect;
    GlRect advRect;
    Coord _layoutCenter;

    bool computeFFD(GlWidget *);
    void getOperation(GlEntity *select);

  protected:
    EditOperation operation;
    OperationTarget mode;

    Coord editCenter;
    Coord editPosition;
    Graph *_graph;
    LayoutProperty *_layout;
    BooleanProperty *_selection;
    SizeProperty *_sizes;
    Coord editLayoutCenter;

    virtual void mMouseTranslate(double, double, GlWidget *);
    virtual void mMouseRotate(double, double, GlWidget *);
    virtual void mMouseStretchAxis(double, double, GlWidget *);
    virtual void mAlign(EditOperation operation, GlWidget *);
};
}

#endif // TALIPOT_MOUSE_SELECTION_EDITOR_H
