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

#ifndef TALIPOT_API_DATABASE_H
#define TALIPOT_API_DATABASE_H

#include <QSet>
#include <QVector>
#include <QHash>
#include <QString>

#include <talipot/config.h>
#include <talipot/Singleton.h>
#include <talipot/TlpTools.h>

namespace tlp {

class APIDataBase;
DECLARE_DLL_TEMPLATE_INSTANCE(Singleton<APIDataBase>, TLP_PYTHON_TEMPLATE_DECLARE_SCOPE)

class TLP_PYTHON_SCOPE APIDataBase : public Singleton<APIDataBase> {

    friend class Singleton<APIDataBase>;

    APIDataBase();

    QHash<QString, QSet<QString>> _dictContent;
    QHash<QString, QString> _returnType;
    QHash<QString, QVector<QVector<QString>>> _paramTypes;

  public:
    void loadApiFile(const QString &apiFilePath);

    void addApiEntry(const QString &apiEntry);

    QSet<QString> getTypesList() const;
    QSet<QString> getDictContentForType(const QString &type, const QString &prefix = "") const;
    QString getReturnTypeForMethodOrFunction(const QString &funcName) const;
    QVector<QVector<QString>> getParamTypesForMethodOrFunction(const QString &funcName) const;
    bool functionExists(const QString &funcName) const;
    QVector<QString> findTypesContainingDictEntry(const QString &dictEntry) const;
    QSet<QString> getAllDictEntriesStartingWithPrefix(const QString &prefix) const;

    bool typeExists(const QString &type) const;
    QString getFullTypeName(const QString &type) const;
    bool dictEntryExists(const QString &type, const QString &dictEntry) const;
};
}

#endif // TALIPOT_API_DATABASE_H
