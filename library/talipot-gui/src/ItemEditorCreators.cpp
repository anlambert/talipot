/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <QColorDialog>
#include <QMainWindow>
#include <QFileDialog>
#include <QCheckBox>

#include <talipot/ColorScaleButton.h>
#include <talipot/Vec3fEditor.h>
#include <talipot/StringEditor.h>
#include <talipot/GlyphRenderer.h>
#include <talipot/EdgeExtremityGlyphManager.h>
#include <talipot/EdgeExtremityGlyph.h>
#include <talipot/FontDialog.h>
#include <talipot/GlyphManager.h>
#include <talipot/ItemEditorCreators.h>
#include <talipot/TextureFileDialog.h>
#include <talipot/FontIconDialog.h>
#include <talipot/ShapeDialog.h>

using namespace tlp;

static std::pair<QColor, QColor> modelIndexColors(const QModelIndex &index,
                                                  const QStyleOptionViewItem &option) {
  QColor backgroundColor = index.model()->data(index, Qt::BackgroundRole).value<QColor>();
  if (!backgroundColor.isValid()) {
    backgroundColor =
        (index.row() % 2) ? option.palette.alternateBase().color() : option.palette.base().color();
  }
  QColor foregroundColor = index.model()->data(index, Qt::ForegroundRole).value<QColor>();
  if (!foregroundColor.isValid()) {
    foregroundColor = textColor();
  }
  return {backgroundColor, foregroundColor};
}

/*
 * Base class
 */
bool ItemEditorCreator::paint(QPainter *painter, const QStyleOptionViewItem &option,
                              const QVariant &, const QModelIndex &) const {
  if (option.state.testFlag(QStyle::State_Selected) && option.showDecorationSelected) {
    painter->setBrush(option.palette.highlight());
    painter->setPen(Qt::transparent);
    painter->drawRect(option.rect);
  }

  return false;
}

QSize ItemEditorCreator::sizeHint(const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const {
  QVariant data = index.model()->data(index);
  QString line = displayText(data);
  QFontMetrics fontMetrics(option.font);
  QRect textBB = fontMetrics.boundingRect(line);
  return QSize(textBB.width() + 15, textBB.height() + 5);
}

// this class is defined to properly catch the return status
// of a QColorDialog. calling QDialog::result() instead does not work
class ColorDialog : public QColorDialog {
public:
  ColorDialog(QWidget *w) : QColorDialog(w), previousColor(), ok(QDialog::Rejected) {
    // we don't use native dialog to ensure alpha channel can be set
    // it may not be shown when using gnome
    setOptions(QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);
  }
  ~ColorDialog() override = default;
  tlp::Color previousColor;
  int ok;
  void done(int res) override {
    ok = res;
    QColorDialog::done(res);
  }
  void showEvent(QShowEvent *ev) override {
    QDialog::showEvent(ev);

    if (parentWidget()) {
      move(parentWidget()->window()->frameGeometry().topLeft() +
           parentWidget()->window()->rect().center() - rect().center());
    }
  }
};

/*
  ColorEditorCreator
*/
QWidget *ColorEditorCreator::createWidget(QWidget *parent) const {
  QMainWindow *mainWindow = getMainWindow();
  auto *colorDialog = new ColorDialog(mainWindow ? mainWindow : parent);
  colorDialog->setModal(true);
  return colorDialog;
}

bool ColorEditorCreator::paint(QPainter *painter, const QStyleOptionViewItem &option,
                               const QVariant &v, const QModelIndex &index) const {
  ItemEditorCreator::paint(painter, option, v, index);
  painter->setBrush(colorToQColor(v.value<tlp::Color>()));
  painter->setPen(Qt::black);
  painter->drawRect(option.rect.x() + 6, option.rect.y() + 6, option.rect.width() - 12,
                    option.rect.height() - 12);
  return true;
}

void ColorEditorCreator::setEditorData(QWidget *editor, const QModelIndex &, const QVariant &data,
                                       bool, tlp::Graph *) {
  auto *dlg = static_cast<ColorDialog *>(editor);

  dlg->previousColor = data.value<tlp::Color>();
  dlg->setCurrentColor(colorToQColor(dlg->previousColor));
  dlg->move(QCursor::pos() - QPoint(dlg->width() / 2, dlg->height() / 2));
}

QVariant ColorEditorCreator::editorData(QWidget *editor, tlp::Graph *) {
  auto *dlg = static_cast<ColorDialog *>(editor);

  if (dlg->ok == QDialog::Rejected) {
    // restore the previous color
    return QVariant::fromValue<tlp::Color>(dlg->previousColor);
  }

  return QVariant::fromValue<tlp::Color>(QColorToColor(dlg->currentColor()));
}

class BooleanCheckBox : public QCheckBox {

public:
  BooleanCheckBox(QWidget *parent = nullptr) : QCheckBox(parent) {
    connect(this, QCheckBoxStateChangedSignal, this, &BooleanCheckBox::stateChangedSlot);
  }

private slots:

  void stateChangedSlot(int state) {
    setText(state == Qt::Checked ? "true" : "false");
  }
};

/*
  BooleanEditorCreator
*/
QWidget *BooleanEditorCreator::createWidget(QWidget *parent) const {
  return new BooleanCheckBox(parent);
}

void BooleanEditorCreator::setEditorData(QWidget *editor, const QModelIndex &index,
                                         const QVariant &v, bool, tlp::Graph *) {
  auto *cb = static_cast<QCheckBox *>(editor);
  cb->setChecked(v.toBool());
  cb->setText(v.toBool() ? "true" : "false");
  if (index.isValid()) {
    auto [backgroundColor, foregroundColor] = modelIndexColors(index, QStyleOptionViewItem());
    cb->setStyleSheet(QString("QCheckBox { background: %1; color: %2; }")
                          .arg(backgroundColor.name())
                          .arg(foregroundColor.name()));
  }
}

QVariant BooleanEditorCreator::editorData(QWidget *editor, tlp::Graph *) {
  return static_cast<QCheckBox *>(editor)->isChecked();
}

QString BooleanEditorCreator::displayText(const QVariant &v) const {
  return v.toBool() ? "true" : "false";
}

bool BooleanEditorCreator::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                 const QVariant &v, const QModelIndex &index) const {
  ItemEditorCreator::paint(painter, option, v, index);

  auto [backgroundColor, foregroundColor] = modelIndexColors(index, option);

  bool checked = v.toBool();

  QStyleOptionViewItem opt = option;
  opt.backgroundBrush = backgroundColor;
  opt.palette.setColor(QPalette::Text, foregroundColor);
  opt.features |= QStyleOptionViewItem::HasDisplay;
  opt.features |= QStyleOptionViewItem::HasCheckIndicator;
  opt.text = displayText(v);
  opt.rect = {opt.rect.x(), opt.rect.y(), opt.rect.width(), opt.rect.height()};
  opt.checkState = checked ? Qt::Checked : Qt::Unchecked;

  QStyle *style = QApplication::style();
  style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, nullptr);

  if (applicationHasDarkGuiTheme() && qApp->style()->objectName() == "QFusionStyle") {
    // ensure checkbox indicator is visible with dark them when using Qt Fusion style
    opt.backgroundBrush = Qt::transparent;
    opt.palette.setColor(QPalette::Text, qApp->palette().color(QPalette::Text));
    if (foregroundColor == darkColor) {
      opt.text = "";
    }
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, nullptr);
  }

  return true;
}

