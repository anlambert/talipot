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

namespace tlp {

class PropertyAlgorithm : tlp::Algorithm {
%TypeHeaderCode
#include <talipot/PropertyAlgorithm.h>
%End

public :

  PropertyAlgorithm(const tlp::PluginContext* context);

  virtual std::string category() const;

};

class BooleanAlgorithm : tlp::PropertyAlgorithm {
%Docstring
Bases: :class:`tlp.Algorithm`

This class is the base interface for selection plugins.

.. rubric:: class attributes

.. py:attribute:: result

   The result of the algorithm must be stored in this :class:`tlp.BooleanProperty`.
%End

%TypeHeaderCode
#include <talipot/PropertyAlgorithm.h>
%End

public:

  tlp::BooleanProperty *result;

  virtual std::string category() const;

protected:

  BooleanAlgorithm (const tlp::PluginContext*);

};


class ColorAlgorithm : tlp::PropertyAlgorithm {
%Docstring
Bases: :class:`tlp.Algorithm`

This class is the base interface for color plugins.

.. rubric:: class attributes

.. py:attribute:: result

   The result of the algorithm must be stored in this :class:`tlp.ColorProperty`.
%End

%TypeHeaderCode
#include <talipot/PropertyAlgorithm.h>
%End

public:

  tlp::ColorProperty *result;

  virtual std::string category() const;

protected:

  ColorAlgorithm (const tlp::PluginContext*);

};

class DoubleAlgorithm : tlp::PropertyAlgorithm {
%Docstring
Bases: :class:`tlp.Algorithm`

This class is the base interface for metric plugins.

.. rubric:: class attributes

.. py:attribute:: result

   The result of the algorithm must be stored in this :class:`tlp.DoubleProperty`.
%End

%TypeHeaderCode
#include <talipot/PropertyAlgorithm.h>
%End

public:

  tlp::DoubleProperty *result;

  virtual std::string category() const;

protected:

  DoubleAlgorithm (const tlp::PluginContext*);

};

class IntegerAlgorithm : tlp::PropertyAlgorithm {
%Docstring
Bases: :class:`tlp.IntegerAlgorithm`

This class is the base interface for integer plugins.

.. rubric:: class attributes

.. py:attribute:: result

   The result of the algorithm must be stored in this :class:`tlp.IntegerProperty`.
%End

%TypeHeaderCode
#include <talipot/PropertyAlgorithm.h>
%End

public:

  tlp::IntegerProperty *result;

  virtual std::string category() const;

protected:

  IntegerAlgorithm (const tlp::PluginContext*);
};

class LayoutAlgorithm : tlp::PropertyAlgorithm {
%Docstring
Bases: :class:`tlp.Algorithm`

This class is the base interface for layout plugins.

.. rubric:: class attributes

.. py:attribute:: result

   The result of the algorithm must be stored in this :class:`tlp.LayoutProperty`.
%End

%TypeHeaderCode
#include <talipot/PropertyAlgorithm.h>
%End

public:

  tlp::LayoutProperty *result;

  virtual std::string category() const;

protected:

  LayoutAlgorithm (const tlp::PluginContext*);

};

class SizeAlgorithm : tlp::PropertyAlgorithm {
%Docstring
Bases: :class:`tlp.Algorithm`

This class is the base interface for size plugins.

.. rubric:: class attributes

.. py:attribute:: result

The result of the algorithm must be stored in this :class:`tlp.SizeProperty`.
%End

%TypeHeaderCode
#include <talipot/PropertyAlgorithm.h>
%End

public:

  tlp::SizeProperty *result;

  virtual std::string category() const;

protected:

  SizeAlgorithm (const tlp::PluginContext*);

};

class StringAlgorithm : tlp::PropertyAlgorithm {
%Docstring
Bases: :class:`tlp.Algorithm`

This class is the base interface for string plugins.

.. rubric:: class attributes

.. py:attribute:: result

   The result of the algorithm must be stored in this :class:`tlp.StringProperty`.
%End

%TypeHeaderCode
#include <talipot/PropertyAlgorithm.h>
%End

public:

  tlp::StringProperty *result;

  virtual std::string category() const;

protected:

  StringAlgorithm (const tlp::PluginContext*);
};

};
