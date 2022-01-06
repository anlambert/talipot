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

#include "AlgorithmRunner.h"

#include "ExpandableGroupBox.h"

#include <QActionGroup>
#include <QDropEvent>
#include <QPainter>
#include <QToolButton>
#include <QMenu>

#include <talipot/Mimes.h>
#include <talipot/Settings.h>

struct FavoriteBox : public ExpandableGroupBox {
  bool _droppingFavorite;

  explicit FavoriteBox(QWidget *parent = nullptr, const QString &title = QString())
      : ExpandableGroupBox(parent, title), _droppingFavorite(false) {}

protected:
  void paintEvent(QPaintEvent *event) override {
    ExpandableGroupBox::paintEvent(event);
    QPainter painter(this);
    QPixmap px((_droppingFavorite ? ":/talipot/app/icons/svg/star.svg"
                                  : ":/talipot/app/icons/svg/star-outline.svg"));
    painter.drawPixmap(20, -2, px);
  }
};

#include "ui_AlgorithmRunner.h"

using namespace tlp;

static ExpandableGroupBox *createGroupBox(QString name, bool root = false) {
  auto *groupBox = new ExpandableGroupBox(nullptr, name);
  groupBox->setObjectName(name);
  groupBox->setProperty("root", root);
  auto *groupWidget = new QWidget();
  groupWidget->setLayout(new QVBoxLayout);
  groupWidget->layout()->setContentsMargins(0, 15, 0, 6);
  groupWidget->layout()->setSpacing(0);
  groupBox->setWidget(groupWidget);
  return groupBox;
}

void AlgorithmRunner::buildTreeUi(QWidget *w, PluginModel<tlp::Algorithm> *model,
                                  const QModelIndex &parent, bool root) {
  for (int i = 0; i < model->rowCount(parent); ++i) {
    QModelIndex index = model->index(i, 0, parent);
    QString name = model->data(index).toString();

    if (model->rowCount(index) > 0) {
      ExpandableGroupBox *groupBox = createGroupBox(name, root);
      w->layout()->addWidget(groupBox);
      buildTreeUi(groupBox->widget(), model, index);
    } else {
      auto *item = new AlgorithmRunnerItem(name);
      QObject::connect(this, QOverload<bool>::of(&AlgorithmRunner::setStoreResultAsLocal), item,
                       &AlgorithmRunnerItem::setStoreResultAsLocal);
      w->layout()->addWidget(item);
    }
  }
}

void AlgorithmRunner::insertItem(QWidget *w, const QString &name) const {

  const Plugin &plugin = PluginsManager::pluginInformation(QStringToTlpString(name));
  QString category = plugin.category().c_str();
  QString group = plugin.group().c_str();

  ExpandableGroupBox *categoryBox = nullptr, *groupBox = nullptr;

  for (auto *box : w->findChildren<ExpandableGroupBox *>()) {
    if (box->objectName() == category) {
      categoryBox = box;
      break;
    }
  }

  if (categoryBox == nullptr) {
    return;
  }

  if (!group.isEmpty()) {
    for (auto *box : categoryBox->findChildren<ExpandableGroupBox *>()) {
      if (box->objectName() == group) {
        groupBox = box;
        break;
      }
    }
  }

  if (!group.isEmpty() && groupBox == nullptr) {
    groupBox = createGroupBox(group);
    auto *categoryLayout = static_cast<QVBoxLayout *>(categoryBox->widget()->layout());
    int index = 0;

    while (index < categoryLayout->count()) {
      auto *gb = dynamic_cast<ExpandableGroupBox *>(categoryLayout->itemAt(index)->widget());

      if (gb && group < gb->title()) {
        break;
      }

      ++index;
    }

    categoryLayout->insertWidget(index, groupBox);
  } else if (group.isEmpty()) {
    groupBox = categoryBox;
  }

  auto *item = new AlgorithmRunnerItem(name);
  QObject::connect(this, QOverload<bool>::of(&AlgorithmRunner::setStoreResultAsLocal), item,
                   &AlgorithmRunnerItem::setStoreResultAsLocal);
  QObject::connect(item, &AlgorithmRunnerItem::favorized, this, &AlgorithmRunner::favorized);
  auto *groupLayout = static_cast<QVBoxLayout *>(groupBox->widget()->layout());
  int index = 0;

  while (index < groupLayout->count()) {
    auto *i = dynamic_cast<AlgorithmRunnerItem *>(groupLayout->itemAt(index)->widget());

    if (i && name < i->name()) {
      break;
    }

    ++index;
  }

  groupLayout->insertWidget(index, item);
}

