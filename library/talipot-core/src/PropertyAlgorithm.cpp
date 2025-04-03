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

#include <talipot/PropertyAlgorithm.h>

#include <talipot/BooleanProperty.h>
#include <talipot/ColorProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/IntegerProperty.h>
#include <talipot/LayoutProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/StringProperty.h>

tlp::BooleanAlgorithm::BooleanAlgorithm(const tlp::PluginContext *context)
    : tlp::TemplateAlgorithm<BooleanProperty>(context) {
    addOutParameter<tlp::BooleanProperty>(
        "result", "This parameter indicates the property to compute.", "viewSelection");
}

tlp::ColorAlgorithm::ColorAlgorithm(const tlp::PluginContext *context)
    : tlp::TemplateAlgorithm<ColorProperty>(context) {
    addOutParameter<tlp::ColorProperty>(
        "result", "This parameter indicates the property to compute.", "viewColor");
}

tlp::DoubleAlgorithm::DoubleAlgorithm(const tlp::PluginContext *context)
    : tlp::TemplateAlgorithm<DoubleProperty>(context) {
    addOutParameter<tlp::DoubleProperty>(
        "result", "This parameter indicates the property to compute.", "viewMetric");
}

tlp::IntegerAlgorithm::IntegerAlgorithm(const tlp::PluginContext *context)
    : tlp::TemplateAlgorithm<IntegerProperty>(context) {
    addOutParameter<tlp::IntegerProperty>("result",
                                          "This parameter indicates the property to compute.");
}

tlp::LayoutAlgorithm::LayoutAlgorithm(const tlp::PluginContext *context)
    : tlp::TemplateAlgorithm<LayoutProperty>(context) {
    addOutParameter<tlp::LayoutProperty>(
        "result", "This parameter indicates the property to compute.", "viewLayout");
}

tlp::SizeAlgorithm::SizeAlgorithm(const tlp::PluginContext *context)
    : tlp::TemplateAlgorithm<SizeProperty>(context) {
    addOutParameter<tlp::SizeProperty>(
        "result", "This parameter indicates the property to compute.", "viewSize");
}

tlp::StringAlgorithm::StringAlgorithm(const tlp::PluginContext *context)
    : tlp::TemplateAlgorithm<StringProperty>(context) {
    addOutParameter<tlp::StringProperty>(
        "result", "This parameter indicates the property to compute.", "viewLabel");
}
