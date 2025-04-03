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

#ifndef TALIPOT_COLOR_H
#define TALIPOT_COLOR_H

#include <talipot/Vector.h>

namespace tlp {

class TLP_SCOPE Color : public Vector<unsigned char, 4> {
  public:
    Color(const Vector<unsigned char, 4> &c) : tlp::Vector<unsigned char, 4>(c) {}

    Color(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0,
          unsigned char alpha = 255) {
        set(red, green, blue, alpha);
    }

    float getRGL() const {
        return (*this)[0] / 255.f;
    }

    float getGGL() const {
        return (*this)[1] / 255.f;
    }

    float getBGL() const {
        return (*this)[2] / 255.f;
    }

    float getAGL() const {
        return (*this)[3] / 255.f;
    }

    float *getGL() const {
        auto *result = new float[4];
        result[0] = getRGL();
        result[1] = getGGL();
        result[2] = getBGL();
        result[3] = getAGL();
        return result;
    }

    unsigned char getR() const {
        return (*this)[0];
    }

    unsigned char getG() const {
        return (*this)[1];
    }

    unsigned char getB() const {
        return (*this)[2];
    }

    unsigned char getA() const {
        return (*this)[3];
    }

    void setR(unsigned char red) {
        (*this)[0] = red;
    }

    void setG(unsigned char green) {
        (*this)[1] = green;
    }

    void setB(unsigned char blue) {
        (*this)[2] = blue;
    }

    void setA(unsigned char alpha) {
        (*this)[3] = alpha;
    }

    long getTrueColor();

    int getH() const;

    int getS() const;

    int getV() const;

    void setH(int);

    void setS(int);

    void setV(int);

    static const Color Amaranth;
    static const Color Amber;
    static const Color Apricot;
    static const Color Aquamarine;
    static const Color Azure;
    static const Color BabyBlue;
    static const Color Beige;
    static const Color Black;
    static const Color Blue;
    static const Color BlueGreen;
    static const Color BlueViolet;
    static const Color Blush;
    static const Color Bronze;
    static const Color Brown;
    static const Color Burgundy;
    static const Color Byzantium;
    static const Color Carmine;
    static const Color Cerise;
    static const Color Cerulean;
    static const Color Champagne;
    static const Color ChartreuseGreen;
    static const Color Chocolate;
    static const Color Coffee;
    static const Color Copper;
    static const Color Coral;
    static const Color Crimson;
    static const Color Cyan;
    static const Color DesertSand;
    static const Color ElectricBlue;
    static const Color Erin;
    static const Color Gold;
    static const Color Gray;
    static const Color Green;
    static const Color Harlequin;
    static const Color Indigo;
    static const Color Ivory;
    static const Color Jade;
    static const Color JungleGreen;
    static const Color Lavender;
    static const Color Lemon;
    static const Color Lilac;
    static const Color Lime;
    static const Color Magenta;
    static const Color MagentaRose;
    static const Color Maroon;
    static const Color Mauve;
    static const Color NavyBlue;
    static const Color Olive;
    static const Color Orange;
    static const Color OrangeRed;
    static const Color Orchid;
    static const Color Peach;
    static const Color Pear;
    static const Color Periwinkle;
    static const Color PersianBlue;
    static const Color Pink;
    static const Color Plum;
    static const Color PrussianBlue;
    static const Color Puce;
    static const Color Purple;
    static const Color Raspberry;
    static const Color Red;
    static const Color RedViolet;
    static const Color Rose;
    static const Color Salmon;
    static const Color Sapphire;
    static const Color Scarlet;
    static const Color Silver;
    static const Color SlateGray;
    static const Color SpringBud;
    static const Color SpringGreen;
    static const Color Tan;
    static const Color Taupe;
    static const Color Teal;
    static const Color Turquoise;
    static const Color Violet;
    static const Color Viridian;
    static const Color White;
    static const Color Yellow;
};

TLP_SCOPE std::ostream &operator<<(std::ostream &os, const tlp::Color &);
TLP_SCOPE std::istream &operator>>(std::istream &is, tlp::Color &);
}

namespace std {
template <>
struct hash<tlp::Color> {
    std::size_t operator()(const tlp::Color &c) const {
        return hash_vector(c);
    }
};
}

#endif // TALIPOT_COLOR_H