void AlgorithmRunner::refreshTreeUi(QWidget *w) {
  QStringList visibleItems;

  for (auto *ari : w->findChildren<AlgorithmRunnerItem *>()) {
    if (PluginsManager::pluginExists(QStringToTlpString(ari->name()))) {
      visibleItems.push_back(ari->name());
    } else {
      _favorites.removeAll(ari);
      delete ari;
    }
  }

  for (auto *gb : w->findChildren<ExpandableGroupBox *>()) {
    if (!gb->property("root").toBool() && gb->findChildren<AlgorithmRunnerItem *>().empty()) {
      delete gb;
    }
  }

  std::list<std::string> installedPlugins = PluginsManager::availablePlugins<tlp::Algorithm>();

  for (const auto &name : installedPlugins) {
    if (!visibleItems.contains(name.c_str())) {
      insertItem(w, name.c_str());
    }
  }
}

AlgorithmRunner::AlgorithmRunner(QWidget *parent)
    : QWidget(parent), _ui(new Ui::AlgorithmRunner), _graph(nullptr) {
  _ui->setupUi(this);
  _ui->favoritesBox->setWidget(new QWidget());
  _ui->favoritesBox->widget()->setAcceptDrops(true);
  _ui->favoritesBox->widget()->setMinimumHeight(45);
  _ui->favoritesBox->widget()->setLayout(new QVBoxLayout);
  _ui->favoritesBox->widget()->layout()->setContentsMargins(0, 15, 0, 5);
  _ui->favoritesBox->widget()->layout()->setSpacing(5);
  _ui->favoritesBox->widget()->installEventFilter(this);

  _ui->contents->setEnabled(false);
  _storeResultAsLocalButton = new QToolButton(_ui->header);
  _storeResultAsLocalButton->setMaximumSize(23, 23);
  _storeResultAsLocalButton->setMinimumSize(23, 23);
  _storeResultAsLocalButton->setIcon(
      FontIcon::icon(MaterialDesignIcons::Tournament, QColor(Qt::white), 0.7, -90));
  _storeResultAsLocalButton->setIconSize(QSize(23, 23));
  _storeResultAsLocalButton->setStyleSheet("QToolButton::menu-indicator { image: none; }");
  _storeResultAsLocalButton->setToolTip(
      "Choose the storage policy for the result of property algorithms\nWhen they are "
      "applied to a subgraph, this result can be stored either\n- in a local subgraph "
      "property (created on the fly if needed),\nor\n- in a property already existing in "
      "the ascendant hierarchy (inherited or local).");
  _ui->header->layout()->addWidget(_storeResultAsLocalButton);
  auto *resultMenu = new QMenu(this);
  _resultAsLocalPropAction =
      resultMenu->addAction("Always store result in a local property of the graph");
  _resultAsLocalPropAction->setIconVisibleInMenu(true);
  _resultAsLocalPropAction->setCheckable(true);
  QAction *resultAsPredefinedPropAction =
      resultMenu->addAction("Store result in an existing property of the graphs hierarchy");
  resultAsPredefinedPropAction->setIconVisibleInMenu(true);
  resultAsPredefinedPropAction->setCheckable(true);
  auto *resultGroup = new QActionGroup(resultMenu);
  resultGroup->addAction(_resultAsLocalPropAction);
  resultGroup->addAction(resultAsPredefinedPropAction);
  _resultAsLocalPropAction->setChecked(true);
  _storeResultAsLocalButton->setMenu(resultMenu);
  _storeResultAsLocalButton->setPopupMode(QToolButton::InstantPopup);
  connect(resultMenu, &QMenu::triggered, this,
          QOverload<QAction *>::of(&AlgorithmRunner::setStoreResultAsLocal));

  buildPluginsList();

  connect(_ui->header, &HeaderFrame::expanded, this, &AlgorithmRunner::expanded);
}

