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

#ifndef TALIPOT_ITEM_EDITOR_CREATORS_H
#define TALIPOT_ITEM_EDITOR_CREATORS_H

#include <talipot/config.h>
#include <talipot/PropertyTypes.h>
#include <talipot/TlpQtTools.h>
#include <talipot/ViewSettings.h>

#include <QVariant>
#include <QSize>

class QWidget;
class QPainter;
class QStyleOptionViewItem;
class QModelIndex;

namespace tlp {

static const int cellPadding = 2;

class PropertyInterface;

class TLP_QT_SCOPE ItemEditorCreator {
public:
  virtual ~ItemEditorCreator() = default;
  virtual QWidget *createWidget(QWidget *) const = 0;
  virtual bool paint(QPainter *, const QStyleOptionViewItem &, const QVariant &,
                     const QModelIndex &) const;
  virtual QString displayText(const QVariant &) const {
    return "";
  }

  virtual QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const;

  virtual void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool isMandatory,
                             tlp::Graph *g = nullptr) = 0;
  virtual QVariant editorData(QWidget *, tlp::Graph *g = nullptr) = 0;
  virtual void setPropertyToEdit(tlp::PropertyInterface *) {}
};

template <typename T>
class TLP_QT_SCOPE StringDisplayEditorCreator : public ItemEditorCreator {
public:
  QString displayText(const QVariant &v) const override {
    return tlpStringToQString(T::toString(v.value<REAL_TYPE(T)>()));
  }
};

class TLP_QT_SCOPE ColorEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *) const override;
  bool paint(QPainter *, const QStyleOptionViewItem &, const QVariant &,
             const QModelIndex &) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
};

template <typename T>
class NumberEditorCreator : public StringDisplayEditorCreator<T> {
public:
  QWidget *createWidget(QWidget *) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
};

template <typename T>
class LineEditEditorCreator : public StringDisplayEditorCreator<T> {
public:
  QWidget *createWidget(QWidget *) const;
  virtual void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *);
  virtual QVariant editorData(QWidget *, tlp::Graph *);
};

template <typename T>
class MultiLinesEditEditorCreator : public StringDisplayEditorCreator<T> {
public:
  QWidget *createWidget(QWidget *) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override;
  bool paint(QPainter *, const QStyleOptionViewItem &, const QVariant &,
             const QModelIndex &) const override;
};

class TLP_QT_SCOPE BooleanEditorCreator : public ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QString displayText(const QVariant &) const override;
  bool paint(QPainter *, const QStyleOptionViewItem &, const QVariant &,
             const QModelIndex &) const override;
  QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override;
};

class TLP_QT_SCOPE Vec3fEditorCreator : public ItemEditorCreator {
  bool editSize;

public:
  Vec3fEditorCreator(bool editSize = false) : editSize(editSize) {}
  QWidget *createWidget(QWidget *) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QString displayText(const QVariant &v) const override;
  void setPropertyToEdit(tlp::PropertyInterface *prop) override;
};

template <typename PROP>
class PropertyEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QString displayText(const QVariant &) const override;
};

class TLP_QT_SCOPE PropertyInterfaceEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QString displayText(const QVariant &) const override;
};

class TLP_QT_SCOPE NumericPropertyEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QString displayText(const QVariant &) const override;
};

class TLP_QT_SCOPE ColorScaleEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *) const override;
  bool paint(QPainter *, const QStyleOptionViewItem &, const QVariant &,
             const QModelIndex &) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
};

class TLP_QT_SCOPE StringCollectionEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QString displayText(const QVariant &) const override;
};

class TLP_QT_SCOPE FileDescriptorEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  bool paint(QPainter *, const QStyleOptionViewItem &, const QVariant &,
             const QModelIndex &) const override;
  QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override;
};

class TLP_QT_SCOPE TextureFileEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  bool paint(QPainter *, const QStyleOptionViewItem &, const QVariant &,
             const QModelIndex &index) const override;
  QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override;
};

class TLP_QT_SCOPE FontIconCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  bool paint(QPainter *, const QStyleOptionViewItem &, const QVariant &,
             const QModelIndex &) const override;
  QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override;
  QString displayText(const QVariant &) const override;
};

