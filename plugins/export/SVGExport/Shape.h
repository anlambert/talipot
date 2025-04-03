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

#ifndef SHAPE_H
#define SHAPE_H

#include <talipot/Color.h>
#include <talipot/Coord.h>
#include <talipot/Size.h>

#include <QXmlStreamWriter>
#include <QString>

struct ExtremityShape {

    static void Arrow(QXmlStreamWriter &res, const QString &color, bool tgt);
    static void Circle(QXmlStreamWriter &res, const QString &color, bool tgt);
    static void Cross(QXmlStreamWriter &res, const QString &color, bool tgt);
    static void Diamond(QXmlStreamWriter &res, const QString &color, bool tgt);
    static void Hexagon(QXmlStreamWriter &res, const QString &color, bool tgt);
    static void Pentagon(QXmlStreamWriter &res, const QString &color, bool tgt);
    static void Ring(QXmlStreamWriter &res, const QString &color, bool tgt);
    static void Square(QXmlStreamWriter &res, const QString &color, bool tgt);
    static void Star(QXmlStreamWriter &res, const QString &color, bool tgt);
    static void Cube(QXmlStreamWriter &res, const QString &color, bool tgt);
    static void CubeOutlinedTransparent(QXmlStreamWriter &res, const QString &color, bool tgt);
    static void Cone(QXmlStreamWriter &res, const QString &color, bool tgt);
    static void Cylinder(QXmlStreamWriter &res, const QString &color, bool tgt);
    static void Sphere(QXmlStreamWriter &res, const tlp::Color &color, bool tgt,
                       const unsigned id_src_gradient);
    static void GlowSphere(QXmlStreamWriter &res, const tlp::Color &color, bool tgt,
                           const unsigned id_gradient);
    static void Icon(QXmlStreamWriter &res, const QString &color, const std::string &iconName,
                     bool tgt);
};

#endif // SHAPE_H