QSize BooleanEditorCreator::sizeHint(const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const {
  QVariant data = index.model()->data(index);
  static QSize iconSize(16, 16);
  QFontMetrics fontMetrics(option.font);
  return QSize(iconSize.width() + fontMetrics.boundingRect(displayText(data)).width() + 20,
               iconSize.height());
}

/*
  Vec3fEditorCreator
*/
QWidget *Vec3fEditorCreator::createWidget(QWidget *parent) const {
  QMainWindow *mainWindow = getMainWindow();
  return new Vec3fEditor(mainWindow ? mainWindow : parent, editSize);
}

void Vec3fEditorCreator::setEditorData(QWidget *w, const QModelIndex &, const QVariant &v, bool,
                                       tlp::Graph *) {
  if (editSize) {
    static_cast<Vec3fEditor *>(w)->setVec3f(v.value<tlp::Size>());
  } else {
    static_cast<Vec3fEditor *>(w)->setVec3f(v.value<tlp::Coord>());
  }
}

QVariant Vec3fEditorCreator::editorData(QWidget *w, tlp::Graph *) {
  if (editSize) {
    return QVariant::fromValue<tlp::Size>(static_cast<Vec3fEditor *>(w)->vec3f());
  } else {
    return QVariant::fromValue<tlp::Coord>(static_cast<Vec3fEditor *>(w)->vec3f());
  }
}

QString Vec3fEditorCreator::displayText(const QVariant &v) const {
  if (editSize) {
    return tlpStringToQString(SizeType::toString((v.value<tlp::Size>())));
  } else {
    return tlpStringToQString(PointType::toString(v.value<tlp::Coord>()));
  }
}

void Vec3fEditorCreator::setPropertyToEdit(tlp::PropertyInterface *prop) {
  editSize = (dynamic_cast<tlp::SizeProperty *>(prop) != nullptr);
}

/*
  PropertyInterfaceEditorCreator
*/
QWidget *PropertyInterfaceEditorCreator::createWidget(QWidget *parent) const {
  return new QComboBox(parent);
}

void PropertyInterfaceEditorCreator::setEditorData(QWidget *w, const QModelIndex &,
                                                   const QVariant &val, bool isMandatory,
                                                   tlp::Graph *g) {
  if (g == nullptr) {
    w->setEnabled(false);
    return;
  }

  auto *prop = val.value<PropertyInterface *>();
  auto *combo = static_cast<QComboBox *>(w);
  GraphPropertiesModel<PropertyInterface> *model = nullptr;

  if (isMandatory) {
    model = new GraphPropertiesModel<PropertyInterface>(g, false, combo);
  } else {
    model = new GraphPropertiesModel<PropertyInterface>("Select a property", g, false, combo);
  }

  combo->setModel(model);
  combo->setCurrentIndex(model->rowOf(prop));
}

QVariant PropertyInterfaceEditorCreator::editorData(QWidget *w, tlp::Graph *) {
  auto *combo = static_cast<QComboBox *>(w);
  auto *model = static_cast<GraphPropertiesModel<PropertyInterface> *>(combo->model());
  return model->data(model->index(combo->currentIndex(), 0), Model::PropertyRole);
}

QString PropertyInterfaceEditorCreator::displayText(const QVariant &v) const {
  auto *prop = v.value<PropertyInterface *>();

  if (prop == nullptr) {
    return "";
  }

  return prop->getName().c_str();
}

/*
  NumericPropertyEditorCreator
*/
QWidget *NumericPropertyEditorCreator::createWidget(QWidget *parent) const {
  return new QComboBox(parent);
}

void NumericPropertyEditorCreator::setEditorData(QWidget *w, const QModelIndex &,
                                                 const QVariant &val, bool isMandatory,
                                                 tlp::Graph *g) {
  if (g == nullptr) {
    w->setEnabled(false);
    return;
  }

  auto *prop = val.value<NumericProperty *>();
  auto *combo = static_cast<QComboBox *>(w);
  GraphPropertiesModel<NumericProperty> *model = nullptr;

  if (isMandatory) {
    model = new GraphPropertiesModel<NumericProperty>(g, false, combo);
  } else {
    model = new GraphPropertiesModel<NumericProperty>("Select a property", g, false, combo);
  }

  combo->setModel(model);
  combo->setCurrentIndex(model->rowOf(prop));
}

QVariant NumericPropertyEditorCreator::editorData(QWidget *w, tlp::Graph *) {
  auto *combo = static_cast<QComboBox *>(w);
  auto *model = static_cast<GraphPropertiesModel<NumericProperty> *>(combo->model());
  return model->data(model->index(combo->currentIndex(), 0), Model::PropertyRole);
}

QString NumericPropertyEditorCreator::displayText(const QVariant &v) const {
  auto *prop = v.value<NumericProperty *>();

  if (prop == nullptr) {
    return "";
  }

  return prop->getName().c_str();
}

/*
  ColorScaleEditorCreator
*/

QWidget *ColorScaleEditorCreator::createWidget(QWidget *parent) const {
  return new ColorScaleButton(ColorScale(), parent);
}

bool ColorScaleEditorCreator::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                    const QVariant &var, const QModelIndex &index) const {
  ItemEditorCreator::paint(painter, option, var, index);
  ColorScaleButton::paintScale(painter, option.rect, var.value<ColorScale>());
  return true;
}

