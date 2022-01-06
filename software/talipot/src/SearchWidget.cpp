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

#include "SearchWidget.h"
#include "ui_SearchWidget.h"

#include <talipot/GraphHierarchiesModel.h>
#include <talipot/GraphTableItemDelegate.h>
#include <talipot/GraphModel.h>
#include <talipot/Mimes.h>

#include <QStandardItemModel>

using namespace tlp;
using namespace std;

class SearchOperator {
protected:
  tlp::PropertyInterface *_a;
  tlp::PropertyInterface *_b;

public:
  virtual ~SearchOperator() = default;
  virtual void setProperties(tlp::PropertyInterface *a, tlp::PropertyInterface *b) {
    _a = a;
    _b = b;
  }

  virtual bool compare(tlp::node n) = 0;
  virtual bool compare(tlp::edge e) = 0;

  tlp::BooleanProperty *run(tlp::Graph *g, bool onNodes, bool onEdges) {
    auto *prop = new BooleanProperty(g);

    if (onNodes) {
      for (auto n : g->nodes())
        prop->setNodeValue(n, compare(n));
    }

    if (onEdges) {
      for (auto e : g->edges())
        prop->setEdgeValue(e, compare(e));
    }

    return prop;
  }
};
class StringSearchOperator : public SearchOperator {
public:
  virtual bool compareStrings(const QString &a, const QString &b) = 0;
  bool compare(tlp::node n) override {
    return compareStrings(_a->getNodeStringValue(n).c_str(), _b->getNodeStringValue(n).c_str());
  }
  bool compare(tlp::edge e) override {
    return compareStrings(_a->getEdgeStringValue(e).c_str(), _b->getEdgeStringValue(e).c_str());
  }
};
class NumericSearchOperator : public SearchOperator {
  tlp::NumericProperty *_numericA;
  tlp::NumericProperty *_numericB;

public:
  void setProperties(PropertyInterface *a, PropertyInterface *b) override {
    SearchOperator::setProperties(a, b);
    _numericA = static_cast<NumericProperty *>(_a);
    _numericB = static_cast<NumericProperty *>(_b);
  }
  virtual bool compareDoubles(double a, double b) = 0;
  bool compare(tlp::node n) override {
    return compareDoubles(_numericA->getNodeDoubleValue(n), _numericB->getNodeDoubleValue(n));
  }
  bool compare(tlp::edge e) override {
    return compareDoubles(_numericA->getEdgeDoubleValue(e), _numericB->getEdgeDoubleValue(e));
  }
};

#define STRING_CMP(NAME, CMP)                                 \
  class NAME : public StringSearchOperator {                  \
  public:                                                     \
    bool compareStrings(const QString &a, const QString &b) { \
      return CMP;                                             \
    }                                                         \
  };
STRING_CMP(StringEqualsOperator, a == b)
STRING_CMP(StringDifferentOperator, a != b)
STRING_CMP(StartsWithOperator, a.startsWith(b))
STRING_CMP(EndsWithOperator, a.endsWith(b))
STRING_CMP(ContainsOperator, a.contains(b))
STRING_CMP(NoCaseStringEqualsOperator, a.compare(b, Qt::CaseInsensitive) == 0)
STRING_CMP(NoCaseStringDifferentOperator, a.compare(b, Qt::CaseInsensitive) != 0)
STRING_CMP(NoCaseStartsWithOperator, a.startsWith(b, Qt::CaseInsensitive))
STRING_CMP(NoCaseEndsWithOperator, a.endsWith(b, Qt::CaseInsensitive))
STRING_CMP(NoCaseContainsOperator, a.contains(b, Qt::CaseInsensitive))

class MatchesOperator : public StringSearchOperator {
public:
  bool compareStrings(const QString &a, const QString &b) override {
    QRegularExpression regexp(b);
    return regexp.match(a).hasMatch();
  }
};

class NoCaseMatchesOperator : public StringSearchOperator {
public:
  bool compareStrings(const QString &a, const QString &b) override {
    QRegularExpression regexp(b, QRegularExpression::CaseInsensitiveOption);
    return regexp.match(a).hasMatch();
  }
};

