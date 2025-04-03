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

#include "ReadGraph.h"
#include "ExportSvg.h"

#include <talipot/ExportModule.h>

using namespace std;
using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // color interpolation
    "Indicates if edge color interpolation has to be used.",

    // size interpolation
    "Indicates if edge size interpolation has to be used.",

    // edge extremities
    "Indicates if edge extremities have to be exported.",

    // Background color
    "Specifies the background color.",

    // No background
    "Specifies if a background is needed.",

    // human readable
    "Adds line-breaks and indentation to empty sections between elements"
    " (ignorable whitespace). The main purpose of this parameter is to split"
    " the data into several lines, and to increase readability for a human reader."
    " Be careful, this adds a large amount of data to the output file.",

    // node labels
    "Specifies if node labels have to be exported.",

    // edge labels
    "Specifies if edge labels have to be exported.",

    // metanode labels
    "Specifies if node and edge labels inside metanodes have to be exported.",

};

class SvgExport : public tlp::ExportModule {

  public:
    PLUGININFORMATION("SVG Export", "Sami Gasri, Charles-Antoine Lami, Bruno Pinaud", "16/07/2013",
                      "<p>Supported extensions: svg, svgz (compressed svg).</p><p>Exports a graph "
                      "visualization in a SVG formatted file.</p>",
                      "1.9", "File")

    std::string icon() const override {
        return ":/talipot/app/icons/32/export_svg.png";
    }

    string fileExtension() const override {
        return "svg";
    }

    SvgExport(tlp::PluginContext *context) : tlp::ExportModule(context) {
        addInParameter<bool>("Edge color interpolation", paramHelp[0].data(), "false");
        addInParameter<bool>("Edge size interpolation", paramHelp[1].data(), "true");
        addInParameter<bool>("Edge extremities", paramHelp[2].data(), "false");
        addInParameter<Color>("Background color", paramHelp[3].data(), "(255,255,255,255)");
        addInParameter<bool>("No background", paramHelp[4].data(), "false");
        addInParameter<bool>("Makes SVG output human readable", paramHelp[5].data(), "true");
        addInParameter<bool>("Export node labels", paramHelp[6].data(), "true");
        addInParameter<bool>("Export edge labels", paramHelp[7].data(), "false");
        addInParameter<bool>("Export metanode labels", paramHelp[8].data(), "false");
    }

    bool exportGraph(ostream &os) override {
        pluginProgress->showPreview(false);
        bool autoformatting = true;

        if (dataSet != nullptr) {
            dataSet->get("Makes SVG output human readable", autoformatting);
        }

        ExportSvg svg(pluginProgress, os, autoformatting);
        bool ret = ReadGraph::readGraph(graph, dataSet, pluginProgress, svg);

        if (!ret && autoformatting) {
            pluginProgress->setError(
                pluginProgress->getError() +
                "<br/><br/>Human readable output is on. This adds a large amount of "
                "data to the output file. Try to disable it and try again.");
        }

        return ret;
    }
};
PLUGIN(SvgExport)
