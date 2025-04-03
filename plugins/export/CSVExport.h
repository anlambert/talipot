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

#ifndef CSV_EXPORT_H
#define CSV_EXPORT_H

#include <iostream>

#include <talipot/ExportModule.h>

/// Export plugin for CSV format
/**
 * \
 * \brief This plugin enables to save the values of talipot graph properties
 *  associated to graph elements in a CSV formatted file.
 *
 * The plugin saves one row by graph element exported
 * and have the following parameters:
 *
 * - <b>Type of elements<b> which enables to choose the type of graph elements
 * to export, and has 3 possible values - nodes, edges or both -
 * The default value is nodes.
 *
 * - <b>Export selection<b> which indicates if only selected elements
 * have to be exported. The default value is false.
 *
 * - <b>Export id<b> which indicates if the id of graph elements
 * has to be exported. The default value is false.
 * If it is set to true and nodes are exported, a field named 'node id',
 * whose value is the id of the node, is added at the beginning of each row.
 * If edges are exported, two fields named 'src id' and 'tgt id'
 * corresponding to the id of the source and target nodes of an edge
 * are exported at the beginning of each row (immediately after the node id
 * field if it is present).
 * The 'node id' field is left blank when exporting a row corresponding to
 * an edge; while the 'src id' and 'tgt id' are left blank when exporting
 * a row for a node.
 *
 * - <b>Field separator<b> which enables to choose the field separator and
 * has 4 possible values - semi-colon (;), comma (,), tab, space -
 * The default values is semi-colon.
 *
 * - <b>String delimiter<b> which enables to indicates the delimiter used
 * around the string of characters values.
 * It has 2 possible values - double quote("), quote(') - and its default
 * value is double quote.
 */
class CsvExport : public tlp::ExportModule {
  public:
    PLUGININFORMATION("CSV Export", "David Auber, Patrick Mary", "18/01/2011",
                      "<p>Supported extensions: csv</p><p>Exports the values of talipot graph "
                      "properties associated to graph elements in a CSV file.</p>",
                      "1.0", "File")

    std::string icon() const override {
        return ":/talipot/app/icons/32/spreadsheet.png";
    }

    std::string fileExtension() const override {
        return "csv";
    }

    CsvExport(const tlp::PluginContext *context);
    bool exportGraph(std::ostream &) override;

    std::string fieldSeparator;
    char stringDelimiter;
    char decimalMark;

    void exportString(std::ostream &os, const std::string &s) const {
        os << stringDelimiter << s << stringDelimiter;
    }
};
#endif // CSV_EXPORT_H
