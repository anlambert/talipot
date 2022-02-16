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

#ifndef TALIPOT_MOUSE_SHOW_ELEMENT_INFO_H
#define TALIPOT_MOUSE_SHOW_ELEMENT_INFO_H

#include <talipot/InteractorComposite.h>
#include <talipot/Graph.h>

namespace Ui {
class ElementInformationWidget;
}

class QWidget;
class QGraphicsProxyWidget;
class QTableView;
class QAbstractItemModel;
class QObject;
class QSortFilterProxyModel;

namespace tlp {

class View;
class ViewWidget;
class GlWidget;
struct SelectedEntity;

/**
 * We define a specific interactor to show element graph information in eltProperties
 */
class TLP_QT_SCOPE MouseShowElementInfo : public InteractorComponent {

  Ui::ElementInformationWidget *_ui;
  Q_OBJECT

public:
  MouseShowElementInfo(const bool showVisualPropButton = true);
  ~MouseShowElementInfo() override;
  bool eventFilter(QObject *widget, QEvent *e) override;

  void viewChanged(View *) override;

  void clear() override;

  virtual bool pick(int x, int y, SelectedEntity &selectedEntity);

protected:
  ViewWidget *_view;
  QWidget *_informationWidget;
  QGraphicsProxyWidget *_informationWidgetItem;
  GlWidget *glWidget;
  bool _show;
  QSortFilterProxyModel *_model;

  QTableView *tableView() const;

  /**
   * @brief buildModel creates and returns the model to visualize edit element parameters.
   * @param elementType the type of the element can be ElementType::NODE or ElementType::EDGE
   * @param elementId elementId the id of the element
   * @param parent the parent for the model creation.
   * @return
   */
  virtual QAbstractItemModel *buildModel(ElementType elementType, uint elementId,
                                         QObject *parent) const;
  /**
   * @brief elementName returns the title of the element.
   * @param elementType the type of the element can be ElementType::NODE or ElementType::EDGE
   * @param elementId the id of the element
   * @return
   */
  virtual QString elementName(ElementType elementType, uint elementId) const;

private slots:

  void showVisualProp(bool show);
  void hideInfos();
};
}

#endif // TALIPOT_MOUSE_SHOW_ELEMENT_INFO_H
