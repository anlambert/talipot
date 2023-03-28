/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "talipot/WorkspacePanel.h"
#include "talipot/InteractorConfigWidget.h"
#include "ui_WorkspacePanel.h"

#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QScrollBar>
#include <QPropertyAnimation>

#include <talipot/MetaTypes.h>
#include <talipot/Interactor.h>
#include <talipot/View.h>
#include <talipot/GraphHierarchiesModel.h>
#include <talipot/Mimes.h>
#include <talipot/FontIcon.h>

using namespace tlp;

#ifdef WIN32

class CustomTabBar : public QTabBar {

public:
  CustomTabBar(QWidget *parent = 0) : QTabBar(parent) {
    setDrawBase(false);
  }

protected:
  QSize tabSizeHint(int index) const {
    int width = QTabBar::tabSizeHint(index).width();
    return QSize(width, fontMetrics().horizontalAdvance(tabText(index)) * 2 + iconSize().width());
  }
};

class CustomTabWidget : public QTabWidget {

public:
  CustomTabWidget(QWidget *parent = 0) : QTabWidget(parent) {
    setTabBar(new CustomTabBar());
  }
};

#endif

// ========================

WorkspacePanel::WorkspacePanel(tlp::View *view, QWidget *parent)
    : QFrame(parent), _ui(new Ui::WorkspacePanel),
      _interactorConfigWidget(new InteractorConfigWidget(this)), _view(nullptr),
      _overlayRect(nullptr), _viewConfigurationTabWidget(nullptr),
      _viewConfigurationTabWidgetProxy(nullptr), _viewConfigurationExpanded(false) {
  _ui->setupUi(this);
  _ui->linkButton->setIcon(
      FontIcon::icon(MaterialDesignIcons::LinkVariantOff, QColor(Qt::white), 0.8));
  _ui->dragHandle->setPixmap(
      FontIcon::icon(MaterialDesignIcons::CursorMove, QColor(Qt::white)).pixmap(QSize(16, 16)));
  _ui->closeButton->setIcon(FontIcon::icon(MaterialDesignIcons::Close, QColor(Qt::white)));
  _ui->actionClose->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  _ui->interactorsFrame->installEventFilter(this);
  _ui->dragHandle->setPanel(this);
  _ui->graphCombo->installEventFilter(this);
  connect(_ui->linkButton, &QAbstractButton::toggled, this, &WorkspacePanel::toggleSynchronization);
  connect(_ui->closeButton, &QAbstractButton::clicked, this, &QWidget::close);
  setAttribute(Qt::WA_DeleteOnClose);
  setAutoFillBackground(true);

#ifdef WIN32
  _viewConfigurationTabWidget = new CustomTabWidget();
#else
  _viewConfigurationTabWidget = new QTabWidget();
#endif
  _viewConfigurationTabWidget->setObjectName("ViewConfigurationTabWidget");
  _viewConfigurationTabWidget->setTabsClosable(true);
  connect(_viewConfigurationTabWidget, &QTabWidget::tabCloseRequested, this,
          &WorkspacePanel::hideConfigurationTab);
  _viewConfigurationTabWidget->setTabPosition(QTabWidget::West);
  _viewConfigurationTabWidget->findChild<QTabBar *>()->installEventFilter(this);
  _viewConfigurationTabWidgetProxy = new QGraphicsProxyWidget(view->centralItem());
  _viewConfigurationTabWidgetProxy->installEventFilter(this);
  _viewConfigurationTabWidgetProxy->setWidget(_viewConfigurationTabWidget);
  _viewConfigurationTabWidgetProxy->setZValue(DBL_MAX);
  _interactorConfigWidget->installEventFilter(this);
  setView(view);
}

