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

#include "talipot/ItemDelegate.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QMainWindow>
#include <QLabel>
#include <QTableView>

#include <talipot/GraphModel.h>

using namespace tlp;

ItemDelegate::ItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent), _currentMonitoredChild(nullptr),
      _currentMonitoredCombo(nullptr) {
  registerCreator<bool>(new BooleanEditorCreator);
  registerCreator<int>(new NumberEditorCreator<tlp::IntegerType>);
  registerCreator<uint>(new NumberEditorCreator<tlp::UnsignedIntegerType>);
  registerCreator<long>(new NumberEditorCreator<tlp::LongType>);
  registerCreator<double>(new NumberEditorCreator<tlp::DoubleType>);
  registerCreator<float>(new NumberEditorCreator<tlp::FloatType>);
  registerCreator<std::string>(new StdStringEditorCreator);
  registerCreator<QString>(new MultiLinesEditEditorCreator<tlp::QStringType>);
  registerCreator<QStringList>(new QStringListEditorCreator);
  registerCreator<tlp::Color>(new ColorEditorCreator);
  registerCreator<tlp::Coord>(new Vec3fEditorCreator);
  registerCreator<tlp::Size>(new Vec3fEditorCreator(true));
  registerCreator<tlp::BooleanProperty *>(new PropertyEditorCreator<tlp::BooleanProperty>);
  registerCreator<tlp::DoubleProperty *>(new PropertyEditorCreator<tlp::DoubleProperty>);
  registerCreator<tlp::LayoutProperty *>(new PropertyEditorCreator<tlp::LayoutProperty>);
  registerCreator<tlp::StringProperty *>(new PropertyEditorCreator<tlp::StringProperty>);
  registerCreator<tlp::IntegerProperty *>(new PropertyEditorCreator<tlp::IntegerProperty>);
  registerCreator<tlp::SizeProperty *>(new PropertyEditorCreator<tlp::SizeProperty>);
  registerCreator<tlp::ColorProperty *>(new PropertyEditorCreator<tlp::ColorProperty>);
  registerCreator<tlp::BooleanVectorProperty *>(
      new PropertyEditorCreator<tlp::BooleanVectorProperty>);
  registerCreator<tlp::DoubleVectorProperty *>(
      new PropertyEditorCreator<tlp::DoubleVectorProperty>);
  registerCreator<tlp::CoordVectorProperty *>(new PropertyEditorCreator<tlp::CoordVectorProperty>);
  registerCreator<tlp::StringVectorProperty *>(
      new PropertyEditorCreator<tlp::StringVectorProperty>);
  registerCreator<tlp::IntegerVectorProperty *>(
      new PropertyEditorCreator<tlp::IntegerVectorProperty>);
  registerCreator<tlp::SizeVectorProperty *>(new PropertyEditorCreator<tlp::SizeVectorProperty>);
  registerCreator<tlp::ColorVectorProperty *>(new PropertyEditorCreator<tlp::ColorVectorProperty>);
  registerCreator<tlp::PropertyInterface *>(new PropertyInterfaceEditorCreator);
  registerCreator<tlp::NumericProperty *>(new PropertyEditorCreator<tlp::NumericProperty>);
  registerCreator<tlp::ColorScale>(new ColorScaleEditorCreator);
  registerCreator<tlp::StringCollection>(new StringCollectionEditorCreator);
  registerCreator<TextureFile>(new TextureFileEditorCreator);
  registerCreator<FileDescriptor>(new FileDescriptorEditorCreator);
  registerCreator<NodeShape::NodeShapes>(new NodeShapeEditorCreator);
  registerCreator<EdgeShape::EdgeShapes>(new EdgeShapeEditorCreator);
  registerCreator<EdgeExtremityShape::EdgeExtremityShapes>(new EdgeExtremityShapeEditorCreator);
  // registerCreator<std::vector<bool> >(new VectorEditorCreator<bool>);
  registerCreator<QVector<bool>>(new QVectorBoolEditorCreator);
  registerCreator<std::vector<Color>>(new VectorEditorCreator<Color>);
  registerCreator<std::vector<Coord>>(new VectorEditorCreator<Coord>);
  registerCreator<std::vector<double>>(new VectorEditorCreator<double>);
  registerCreator<std::vector<int>>(new VectorEditorCreator<int>);
  registerCreator<std::vector<std::string>>(new VectorEditorCreator<std::string>);
  registerCreator<Font>(new FontEditorCreator);
  registerCreator<LabelPosition::LabelPositions>(new LabelPositionEditorCreator);
  registerCreator<Graph *>(new GraphEditorCreator);
  registerCreator<std::set<tlp::edge>>(new EdgeSetEditorCreator);
  registerCreator<FontIconName>(new FontIconCreator);
}

