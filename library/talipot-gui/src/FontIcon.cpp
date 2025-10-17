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

#include "talipot/IconicFont.h"
#include "talipot/FontIcon.h"

#include <QFontDatabase>
#include <QPainter>
#include <QIconEngine>
#include <QScreen>
#include <QGuiApplication>

using namespace tlp;

static QMap<QString, QFont> qFonts;
static QFont nullFont;

// The implementation of that class is freely inspired from the QtAwesome project
// (see https://github.com/Gamecreature/qtawesome)
class FontIconEngine : public QIconEngine {
  QString _iconUtf8Str;
  QFont &_font;
  QVariantMap _options;

  QFont &init(const QString &iconName);

public:
  FontIconEngine(const QString &iconName, const QVariantMap &options);
  FontIconEngine(const FontIconEngine &engine)
      : QIconEngine(), _iconUtf8Str(engine._iconUtf8Str), _font(engine._font),
        _options(engine._options) {}

  FontIconEngine *clone() const override {
    return new FontIconEngine(*this);
  }

  void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override;

  QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override;
};

QFont &FontIconEngine::init(const QString &iconName) {
  // first set code point
  _iconUtf8Str = tlpStringToQString(IconicFont::getIconUtf8String(iconName.toStdString()));
  // then get font
  QString fontFile = tlpStringToQString(IconicFont::getTTFLocation(iconName.toStdString()));
  if (!qFonts.contains(fontFile)) {
    // load the font file
    auto fontId = QFontDatabase::addApplicationFont(fontFile);
    if (fontId == -1) {
      tlp::debug() << "Error when loading font file" << fontFile.toStdString() << std::endl;
      return nullFont;
    }
    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
    if (!fontFamilies.empty()) {
      qFonts[fontFile] = QFont(fontFamilies.at(0));
    } else {
      tlp::debug() << "No data found when loading file" << fontFile.toStdString() << std::endl;
      return nullFont;
    }
  }
  return qFonts[fontFile];
}

FontIconEngine::FontIconEngine(const QString &iconName, const QVariantMap &options)
    : _font(init(iconName)), _options(options) {}

static QStringList optionKeysForModeAndState(const QString &key, QIcon::Mode mode,
                                             QIcon::State state) {
  QString modePostfix;
  switch (mode) {
  case QIcon::Disabled:
    modePostfix = "-disabled";
    break;
  case QIcon::Active:
    modePostfix = "-active";
    break;
  case QIcon::Selected:
    modePostfix = "-selected";
    break;
  default: // QIcon::Normal:
    break;
  }

  QString statePostfix;
  if (state == QIcon::Off) {
    statePostfix = "-off";
  }

  // the keys that need to bet tested:   key-mode-state | key-mode | key-state | key
  QStringList result;
  if (!modePostfix.isEmpty()) {
    if (!statePostfix.isEmpty()) {
      result.push_back(key + modePostfix + statePostfix);
    }
    result.push_back(key + modePostfix);
  }
  if (!statePostfix.isEmpty()) {
    result.push_back(key + statePostfix);
  }
  return result;
}

static QVariant optionValueForModeAndState(const QString &baseKey, QIcon::Mode mode,
                                           QIcon::State state, const QVariantMap &options) {
  for (const auto &key : optionKeysForModeAndState(baseKey, mode, state)) {
    if (options.contains(key) && !(options.value(key).toString().isEmpty())) {
      return options.value(key);
    }
  }
  return options.value(baseKey);
}

void FontIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode,
                           QIcon::State state) {
  painter->save();

  auto color = optionValueForModeAndState("color", mode, state, _options).value<QColor>();
  if (!color.isValid()) {
    color = textColor();
  }

  painter->setPen(color);

  // add some 'padding' around the icon
  int drawSize = qRound(rect.height() * _options.value("scale-factor").toFloat());
  _font.setPixelSize(drawSize);

  QPointF translation = _options.value("translation").toPointF();
  painter->translate(translation);

  // set the font
  painter->setFont(_font);

  painter->drawText(rect, _iconUtf8Str, QTextOption(Qt::AlignCenter | Qt::AlignVCenter));

  painter->restore();
}

QPixmap FontIconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) {
  auto scale = QGuiApplication::primaryScreen()->devicePixelRatio();
  QPixmap pm(size);
  pm.setDevicePixelRatio(1.0 / scale);
  pm.fill(Qt::transparent); // we need transparency
  QPainter painter(&pm);
  paint(&painter, QRect(QPoint(0, 0), size), mode, state);
  return pm;
}

static QMap<QString, QIcon> fontIconsCache;

static QVariantMap getOptionsMap(const QColor &color, const QColor &colorDisabled,
                                 const QColor &colorActive, const QColor &colorSelected,
                                 const double scaleFactor, const QPointF &translation) {
  QVariantMap ret;
  ret.insert("color", color);
  ret.insert("color-disabled", colorDisabled);
  ret.insert("color-active", colorActive);
  ret.insert("color-selected", colorSelected);
  ret.insert("scale-factor", scaleFactor);
  ret.insert("translation", translation);
  return ret;
}

static QString getOptionsString(const QColor &color, const QColor &colorDisabled,
                                const QColor &colorActive, const QColor &colorSelected,
                                const double scaleFactor, const double rotation,
                                const QPointF &translation) {
  return color.name() + QString::number(color.alpha()) + colorDisabled.name() +
         QString::number(colorDisabled.alpha()) + colorActive.name() +
         QString::number(colorActive.alpha()) + colorSelected.name() +
         QString::number(colorSelected.alpha()) + QString::number(scaleFactor) +
         QString::number(rotation) + QString::number(translation.x()) +
         QString::number(translation.y());
}

const QIcon &FontIcon::icon(const QString &iconName, const double scaleFactor,
                            const double rotation, const QPointF &translation) {
  static QColor invalidColor;
  return icon(iconName, invalidColor, invalidColor, invalidColor, invalidColor, scaleFactor,
              rotation, translation);
}

const QIcon &FontIcon::icon(const QString &iconName, const QColor &color, const double scaleFactor,
                            const double rotation, const QPointF &translation) {
  return icon(iconName, color, color, color, color, scaleFactor, rotation, translation);
}

const QIcon &FontIcon::icon(const QString &iconName, const QColor &color,
                            const QColor &colorDisabled, const QColor &colorActive,
                            const QColor &colorSelected, const double scaleFactor,
                            const double rotation, const QPointF &translation) {

  QVariantMap options =
      getOptionsMap(color, colorDisabled, colorActive, colorSelected, scaleFactor, translation);

  QString optionsString = getOptionsString(color, colorDisabled, colorActive, colorSelected,
                                           scaleFactor, rotation, translation);
  QString iconCacheKey = iconName + optionsString;
  if (!fontIconsCache.contains(iconCacheKey)) {
    QIcon icon = QIcon(new FontIconEngine(iconName, options));
    if (rotation != 0) {
      QPixmap pix = icon.pixmap(QSize(128, 128));
      pix = pix.transformed(QTransform().rotate(rotation));
      icon = QIcon(pix);
    }
    fontIconsCache[iconCacheKey] = icon;
  }
  return fontIconsCache[iconCacheKey];
}

QIcon FontIcon::stackIcons(const QIcon &backIcon, const QIcon &frontIcon) {
  QPixmap pixmap = backIcon.pixmap(QSize(128, 128));
  QPainter painter(&pixmap);
  painter.drawPixmap(0, 0, frontIcon.pixmap(QSize(128, 128)));
  return QIcon(pixmap);
}