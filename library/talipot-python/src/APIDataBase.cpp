/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "talipot/APIDataBase.h"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

using namespace tlp;

INSTANTIATE_DLL_TEMPLATE(tlp::Singleton<tlp::APIDataBase>, TLP_PYTHON_TEMPLATE_DEFINE_SCOPE)

APIDataBase::APIDataBase() {
  addApiEntry("tlp.node.id");
  addApiEntry("tlp.edge.id");

  addApiEntry("list.append(x)");
  addApiEntry("list.extend(L)");
  addApiEntry("list.insert(i, x)");
  addApiEntry("list.remove(x)");
  addApiEntry("list.pop([i])");
  addApiEntry("list.index(x)");
  addApiEntry("list.count(x)");
  addApiEntry("list.sort()");
  addApiEntry("list.reverse()");

  addApiEntry("dict.clear()");
  addApiEntry("dict.copy()");
  addApiEntry("dict.fromkeys(seq[, value])");
  addApiEntry("dict.get(key[, default])");
  addApiEntry("dict.has_key(key)");
  addApiEntry("dict.items()");
  addApiEntry("dict.iteritems()");
  addApiEntry("dict.iterkeys()");
  addApiEntry("dict.keys()");
  addApiEntry("dict.pop(key[, default])");
  addApiEntry("dict.popitem()");
  addApiEntry("dict.setdefault(key[, default])");
  addApiEntry("dict.update([other])");
  addApiEntry("dict.values()");
  addApiEntry("dict.viewitems()");
  addApiEntry("dict.viewkeys()");
  addApiEntry("dict.viewvalues()");
}

void APIDataBase::loadApiFile(const QString &apiFilePath) {
  QFile apiFile(apiFilePath);

  if (!apiFile.exists()) {
    return;
  }

  apiFile.open(QIODevice::ReadOnly | QIODevice::Text);
  QTextStream in(&apiFile);

  while (!in.atEnd()) {
    QString line = in.readLine();

    if (line.startsWith("talipot.tlp.MaterialDesignIcons?1") ||
        line.startsWith("talipot.tlp.MaterialDesignIcons.__init__") ||
        line.startsWith("talipot.tlp.FontAwesome?1") ||
        line.startsWith("talipot.tlp.FontAwesome.__init__")) {
      continue;
    }

    addApiEntry(line);

    if (line.startsWith("talipot.tlp.Vec3f.")) {
      addApiEntry(line.replace("Vec3f", "Coord"));
      addApiEntry(line.replace("Coord", "Size"));
    }
  }
}

void APIDataBase::addApiEntry(const QString &apiEnt) {
  QString apiEntry(apiEnt);
  int pos = apiEntry.indexOf('.');

  if (apiEntry.contains(QRegularExpression("^talipot.*\\..+"))) {
    apiEntry = apiEntry.mid(pos + 1);
  }

  apiEntry.replace(QRegularExpression("\\?[0-9]+"), "");
  int parenPos = apiEntry.indexOf('(');
  bool func = parenPos != -1;
  QString withoutParams = apiEntry;
  QVector<QString> params;
  QString retType;

  if (func) {
    withoutParams = apiEntry.mid(0, parenPos);
    QString parameters = apiEntry.mid(parenPos + 1, apiEntry.lastIndexOf(')') - parenPos - 1);

    if (!parameters.isEmpty()) {
      QStringList paramsList = parameters.split(',');

      bool dictListSetTupleTypeHint = false;
      QString typeHintParam = "";

      for (const QString &param : paramsList) {
        QString paramClean = param.trimmed();

        if (dictListSetTupleTypeHint) {
          typeHintParam += param;

          if (paramClean.endsWith("]") || paramClean.contains("=")) {
            params.append(typeHintParam.trimmed());
            dictListSetTupleTypeHint = false;
            continue;
          }
        }

        if ((paramClean.startsWith("List") || paramClean.startsWith("Set") ||
             paramClean.startsWith("Tuple") || paramClean.startsWith("Dict") ||
             paramClean.startsWith("Iterable")) &&
            !paramClean.endsWith("]") && !paramClean.contains("=")) {
          typeHintParam = param;
          dictListSetTupleTypeHint = true;
        }

        if (!dictListSetTupleTypeHint) {
          params.append(param.trimmed());
        }
      }
    }

    int retPos = apiEntry.indexOf("->");

    if (retPos != -1) {
      retType = apiEntry.mid(retPos + 2).trimmed();
    }
  }

  pos = withoutParams.indexOf('.');

  while (pos != -1) {
    QString type = withoutParams.mid(0, pos);

    if (!_dictContent.contains(type)) {
      _dictContent[type] = QSet<QString>();
    }

    int newPos = withoutParams.indexOf('.', pos + 1);

    QString dictEntry;

    if (newPos != -1) {
      dictEntry = withoutParams.mid(pos + 1, newPos - pos - 1).trimmed();
    } else {
      dictEntry = withoutParams.mid(pos + 1).trimmed();

      if (func) {

        QString wholeFuncName = type + "." + dictEntry;

        if (!_paramTypes.contains(wholeFuncName)) {
          _paramTypes[wholeFuncName] = QVector<QVector<QString>>();
        }

        _paramTypes[wholeFuncName].append(params);

        if (!retType.isEmpty()) {
          _returnType[wholeFuncName] = retType;
        }
      }
    }

    if (!dictEntry.isEmpty()) {
      _dictContent[type].insert(dictEntry);
    }

    pos = newPos;
  }
}

