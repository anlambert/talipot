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

#ifndef TALIPOT_AUTO_COMPLETION_DATA_BASE_H
#define TALIPOT_AUTO_COMPLETION_DATA_BASE_H

#include <QString>
#include <QSet>
#include <QHash>

#include <talipot/Graph.h>
#include <talipot/APIDataBase.h>

namespace tlp {

class TLP_PYTHON_SCOPE AutoCompletionDataBase {

    QSet<QString> getSubGraphsListIfContext(const QString &context,
                                            const QString &editedFunction) const;
    QSet<QString> getGraphPropertiesListIfContext(const QString &context,
                                                  const QString &editedFunction) const;
    QSet<QString> getPluginParametersListIfContext(const QString &context,
                                                   const QString &editedFunction) const;
    QString getClassAttributeType(const QString &className, const QString &classAttribute) const;
    QSet<QString> getGraphsAttributesListIfContext(const QString &context,
                                                   const QString &editedFunction) const;

    tlp::Graph *_graph;
    APIDataBase *_apiDb;
    QSet<QString> _globalAutoCompletionList;
    QHash<QString, QSet<QString>> _functionAutoCompletionList;
    QHash<QString, QHash<QString, QString>> _varToType;
    QHash<QString, QHash<QString, QString>> _classAttributeToType;
    QHash<QString, QHash<QString, QString>> _varToPluginName;
    QHash<QString, QHash<QString, QSet<QString>>> _pluginParametersDataSet;
    QHash<QString, QString> _iteratorType;
    QHash<QString, QSet<QString>> _classContents;
    QHash<QString, QSet<QString>> _classBases;
    QString _lastFoundType;

  public:
    AutoCompletionDataBase(APIDataBase *_apiDb = nullptr);

    void setGraph(tlp::Graph *graph) {
        _graph = graph;
    }

    void analyseCurrentScriptCode(const QString &code, const int currentLine,
                                  const bool interactiveSession = false,
                                  const QString &moduleName = "");

    QSet<QString> getAutoCompletionListForContext(const QString &context,
                                                  const QString &editedFunction,
                                                  bool dotContext = false);

    QString getLastFoundType() const {
        return _lastFoundType;
    }

    QString findTypeForExpr(const QString &expr, const QString &funcName) const;

    QVector<QVector<QString>> getParamTypesForMethodOrFunction(const QString &type,
                                                               const QString &funcName) const;

    QString getReturnTypeForMethodOrFunction(const QString &type, const QString &funcName) const;

    QSet<QString> getAllDictForType(const QString &type, const QString &prefix,
                                    const bool root = true) const;

    QString getTypeNameForVar(const QString &varName) const;

    QString getTypeNameForExpr(const QString &varName) const;
};
}

#endif // TALIPOT_AUTO_COMPLETION_DATA_BASE_H