ItemDelegate::~ItemDelegate() {
  for (auto *v : _creators.values()) {
    delete v;
  }
}

void ItemDelegate::unregisterCreator(tlp::ItemEditorCreator *c) {
  int k = _creators.key(c, INT_MIN);

  if (k != INT_MIN) {
    _creators.remove(k);
  }
}

tlp::ItemEditorCreator *ItemDelegate::creator(int typeId) const {
  return _creators[typeId];
}

QWidget *ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const {
  QVariant v = index.model()->data(index);
  ItemEditorCreator *c = creator(v.userType());

  if (c == nullptr) {
    return QStyledItemDelegate::createEditor(parent, option, index);
  }

  auto *pi = index.data(Model::PropertyRole).value<PropertyInterface *>();
  if (pi) {
    c->setPropertyToEdit(pi);
  }
  QWidget *w = c->createWidget(parent);
  return w;
}

QString ItemDelegate::displayText(const QVariant &value, const QLocale &locale) const {
  if (value.canConvert<QString>()) {
    return value.toString();
  }

  ItemEditorCreator *c = creator(value.userType());

  if (c != nullptr) {
    return c->displayText(value);
  }

  return QStyledItemDelegate::displayText(value, locale);
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
  const QAbstractItemModel *model = index.model();

  if (model != nullptr) {
    QVariant value = model->data(index);
    ItemEditorCreator *c = creator(value.userType());

    if (c != nullptr) {
      QSize s = c->sizeHint(option, index);

      if (s.isValid()) {
        return s;
      }
    }
  }

  return QStyledItemDelegate::sizeHint(option, index);
}

void ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const {
  QVariant bgColor = index.data(Qt::BackgroundRole), fgColor = index.data(Qt::ForegroundRole);

  if (bgColor.isValid() && bgColor.canConvert<QColor>()) {
    painter->setBrush(bgColor.value<QColor>());
  } else {
    auto *tv = static_cast<QTableView *>(parent());
    painter->setBrush((tv && tv->alternatingRowColors() && (index.row() % 2))
                          ? option.palette.alternateBase()
                          : option.palette.base());
  }

  if (fgColor.isValid() && fgColor.canConvert<QColor>()) {
    painter->setPen(fgColor.value<QColor>());
  } else {
    painter->setPen(option.palette.windowText().color());
  }

  painter->fillRect(option.rect, painter->brush());

  QVariant v = index.data();

  if (!v.isValid()) {
#ifndef NDEBUG
    qWarning() << "Value for row("
               << index.model()->headerData(index.row(), Qt::Vertical).toString() << ") - column("
               << index.model()->headerData(index.column(), Qt::Horizontal).toString()
               << ") is invalid";
#endif
    return;
  }

  ItemEditorCreator *c = creator(v.userType());

  if (c && !c->paint(painter, option, v, index)) {
    QStyledItemDelegate::paint(painter, option, index);
  }

  if (option.state & QStyle::State_HasFocus) {
    painter->setBrush(Qt::transparent);
    painter->setPen(QPen(textColor(), 2));
    painter->drawRect(option.rect.x() + 1, option.rect.y() + 1, option.rect.width() - 2,
                      option.rect.height() - 2);
  }
}

void ItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
  QVariant data = index.data();
  auto *g = index.data(Model::GraphRole).value<tlp::Graph *>();
  bool isMandatory = true;
  QVariant mandatoryVar = index.data(Model::MandatoryRole);

  if (mandatoryVar.isValid()) {
    isMandatory = mandatoryVar.value<bool>();
  }

  ItemEditorCreator *c = creator(data.userType());

  if (!c) {
    return;
  }

  c->setEditorData(editor, index, data, isMandatory, g);
}

void ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const {
  QVariant data = index.data();
  auto *g = index.data(Model::GraphRole).value<tlp::Graph *>();
  ItemEditorCreator *c = creator(data.userType());

  if (!c) {
    return;
  }

  model->setData(index, c->editorData(editor, g));
}

bool ItemDelegate::eventFilter(QObject *object, QEvent *event) {
  if (event->type() == QEvent::FocusOut && dynamic_cast<QComboBox *>(object) != nullptr) {
    return true;
  } else if (event->type() == QEvent::ChildAdded) {
    auto *childEv = static_cast<QChildEvent *>(event);

    if (dynamic_cast<QComboBox *>(object) != nullptr) {
      _currentMonitoredChild = childEv->child();
      _currentMonitoredCombo = static_cast<QComboBox *>(object);
      _currentMonitoredChild->installEventFilter(this);
      _currentMonitoredCombo->removeEventFilter(this);
      connect(_currentMonitoredCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
              &ItemDelegate::comboDataChanged);
    }
  } else if (object == _currentMonitoredChild && event->type() == QEvent::Hide) {
    _currentMonitoredChild->removeEventFilter(this);
    _currentMonitoredChild = nullptr;
    emit commitData(_currentMonitoredCombo);
    _currentMonitoredCombo->deleteLater();
    _currentMonitoredCombo = nullptr;
    return true;
  }

  return QStyledItemDelegate::eventFilter(object, event);
}

void ItemDelegate::comboDataChanged() {
  emit commitData(static_cast<QWidget *>(sender()));
}

QVariant ItemDelegate::showEditorDialog(tlp::ElementType elType, tlp::PropertyInterface *pi,
                                        tlp::Graph *g, ItemDelegate *delegate,
                                        QWidget *dialogParent, uint id) {
  QVariant value;
  bool valid;
  if (elType == tlp::NODE) {
    node n(id);

    if ((valid = n.isValid())) {
      value = GraphModel::nodeValue(id, pi);
    } else {
      value = GraphModel::nodeDefaultValue(pi);
    }
  } else {
    edge e(id);

    if ((valid = e.isValid())) {
      value = GraphModel::edgeValue(id, pi);
    } else {
      value = GraphModel::edgeDefaultValue(pi);
    }
  }

  ItemEditorCreator *creator = delegate->creator(value.userType());

  // Display the dialog on the same screen as the main window
  dialogParent = getMainWindow();

  creator->setPropertyToEdit(pi);
  QWidget *w = creator->createWidget(dialogParent);
  creator->setEditorData(w, QModelIndex(), value, g);

  auto *dlg = dynamic_cast<QDialog *>(w);

  if (dlg == nullptr) {
    QString title(
        QString("Set %1 %2").arg(elType == NODE ? "node" : "edge").arg(valid ? "value" : "values"));
    bool displayPropertyName = true;
    // adjust dialog title for some view properties
    if (pi->getName() == "viewShape" && elType == EDGE) {
      title = "Select an edge shape";
      displayPropertyName = false;
    }
    // create a dialog on the fly
    dlg = new QDialog(dialogParent);
    dlg->setWindowTitle(title);
    auto *layout = new QVBoxLayout;
    dlg->setLayout(layout);
    dlg->setMinimumWidth(250);
    if (displayPropertyName) {
      layout->addWidget(new QLabel(pi->getName().c_str()));
    }
    layout->addWidget(w);
    auto *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, Qt::Horizontal);
    layout->addWidget(buttonBox);
    QWidget::setTabOrder(w, buttonBox);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, dlg, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, dlg, &QDialog::reject);
  }

  QVariant result;

  if (dlg->exec() == QDialog::Accepted) {
    result = creator->editorData(w, g);
  }

  delete dlg;
  return result;
}
