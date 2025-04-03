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

#ifndef TALIPOT_CAPTION_ITEM_H
#define TALIPOT_CAPTION_ITEM_H

#include <QObject>

#include <talipot/Color.h>
#include <talipot/Observable.h>

class QGradient;

namespace tlp {

class View;
class CaptionGraphicsBackgroundItem;
class CaptionGraphicsItem;
class Graph;
class ColorProperty;
class DoubleProperty;
class SizeProperty;

class CaptionItem : public QObject, public Observable {

    Q_OBJECT

  public:
    enum CaptionType {
        NodesColorCaption = 1,
        NodesSizeCaption = 2,
        EdgesColorCaption = 3,
        EdgesSizeCaption = 4
    };

    CaptionItem(View *view);
    ~CaptionItem() override;

    void create(CaptionType captionType);

    void initCaption();

    void generateColorCaption(CaptionType captionType);

    void generateSizeCaption(CaptionType captionType);

    CaptionGraphicsBackgroundItem *captionGraphicsItem();

    void treatEvents(const std::vector<Event> &ev) override;

  signals:

    void filtering(bool);

  public slots:

    void removeObservation(bool);

    void applyNewFilter(float begin, float end);
    void selectedPropertyChanged(std::string propertyName);

  private:
    void clearObservers();

    void generateGradients(const std::vector<std::pair<double, Color>> &metricToColorFiltered,
                           QGradient &activeGradient, QGradient &hideGradient);

    View *view;

    CaptionType _captionType;
    CaptionGraphicsItem *_captionGraphicsItem;

    Graph *_graph;
    DoubleProperty *_metricProperty;
    ColorProperty *_colorProperty;
    SizeProperty *_sizeProperty;
    ColorProperty *_backupColorProperty;
    ColorProperty *_backupBorderColorProperty;
};
}

#endif // TALIPOT_CAPTION_ITEM_H
