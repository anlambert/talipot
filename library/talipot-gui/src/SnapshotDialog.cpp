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

#include "talipot/SnapshotDialog.h"
#include "ui_SnapshotDialog.h"

#include <QMessageBox>
#include <QImageWriter>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QClipboard>
#include <QGraphicsScene>
#include <QPushButton>

#include <talipot/View.h>
#include <talipot/GlOffscreenRenderer.h>
#include <talipot/FontIcon.h>

using namespace std;

namespace tlp {

class LinkWidget : public QWidget {
public:
  LinkWidget() : QWidget(), linked(true), alwaysLinked(false) {
    installEventFilter(this);
    auto *topLabel = new QLabel();
    QPixmap topPixmap = FontIcon::icon(MaterialDesignIcons::AlphaL, 1.0, 0.0, QPointF(-1, -6))
                            .pixmap(20, 20)
                            .transformed(QTransform().scale(-1, -1));
    topLabel->setPixmap(topPixmap);
    auto *bottomLabel = new QLabel();
    QPixmap bottomPixmap = FontIcon::icon(MaterialDesignIcons::AlphaL, 1.0, 0.0, QPointF(-1, -6))
                               .pixmap(20, 20)
                               .transformed(QTransform().scale(-1, 1));
    bottomLabel->setPixmap(bottomPixmap);
    linkLabel = new QLabel();
    linkPixmap = FontIcon::icon(MaterialDesignIcons::Link, 1.0, 0.0, QPointF(0, -3))
                     .pixmap(20, 20)
                     .transformed(QTransform().rotate(90));
    unlinkPixmap = FontIcon::icon(MaterialDesignIcons::LinkOff, 1.0, 0.0, QPointF(0, -3))
                       .pixmap(20, 20)
                       .transformed(QTransform().rotate(90));
    linkLabel->setPixmap(linkPixmap);
    auto *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(topLabel);
    layout->addWidget(linkLabel);
    layout->addWidget(bottomLabel);
    setLayout(layout);
  }

  bool isLinked() const {
    return linked || alwaysLinked;
  }

  void setAlwaysLinked(bool alwaysLinked) {
    this->alwaysLinked = alwaysLinked;

    if (alwaysLinked) {
      linkLabel->setPixmap(linkPixmap);
    }
  }

protected:
  bool eventFilter(QObject *, QEvent *event) override {
    if (event->type() == QEvent::MouseButtonRelease && !alwaysLinked) {
      if (linked) {
        linkLabel->setPixmap(unlinkPixmap);
        linked = false;
      } else {
        linkLabel->setPixmap(linkPixmap);
        linked = true;
      }

      return true;
    }

    return false;
  }

  bool linked;
  bool alwaysLinked;
  QLabel *linkLabel;
  QPixmap linkPixmap;
  QPixmap unlinkPixmap;
};

SnapshotDialog::SnapshotDialog(const View *v, QWidget *parent)
    : QDialog(parent), ui(new Ui::SnapshotDialog()), view(v), ratio(-1),
      inSizeSpinBoxValueChanged(false) {
  ui->setupUi(this);

  GlOffscreenRenderer::instance().makeOpenGLContextCurrent();
  int maxTextureSize = 0;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
  GlOffscreenRenderer::instance().doneOpenGLContextCurrent();

  // restrict snapshot width and height to the half of the GL_MAX_TEXTURE_SIZE value
  ui->widthSpinBox->setMaximum(maxTextureSize / 2);
  ui->heightSpinBox->setMaximum(maxTextureSize / 2);

  ui->widthSpinBox->setValue(view->centralItem()->scene()->sceneRect().width());
  ui->heightSpinBox->setValue(view->centralItem()->scene()->sceneRect().height());

  connect(ui->widthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &SnapshotDialog::widthSpinBoxValueChanged);
  connect(ui->heightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &SnapshotDialog::heightSpinBoxValueChanged);

  QPushButton *copyButton = ui->buttonBox->button(QDialogButtonBox::Apply);
  copyButton->setText("&Copy to clipboard");
  copyButton->setIcon(FontIcon::icon(MaterialDesignIcons::ContentCopy));
  ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)
      ->setIcon(FontIcon::icon(MaterialDesignIcons::Restore));
  ui->buttonBox->addButton(copyButton, QDialogButtonBox::ActionRole);
  connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &SnapshotDialog::clicked);

  linkWidget = new LinkWidget();
  ui->horizontalLayout_5->insertWidget(2, linkWidget);
  ui->horizontalLayout_5->setAlignment(linkWidget, Qt::AlignLeft | Qt::AlignVCenter);
}

