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

#include <sstream>
#include <climits>
#include <cfloat>

#include <QSet>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPainter>
#include <QDoubleSpinBox>
#include <QKeyEvent>

#include <talipot/DataSet.h>
#include <talipot/VectorEditor.h>
#include <talipot/GraphPropertiesModel.h>
#include <talipot/TlpQtTools.h>
#include <talipot/MetaTypes.h>
#include <talipot/ScientificDoubleSpinBox.h>

namespace tlp {

template <typename T>
QWidget *NumberEditorCreator<T>::createWidget(QWidget *parent) const {
  QDoubleSpinBox *dsb = nullptr;

  // emulate a QSpinBox for integer types
  if (typeid(T).name() == typeid(tlp::IntegerType).name() ||
      typeid(T).name() == typeid(tlp::UnsignedIntegerType).name() ||
      typeid(T).name() == typeid(tlp::LongType).name()) {
    dsb = new QDoubleSpinBox(parent);
    dsb->setDecimals(0);
    // otherwise for floating point number types
  } else {
    // use a dedicated QDoubleSpinBox supporting scientific notation
    dsb = new tlp::ScientificDoubleSpinBox(parent);
    // force the use of dot character for decimal separator
    dsb->setLocale(QLocale(QLocale::C));
  }

  // set correct range of values according to type
  if (typeid(T).name() == typeid(tlp::IntegerType).name()) {
    dsb->setRange(-INT_MAX, INT_MAX);
  } else if (typeid(T).name() == typeid(tlp::UnsignedIntegerType).name()) {
    dsb->setRange(0, UINT_MAX);
  } else if (typeid(T).name() == typeid(tlp::LongType).name()) {
    dsb->setRange(double(-LONG_MAX), double(LONG_MAX));
  } else if (typeid(T).name() == typeid(tlp::FloatType).name()) {
    dsb->setRange(-FLT_MAX, FLT_MAX);
  } else {
    dsb->setRange(-DBL_MAX, DBL_MAX);
  }

  return dsb;
}

template <typename T>
void NumberEditorCreator<T>::setEditorData(QWidget *editor, const QModelIndex &,
                                           const QVariant &data, bool, tlp::Graph *) {
  static_cast<QDoubleSpinBox *>(editor)->setValue(data.value<REAL_TYPE(T)>());
}

template <typename T>
QVariant NumberEditorCreator<T>::editorData(QWidget *editor, tlp::Graph *) {
  QVariant result;
  result.setValue(static_cast<REAL_TYPE(T)>(static_cast<QDoubleSpinBox *>(editor)->value()));
  return result;
}

template <typename T>
QWidget *LineEditEditorCreator<T>::createWidget(QWidget *parent) const {
  return new QLineEdit(parent);
}

template <typename T>
void LineEditEditorCreator<T>::setEditorData(QWidget *editor, const QModelIndex &,
                                             const QVariant &data, bool, tlp::Graph *) {
  REAL_TYPE(T::RealType val = data.value < typename T) > ();
  static_cast<QLineEdit *>(editor)->setText(tlpStringToQString(T::toString(val)));
  static_cast<QLineEdit *>(editor)->selectAll();
}

template <typename T>
QVariant LineEditEditorCreator<T>::editorData(QWidget *editor, tlp::Graph *) {
  std::string strVal = QStringToTlpString(static_cast<QLineEdit *>(editor)->text());
  QVariant result;
  REAL_TYPE(T) val;

  if (T::fromString(val, strVal)) {
    result.setValue<REAL_TYPE(T)>(val);
  }

  return result;
}

class MultiLineTextEdit : public QTextEdit {
public:
  MultiLineTextEdit(QWidget *parent) : QTextEdit(parent) {}