WorkspacePanel::~WorkspacePanel() {
  delete _ui;
  // because of possible mis-synchronization of Qt events
  // set it to nullptr
  // to avoid any invalid read in the eventFilter method
  _ui = nullptr;

  if (_view != nullptr) {
    disconnect(_view, &QObject::destroyed, this, &WorkspacePanel::viewDestroyed);
    _interactorConfigWidget->clearWidgets();
    delete _view;
    // same as above
    _view = nullptr;
  }
}
void WorkspacePanel::viewDestroyed() {
  if (_view != nullptr) {
    disconnect(_view, &QObject::destroyed, this, &WorkspacePanel::viewDestroyed);
    _interactorConfigWidget->clearWidgets();
    _view = nullptr;
  }

  deleteLater();
}

View *WorkspacePanel::view() const {
  return _view;
}

QString WorkspacePanel::viewName() const {
  return tlp::tlpStringToQString(_view->name());
}

void WorkspacePanel::setView(tlp::View *view) {
  assert(view != nullptr);
  _ui->currentInteractorButton->setChecked(false);

  if (_view != nullptr) {
    disconnect(_view, &QObject::destroyed, this, &WorkspacePanel::viewDestroyed);
    disconnect(_view, &View::graphSet, this, &WorkspacePanel::viewGraphSet);
    disconnect(_view, &View::drawNeeded, this, &WorkspacePanel::drawNeeded);
    delete _view->graphicsView();
  }

  delete _view;

  _view = view;

  QList<Interactor *> compatibleInteractors;
  QList<std::string> interactorNames = InteractorLister::compatibleInteractors(view->name());

  for (const std::string &name : interactorNames) {
    compatibleInteractors << PluginsManager::getPluginObject<Interactor>(name);
  }

  connect(_view, &QObject::destroyed, this, &WorkspacePanel::viewDestroyed);
  connect(_view, &View::graphSet, this, &WorkspacePanel::viewGraphSet);
  connect(_view, &View::drawNeeded, this, &WorkspacePanel::drawNeeded);
  connect(_view, &View::interactorsChanged, this, &WorkspacePanel::refreshInteractorsToolbar);
  _view->graphicsView()->scene()->installEventFilter(this);

  _viewConfigurationTabWidget->clear();
  if (!_view->configurationWidgets().empty()) {
    for (auto *w : _view->configurationWidgets()) {
      w->installEventFilter(this);
      w->resize(w->width(), w->sizeHint().height());
      _viewConfigurationTabWidget->addTab(w, w->windowTitle());
    }
    if (!compatibleInteractors.empty()) {
      _viewConfigurationTabWidget->addTab(_interactorConfigWidget, "Interactor");
    }
  }

  _view->setInteractors(compatibleInteractors);
  _ui->scrollArea->setVisible(!compatibleInteractors.empty());
  _view->graphicsView()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  _view->graphicsView()->addAction(_ui->actionClose);
  layout()->addWidget(_view->graphicsView());
  refreshInteractorsToolbar();

  if (!compatibleInteractors.empty()) {
    setCurrentInteractor(compatibleInteractors[0]);
  } else {
    _ui->currentInteractorWidget->hide();
    _ui->sep4->hide();
  }

  resetInteractorsScrollButtonsVisibility();
}