void ColorScaleEditorCreator::setEditorData(QWidget *w, const QModelIndex &, const QVariant &var,
                                            bool, tlp::Graph *) {
  static_cast<ColorScaleButton *>(w)->editColorScale(var.value<ColorScale>());
}

QVariant ColorScaleEditorCreator::editorData(QWidget *w, tlp::Graph *) {
  return QVariant::fromValue<ColorScale>(static_cast<ColorScaleButton *>(w)->colorScale());
}

/*
  StringCollectionEditorCreator
*/
QWidget *StringCollectionEditorCreator::createWidget(QWidget *parent) const {
  return new QComboBox(parent);
}

void StringCollectionEditorCreator::setEditorData(QWidget *widget, const QModelIndex &,
                                                  const QVariant &var, bool, tlp::Graph *) {
  auto col = var.value<StringCollection>();
  auto *combo = static_cast<QComboBox *>(widget);

  for (uint i = 0; i < col.size(); ++i) {
    combo->addItem(tlp::tlpStringToQString(col[i]));
  }

  combo->setCurrentIndex(col.getCurrent());
}

QVariant StringCollectionEditorCreator::editorData(QWidget *widget, tlp::Graph *) {
  auto *combo = static_cast<QComboBox *>(widget);
  StringCollection col;

  for (int i = 0; i < combo->count(); ++i) {
    col.push_back(QStringToTlpString(combo->itemText(i)));
  }

  col.setCurrent(combo->currentIndex());
  return QVariant::fromValue<StringCollection>(col);
}

QString StringCollectionEditorCreator::displayText(const QVariant &var) const {
  auto col = var.value<StringCollection>();
  return tlpStringToQString(col[col.getCurrent()]);
}

// this class is defined to properly catch the return status
// of a QFileDialog. calling QDialog::result() instead does not work
class FileDialog : public QFileDialog {

public:
  FileDialog(QWidget *w) : QFileDialog(w), ok(QDialog::Rejected) {}
  ~FileDialog() override = default;
  int ok;
  FileDescriptor previousFileDescriptor;

  void done(int res) override {
    ok = res;
    QFileDialog::done(res);
  }

  void showEvent(QShowEvent *ev) override {
    QDialog::showEvent(ev);

    if (parentWidget()) {
      move(parentWidget()->window()->frameGeometry().topLeft() +
           parentWidget()->window()->rect().center() - rect().center());
    }
  }
};

/*
  FileDescriptorEditorCreator
  */
QWidget *FileDescriptorEditorCreator::createWidget(QWidget *parent) const {
  QMainWindow *mainWindow = getMainWindow();
  QFileDialog *dlg = new FileDialog(mainWindow ? mainWindow : parent);
#if defined(__APPLE__)
  dlg->setOption(QFileDialog::DontUseNativeDialog, true);
#else
  dlg->setOption(QFileDialog::DontUseNativeDialog, false);
#endif
  dlg->setMinimumSize(300, 400);
  return dlg;
}

void FileDescriptorEditorCreator::setEditorData(QWidget *w, const QModelIndex &, const QVariant &v,
                                                bool, tlp::Graph *) {
  auto desc = v.value<FileDescriptor>();
  auto *dlg = static_cast<FileDialog *>(w);
  dlg->previousFileDescriptor = desc;

  // force the dialog initial directory
  // only if there is a non empty absolute path
  if (!desc.absolutePath.isEmpty()) {
    dlg->setDirectory(QFileInfo(desc.absolutePath).absolutePath());
  }

  if (desc.type == FileDescriptor::Directory) {
    dlg->setFileMode(QFileDialog::Directory);
    dlg->setOption(QFileDialog::ShowDirsOnly);
  } else {
    dlg->setFileMode(desc.mustExist ? QFileDialog::ExistingFile : QFileDialog::AnyFile);
  }

  dlg->setModal(true);
  dlg->move(QCursor::pos() - QPoint(150, 200));
}