#define NUM_CMP(NAME, CMP)                    \
  class NAME : public NumericSearchOperator { \
  public:                                     \
    bool compareDoubles(double a, double b) { \
      return a CMP b;                         \
    }                                         \
  };
NUM_CMP(DoubleEqualsOperator, ==)
NUM_CMP(DoubleDifferentOperator, !=)
NUM_CMP(GreaterOperator, >)
NUM_CMP(GreaterEqualOperator, >=)
NUM_CMP(LesserOperator, <)
NUM_CMP(LesserEqualOperator, <=)

SearchWidget::SearchWidget(QWidget *parent)
    : QWidget(parent), _ui(new Ui::SearchWidget), _graph(nullptr) {
  _ui->setupUi(this);
  _ui->searchButton->setIcon(FontIcon::icon(MaterialDesignIcons::Magnify));
  _ui->tableWidget->hide();
  _ui->tableWidget->setItemDelegate(new ItemDelegate(_ui->tableWidget));

  NUMERIC_OPERATORS << new DoubleEqualsOperator << new DoubleDifferentOperator
                    << new GreaterOperator << new GreaterEqualOperator << new LesserOperator
                    << new LesserEqualOperator << new StartsWithOperator << new EndsWithOperator
                    << new ContainsOperator << new MatchesOperator;

  STRING_OPERATORS << new StringEqualsOperator << new StringDifferentOperator << nullptr << nullptr
                   << nullptr << nullptr << new StartsWithOperator << new EndsWithOperator
                   << new ContainsOperator << new MatchesOperator;

  NOCASE_STRING_OPERATORS << new NoCaseStringEqualsOperator << new NoCaseStringDifferentOperator
                          << nullptr << nullptr << nullptr << nullptr
                          << new NoCaseStartsWithOperator << new NoCaseEndsWithOperator
                          << new NoCaseContainsOperator << new NoCaseMatchesOperator;

  _ui->resultsStorageCombo->setModel(
      new GraphPropertiesModel<BooleanProperty>(nullptr, false, _ui->resultsStorageCombo));
  _ui->searchTermACombo->setModel(
      new GraphPropertiesModel<PropertyInterface>(nullptr, false, _ui->searchTermACombo));
  _ui->searchTermBCombo->setModel(new GraphPropertiesModel<PropertyInterface>(
      "Custom value", nullptr, false, _ui->searchTermBCombo));
  connect(_ui->graphCombo, &TreeViewComboBox::currentItemChanged, this,
          &SearchWidget::graphIndexChanged);
  connect(_ui->selectionModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &SearchWidget::selectionModeChanged);
}

SearchWidget::~SearchWidget() {
  delete _ui;

  for (auto *op : NUMERIC_OPERATORS) {
    delete op;
  }

  for (auto *op : STRING_OPERATORS) {
    delete op;
  }

  for (auto *op : NOCASE_STRING_OPERATORS) {
    delete op;
  }
}

void SearchWidget::setModel(tlp::GraphHierarchiesModel *model) {
  _ui->graphCombo->setModel(model);
  setGraph(model->currentGraph());
}

void SearchWidget::currentGraphChanged(tlp::Graph *g) {
  auto *graphsModel = static_cast<GraphHierarchiesModel *>(_ui->graphCombo->model());
  QModelIndex idx = graphsModel->indexOf(g);
  _ui->graphCombo->setRootModelIndex(idx.parent());
  _ui->graphCombo->setCurrentIndex(idx.row());
}

void searchForIndex(QComboBox *combo, const QString &s) {
  combo->setCurrentIndex(0);

  if (!s.isEmpty()) {
    QAbstractItemModel *model = combo->model();

    for (int i = 0; i < model->rowCount(); ++i) {
      if (model->index(i, 0).data().toString() == s) {
        combo->setCurrentIndex(i);
        break;
      }
    }
  }
}