// Workaround to avoid a Qt5 bug :
// After the panels containing QGraphicsView objects were rearranged in the workspace,
// some events were no more sent to the QGraphicsWidget objects embedded in the asoociated
// QGraphicScene objects.
// Those events are necessary for important parts of the view GUI (context menu, keyboard focus) to
// work correctly.
// So add a hack that, each time a view is shown, creates a new QGraphicsScene object
// and refill it with QGraphicsItem objects contained in the previous one.
// Seems to be the only way to workaround that issue.
void WorkspacePanel::showEvent(QShowEvent *event) {
  QFrame::showEvent(event);

  if (_view && _view->graphicsView() && _view->graphicsView()->scene()) {
    // first remove central item of the scene and its children
    _view->graphicsView()->scene()->removeItem(_view->centralItem());
    // get remaining items (if any) that were not descendant of the central item
    // and remove it from the scene
    QList<QGraphicsItem *> items = _view->graphicsView()->scene()->items();

    for (auto *item : items) {
      _view->graphicsView()->scene()->removeItem(item);
    }

    // get old scene pointer for further deletion
    QGraphicsScene *oldScene = _view->graphicsView()->scene();
    auto *newScene = new QGraphicsScene();
    newScene->setSceneRect(oldScene->sceneRect());
    // create a new QGraphicsScene and set it in the QGraphicsView
    _view->graphicsView()->setScene(newScene);
    // restore central item and its children in the new scene
    _view->graphicsView()->scene()->addItem(_view->centralItem());

    // restore remaining items in the new scene
    for (auto *item : items) {
      _view->graphicsView()->scene()->addItem(item);
    }

    // set event filter for the new scene
    _view->graphicsView()->installEventFilter(this);
    // restore any specific behavior of the QGraphicsScene
    _view->resetGraphicsScene();

    // delete old scene
    delete oldScene;
  }
}

void WorkspacePanel::closeEvent(QCloseEvent *event) {
  if (_view->checkOnClose()) {
    event->accept();
  } else {
    event->ignore();
  }
}

bool WorkspacePanel::eventFilter(QObject *obj, QEvent *ev) {
  // we must check _ui has not been deleted
  // because of possible mis-synchronization of Qt events
  if (_ui != nullptr) {
    if (_view != nullptr) {
      if (ev->type() == QEvent::ContextMenu) {
        _view->showContextMenu(QCursor::pos(), static_cast<QContextMenuEvent *>(ev)->pos());
      } else if (_viewConfigurationTabWidgetProxy != nullptr &&
                 _viewConfigurationTabWidget->indexOf(qobject_cast<QWidget *>(obj)) != -1) {
        ev->accept();
        return true;

      } else if (ev->type() == QEvent::MouseButtonPress && !_viewConfigurationExpanded &&
                 qobject_cast<QTabBar *>(obj) != nullptr) {
        setConfigurationTabExpanded(true);
      } else if (ev->type() == QEvent::Wheel && qobject_cast<QTabBar *>(obj) != nullptr) {
        return true;
      }
    }

    if (obj == _ui->interactorsFrame && ev->type() == QEvent::Wheel) {
      if (static_cast<QWheelEvent *>(ev)->angleDelta().y() > 0) {
        scrollInteractorsLeft();
      } else {
        scrollInteractorsRight();
      }
    }

    if (obj == _ui->graphCombo && ev->type() == QEvent::Wheel) {
      return true;
    }
  }

  return QWidget::eventFilter(obj, ev);
}

void WorkspacePanel::setCurrentInteractor(tlp::Interactor *interactor) {
  view()->setCurrentInteractor(interactor);
  _ui->currentInteractorButton->setIcon(interactor->action()->icon());
  updateCurrentInteractorButtonText();
  _ui->currentInteractorButton->setToolTip(
      "Active tool:<br/><b>" + interactor->action()->text() +
      QString(interactor->configurationWidget()
                  ? "</b><br/><i>click to show/hide its configuration panel.</i>"
                  : "</b>"));
  if (_interactorConfigWidget->setWidgets(interactor)) {
    _viewConfigurationTabWidget->setTabEnabled(
        _viewConfigurationTabWidget->indexOf(_interactorConfigWidget), true);
  } else {
    _viewConfigurationTabWidget->setTabEnabled(
        _viewConfigurationTabWidget->indexOf(_interactorConfigWidget), false);
  }
}

void WorkspacePanel::setCurrentInteractorConfigurationVisible(bool) {
  if (_view->currentInteractor() == nullptr) {
    return;
  }
  _viewConfigurationTabWidget->setCurrentWidget(_interactorConfigWidget);
  setConfigurationTabExpanded(true);
}

void WorkspacePanel::interactorActionTriggered() {
  auto *action = static_cast<QAction *>(sender());
  auto *interactor = static_cast<Interactor *>(action->parent());

  if (interactor == view()->currentInteractor()) {
    return;
  }

  setCurrentInteractor(interactor);
}