void SnapshotDialog::clicked(QAbstractButton *b) {
  if (ui->buttonBox->buttonRole(b) == QDialogButtonBox::ResetRole) {
    ui->widthSpinBox->setValue(view->centralItem()->scene()->sceneRect().width());
    ui->heightSpinBox->setValue(view->centralItem()->scene()->sceneRect().height());
    ui->qualitySpinBox->setValue(100);
  }

  if (ui->buttonBox->buttonRole(b) == QDialogButtonBox::ActionRole) {
    QPixmap pixmap = view->snapshot(QSize(ui->widthSpinBox->value(), ui->heightSpinBox->value()));
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setPixmap(pixmap);
  }
}

SnapshotDialog::~SnapshotDialog() {
  delete ui;
}

void SnapshotDialog::resizeEvent(QResizeEvent *) {
  sizeSpinBoxValueChanged();
}

static const QString default_filter("png");

void SnapshotDialog::accept() {
  QString formatedFormatList;

  // Put the default save format as the first choice (selectedFilter not supported under MacOSX and
  // some Linux window managers)
  for (QString ext : QImageWriter::supportedImageFormats()) {
    ext = ext.toLower();

    if ((formatedFormatList.indexOf(ext) == -1) && (ext != default_filter)) {
      formatedFormatList += ext + " (*." + ext + ");;";
    }
  }

  QString selectedFilter(default_filter + " (*." + default_filter + ")");
  formatedFormatList = selectedFilter + ";;" + formatedFormatList;
  // remove last ;;
  formatedFormatList.resize(formatedFormatList.size() - 2);

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save image as..."), QString(),
                                                  formatedFormatList, &selectedFilter
// on MacOSX selectedFilter is ignored by the
// native dialog
#ifdef __APPLE__
                                                  ,
                                                  QFileDialog::DontUseNativeDialog
#endif
  );

  if (fileName.isEmpty()) {
    return;
  }

  // force file extension
  QString selectedExtension = QString('.') + selectedFilter.section(' ', 0, 0);

  if (!fileName.endsWith(selectedExtension)) {
    fileName += selectedExtension;
  }

  this->setEnabled(false);

  QPixmap pixmap = view->snapshot(QSize(ui->widthSpinBox->value(), ui->heightSpinBox->value()));

  if (!pixmap.save(fileName, nullptr, ui->qualitySpinBox->value())) {
    QMessageBox::critical(this, "Snapshot cannot be saved",
                          "Snapshot cannot be saved in file: " + fileName);
    this->setEnabled(true);
  } else {
    QDialog::accept();
  }
}

void SnapshotDialog::widthSpinBoxValueChanged(int value) {
  if (inSizeSpinBoxValueChanged) {
    return;
  }

  inSizeSpinBoxValueChanged = true;

  if (linkWidget->isLinked()) {
    ui->heightSpinBox->setValue(value / ratio);
  } else {
    sizeSpinBoxValueChanged();
  }

  inSizeSpinBoxValueChanged = false;
}

void SnapshotDialog::heightSpinBoxValueChanged(int value) {
  if (inSizeSpinBoxValueChanged) {
    return;
  }

  inSizeSpinBoxValueChanged = true;

  if (linkWidget->isLinked()) {
    ui->widthSpinBox->setValue(value * ratio);
  } else {
    sizeSpinBoxValueChanged();
  }

  inSizeSpinBoxValueChanged = false;
}

void SnapshotDialog::sizeSpinBoxValueChanged() {

  if (ui->widthSpinBox->value() < 10 || ui->heightSpinBox->value() < 10) {
    return;
  }

  float imageRatio = float(ui->widthSpinBox->value()) / float(ui->heightSpinBox->value());

  if (imageRatio != ratio) {
    // regenerate preview pixmap only if the aspect ratio changed
    QPixmap pixmap;

    pixmap =
        view->snapshot(QSize((view->centralItem()->scene()->sceneRect().height() - 2) * imageRatio,
                             view->centralItem()->scene()->sceneRect().height() - 2));
    ratio = float(ui->widthSpinBox->value()) / float(ui->heightSpinBox->value());
    ui->snapshotLabel->setPixmap(pixmap);
  }
  // resize snapshotLabel
  QSize sSize = ui->snapshotWidget->size();
  sSize -= QSize(2, 2);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
  QSize psize = ui->snapshotLabel->pixmap(Qt::ReturnByValue).size();
#else
  QSize psize = ui->snapshotLabel->pixmap()->size();
#endif
  psize.scale(sSize, Qt::KeepAspectRatio);
  ui->snapshotLabel->resize(psize);
  sSize -= psize;
  ui->snapshotLabel->move(sSize.width() / 2, sSize.height() / 2);
}

void SnapshotDialog::setSnapshotHasViewSizeRatio(bool snapshotHasViewSizeRatio) {
  linkWidget->setAlwaysLinked(snapshotHasViewSizeRatio);
}
}