QVariant FileDescriptorEditorCreator::editorData(QWidget *w, tlp::Graph *) {
  auto *dlg = static_cast<FileDialog *>(w);

  int result = dlg->ok;

  if (result == QDialog::Rejected) {
    return QVariant::fromValue<FileDescriptor>(dlg->previousFileDescriptor);
  }

  if (!dlg->selectedFiles().empty()) {
    return QVariant::fromValue<FileDescriptor>(
        FileDescriptor(dlg->selectedFiles()[0], (dlg->fileMode() == QFileDialog::Directory)
                                                    ? FileDescriptor::Directory
                                                    : FileDescriptor::File));
  }

  return QVariant::fromValue<FileDescriptor>(FileDescriptor());
}

class QImageIconPool {

public:
  const QIcon &getIconForImageFile(const QString &file) {
    if (iconPool.contains(file)) {
      return iconPool[file];
    } else if (!file.isEmpty()) {
      QImage image;

      QFile imageFile(file);

      if (imageFile.open(QIODevice::ReadOnly)) {
        image.loadFromData(imageFile.readAll());
      }

      if (!image.isNull()) {
        iconPool[file] = QPixmap::fromImage(image.scaled(32, 32));
        return iconPool[file];
      }
    }

    return nullIcon;
  }

  QMap<QString, QIcon> iconPool;

private:
  QIcon nullIcon;
};

static QImageIconPool imageIconPool;

void tlp::addIconToPool(const QString &iconName, const QIcon &icon) {
  imageIconPool.iconPool[iconName] = icon;
}

bool FileDescriptorEditorCreator::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                        const QVariant &v, const QModelIndex &index) const {
  ItemEditorCreator::paint(painter, option, v, index);
  QRect rect = option.rect;
  auto fileDesc = v.value<FileDescriptor>();
  QFileInfo fileInfo(fileDesc.absolutePath);
  QString imageFilePath = fileInfo.absoluteFilePath();

  QIcon icon;
  QString text;

  const QIcon &imageIcon = imageIconPool.getIconForImageFile(imageFilePath);

  if (!imageIcon.isNull()) {
    icon = imageIcon;
    text = fileInfo.fileName();
  } else if (fileInfo.isFile()) {
    icon = FontIcon::icon(MaterialDesignIcons::FileOutline);
    text = fileInfo.fileName();
  } else if (fileInfo.isDir()) {
    icon = FontIcon::icon(MaterialDesignIcons::FolderOutline);
    QDir d1 = fileInfo.dir();
    d1.cdUp();
    text = fileInfo.absoluteFilePath().remove(0, d1.absolutePath().length() - 1);
  }

  int iconSize = rect.height() - 4;

  painter->drawPixmap(rect.x() + 2, rect.y() + 2, iconSize, iconSize, icon.pixmap(iconSize));

  int textXPos = rect.x() + iconSize + 5;

  if (option.state.testFlag(QStyle::State_Selected) && option.showDecorationSelected) {
    painter->setPen(option.palette.highlightedText().color());
    painter->setBrush(option.palette.highlightedText());
  } else {
    painter->setPen(option.palette.text().color());
    painter->setBrush(option.palette.text());
  }

  painter->drawText(textXPos, rect.y() + 2, rect.width() - (textXPos - rect.x()), rect.height() - 4,
                    Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap,
                    QFileInfo(fileDesc.absolutePath).fileName());

  return true;
}

QSize FileDescriptorEditorCreator::sizeHint(const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const {
  QVariant data = index.model()->data(index);
  auto fileDesc = data.value<FileDescriptor>();
  QFileInfo fileInfo(fileDesc.absolutePath);
  QString text;

  if (fileInfo.isDir()) {
    QDir d1 = fileInfo.dir();
    d1.cdUp();
    text = fileInfo.absoluteFilePath().remove(0, d1.absolutePath().length() - 1);
  } else {
    text = fileInfo.fileName();
  }

  const int pixmapWidth = 32;

  QFontMetrics fontMetrics(option.font);

  return QSize(pixmapWidth + fontMetrics.boundingRect(text).width(), pixmapWidth);
}

/*
  TextureFileEditorCreator
  */
QWidget *TextureFileEditorCreator::createWidget(QWidget *parent) const {
  QMainWindow *mainWindow = getMainWindow();
  return new TextureFileDialog(mainWindow ? mainWindow : parent);
}

void TextureFileEditorCreator::setEditorData(QWidget *w, const QModelIndex &, const QVariant &v,
                                             bool, tlp::Graph *) {
  auto desc = v.value<TextureFile>();
  auto *dlg = static_cast<TextureFileDialog *>(w);
  dlg->setData(desc);
}

QVariant TextureFileEditorCreator::editorData(QWidget *w, tlp::Graph *) {
  auto *dlg = static_cast<TextureFileDialog *>(w);
  return QVariant::fromValue<TextureFile>(dlg->data());
}

bool TextureFileEditorCreator::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                     const QVariant &v, const QModelIndex &index) const {
  ItemEditorCreator::paint(painter, option, v, index);
  QRect rect = option.rect;
  auto tf = v.value<TextureFile>();
  QFileInfo fileInfo(tf.texturePath);
  QString imageFilePath = fileInfo.absoluteFilePath();

  QIcon icon;
  QString text = fileInfo.fileName();

  if (tf.texturePath.startsWith("http")) {
    imageFilePath = tf.texturePath;
  }

  truncateText(text);

  const QIcon &imageIcon = imageIconPool.getIconForImageFile(imageFilePath);

  if (!imageIcon.isNull()) {
    icon = imageIcon;
  }

  int iconSize = rect.height() - 4;

  painter->drawPixmap(rect.x() + 2, rect.y() + 2, iconSize, iconSize, icon.pixmap(iconSize));

  int textXPos = rect.x() + iconSize + 5;

  if (option.state.testFlag(QStyle::State_Selected) && option.showDecorationSelected) {
    painter->setPen(option.palette.highlightedText().color());
    painter->setBrush(option.palette.highlightedText());
  } else {
    painter->setPen(option.palette.text().color());
    painter->setBrush(option.palette.text());
  }

  painter->drawText(textXPos, rect.y() + 2, rect.width() - (textXPos - rect.x()), rect.height() - 4,
                    Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap, text);

  return true;
}