void AlgorithmRunner::buildPluginsList() {
  clearLayout(_ui->contents->layout(), true);
  clearLayout(_ui->favoritesBox->widget()->layout(), true);
  _favorites.clear();
  PluginModel<tlp::Algorithm> model;
  buildTreeUi(_ui->contents, &model, QModelIndex(), true);
  _ui->contents->layout()->addItem(
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

  for (auto *ari : findChildren<AlgorithmRunnerItem *>()) {
    connect(ari, &AlgorithmRunnerItem::favorized, this, &AlgorithmRunner::favorized);
  }

  for (const QString &a : Settings::favoriteAlgorithms()) {
    addFavorite(a);
  }
}

AlgorithmRunner::~AlgorithmRunner() {
  delete _ui;
}

void AlgorithmRunner::setGraph(Graph *g) {
  _ui->contents->setEnabled(g != nullptr);
  _graph = g;

  for (auto *item : findChildren<AlgorithmRunnerItem *>()) {
    if (item->graph() != g) {
      item->setGraph(g);
    }
  }
}

void AlgorithmRunner::findPlugins() {
  _ui->searchBox->selectAll();
  _ui->searchBox->setFocus(Qt::ShortcutFocusReason);
}

void AlgorithmRunner::refreshPluginsList() {
  refreshTreeUi(_ui->contents);
  setGraph(_graph);
}

void AlgorithmRunner::favorized(bool f) {
  auto *item = static_cast<AlgorithmRunnerItem *>(sender());

  if (f) {
    addFavorite(item->name(), item->data());
  } else {
    removeFavorite(item->name());
  }
}

// A not recursive implementation of QObject::findChildren
template <typename T>
QList<T> childrenObj(QObject *obj) {
  QList<T> result;

  for (auto *o : obj->children()) {
    T var = dynamic_cast<T>(o);

    if (var != nullptr)
      result += var;
  }

  return result;
}
bool filterGroup(ExpandableGroupBox *group, QString filter) {
  QList<ExpandableGroupBox *> subGroups = childrenObj<ExpandableGroupBox *>(group->widget());
  QList<AlgorithmRunnerItem *> subItems = childrenObj<AlgorithmRunnerItem *>(group->widget());

  if (group->title().contains(filter, Qt::CaseInsensitive)) {
    group->show();

    for (auto *g : subGroups) {
      g->show();
      subItems += childrenObj<AlgorithmRunnerItem *>(g->widget());
    }

    for (auto *ari : subItems) {
      ari->show();
    }

    return true;
  }

  bool groupVisible = false;

  for (auto *g : subGroups) {
    groupVisible |= filterGroup(g, filter);
  }

  for (auto *ari : subItems) {
    bool itemVisible = ari->name().contains(filter, Qt::CaseInsensitive);
    ari->setVisible(itemVisible);
    groupVisible |= itemVisible;
  }

  group->setVisible(groupVisible);
  return groupVisible;
}

void AlgorithmRunner::setFilter(QString filter) {
  for (auto *group : childrenObj<ExpandableGroupBox *>(_ui->contents)) {
    if (group != _ui->favoritesBox)
      filterGroup(group, filter);
  }

  filterGroup(_ui->favoritesBox, filter);
}

bool AlgorithmRunner::eventFilter(QObject *obj, QEvent *ev) {
  bool draggableObject = obj == _ui->favoritesBox->widget() ||
                         _favorites.contains(dynamic_cast<AlgorithmRunnerItem *>(obj));

  if (ev->type() == QEvent::Paint) {
    if (obj == _ui->favoritesBox->widget() && _favorites.empty()) {
      QPainter painter(_ui->favoritesBox->widget());
      QFont f;
      f.setItalic(true);
      painter.setFont(f);
      painter.setBrush(QColor(107, 107, 107));
      painter.setPen(QColor(107, 107, 107));
      painter.drawText(0, 20, _ui->favoritesBox->widget()->width(), 65535, Qt::TextWordWrap,
                       "Put your favorite algorithms here");
    }
  } else if ((ev->type() == QEvent::DragEnter || ev->type() == QEvent::DragMove) &&
             draggableObject) {
    auto *dropEv = static_cast<QDropEvent *>(ev);

    if (dynamic_cast<const AlgorithmMimeType *>(dropEv->mimeData()) != nullptr) {
      _ui->favoritesBox->_droppingFavorite = true;
      ev->accept();
      _ui->favoritesBox->repaint();
    }

    return true;
  } else if (ev->type() == QEvent::DragLeave && draggableObject) {
    _ui->favoritesBox->_droppingFavorite = false;
    _ui->favoritesBox->repaint();
  } else if (ev->type() == QEvent::Drop && draggableObject) {
    auto *dropEv = static_cast<QDropEvent *>(ev);
    const auto *mime = dynamic_cast<const AlgorithmMimeType *>(dropEv->mimeData());

    if (mime != nullptr) {
      addFavorite(mime->algorithm(), mime->params());
    }

    _ui->favoritesBox->_droppingFavorite = false;
    _ui->favoritesBox->repaint();
  }

  return false;
}

void AlgorithmRunner::removeFavorite(const QString &algName) {
  for (auto *ari : _favorites) {
    if (ari->name() == algName) {
      _favorites.removeAll(ari);
      ari->deleteLater();

      for (auto *item : findChildren<AlgorithmRunnerItem *>()) {

        if (item != ari && item->name() == algName) {
          item->setFavorite(false);
        }
      }

      break;
    }
  }

  Settings::removeFavoriteAlgorithm(algName);

  if (_favorites.isEmpty()) {
    _ui->favoritesBox->widget()->setMinimumHeight(45);
  }
}

void AlgorithmRunner::addFavorite(const QString &algName, const DataSet &data) {
  if (!PluginsManager::pluginExists(QStringToTlpString(algName))) {
    return;
  }

  Settings::addFavoriteAlgorithm(algName);

  for (auto ar : _favorites) {
    if (ar->name() == algName) {
      return;
    }
  }

  _ui->favoritesBox->widget()->setMinimumHeight(0);
  auto *item = new AlgorithmRunnerItem(algName);
  item->setGraph(_graph);

  if (!data.empty()) {
    item->setData(data);
  }

  item->setFavorite(true);
  int itemPos = 0;

  for (auto *ari : _ui->favoritesBox->widget()->findChildren<AlgorithmRunnerItem *>()) {
    if (ari->name() > item->name()) {
      break;
    }

    ++itemPos;
  }

  static_cast<QBoxLayout *>(_ui->favoritesBox->widget()->layout())->insertWidget(itemPos, item);

  _favorites += item;

  item->installEventFilter(this);

  item->setAcceptDrops(true);

  connect(item, &AlgorithmRunnerItem::favorized, this, &AlgorithmRunner::favorized);

  for (auto *ari : findChildren<AlgorithmRunnerItem *>()) {
    if (ari != item && ari->name() == algName) {
      ari->setFavorite(true);
    }
  }
}

void AlgorithmRunner::setStoreResultAsLocal(QAction *action) {
  _storeResultAsLocalButton->setIcon(action->icon());
  emit setStoreResultAsLocal(action == _resultAsLocalPropAction);
}

void AlgorithmRunner::expanded(bool) {
  // There is a strange and unexplainable side effect
  // which displays the _storeResultAsLocalButton->menu()
  // when this panel is expanding.
  // So we add the code below to prevent this
  _storeResultAsLocalButton->menu()->hide();
}

void AlgorithmRunner::resizeEvent(QResizeEvent *) {
  for (auto *item : findChildren<AlgorithmRunnerItem *>()) {
    item->elideAlgorithmButtonText(_ui->contents->width());
  }
}