void SearchWidget::setGraph(Graph *g) {
  if (g != nullptr) {
    // Force creation of viewSelection to ensure we have at least one boolean property existing in
    // the graph
    g->getBooleanProperty("viewSelection");
  }

  else {
    _ui->resultsCountLabel->setText("");
  }

  _graph = g;

  QString oldStorageName;
  QString oldTermAName;
  QString oldTermBName;

  if (_ui->resultsStorageCombo->model() != nullptr) {
    oldStorageName = _ui->resultsStorageCombo->currentText();
  }

  if (_ui->searchTermACombo->model() != nullptr) {
    oldTermAName = _ui->searchTermACombo->currentText();
  }

  if (_ui->searchTermBCombo->model() != nullptr) {
    oldTermBName = _ui->searchTermBCombo->currentText();
  }

  _ui->resultsStorageCombo->setModel(
      new GraphPropertiesModel<BooleanProperty>(g, false, _ui->resultsStorageCombo));
  _ui->searchTermACombo->setModel(
      new GraphPropertiesModel<PropertyInterface>(g, false, _ui->searchTermACombo));
  _ui->searchTermBCombo->setModel(
      new GraphPropertiesModel<PropertyInterface>("Custom value", g, false, _ui->searchTermBCombo));

  if (!oldStorageName.isEmpty()) {
    searchForIndex(_ui->resultsStorageCombo, oldStorageName);
  } else {
    searchForIndex(_ui->resultsStorageCombo, "viewSelection");
  }

  if (!oldTermAName.isEmpty()) {
    searchForIndex(_ui->searchTermACombo, oldTermAName);
  } else {
    searchForIndex(_ui->searchTermACombo, "viewMetric");
  }

  if (!oldTermBName.isEmpty()) {
    searchForIndex(_ui->searchTermBCombo, oldTermBName);
  } else {
    searchForIndex(_ui->searchTermBCombo, "Custom value");
  }
}

void SearchWidget::selectionModeChanged(int index) {
  _ui->resultsStorageCombo->setEnabled(index != 3);
}