QSize TextureFileEditorCreator::sizeHint(const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const {
  QVariant data = index.model()->data(index);
  auto tf = data.value<TextureFile>();
  QFileInfo fileInfo(tf.texturePath);
  QString text = fileInfo.fileName();

  truncateText(text);

  const int pixmapWidth = 32;

  QFontMetrics fontMetrics(option.font);

  return QSize(pixmapWidth + fontMetrics.boundingRect(text).width() + 20, pixmapWidth);
}

/*
  FontIconCreator
  */
QWidget *FontIconCreator::createWidget(QWidget *parent) const {
  // Due to a Qt issue when embedding a combo box with a large amount
  // of items in a QGraphicsScene (popup has a too large height,
  // making the scrollbars unreachable ...), we use a native
  // dialog with the combo box inside
  QMainWindow *mainWindow = getMainWindow();
  return new FontIconDialog(mainWindow ? mainWindow : parent);
}

void FontIconCreator::setEditorData(QWidget *w, const QModelIndex &, const QVariant &v, bool,
                                    tlp::Graph *) {
  auto *tfid = static_cast<FontIconDialog *>(w);
  tfid->setSelectedIconName(v.value<FontIconName>().iconName);
}

QVariant FontIconCreator::editorData(QWidget *w, tlp::Graph *) {
  return QVariant::fromValue<FontIconName>(
      FontIconName(static_cast<FontIconDialog *>(w)->getSelectedIconName()));
}

QString FontIconCreator::displayText(const QVariant &data) const {
  return data.value<FontIconName>().iconName;
}

bool FontIconCreator::paint(QPainter *painter, const QStyleOptionViewItem &option,
                            const QVariant &v, const QModelIndex &index) const {
  ItemEditorCreator::paint(painter, option, v, index);

  auto [backgroundColor, foregroundColor] = modelIndexColors(index, option);

  QString iconName = v.value<FontIconName>().iconName;

  if (iconName.isEmpty()) {
    return true;
  }

  QStyleOptionViewItem opt = option;
  opt.backgroundBrush = backgroundColor;
  opt.palette.setColor(QPalette::Text, foregroundColor);
  opt.features |= QStyleOptionViewItem::HasDecoration;
  opt.features |= QStyleOptionViewItem::HasDisplay;
  opt.icon = FontIcon::icon(iconName, foregroundColor);
  opt.decorationSize = opt.icon.actualSize(QSize(16, 16));
  opt.text = displayText(v);
  opt.rect = {opt.rect.x() + cellPadding, opt.rect.y(), opt.rect.width() - cellPadding,
              opt.rect.height()};

  QStyle *style = QApplication::style();
  style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, nullptr);
  return true;
}

QSize FontIconCreator::sizeHint(const QStyleOptionViewItem &option,
                                const QModelIndex &index) const {
  QVariant data = index.model()->data(index);
  static QSize iconSize(16, 16);
  QFontMetrics fontMetrics(option.font);
  return QSize(iconSize.width() + fontMetrics.boundingRect(displayText(data)).width() + 20,
               iconSize.height());
}

/// NodeShapeEditorCreator
QWidget *NodeShapeEditorCreator::createWidget(QWidget *parent) const {
  // Due to a Qt issue when embedding a combo box with a large amount
  // of items in a QGraphicsScene (popup has a too large height,
  // making the scrollbars unreachable ...), we use a native
  // dialog with a QListWidget inside
  std::list<std::pair<QString, QPixmap>> shapes;
  auto glyphs = PluginsManager::availablePlugins<Glyph>();

  for (const auto &glyph : glyphs) {
    QString shapeName = tlpStringToQString(glyph);
    QPixmap pixmap =
        GlyphRenderer::render(GlyphManager::glyphId(glyph), backgroundColor(), textColor());
    shapes.push_back({shapeName, pixmap});
  }

  QMainWindow *mainWindow = getMainWindow();
  return new ShapeDialog(shapes, mainWindow ? mainWindow : parent);
}

void NodeShapeEditorCreator::setEditorData(QWidget *w, const QModelIndex &, const QVariant &data,
                                           bool, tlp::Graph *) {
  auto *nsd = static_cast<ShapeDialog *>(w);
  nsd->setSelectedShapeName(
      tlpStringToQString(GlyphManager::glyphName(data.value<NodeShape::NodeShapes>())));
}

QVariant NodeShapeEditorCreator::editorData(QWidget *w, tlp::Graph *) {
  auto *nsd = static_cast<ShapeDialog *>(w);
  return QVariant::fromValue<NodeShape::NodeShapes>(static_cast<NodeShape::NodeShapes>(
      GlyphManager::glyphId(QStringToTlpString(nsd->getSelectedShapeName()))));
}

QString NodeShapeEditorCreator::displayText(const QVariant &data) const {
  return tlpStringToQString(GlyphManager::glyphName(data.value<NodeShape::NodeShapes>()));
}