void WorkspacePanel::hideConfigurationTab() {
  setConfigurationTabExpanded(false);
}

void WorkspacePanel::refreshInteractorsToolbar() {
  _actionTriggers.clear();
  auto compatibleInteractors = _view->interactors();

  if (_ui->interactorsFrame->layout()) {
    clearLayout(_ui->interactorsFrame->layout());
  }

  delete _ui->interactorsFrame->layout();
  bool interactorsUiShown = !compatibleInteractors.isEmpty();
  _ui->currentInteractorButton->setVisible(interactorsUiShown);
  _ui->interactorsFrame->setVisible(interactorsUiShown);
  _ui->sep2->setVisible(interactorsUiShown);

  if (interactorsUiShown) {
    auto *interactorsLayout = new QHBoxLayout;
    interactorsLayout->setContentsMargins(0, 0, 0, 0);
    interactorsLayout->setSpacing(4);

    for (auto *i : compatibleInteractors) {
      auto *button = new QPushButton();
      button->setMinimumSize(22, 22);
      button->setFlat(true);
      button->setMaximumSize(22, 22);
      button->setIcon(i->action()->icon());
      button->setToolTip(i->action()->text());
      interactorsLayout->addWidget(button);
      button->setEnabled(i->action()->isEnabled());
      connect(button, &QAbstractButton::clicked, i->action(), &QAction::trigger);
      connect(i->action(), &QAction::triggered, this, &WorkspacePanel::interactorActionTriggered);
      connect(i->action(), &QAction::changed, this, &WorkspacePanel::actionChanged);
      _actionTriggers[i->action()] = button;
    }

    _ui->interactorsFrame->setLayout(interactorsLayout);
    setCurrentInteractor(compatibleInteractors[0]);
  }
}

void WorkspacePanel::actionChanged() {
  auto *action = static_cast<QAction *>(sender());

  if (!_actionTriggers.contains(action)) {
    return;
  }

  _actionTriggers[action]->setEnabled(action->isEnabled());
}

void WorkspacePanel::scrollInteractorsRight() {
  QScrollBar *scrollBar = _ui->scrollArea->horizontalScrollBar();
  scrollBar->setSliderPosition(scrollBar->sliderPosition() + scrollBar->singleStep());
  if (scrollBar->sliderPosition() == scrollBar->maximum()) {
    _ui->interactorsRight->setEnabled(false);
  }
  if (scrollBar->sliderPosition() > scrollBar->minimum()) {
    _ui->interactorsLeft->setEnabled(true);
  }
}

void WorkspacePanel::scrollInteractorsLeft() {
  QScrollBar *scrollBar = _ui->scrollArea->horizontalScrollBar();
  scrollBar->setSliderPosition(scrollBar->sliderPosition() - scrollBar->singleStep());
  if (scrollBar->sliderPosition() < scrollBar->maximum()) {
    _ui->interactorsRight->setEnabled(true);
  }
  if (scrollBar->sliderPosition() == scrollBar->minimum()) {
    _ui->interactorsLeft->setEnabled(false);
  }
}

void WorkspacePanel::resetInteractorsScrollButtonsVisibility() {
  QScrollBar *scrollBar = _ui->scrollArea->horizontalScrollBar();
  bool interactorScrollBtnVisible = scrollBar->minimum() != scrollBar->maximum();
  _ui->interactorsLeft->setVisible(interactorScrollBtnVisible);
  _ui->interactorsRight->setVisible(interactorScrollBtnVisible);
  _ui->interactorsRight->setEnabled(scrollBar->sliderPosition() != scrollBar->maximum());
  _ui->interactorsLeft->setEnabled(scrollBar->sliderPosition() != scrollBar->minimum());
}

void WorkspacePanel::setGraphsModel(tlp::GraphHierarchiesModel *model) {
  _ui->graphCombo->setModel(model);
  connect(_ui->graphCombo, &TreeViewComboBox::currentItemChanged, this,
          &WorkspacePanel::graphComboIndexChanged);
}

