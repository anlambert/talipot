/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/ColorScalesManager.h>
#include <talipot/TlpQtTools.h>
#include <talipot/Settings.h>

#include <QDir>
#include <QImage>

using namespace std;
using namespace tlp;

void ColorScalesManager::getColorScalesFromDir(const string &colorScalesDir,
                                               list<string> &colorScalesList) {

    QFileInfo colorscaleDirectory(tlpStringToQString(colorScalesDir));

    if (colorscaleDirectory.exists() && colorscaleDirectory.isDir()) {
        QDir dir(colorscaleDirectory.absoluteFilePath());
        dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        QFileInfoList list = dir.entryInfoList();

        for (const auto &fileInfo : list) {
            if (fileInfo.isDir()) {
                getColorScalesFromDir(QStringToTlpString(fileInfo.absoluteFilePath()),
                                      colorScalesList);
            } else if (fileInfo.suffix() == "png") {
                QString colorScaleName = fileInfo.fileName();
                colorScaleName.replace(".png", "");
                colorScalesList.push_back(QStringToTlpString(colorScaleName));
            }
        }
    }
}

list<string> ColorScalesManager::getColorScalesList() {

    list<string> ret;
    getColorScalesFromDir(TalipotBitmapDir + "colorscales", ret);

    Settings::instance().beginGroup("ColorScales");
    QStringList savedColorScalesIdList = Settings::instance().childKeys();

    for (int i = 0; i < savedColorScalesIdList.size(); ++i) {
        if (!savedColorScalesIdList.at(i).contains("_gradient?")) {
            ret.push_back(QStringToTlpString(savedColorScalesIdList.at(i)));
        }
    }

    Settings::instance().endGroup();

    Settings::instance().beginGroup("ColorScalesNoRegular");
    savedColorScalesIdList = Settings::instance().childKeys();

    for (int i = 0; i < savedColorScalesIdList.size(); ++i) {
        if (!savedColorScalesIdList.at(i).contains("_gradient?")) {
            ret.push_back(QStringToTlpString(savedColorScalesIdList.at(i)));
        }
    }

    Settings::instance().endGroup();

    return ret;
}

static ColorScale getColorScaleFromImageFile(const QString &imageFilePath) {
    QImage gradientImage(imageFilePath);
    uint imageHeight = gradientImage.height();

    uint step = 1;

    if (imageHeight > 50) {
        step = 10;
    }

    vector<Color> colors;

    for (uint i = 0; i < imageHeight; i += step) {
        QRgb pixelValue = gradientImage.pixel(0, i);
        colors.push_back(
            Color(qRed(pixelValue), qGreen(pixelValue), qBlue(pixelValue), qAlpha(pixelValue)));
    }

    if (imageHeight % step != 0) {
        QRgb pixelValue = gradientImage.pixel(0, imageHeight - 1);
        colors.push_back(
            Color(qRed(pixelValue), qGreen(pixelValue), qBlue(pixelValue), qAlpha(pixelValue)));
    }

    reverse(colors.begin(), colors.end());
    return ColorScale(colors);
}

string ColorScalesManager::findColorScaleFile(const string &rootDir, const string &colorScaleName) {
    QFileInfo colorscaleDirectory(tlpStringToQString(rootDir));

    string ret;

    if (colorscaleDirectory.exists() && colorscaleDirectory.isDir()) {
        QDir dir(colorscaleDirectory.absoluteFilePath());
        dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        QFileInfoList list = dir.entryInfoList();

        for (const auto &fileInfo : list) {
            if (fileInfo.isDir()) {
                ret = findColorScaleFile(QStringToTlpString(fileInfo.absoluteFilePath()),
                                         colorScaleName);

                if (!ret.empty()) {
                    return ret;
                }
            } else if (fileInfo.suffix() == "png" &&
                       QStringToTlpString(fileInfo.baseName()) == colorScaleName) {
                return QStringToTlpString(fileInfo.absoluteFilePath());
            }
        }
    }

    return ret;
}