  void keyPressEvent(QKeyEvent *event) override {
    // only insert a line break when hitting return key with a modifier,
    // validate entered text otherwise
    if (event->key() == Qt::Key_Return && event->modifiers() == Qt::NoModifier) {
      close();
      static_cast<QWidget *>(parent())->setFocus(Qt::MouseFocusReason);
    } else {
      QTextEdit::keyPressEvent(event);
    }
  }
};

template <typename T>
QWidget *MultiLinesEditEditorCreator<T>::createWidget(QWidget *parent) const {
  QTextEdit *edit = new MultiLineTextEdit(parent);
  edit->setFocusPolicy(Qt::StrongFocus);
  edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  return edit;
}

template <typename T>
void MultiLinesEditEditorCreator<T>::setEditorData(QWidget *editor, const QModelIndex &,
                                                   const QVariant &data, bool, tlp::Graph *) {
  auto val = data.value<REAL_TYPE(T)>();
  static_cast<QTextEdit *>(editor)->setPlainText(tlpStringToQString(T::toString(val)));
  static_cast<QTextEdit *>(editor)->selectAll();
}

template <typename T>
QVariant MultiLinesEditEditorCreator<T>::editorData(QWidget *editor, tlp::Graph *) {
  std::string strVal = QStringToTlpString(static_cast<QTextEdit *>(editor)->toPlainText());
  QVariant result;
  REAL_TYPE(T) val;

  if (T::fromString(val, strVal)) {
    result.setValue(val);
  }

  return result;
}

template <typename T>
QSize MultiLinesEditEditorCreator<T>::sizeHint(const QStyleOptionViewItem &option,
                                               const QModelIndex &index) const {
  QVariant data = index.model()->data(index);
  auto val = data.value<REAL_TYPE(T)>();
  QString valS = tlpStringToQString(T::toString(val));
  QStringList lines = valS.split(QLatin1Char('\n'));
  QFontMetrics fontMetrics(option.font);
  int height = 0;
  int width = 0;

  for (int i = 0; i < lines.count(); ++i) {
    QRect textBB = fontMetrics.boundingRect(lines.at(i));
    height += fontMetrics.boundingRect("|").height();
    width = std::max(width, textBB.width());
  }

  // restrict column width in case of really large string to display
  width = std::min(width, 500);

  return QSize(width + 15, height + 5);
}

template <typename T>
bool MultiLinesEditEditorCreator<T>::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                           const QVariant &data, const QModelIndex &index) const {
  ItemEditorCreator::paint(painter, option, data, index);
  QRect rect = option.rect;
  auto val = data.value<REAL_TYPE(T)>();
  QString valS = tlpStringToQString(T::toString(val));
  QStringList lines = valS.split(QLatin1Char('\n'));

  QColor foregroundColor = index.model()->data(index, Qt::ForegroundRole).value<QColor>();
  if (!foregroundColor.isValid()) {
    foregroundColor = option.palette.text().color();
  }

  if (option.state.testFlag(QStyle::State_Selected) && option.showDecorationSelected) {
    painter->setPen(option.palette.highlightedText().color());
    painter->setBrush(option.palette.highlightedText());
  } else {
    painter->setPen(foregroundColor);
    painter->setBrush(foregroundColor);
  }

  for (int i = 0; i < lines.count(); ++i) {
    painter->drawText(rect.x() + cellPadding, rect.y() + i * rect.height() / lines.count(),
                      rect.width() - cellPadding, rect.height() / lines.count(),
                      Qt::AlignLeft | Qt::AlignVCenter, lines.at(i));
  }

  return true;
}

template <typename PROPTYPE>
QWidget *PropertyEditorCreator<PROPTYPE>::createWidget(QWidget *parent) const {
  return new QComboBox(parent);
}