void WorkspacePanel::viewGraphSet(tlp::Graph *g) {
  assert(dynamic_cast<tlp::GraphHierarchiesModel *>(_ui->graphCombo->model()));
#ifndef NDEBUG

  if (g) {
    qDebug() << "Setting graph " << tlp::tlpStringToQString(g->getName()) << " for panel "
             << windowTitle();
  }

#endif // NDEBUG

  auto *model = static_cast<tlp::GraphHierarchiesModel *>(_ui->graphCombo->model());
  QModelIndex graphIndex = model->indexOf(g);

  if (graphIndex == _ui->graphCombo->selectedIndex()) {
    return;
  }

  _ui->graphCombo->selectIndex(graphIndex);
}

void WorkspacePanel::graphComboIndexChanged() {
  auto *g = _ui->graphCombo->model()
                ->data(_ui->graphCombo->selectedIndex(), Model::GraphRole)
                .value<tlp::Graph *>();
#ifndef NDEBUG

  if (g != nullptr) {
    qDebug() << "selecting graph " << tlp::tlpStringToQString(g->getName()) << " in view";
  }

#endif /* NDEBUG */

  if (g != nullptr && _view != nullptr && g != _view->graph()) {
    _view->setGraph(g);
  }
}

void WorkspacePanel::resizeEvent(QResizeEvent *ev) {
  if (_viewConfigurationTabWidgetProxy) {
    setConfigurationTabExpanded(_viewConfigurationExpanded, false);
  }

  resetInteractorsScrollButtonsVisibility();

  QWidget::resizeEvent(ev);
  updateCurrentInteractorButtonText();
}

void WorkspacePanel::setConfigurationTabExpanded(bool expanded, bool animate) {

  if (_view != nullptr) {
    _viewConfigurationTabWidgetProxy->setMinimumHeight(_view->graphicsView()->height());
    _viewConfigurationTabWidgetProxy->setMaximumHeight(_view->graphicsView()->height());
    _viewConfigurationTabWidgetProxy->setMaximumWidth(_view->graphicsView()->width());
  }

  QPointF newPos = configurationTabPosition(expanded);

  if (newPos == _viewConfigurationTabWidgetProxy->pos()) {
    return;
  }

  if (animate) {
    auto *anim = new QPropertyAnimation(_viewConfigurationTabWidgetProxy, "pos",
                                        _viewConfigurationTabWidgetProxy);
    anim->setDuration(250);
    anim->setStartValue(_viewConfigurationTabWidgetProxy->pos());
    anim->setEndValue(newPos);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
  } else {
    _viewConfigurationTabWidgetProxy->setPos(newPos);
  }

  // there are artefacts in the fonts when the opacity is 1; ugly fix
  _viewConfigurationTabWidgetProxy->setOpacity((expanded ? 0.99 : 0.6));

  if (!expanded && _viewConfigurationExpanded) {
    _view->applySettings();
  }

  _viewConfigurationExpanded = expanded;
}

QPointF WorkspacePanel::configurationTabPosition(bool expanded) const {
  if (expanded) {
    return QPointF(width() - _viewConfigurationTabWidgetProxy->size().width(), 10);
  } else {
    auto *tabWidget = static_cast<QTabWidget *>(_viewConfigurationTabWidgetProxy->widget());
    int tabWidth =
        (tabWidget != nullptr)
            ? (_viewConfigurationTabWidgetProxy->size().width() - tabWidget->widget(0)->width())
            : 0;
    return QPointF(width() - tabWidth, 10);
  }
}