QSize NodeShapeEditorCreator::sizeHint(const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const {
  QVariant data = index.model()->data(index);
  static QPixmap pixmap =
      GlyphRenderer::render(data.value<NodeShape::NodeShapes>(), backgroundColor(), textColor());
  QFontMetrics fontMetrics(option.font);
  return QSize(pixmap.width() + fontMetrics.boundingRect(displayText(data)).width() + 20,
               pixmap.height());
}

bool NodeShapeEditorCreator::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                   const QVariant &data, const QModelIndex &index) const {
  ItemEditorCreator::paint(painter, option, data, index);

  auto [backgroundColor, foregroundColor] = modelIndexColors(index, option);

  QPixmap pixmap =
      GlyphRenderer::render(data.value<NodeShape::NodeShapes>(), backgroundColor, foregroundColor);

  QStyleOptionViewItem opt = option;
  opt.backgroundBrush = backgroundColor;
  opt.palette.setColor(QPalette::Text, foregroundColor);
  opt.features |= QStyleOptionViewItem::HasDecoration;
  opt.features |= QStyleOptionViewItem::HasDisplay;
  opt.icon = QIcon(pixmap);
  opt.decorationSize = pixmap.size();
  opt.text = displayText(data);
  opt.rect = {opt.rect.x() + cellPadding, opt.rect.y(), opt.rect.width() - cellPadding,
              opt.rect.height()};

  QStyle *style = QApplication::style();

  style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, nullptr);
  return true;
}

/// EdgeExtremityShapeEditorCreator
QWidget *EdgeExtremityShapeEditorCreator::createWidget(QWidget *parent) const {
  // Due to a Qt issue when embedding a combo box with a large amount
  // of items in a QGraphicsScene (popup has a too large height,
  // making the scrollbars unreachable ...), we use a native
  // dialog with a QListWidget inside
  std::list<std::pair<QString, QPixmap>> shapes;
  shapes.push_back({"NONE", QPixmap()});

  auto glyphs = PluginsManager::availablePlugins<EdgeExtremityGlyph>();

  for (const auto &glyph : glyphs) {
    QString shapeName = tlpStringToQString(glyph);
    QPixmap pixmap = EdgeExtremityGlyphRenderer::render(EdgeExtremityGlyphManager::glyphId(glyph),
                                                        backgroundColor(), textColor());
    shapes.push_back({shapeName, pixmap});
  }

  QMainWindow *mainWindow = getMainWindow();
  auto *shapeDialog = new ShapeDialog(shapes, mainWindow ? mainWindow : parent);
  shapeDialog->setWindowTitle("Select an edge extremity shape");
  return shapeDialog;
}

void EdgeExtremityShapeEditorCreator::setEditorData(QWidget *w, const QModelIndex &,
                                                    const QVariant &data, bool, tlp::Graph *) {
  auto *nsd = static_cast<ShapeDialog *>(w);
  nsd->setSelectedShapeName(tlpStringToQString(
      EdgeExtremityGlyphManager::glyphName(data.value<EdgeExtremityShape::EdgeExtremityShapes>())));
}

QVariant EdgeExtremityShapeEditorCreator::editorData(QWidget *w, tlp::Graph *) {
  auto *nsd = static_cast<ShapeDialog *>(w);
  return QVariant::fromValue<EdgeExtremityShape::EdgeExtremityShapes>(
      static_cast<EdgeExtremityShape::EdgeExtremityShapes>(
          EdgeExtremityGlyphManager::glyphId(QStringToTlpString(nsd->getSelectedShapeName()))));
}

QString EdgeExtremityShapeEditorCreator::displayText(const QVariant &data) const {
  return tlpStringToQString(
      EdgeExtremityGlyphManager::glyphName(data.value<EdgeExtremityShape::EdgeExtremityShapes>()));
}

bool EdgeExtremityShapeEditorCreator::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                            const QVariant &data, const QModelIndex &index) const {
  ItemEditorCreator::paint(painter, option, data, index);

  auto [backgroundColor, foregroundColor] = modelIndexColors(index, option);

  QPixmap pixmap = EdgeExtremityGlyphRenderer::render(
      data.value<EdgeExtremityShape::EdgeExtremityShapes>(), backgroundColor, foregroundColor);

  QStyleOptionViewItem opt = option;
  opt.backgroundBrush = backgroundColor;
  opt.palette.setColor(QPalette::Text, foregroundColor);
  opt.features |= QStyleOptionViewItem::HasDecoration;
  opt.features |= QStyleOptionViewItem::HasDisplay;
  opt.icon = QIcon(pixmap);
  opt.decorationSize = pixmap.size();
  opt.text = displayText(data);
  opt.rect = {opt.rect.x() + cellPadding, opt.rect.y(), opt.rect.width() - cellPadding,
              opt.rect.height()};

  QStyle *style = QApplication::style();
  style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, nullptr);
  return true;
}

QSize EdgeExtremityShapeEditorCreator::sizeHint(const QStyleOptionViewItem &option,
                                                const QModelIndex &index) const {
  QVariant data = index.model()->data(index);
  static QPixmap pixmap = EdgeExtremityGlyphRenderer::render(
      data.value<EdgeExtremityShape::EdgeExtremityShapes>(), backgroundColor(), textColor());
  QFontMetrics fontMetrics(option.font);
  return QSize(pixmap.width() + fontMetrics.boundingRect(displayText(data)).width() + 40,
               pixmap.height());
}