ColorScale ColorScalesManager::getColorScale(const string &colorScaleName) {
    string colorScaleFile = findColorScaleFile(TalipotBitmapDir + "colorscales", colorScaleName);

    if (!colorScaleFile.empty()) {
        return getColorScaleFromImageFile(tlpStringToQString(colorScaleFile));
    }

    map<float, Color> colorsMap;
    bool gradient = true;

    Settings::instance().beginGroup("ColorScales");

    if (Settings::instance().contains(colorScaleName.c_str())) {
        QList<QVariant> colorsListv = Settings::instance().value(colorScaleName.c_str()).toList();
        std::reverse(colorsListv.begin(), colorsListv.end());

        for (int i = 0; i < colorsListv.size(); ++i) {
            auto color = colorsListv.at(i).value<QColor>();
            float stop = i / float(colorsListv.size() - 1);
            colorsMap[stop] = QColorToColor(color);
        }
    }

    QString gradientScaleId = tlpStringToQString(colorScaleName) + "_gradient?";
    gradient = Settings::instance().value(gradientScaleId).toBool();
    Settings::instance().endGroup();

    Settings::instance().beginGroup("ColorScalesNoRegular");

    if (Settings::instance().contains(colorScaleName.c_str())) {
        QVariantMap colorsMapv = Settings::instance().value(colorScaleName.c_str()).toMap();

        for (auto it = colorsMapv.begin(); it != colorsMapv.end(); ++it) {
            colorsMap[(it.key()).toDouble()] = QColorToColor(it.value().value<QColor>());
        }

        gradient = Settings::instance().value(gradientScaleId).toBool();
    }

    return ColorScale(colorsMap, gradient);
}

void ColorScalesManager::registerColorScale(const string &colorScaleName,
                                            const ColorScale &colorScale) {
    list<string> colorScalesList = ColorScalesManager::getColorScalesList();

    if (find(colorScalesList.begin(), colorScalesList.end(), colorScaleName) !=
        colorScalesList.end()) {
        if (colorScale.hasRegularStops()) {
            QList<QVariant> colorsVector;

            for (uint i = 0; i < const_cast<ColorScale &>(colorScale).getStopsCount(); ++i) {
                float stop = i / float(const_cast<ColorScale &>(colorScale).getStopsCount() - 1);
                Color color = colorScale.getColorAtPos(stop);
                colorsVector.push_back(QVariant(colorToQColor(color)));
            }

            std::reverse(colorsVector.begin(), colorsVector.end());
            Settings::instance().beginGroup("ColorScales");
            Settings::instance().setValue(tlpStringToQString(colorScaleName), colorsVector);
            QString gradientId = tlpStringToQString(colorScaleName) + "_gradient?";
            Settings::instance().setValue(gradientId, colorScale.isGradient());
            Settings::instance().endGroup();
        } else {
            QVariantMap colorsMap;

            for (const auto &it : colorScale.getColorMap()) {
                colorsMap[QString::number(it.first)] = colorToQColor(it.second);
            }

            Settings::instance().beginGroup("ColorScalesNoRegular");
            Settings::instance().setValue(tlpStringToQString(colorScaleName), colorsMap);
            QString gradientId = tlpStringToQString(colorScaleName) + "_gradient?";
            Settings::instance().setValue(gradientId, colorScale.isGradient());
            Settings::instance().endGroup();
        }
    }
}

void ColorScalesManager::removeColorScale(const string &colorScaleName) {
    Settings::instance().beginGroup("ColorScales");
    QString savedColorScaleId = tlpStringToQString(colorScaleName);

    if (Settings::instance().contains(savedColorScaleId)) {
        Settings::instance().remove(savedColorScaleId);
        Settings::instance().remove(savedColorScaleId + "_gradient?");
    }

    Settings::instance().endGroup();
    Settings::instance().beginGroup("ColorScalesNoRegular");
    savedColorScaleId = tlpStringToQString(colorScaleName);

    if (Settings::instance().contains(savedColorScaleId)) {
        Settings::instance().remove(savedColorScaleId);
        Settings::instance().remove(savedColorScaleId + "_gradient?");
    }

    Settings::instance().endGroup();
}

void ColorScalesManager::setLatestColorScale(ColorScale &cs) {
    QList<QVariant> colors;
    QList<QVariant> stops;

    for (const auto &it : cs.getColorMap()) {
        Color c = it.second;
        QColor qc(c.getR(), c.getG(), c.getB(), c.getA());
        colors.push_back(QVariant(qc));
        stops.push_back(QVariant(it.first));
    }

    Settings::instance().beginGroup("viewLatestColorScale");
    Settings::instance().setValue("colors", colors);
    Settings::instance().setValue("stops", stops);
    Settings::instance().setValue("gradient?", cs.isGradient());
    Settings::instance().endGroup();
}

ColorScale ColorScalesManager::getLatestColorScale() {
    Settings::instance().beginGroup("viewLatestColorScale");

    if (Settings::instance().contains("colors")) {
        QList<QVariant> colorsListv = Settings::instance().value("colors").toList();
        QList<QVariant> stopsListv = Settings::instance().value("stops").toList();
        bool gradient = Settings::instance().value("gradient?").toBool();

        map<float, Color> cm;

        for (int i = 0; i < colorsListv.size(); ++i) {
            auto color = colorsListv.at(i).value<QColor>();
            auto stop = stopsListv.at(i).value<float>();
            cm[stop] = QColorToColor(color);
        }

        Settings::instance().endGroup();
        return ColorScale(cm, gradient);
    }

    Settings::instance().endGroup();
    return ColorScale();
}
