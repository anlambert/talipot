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

#include "talipot/Mimes.h"

using namespace tlp;

AlgorithmMimeType::AlgorithmMimeType(QString algorithmName, const DataSet &data)
    : _algorithm(algorithmName), _params(data) {}

void AlgorithmMimeType::run(Graph *g) const {
    emit mimeRun(g);
}

QStringList GraphMimeType::formats() const {
    return QMimeData::formats() << GRAPH_MIME_TYPE;
}

QStringList AlgorithmMimeType::formats() const {
    return QMimeData::formats() << ALGORITHM_NAME_MIME_TYPE << DATASET_MIME_TYPE;
}

QStringList PanelMimeType::formats() const {
    return QMimeData::formats() << WORKSPACE_PANEL_MIME_TYPE;
}