void WorkspacePanel::setOverlayMode(bool m) {
  if (m && _overlayRect == nullptr) {
    _overlayRect = new QGraphicsRectItem(_view->graphicsView()->sceneRect());
    _overlayRect->setBrush(QColor::fromHsv(0, 0, 0, 50));
    _overlayRect->setPen(QColor(67, 86, 108));
    _view->graphicsView()->scene()->addItem(_overlayRect);
    _overlayRect->setZValue(30);
  }

  if (!m && _overlayRect != nullptr) {
    delete _overlayRect;
    _overlayRect = nullptr;
  }
}

void WorkspacePanel::setHighlightMode(bool hm) {
  setProperty("focused", hm);
  qApp->style()->unpolish(qApp);
  qApp->style()->polish(qApp);
  update();
}

void WorkspacePanel::dragEnterEvent(QDragEnterEvent *evt) {
  handleDragEnterEvent(evt, evt->mimeData());
}

void WorkspacePanel::dropEvent(QDropEvent *evt) {
  handleDropEvent(evt->mimeData());
}

void WorkspacePanel::dragLeaveEvent(QDragLeaveEvent *) {
  setOverlayMode(false);
}

bool WorkspacePanel::handleDragEnterEvent(QEvent *e, const QMimeData *mimedata) {
  if (dynamic_cast<const GraphMimeType *>(mimedata) != nullptr ||
      dynamic_cast<const PanelMimeType *>(mimedata) != nullptr ||
      dynamic_cast<const AlgorithmMimeType *>(mimedata) != nullptr) {
    setOverlayMode(true);
    e->accept();
    return true;
  }

  return false;
}

bool WorkspacePanel::handleDropEvent(const QMimeData *mimedata) {
  const auto *graphMime = dynamic_cast<const GraphMimeType *>(mimedata);
  const auto *panelMime = dynamic_cast<const PanelMimeType *>(mimedata);
  const auto *algorithmMime = dynamic_cast<const AlgorithmMimeType *>(mimedata);

  if (graphMime != nullptr && graphMime->graph()) {
    viewGraphSet(graphMime->graph());
  } else if (panelMime) {
    // Emit swap panels
    emit swapWithPanels(panelMime->panel());
  } else if (algorithmMime) {
    algorithmMime->run(view()->graph());
  }

  setOverlayMode(false);
  return graphMime != nullptr || panelMime != nullptr || algorithmMime != nullptr;
}

bool WorkspacePanel::isGraphSynchronized() const {
  return _ui->linkButton->isChecked();
}

void WorkspacePanel::toggleSynchronization(bool f) {
  if (f) {
    _ui->linkButton->setIcon(
        FontIcon::icon(MaterialDesignIcons::LinkVariant, QColor(Qt::white), 0.8));
    _ui->linkButton->setToolTip("Click here to disable the synchronization with the Graphs "
                                "panel.\nWhen synchronization is enabled, the current graph of the "
                                "Graphs panel,\nbecomes the current one in the workspace active "
                                "panel.");
  } else {
    _ui->linkButton->setIcon(
        FontIcon::icon(MaterialDesignIcons::LinkVariantOff, QColor(Qt::white), 0.8));
    _ui->linkButton->setToolTip("Click here to enable the synchronization with the Graphs "
                                "panel.\nWhen synchronization is enabled, the current graph of the "
                                "Graphs panel,\nbecomes the current one in the workspace active "
                                "panel.");
  }

  emit changeGraphSynchronization(f);
}

void WorkspacePanel::updateCurrentInteractorButtonText() {
  if (!_view || !_view->currentInteractor()) {
    return;
  }
  auto fm = fontMetrics();
  auto *interactor = _view->currentInteractor();
  auto text = interactor->action()->text();
  int width = _ui->sep4->pos().x() - 20;
// QToolButton text is automatically elided by the middle on windows
#ifndef Q_OS_WIN
  while (fm.boundingRect(text).width() < width - 10) {
    text += " ";
  }
  text = fm.elidedText(text, Qt::ElideRight, width);
  text = text.replace("  …", "  ");
#else
  while (fm.boundingRect(text).width() < width - 20) {
    text += " ";
  }
#endif
  _ui->currentInteractorButton->setText(text);
}
