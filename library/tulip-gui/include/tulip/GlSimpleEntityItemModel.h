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
///@cond DOXYGEN_HIDDEN

#ifndef GLSIMPLEENTITYITEMMODEL_H
#define GLSIMPLEENTITYITEMMODEL_H

#include <QAbstractItemModel>

#include <tulip/tulipconf.h>

namespace tlp {

class GlSimpleEntity;

class TLP_QT_SCOPE GlSimpleEntityItemEditor {
public:
  GlSimpleEntityItemEditor(GlSimpleEntity *ent) : entity(ent) {}
  virtual ~GlSimpleEntityItemEditor() {}

  /**
   * @brief Return properties names for this entity
   * These properties names are used to dynamically configure the embedded entity
   * for example these function can be used by Mouse information interactor
   * If you create a class that inherits of GlSimpleEntityItemEditor : you can reimplement this
   * function to return your properties names
   * for example : return QStringList() << "fillColor" << "outlineColor";
   * @return QList of properties names
   */
  virtual QStringList propertiesNames() const;

  /**
   * @brief Return properties (in  QVariant format) for this entity
   * These properties QVariant are used to dynamically configure the entity
   * for example these function can be used by Mouse information interactor
   * If you create a class that inherits of GlSimpleEntity : you can reimplement this function to
   * return your properties
   * for example : return QVariantList() << QVariant::fromValue<Color>(getFillColor()) <<
   * QVariant::fromValue<Color>(getOutlineColor());
   * @return QList of properties (in QVariant format)
   */
  virtual QVariantList propertiesQVariant() const;

  /**
   * @brief Set value for a property previously returned by propertiesNames() and properties()
   * functions
   * This function is call when we want to set value of a property
   * this parameter is returned in list by propertiesNames() and properties funtions
   * If you create a class that inherits of GlSimpleEntityItemEditor : you can reimplement this
   * function to set your properties
   * For example :
   * if(name=="fillColor")
   *   setFillColor(value.value<Color>());
   * else if(name=="outlineColor")
   *   setOutlineColor(value.value<Color>());
   */
  virtual void setProperty(const QString &name, const QVariant &value);

protected:
  GlSimpleEntity *entity;
};

class TLP_QT_SCOPE GlSimpleEntityItemModel : public QAbstractItemModel {
  Q_OBJECT
  Q_ENUMS(SimpleEntityRole)

public:
  enum SimpleEntityRole { SimpleEntityRole = Qt::UserRole + 1 };

  GlSimpleEntityItemModel(GlSimpleEntityItemEditor *itemEditor, QObject *parent = nullptr);
  ~GlSimpleEntityItemModel() override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &child) const override;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  QString headerText() const {
    return QString("toto");
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) override;

  Qt::ItemFlags flags(const QModelIndex &index) const override {
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
  }

protected:
  GlSimpleEntityItemEditor *editor;
};
} // namespace tlp

#endif // GLSIMPLEENTITYITEMMODEL_H
///@endcond