template <typename PROPTYPE>
void PropertyEditorCreator<PROPTYPE>::setEditorData(QWidget *w, const QModelIndex &,
                                                    const QVariant &val, bool isMandatory,
                                                    tlp::Graph *g) {
  if (g == nullptr) {
    w->setEnabled(false);
    return;
  }

  auto *prop = val.value<PROPTYPE *>();
  auto *combo = static_cast<QComboBox *>(w);
  GraphPropertiesModel<PROPTYPE> *model = nullptr;

  if (isMandatory) {
    model = new GraphPropertiesModel<PROPTYPE>(g, false, combo);
  } else {
    model = new GraphPropertiesModel<PROPTYPE>(QObject::tr("Select a property"), g, false, combo);
  }

  combo->setModel(model);
  combo->setCurrentIndex(model->rowOf(prop));
}

template <typename PROPTYPE>
QVariant PropertyEditorCreator<PROPTYPE>::editorData(QWidget *w, tlp::Graph *g) {
  if (g == nullptr) {
    return QVariant();
  }

  auto *combo = static_cast<QComboBox *>(w);
  auto *model = static_cast<GraphPropertiesModel<PROPTYPE> *>(combo->model());
  QVariant var = model->data(model->index(combo->currentIndex(), 0), Model::PropertyRole);
  auto *pi = var.value<tlp::PropertyInterface *>();
  auto *prop = static_cast<PROPTYPE *>(pi);
  return QVariant::fromValue<PROPTYPE *>(prop);
}

template <typename PROPTYPE>
QString PropertyEditorCreator<PROPTYPE>::displayText(const QVariant &v) const {
  auto *prop = v.value<PROPTYPE *>();

  if (prop == nullptr) {
    return QObject::tr("Select a property");
  }

  return tlpStringToQString(prop->getName());
}

template <typename ElementType>
QWidget *VectorEditorCreator<ElementType>::createWidget(QWidget *) const {
  auto *w = new VectorEditor(nullptr);
  w->setWindowFlags(Qt::Dialog);
  w->setWindowModality(Qt::ApplicationModal);
  return w;
}

template <typename ElementType>
void VectorEditorCreator<ElementType>::setEditorData(QWidget *editor, const QModelIndex &,
                                                     const QVariant &v, bool, tlp::Graph *) {
  QVector<QVariant> editorData;
  auto vect = v.value<std::vector<ElementType>>();

  for (size_t i = 0; i < vect.size(); ++i) {
    editorData.push_back(QVariant::fromValue<ElementType>(vect[i]));
  }

  static_cast<VectorEditor *>(editor)->setVector(editorData, qMetaTypeId<ElementType>());

  static_cast<VectorEditor *>(editor)->move(QCursor::pos());
}

template <typename ElementType>
QVariant VectorEditorCreator<ElementType>::editorData(QWidget *editor, tlp::Graph *) {
  std::vector<ElementType> result;
  QVector<QVariant> editorData = static_cast<VectorEditor *>(editor)->vector();

  for (const QVariant &v : editorData) {
    result.push_back(v.value<ElementType>());
  }

  return QVariant::fromValue<std::vector<ElementType>>(result);
}

// the template below is only used for displayText method implementation
template <typename T>
struct DisplayVectorDataType : public DataType {
  DisplayVectorDataType(void *value) : DataType(value) {}
  ~DisplayVectorDataType() override = default;
  DataType *clone() const override {
    return nullptr;
  }

  std::string getTypeName() const override {
    return std::string(typeid(std::vector<T>).name());
  }
};

template <typename ElementType>
QString VectorEditorCreator<ElementType>::displayText(const QVariant &data) const {
  auto v = data.value<std::vector<ElementType>>();

  if (v.empty()) {
    return QString();
  }

  // use a DataTypeSerializer if any
  DataTypeSerializer *dts = DataSet::typenameToSerializer(std::string(typeid(v).name()));

  if (dts) {
    DisplayVectorDataType<ElementType> dt(&v);

    std::stringstream sstr;
    dts->writeData(sstr, &dt);

    std::string str = sstr.str();

    QString qstr = tlpStringToQString(str);

    return truncateText(qstr);
  }

  if (v.size() == 1) {
    return QString("1 element");
  }

  return QString::number(v.size()) + QObject::tr(" elements");
}
}