class TLP_QT_SCOPE NodeShapeEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QString displayText(const QVariant &) const override;
  bool paint(QPainter *, const QStyleOptionViewItem &, const QVariant &,
             const QModelIndex &index) const override;
  QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override;
};

class TLP_QT_SCOPE EdgeExtremityShapeEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QString displayText(const QVariant &) const override;
  bool paint(QPainter *, const QStyleOptionViewItem &, const QVariant &,
             const QModelIndex &index) const override;
  QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override;
};

class TLP_QT_SCOPE EdgeShapeEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QString displayText(const QVariant &) const override;
};

template <typename ElementType>
class VectorEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *parent) const override;
  void setEditorData(QWidget *editor, const QModelIndex &, const QVariant &data, bool,
                     tlp::Graph *) override;
  QVariant editorData(QWidget *editor, tlp::Graph *) override;
  QString displayText(const QVariant &data) const override;
};

class QVectorBoolEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *parent) const override;
  void setEditorData(QWidget *editor, const QModelIndex &, const QVariant &data, bool,
                     tlp::Graph *) override;
  QVariant editorData(QWidget *editor, tlp::Graph *) override;
  QString displayText(const QVariant &data) const override;
};

struct TLP_QT_SCOPE FontEditorCreator : public tlp::ItemEditorCreator {
  QWidget *createWidget(QWidget *) const override;
  void setEditorData(QWidget *, const QModelIndex &, const QVariant &, bool, tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QString displayText(const QVariant &) const override;
  bool paint(QPainter *, const QStyleOptionViewItem &, const QVariant &,
             const QModelIndex &index) const override;
};

class TLP_QT_SCOPE LabelPositionEditorCreator : public tlp::ItemEditorCreator {

public:
  QWidget *createWidget(QWidget *) const override;
  void setEditorData(QWidget *w, const QModelIndex &, const QVariant &var, bool,
                     tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QString displayText(const QVariant &) const override;
};

class TLP_QT_SCOPE GraphEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *parent) const override;
  void setEditorData(QWidget *w, const QModelIndex &, const QVariant &var, bool,
                     tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QString displayText(const QVariant &) const override;
};

class TLP_QT_SCOPE EdgeSetEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *parent) const override;
  void setEditorData(QWidget *w, const QModelIndex &, const QVariant &var, bool,
                     tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QString displayText(const QVariant &) const override;
};

class TLP_QT_SCOPE QStringEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *parent) const override;
  void setEditorData(QWidget *w, const QModelIndex &, const QVariant &var, bool,
                     tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QString displayText(const QVariant &) const override;
  void setPropertyToEdit(tlp::PropertyInterface *prop) override;

protected:
  std::string propName;
};

class TLP_QT_SCOPE StdStringEditorCreator : public QStringEditorCreator {
public:
  void setEditorData(QWidget *w, const QModelIndex &, const QVariant &var, bool,
                     tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QString displayText(const QVariant &) const override;
};

class TLP_QT_SCOPE QStringListEditorCreator : public tlp::ItemEditorCreator {
public:
  QWidget *createWidget(QWidget *parent) const override;
  void setEditorData(QWidget *w, const QModelIndex &, const QVariant &var, bool,
                     tlp::Graph *) override;
  QVariant editorData(QWidget *, tlp::Graph *) override;
  QString displayText(const QVariant &) const override;
};

TLP_QT_SCOPE void addIconToPool(const QString &iconName, const QIcon &icon);
}

#if defined(__clang__)
template class tlp::StringDisplayEditorCreator<tlp::PointType>;
template class tlp::StringDisplayEditorCreator<tlp::SizeType>;
#endif

inline QString &truncateText(QString &text, const QString &trailingChars = " ...",
                             int maxChars = 45) {
  if (text.size() > maxChars) {
    text.truncate(maxChars - trailingChars.length());
    text.append(trailingChars);
  }

  return text;
}

#include "cxx/ItemEditorCreators.cxx"

#endif // TALIPOT_ITEM_EDITOR_CREATORS_H