void SearchWidget::search() {
  if (_graph == nullptr) {
    return;
  }

  _graph->push();
  Observable::holdObservers();
  SearchOperator *op = searchOperator();

  tlp::PropertyInterface *a = term(_ui->searchTermACombo);
  tlp::PropertyInterface *b = nullptr;
  bool deleteTermB = false;

  if (_ui->tableWidget->isVisible()) {
    deleteTermB = true;

    if (isNumericComparison()) {
      auto *doubleProp = new DoubleProperty(_graph);
      doubleProp->setAllNodeValue(_ui->tableWidget->item(0, 0)->data(Qt::DisplayRole).toDouble());
      doubleProp->setAllEdgeValue(_ui->tableWidget->item(0, 0)->data(Qt::DisplayRole).toDouble());
      b = doubleProp;
    } else {
      auto *stringProp = new StringProperty(_graph);
      DataType *talipotData =
          MetaTypes::qVariantToDataType(_ui->tableWidget->item(0, 0)->data(Qt::DisplayRole));

      if (talipotData == nullptr) {
        qCritical() << "could not convert this type correctly "
                    << _ui->tableWidget->item(0, 0)->data(Qt::DisplayRole)
                    << ", please report this as a bug";
#ifdef NDEBUG
        delete stringProp;
        return;
#endif
      }

      DataTypeSerializer *serializer = DataSet::typenameToSerializer(talipotData->getTypeName());

      if (serializer == nullptr) {
        qCritical() << "no type serializer found for \"" << talipotData->getTypeName().c_str()
                    << "\", please report this as a bug";
#ifdef NDEBUG
        delete stringProp;
        return;
#endif
      }

      stringstream temp;
      serializer->writeData(temp, talipotData);
      QString serializedValue = temp.str().c_str();

      // Tulip serializers add quotes around the serialized value, remove them for comparison
      if (serializedValue.startsWith('"') && serializedValue.endsWith('"')) {
        serializedValue = serializedValue.mid(1, serializedValue.length() - 2);
      }

      stringProp->setAllNodeValue(QStringToTlpString(serializedValue));
      stringProp->setAllEdgeValue(QStringToTlpString(serializedValue));
      b = stringProp;
    }
  } else {
    b = term(_ui->searchTermBCombo);
  }

  op->setProperties(a, b);
  int scopeIndex = _ui->scopeCombo->currentIndex();
  bool onNodes = scopeIndex == 0 || scopeIndex == 1;
  bool onEdges = scopeIndex == 0 || scopeIndex == 2;
  BooleanProperty *result = op->run(_graph, onNodes, onEdges);

  auto *outputInterface = _ui->resultsStorageCombo->model()
                              ->data(_ui->resultsStorageCombo->model()->index(
                                         _ui->resultsStorageCombo->currentIndex(), 0),
                                     Model::PropertyRole)
                              .value<PropertyInterface *>();
  auto *output = static_cast<BooleanProperty *>(outputInterface);

  node n;
  edge e;

  QString searchOpDescription;
  uint resultsCountNodes = 0, resultsCountEdges = 0;

  if (_ui->selectionModeCombo->currentIndex() == 0) { // replace current selection
    output->copy(result);
    searchOpDescription = "found";
    resultsCountNodes = iteratorCount(result->getNodesEqualTo(true));
    resultsCountEdges = iteratorCount(result->getEdgesEqualTo(true));
  } else if (_ui->selectionModeCombo->currentIndex() == 1) { // add to current selection
    if (onNodes) {
      for (auto n : result->getNodesEqualTo(true)) {
        output->setNodeValue(n, true);
        resultsCountNodes++;
      }
    }

    if (onEdges) {
      for (auto e : result->getEdgesEqualTo(true)) {
        output->setEdgeValue(e, true);
        resultsCountEdges++;
      }
    }

    searchOpDescription = "added to selection";
  } else if (_ui->selectionModeCombo->currentIndex() == 2) { // remove from current selection
    if (onNodes) {
      for (auto n : output->getNodesEqualTo(true)) {
        if (result->getNodeValue(n)) {
          output->setNodeValue(n, false);
          resultsCountNodes++;
        }
      }
    }

    if (onEdges) {
      for (auto e : output->getEdgesEqualTo(true)) {
        if (result->getEdgeValue(e)) {
          output->setEdgeValue(e, false);
          resultsCountEdges++;
        }
      }
    }

    searchOpDescription = "removed from selection";
  } else if (_ui->selectionModeCombo->currentIndex() == 3) { // no modification
    output = result;
    searchOpDescription = "found but not added to selection";
    resultsCountNodes = iteratorCount(result->getNodesEqualTo(true));
    resultsCountEdges = iteratorCount(result->getEdgesEqualTo(true));
  }

  Observable::unholdObservers();
  _graph->popIfNoUpdates();

  if (deleteTermB) {
    delete b;
  }

  delete result;

  if (onNodes && !onEdges) {
    _ui->resultsCountLabel->setText(QString::number(resultsCountNodes) + " node(s) " +
                                    searchOpDescription);
  } else if (!onNodes && onEdges) {
    _ui->resultsCountLabel->setText(QString::number(resultsCountEdges) + " edge(s) " +
                                    searchOpDescription);
  } else {
    _ui->resultsCountLabel->setText(QString::number(resultsCountNodes) + " node(s) and " +
                                    QString::number(resultsCountEdges) + " edge(s) " +
                                    searchOpDescription);
  }
}

void SearchWidget::graphIndexChanged() {
  auto *g = _ui->graphCombo->model()
                ->data(_ui->graphCombo->selectedIndex(), Model::GraphRole)
                .value<tlp::Graph *>();
  setGraph(g);
}

void SearchWidget::termAChanged() {
  tlp::PropertyInterface *prop = term(_ui->searchTermACombo);

  // isHidden checks if the widget was set hidden; using isVisible would check if the widget is
  // currently displayed
  if (!_ui->tableWidget->isHidden()) {
    updateEditorWidget();
    updateOperators(prop, _ui->tableWidget->item(0, 0)->data(Qt::DisplayRole).toString());
  } else {
    updateOperators(prop, term(_ui->searchTermBCombo));
  }
}

