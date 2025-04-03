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

#ifndef TALIPOT_MIMES_H
#define TALIPOT_MIMES_H

#include <QMimeData>
#include <QStringList>

#include <talipot/config.h>
#include <talipot/DataSet.h>

namespace tlp {
class Graph;
class WorkspacePanel;
class Algorithm;
class DataSet;

const QString GRAPH_MIME_TYPE = QString("application/x-talipot-mime;value=\"graph\"");
const QString WORKSPACE_PANEL_MIME_TYPE =
    QString("application/x-talipot-mime;value=\"workspace-panel\"");
const QString ALGORITHM_NAME_MIME_TYPE =
    QString("application/x-talipot-mime;value=\"algorithm-name\"");
const QString DATASET_MIME_TYPE = QString("application/x-talipot-mime;value=\"dataset\"");

/**
 * @brief The GraphMimeType class allows to transfer a graph pointer through a QMimeData
 */
class TLP_QT_SCOPE GraphMimeType : public QMimeData {
  public:
    GraphMimeType() : QMimeData(), _graph(nullptr) {}
    void setGraph(tlp::Graph *graph) {
        _graph = graph;
    }

    tlp::Graph *graph() const {
        return _graph;
    }

    QStringList formats() const override;

  private:
    tlp::Graph *_graph;
};

class TLP_QT_SCOPE AlgorithmMimeType : public QMimeData {
    Q_OBJECT

    QString _algorithm;
    tlp::DataSet _params;

  public:
    AlgorithmMimeType(QString algorithmName, const tlp::DataSet &data);
    void run(tlp::Graph *) const;

    QString algorithm() const {
        return _algorithm;
    }
    tlp::DataSet params() const {
        return _params;
    }

    QStringList formats() const override;

  signals:
    void mimeRun(tlp::Graph *) const;
};

class TLP_QT_SCOPE PanelMimeType : public QMimeData {
  public:
    void setPanel(tlp::WorkspacePanel *panel) {
        _panel = panel;
    }

    tlp::WorkspacePanel *panel() const {
        return _panel;
    }

    QStringList formats() const override;

  private:
    tlp::WorkspacePanel *_panel;
};
}
#endif // TALIPOT_MIMES_H
