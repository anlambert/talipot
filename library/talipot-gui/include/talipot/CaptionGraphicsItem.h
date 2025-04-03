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

#ifndef TALIPOT_CAPTION_GRAPHICS_ITEM_H
#define TALIPOT_CAPTION_GRAPHICS_ITEM_H

#include <QObject>

#include <vector>
#include <string>

class QGradient;
class QGraphicsProxyWidget;
class QGraphicsSimpleTextItem;
class QPushButton;

namespace tlp {

class GlView;
class View;
class CaptionGraphicsBackgroundItem;

class CaptionGraphicsItem : public QObject {

    Q_OBJECT

  public:
    CaptionGraphicsItem(View *view);
    ~CaptionGraphicsItem() override;

    void setType(uint type);

    void loadConfiguration();

    void generateColorCaption(const QGradient &activeGradient, const QGradient &hideGradient,
                              const std::string &propertyName, double minValue, double maxValue);

    void generateSizeCaption(const std::vector<std::pair<double, float>> &metricToSizeFilteredList,
                             const std::string &propertyName, double minValue, double maxValue);

    CaptionGraphicsBackgroundItem *getCaptionItem() const {
        return _rondedRectItem;
    }

    std::string usedProperty();

  signals:

    void filterChanged(float begin, float end);
    void selectedPropertyChanged(std::string propertyName);

  protected slots:

    void filterChangedSlot(float begin, float end);
    void selectPropertyButtonClicked();
    void propertySelectedSlot();

  private:
    QString wrappedPropName(const QString &originalName) const;

    void constructConfigWidget();

    View *_view;

    CaptionGraphicsBackgroundItem *_rondedRectItem;

    QGraphicsProxyWidget *_confPropertySelectionItem;
    QGraphicsSimpleTextItem *_nodesEdgesTextItem;
    QPushButton *_confPropertySelectionWidget;
};
}

#endif // TALIPOT_CAPTION_GRAPHICS_ITEM_H