/// EdgeShapeEditorCreator
QWidget *EdgeShapeEditorCreator::createWidget(QWidget *parent) const {
  auto *combobox = new QComboBox(parent);

  for (int i = 0; i < GlGraphStaticData::edgeShapesCount; i++) {
    combobox->addItem(
        tlpStringToQString(GlGraphStaticData::edgeShapeName(GlGraphStaticData::edgeShapeIds[i])),
        QVariant(GlGraphStaticData::edgeShapeIds[i]));
  }

  return combobox;
}
void EdgeShapeEditorCreator::setEditorData(QWidget *editor, const QModelIndex &,
                                           const QVariant &data, bool, Graph *) {
  auto *combobox = static_cast<QComboBox *>(editor);
  combobox->setCurrentIndex(combobox->findData(int(data.value<EdgeShape::EdgeShapes>())));
}

QVariant EdgeShapeEditorCreator::editorData(QWidget *editor, Graph *) {
  auto *combobox = static_cast<QComboBox *>(editor);
  return QVariant::fromValue<EdgeShape::EdgeShapes>(
      static_cast<EdgeShape::EdgeShapes>(combobox->itemData(combobox->currentIndex()).toInt()));
}

QString EdgeShapeEditorCreator::displayText(const QVariant &data) const {
  return tlpStringToQString(GlGraphStaticData::edgeShapeName(data.value<EdgeShape::EdgeShapes>()));
}

// FontEditorCreator
QWidget *FontEditorCreator::createWidget(QWidget *parent) const {
  QMainWindow *mainWindow = getMainWindow();
  return new FontDialog(mainWindow ? mainWindow : parent);
}
void FontEditorCreator::setEditorData(QWidget *editor, const QModelIndex &, const QVariant &data,
                                      bool, tlp::Graph *) {
  Font font = data.value<Font>();
  auto *fontWidget = static_cast<FontDialog *>(editor);
  fontWidget->selectFont(font);
  fontWidget->move(QCursor::pos() - QPoint(fontWidget->width() / 2, fontWidget->height() / 2));
}

QVariant FontEditorCreator::editorData(QWidget *editor, tlp::Graph *) {
  auto *fontWidget = static_cast<FontDialog *>(editor);
  return QVariant::fromValue<Font>(fontWidget->getSelectedFont());
}

QString FontEditorCreator::displayText(const QVariant &data) const {
  Font font = data.value<Font>();
  return tlpStringToQString(font.fontFamily()) + " " + tlpStringToQString(font.fontStyle());
}

bool FontEditorCreator::paint(QPainter *painter, const QStyleOptionViewItem &option,
                              const QVariant &v, const QModelIndex &index) const {
  ItemEditorCreator::paint(painter, option, v, index);
  Font font = v.value<Font>();
  QFont qFont = option.font;
  qFont.setFamily(tlpStringToQString(font.fontFamily()));
  qFont.setStyleName(tlpStringToQString(font.fontStyle()));
  qFont.setPointSize(9);
  painter->setFont(qFont);
  if (option.state.testFlag(QStyle::State_Selected) && option.showDecorationSelected) {
    painter->setPen(option.palette.highlightedText().color());
  }
  QRect rect = {option.rect.x() + cellPadding, option.rect.y(), option.rect.width() - cellPadding,
                option.rect.height()};
  painter->drawText(rect, displayText(v), QTextOption(Qt::AlignCenter));
  return true;
}

QWidget *LabelPositionEditorCreator::createWidget(QWidget *parent) const {
  auto *result = new QComboBox(parent);

  for (const auto &[labelPosition, labelPositionName] : ViewSettings::POSITION_LABEL_MAP) {
    result->addItem(tlp::tlpStringToQString(labelPositionName),
                    QVariant::fromValue<LabelPosition::LabelPositions>(labelPosition));
  }

  return result;
}
void LabelPositionEditorCreator::setEditorData(QWidget *w, const QModelIndex &, const QVariant &var,
                                               bool, tlp::Graph *) {
  auto *comboBox = static_cast<QComboBox *>(w);
  comboBox->setCurrentIndex(int(var.value<LabelPosition::LabelPositions>()));
}
QVariant LabelPositionEditorCreator::editorData(QWidget *w, tlp::Graph *) {
  return QVariant::fromValue<LabelPosition::LabelPositions>(
      static_cast<LabelPosition::LabelPositions>(static_cast<QComboBox *>(w)->currentIndex()));
}
QString LabelPositionEditorCreator::displayText(const QVariant &v) const {
  return tlp::tlpStringToQString(
      ViewSettings::POSITION_LABEL_MAP[v.value<LabelPosition::LabelPositions>()]);
}

// GraphEditorCreator
QWidget *GraphEditorCreator::createWidget(QWidget *parent) const {
  return new QLabel(parent);
}

void GraphEditorCreator::setEditorData(QWidget *w, const QModelIndex &, const QVariant &var, bool,
                                       tlp::Graph *) {
  auto *g = var.value<Graph *>();

  if (g != nullptr) {
    std::string name;
    g->getAttribute<std::string>("name", name);
    static_cast<QLabel *>(w)->setText(name.c_str());
  }
}

QVariant GraphEditorCreator::editorData(QWidget *, tlp::Graph *) {
  return QVariant();
}

QString GraphEditorCreator::displayText(const QVariant &var) const {
  auto *g = var.value<Graph *>();

  if (g == nullptr) {
    return QString();
  }

  std::string name;
  g->getAttribute<std::string>("name", name);
  return name.c_str();
}

// EdgeSetEditorCreator
QWidget *EdgeSetEditorCreator::createWidget(QWidget *parent) const {
  return new QLabel(parent);
}