QSet<QString> APIDataBase::getTypesList() const {
  QSet<QString> ret;
  QList<QString> keys = _dictContent.keys();

  for (const QString &type : keys) {
    ret.insert(type);
  }

  return ret;
}

QSet<QString> APIDataBase::getDictContentForType(const QString &type, const QString &prefix) const {
  QSet<QString> ret;

  if (_dictContent.contains(type)) {
    for (const QString &s : _dictContent[type]) {
      if (s.toLower().startsWith(prefix.toLower())) {
        ret.insert(s);
      }
    }
  }

  return ret;
}

QString APIDataBase::getReturnTypeForMethodOrFunction(const QString &funcName) const {
  QString ret;

  if (_returnType.contains(funcName)) {
    ret = _returnType[funcName];
  }

  return ret;
}

QVector<QVector<QString>>
APIDataBase::getParamTypesForMethodOrFunction(const QString &funcName) const {
  QVector<QVector<QString>> ret;

  if (_paramTypes.contains(funcName)) {
    ret = _paramTypes[funcName];
  }

  return ret;
}

bool APIDataBase::functionExists(const QString &funcName) const {
  return _paramTypes.contains(funcName);
}

QVector<QString> APIDataBase::findTypesContainingDictEntry(const QString &dictEntry) const {
  QVector<QString> ret;
  QHashIterator<QString, QSet<QString>> i(_dictContent);

  while (i.hasNext()) {
    i.next();

    for (const QString &s : i.value()) {
      if (s == dictEntry) {
        ret.append(i.key());
        break;
      }
    }
  }

  return ret;
}

QSet<QString> APIDataBase::getAllDictEntriesStartingWithPrefix(const QString &prefix) const {
  QSet<QString> ret;
  QHashIterator<QString, QSet<QString>> i(_dictContent);

  while (i.hasNext()) {
    i.next();

    for (const QString &s : i.value()) {
      if (s.toLower().startsWith(prefix.toLower())) {
        ret.insert(s);
      }
    }
  }

  return ret;
}

bool APIDataBase::typeExists(const QString &type) const {
  return _dictContent.contains(type);
}

QString APIDataBase::getFullTypeName(const QString &t) const {
  QList<QString> keys = _dictContent.keys();

  for (const QString &type : keys) {
    int pos = type.lastIndexOf(t);

    if (pos != -1 && (pos + t.length()) == type.length() && (pos == 0 || type[pos - 1] == '.')) {
      return type;
    }
  }

  return "";
}

bool APIDataBase::dictEntryExists(const QString &type, const QString &dictEntry) const {
  if (!_dictContent.contains(type)) {
    return false;
  }

  return _dictContent[type].contains(dictEntry);
}