void SearchWidget::termBChanged() {
  if (_ui->searchTermBCombo->currentIndex() != 0) {
    _ui->tableWidget->hide();
    tlp::PropertyInterface *prop = term(_ui->searchTermBCombo);
    updateOperators(term(_ui->searchTermACombo), prop);
  } else {
    _ui->tableWidget->show();
    updateEditorWidget();
    updateOperators(term(_ui->searchTermACombo),
                    _ui->tableWidget->item(0, 0)->data(Qt::DisplayRole).toString());
  }
}

void SearchWidget::updateOperators(tlp::PropertyInterface *a, tlp::PropertyInterface *b) {
  setNumericOperatorsEnabled(dynamic_cast<tlp::NumericProperty *>(a) != nullptr &&
                             dynamic_cast<tlp::NumericProperty *>(b) != nullptr);
}

void SearchWidget::updateOperators(PropertyInterface *a, const QString &b) {
  bool isCustomValueDouble = false;

  if (b.isEmpty()) {
    isCustomValueDouble = true;
  } else {
    b.toDouble(&isCustomValueDouble);
  }

  setNumericOperatorsEnabled(dynamic_cast<tlp::NumericProperty *>(a) != nullptr &&
                             isCustomValueDouble);
}

void SearchWidget::setNumericOperatorsEnabled(bool e) {
  for (int i = 2; i <= 5; ++i) {
    static_cast<QStandardItemModel *>(_ui->operatorCombo->model())->item(i)->setEnabled(e);

    if (_ui->operatorCombo->currentIndex() == i && !e) {
      _ui->operatorCombo->setCurrentIndex(0);
    }
  }
}

void SearchWidget::updateEditorWidget() {
  QVariant defaultValue;
  tlp::PropertyInterface *prop = term(_ui->searchTermACombo);
  int scopeIndex = _ui->scopeCombo->currentIndex();

  if (scopeIndex == 1 || scopeIndex == 0) {
    defaultValue = GraphModel::nodeDefaultValue(prop);
  } else {
    defaultValue = GraphModel::edgeDefaultValue(prop);
  }

  // workaround for sf bug #716
  // I suspect that because 0 == false (when updating termA from viewMetric
  // from viewSelection) the table item was not properly refreshed.
  // So force the refresh of the table item with defaultValue
  // in first displaying a 'blank' value
  _ui->tableWidget->item(0, 0)->setData(Qt::DisplayRole, QVariant(QString()));
  _ui->tableWidget->item(0, 0)->setData(Qt::DisplayRole, defaultValue);
  _ui->tableWidget->item(0, 0)->setData(Qt::EditRole, defaultValue);
}

void SearchWidget::dragEnterEvent(QDragEnterEvent *dragEv) {
  const auto *mimeType = dynamic_cast<const GraphMimeType *>(dragEv->mimeData());

  if (mimeType != nullptr) {
    dragEv->accept();
  }
}

void SearchWidget::dropEvent(QDropEvent *dropEv) {
  const auto *mimeType = dynamic_cast<const GraphMimeType *>(dropEv->mimeData());

  if (mimeType != nullptr) {
    currentGraphChanged(mimeType->graph());
    dropEv->accept();
  }
}

PropertyInterface *SearchWidget::term(QComboBox *combo) {
  auto *model = static_cast<GraphPropertiesModel<PropertyInterface> *>(combo->model());
  return model->data(model->index(combo->currentIndex(), 0), Model::PropertyRole)
      .value<PropertyInterface *>();
}

SearchOperator *SearchWidget::searchOperator() {
  SearchOperator *op = nullptr;

  if (isNumericComparison()) {
    op = NUMERIC_OPERATORS[_ui->operatorCombo->currentIndex()];
  } else {
    op = _ui->caseSensitivityCheck->isChecked()
             ? STRING_OPERATORS[_ui->operatorCombo->currentIndex()]
             : NOCASE_STRING_OPERATORS[_ui->operatorCombo->currentIndex()];
  }

  return op;
}

bool SearchWidget::isNumericComparison() {
  return static_cast<QStandardItemModel *>(_ui->operatorCombo->model())->item(2)->isEnabled();
}
