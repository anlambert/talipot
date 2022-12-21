/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_QUICK_ACCESS_BAR_H
#define TALIPOT_QUICK_ACCESS_BAR_H

#include <QWidget>

#include <talipot/config.h>
#include <talipot/CaptionItem.h>
#include <talipot/Graph.h>

class QGraphicsItem;
class QAbstractButton;
class QPushButton;

namespace Ui {
class QuickAccessBar;
}

namespace tlp {
class GlView;
class GlGraphRenderingParameters;
class GlGraphInputData;
class GlScene;
class ItemDelegate;
class ColorProperty;
class Color;
class PropertyInterface;
class ColorButton;

class TLP_QT_SCOPE QuickAccessBar : public QWidget {
  Q_OBJECT

protected:
  GlView *_mainView;
  GlScene *scene() const;
  GlGraphInputData *inputData() const;
  GlGraphRenderingParameters *renderingParameters() const;

public:
  QuickAccessBar(QWidget *parent = nullptr);
public slots:
  void setGlView(tlp::GlView *);
  virtual void reset() = 0;

signals:
  void settingsChanged();
};

class TLP_QT_SCOPE QuickAccessBarImpl : public QuickAccessBar {
  Q_OBJECT
  Ui::QuickAccessBar *_ui;
  QGraphicsItem *_quickAccessBarItem;

  ItemDelegate *delegate;
  bool _resetting;
  double _oldFontScale;
  double _oldNodeScale;
  bool _captionsInitialized;
  CaptionItem *_captions[4];

public:
  enum QuickAccessButton {
    NODESCOLORCAPTION = 0x1,
    NODESSIZECAPTION = 0x2,
    EDGESCOLORCAPTION = 0x4,
    EDGESIZECAPTION = 0x8,
    SCREENSHOT = 0x10,
    BACKGROUNDCOLOR = 0x20,
    NODECOLOR = 0x40,
    EDGECOLOR = 0x80,
    NODEBORDERCOLOR = 0x100,
    EDGEBORDERCOLOR = 0x200,
    LABELCOLOR = 0x400,
    COLORINTERPOLATION = 0x800,
    SIZEINTERPOLATION = 0x1000,
    SHOWEDGES = 0x2000,
    SHOWLABELS = 0x4000,
    LABELSSCALED = 0x8000,
    NODESHAPE = 0x10000,
    EDGESHAPE = 0x20000,
    NODESIZE = 0x40000,
    EDGESIZE = 0x80000,
    NODELABELPOSITION = 0x100000,
    SELECTFONT = 0x200000,
    SHOWNODES = 0x400000,
    NODEICON = 0x400000,
    ALLBUTTONS = 0xFFFFFF
  };
  Q_DECLARE_FLAGS(QuickAccessButtons, QuickAccessButton)

  explicit QuickAccessBarImpl(QGraphicsItem *quickAccessBarItem = nullptr,
                              QuickAccessButtons button = ALLBUTTONS, QWidget *parent = nullptr);
  ~QuickAccessBarImpl() override;

  QPushButton *showEdgesButton();
  QPushButton *showNodesButton();
  ColorButton *backgroundColorButton();
  QPushButton *showLabelsButton();
  QPushButton *showLabelScaled();
  QPushButton *showColorInterpolation();

protected:
  void addButtonAtEnd(QAbstractButton *button);
  void addButtonsAtEnd(const QVector<QAbstractButton *> &buttonvect);
  void addSeparator();
  void updateFontButtonStyle();
  void showHideCaption(CaptionItem::CaptionType captionType);
  bool setAllValues(ElementType eltType, PropertyInterface *prop);
  void setAllColorValues(ElementType eltType, ColorProperty *prop, const Color &color);

public slots:
  void reset() override;

  virtual void setBackgroundColor(const QColor &);
  virtual void setColorInterpolation(bool);
  void setLabelColor(const QColor &);
  void setNodeColor(const QColor &);
  void setNodeBorderColor(const QColor &);
  void setEdgeColor(const QColor &);
  void setEdgeBorderColor(const QColor &);
  void setNodeShape();
  void setNodeIcon();
  void setEdgeShape();
  void setNodeSize();
  void setEdgeSize();
  void setSizeInterpolation(bool);
  void showHideNodesColorCaption(bool);
  void showHideNodesSizeCaption(bool);
  void showHideEdgesColorCaption(bool);
  void showHideEdgesSizeCaption(bool);
  void takeSnapshot();
  virtual void setEdgesVisible(bool);
  virtual void setNodesVisible(bool);
  virtual void setLabelsVisible(bool);
  virtual void setLabelsScaled(bool);
  void selectFont();
  void setNodeLabelPosition();
};
}
Q_DECLARE_OPERATORS_FOR_FLAGS(tlp::QuickAccessBarImpl::QuickAccessButtons)
#endif // TALIPOT_QUICK_ACCESS_BAR_H