void EdgeSetEditorCreator::setEditorData(QWidget *w, const QModelIndex &, const QVariant &var, bool,
                                         tlp::Graph *) {
  auto eset = var.value<std::set<tlp::edge>>();

  std::stringstream ss;
  tlp::EdgeSetType::write(ss, eset);
  static_cast<QLabel *>(w)->setText(ss.str().c_str());
}

QVariant EdgeSetEditorCreator::editorData(QWidget *, tlp::Graph *) {
  return QVariant();
}

QString EdgeSetEditorCreator::displayText(const QVariant &var) const {
  auto eset = var.value<std::set<tlp::edge>>();

  std::stringstream ss;
  tlp::EdgeSetType::write(ss, eset);

  return ss.str().c_str();
}

QWidget *QVectorBoolEditorCreator::createWidget(QWidget *parent) const {
  QMainWindow *mainWindow = getMainWindow();
  auto *w = new VectorEditor(mainWindow ? mainWindow : parent);
  w->setWindowFlags(Qt::Dialog);
  w->setWindowModality(Qt::ApplicationModal);
  return w;
}

void QVectorBoolEditorCreator::setEditorData(QWidget *editor, const QModelIndex &,
                                             const QVariant &v, bool, tlp::Graph *) {
  QVector<QVariant> editorData;
  auto vect = v.value<QVector<bool>>();

  for (bool b : vect) {
    editorData.push_back(QVariant::fromValue<bool>(b));
  }

  static_cast<VectorEditor *>(editor)->setVector(editorData, qMetaTypeId<bool>());

  static_cast<VectorEditor *>(editor)->move(QCursor::pos());
}

QVariant QVectorBoolEditorCreator::editorData(QWidget *editor, tlp::Graph *) {
  QVector<bool> result;
  QVector<QVariant> editorData = static_cast<VectorEditor *>(editor)->vector();

  for (const QVariant &v : editorData) {
    result.push_back(v.value<bool>());
  }

  return QVariant::fromValue<QVector<bool>>(result);
}

QString QVectorBoolEditorCreator::displayText(const QVariant &data) const {
  auto vb = data.value<QVector<bool>>();
  std::vector<bool> v(vb.begin(), vb.end());

  if (v.empty()) {
    return QString();
  }

  // use a DataTypeSerializer if any
  DataTypeSerializer *dts = DataSet::typenameToSerializer(std::string(typeid(v).name()));

  if (dts) {
    DisplayVectorDataType<bool> dt(&v);

    std::stringstream sstr;
    dts->writeData(sstr, &dt);

    QString str = tlpStringToQString(sstr.str());

    return truncateText(str, " ...)");
  }

  if (v.size() == 1) {
    return QString("1 element");
  }

  return QString::number(v.size()) + QString(" elements");
}

// QStringEditorCreator
QWidget *QStringEditorCreator::createWidget(QWidget *parent) const {
  QMainWindow *mainWindow = getMainWindow();
  auto *editor = new StringEditor(mainWindow ? mainWindow : parent);
  editor->setWindowTitle(QString("Set ") + propName.c_str() + " value");
  editor->setMinimumSize(QSize(250, 250));
  return editor;
}

void QStringEditorCreator::setEditorData(QWidget *editor, const QModelIndex &, const QVariant &var,
                                         bool, tlp::Graph *) {
  static_cast<StringEditor *>(editor)->setString(var.toString());
}

QVariant QStringEditorCreator::editorData(QWidget *editor, tlp::Graph *) {
  return static_cast<StringEditor *>(editor)->getString();
}

QString QStringEditorCreator::displayText(const QVariant &var) const {
  QString qstr = var.toString();
  return truncateText(qstr);
}

void QStringEditorCreator::setPropertyToEdit(tlp::PropertyInterface *prop) {
  // we should have a property
  // but it cannot be the case when editing a string vector element
  if (prop) {
    propName = prop->getName();
  }
}

// StdStringEditorCreator
void StdStringEditorCreator::setEditorData(QWidget *editor, const QModelIndex &,
                                           const QVariant &var, bool, tlp::Graph *) {
  static_cast<StringEditor *>(editor)->setString(tlpStringToQString(var.value<std::string>()));
}

QVariant StdStringEditorCreator::editorData(QWidget *editor, tlp::Graph *) {
  return QVariant::fromValue<std::string>(
      QStringToTlpString(static_cast<StringEditor *>(editor)->getString()));
}

QString StdStringEditorCreator::displayText(const QVariant &var) const {
  QString qstr = tlpStringToQString(var.value<std::string>());
  return truncateText(qstr);
}

// QStringListEditorCreator
QWidget *QStringListEditorCreator::createWidget(QWidget *parent) const {
  QMainWindow *mainWindow = getMainWindow();
  auto *w = new VectorEditor(mainWindow ? mainWindow : parent);
  w->setWindowFlags(Qt::Dialog);
  w->setWindowModality(Qt::ApplicationModal);
  return w;
}

void QStringListEditorCreator::setEditorData(QWidget *w, const QModelIndex &, const QVariant &var,
                                             bool, Graph *) {
  QStringList strList = var.toStringList();
  QVector<QVariant> vect(strList.length());
  int i = 0;

  for (const QString &s : strList) {
    vect[i++] = s;
  }

  static_cast<VectorEditor *>(w)->setVector(vect, qMetaTypeId<QString>());
}

QVariant QStringListEditorCreator::editorData(QWidget *w, Graph *) {
  auto vect = static_cast<VectorEditor *>(w)->vector();
  QStringList lst;

  for (const QVariant &v : vect) {
    lst.push_back(v.toString());
  }

  return lst;
}

QString QStringListEditorCreator::displayText(const QVariant &var) const {
  return QStringListType::toString(var.toStringList()).c_str();
}
