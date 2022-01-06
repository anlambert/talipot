/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "talipot/MetaTypes.h"
#include "talipot/TlpQtTools.h"

using namespace tlp;

// QStringListType
void QStringListType::write(std::ostream &oss, const RealType &t) {
  tlp::StringVectorType::RealType stdVect(t.size());
  int i = 0;

  for (const QString &s : t) {
    stdVect[i] = tlp::QStringToTlpString(s);
    ++i;
  }

  StringVectorType::write(oss, stdVect);
}

bool QStringListType::read(std::istream &iss, RealType &t) {
  StringVectorType::RealType stdVect;

  if (!StringVectorType::read(iss, stdVect)) {
    return false;
  }

  for (const auto &s : stdVect) {
    t.push_back(tlp::tlpStringToQString(s));
  }

  return true;
}

void QStringType::write(std::ostream &oss, const QString &t) {
  StringType::write(oss, QStringToTlpString(t));
}

bool QStringType::read(std::istream &iss, QString &t) {
  std::string s;

  if (!StringType::read(iss, s)) {
    return false;
  }

  t = tlp::tlpStringToQString(s);
  return true;
}

std::string QStringType::toString(const QString &s) {
  return tlp::QStringToTlpString(s);
}

bool QStringType::fromString(QString &s, const std::string &str) {
  s = tlp::tlpStringToQString(str);
  return true;
}

#define CHECK_QVARIANT(TYPE)               \
  if (v.userType() == qMetaTypeId<TYPE>()) \
    return new TypedData<TYPE>(new TYPE(v.value<TYPE>()));

tlp::DataType *MetaTypes::qVariantToDataType(const QVariant &v) {
  CHECK_QVARIANT(tlp::GraphType::RealType);
  CHECK_QVARIANT(tlp::EdgeSetType::RealType);
  CHECK_QVARIANT(tlp::DoubleType::RealType);
  CHECK_QVARIANT(tlp::FloatType::RealType);
  CHECK_QVARIANT(tlp::IntegerType::RealType);
  CHECK_QVARIANT(tlp::UnsignedIntegerType::RealType);
  CHECK_QVARIANT(tlp::LongType::RealType);
  CHECK_QVARIANT(tlp::BooleanType::RealType);

  // CHECK_QVARIANT(tlp::BooleanVectorType::RealType);
  if (v.userType() == qMetaTypeId<QVector<bool>>()) {
    auto vb = v.value<QVector<bool>>();
    return new TypedData<tlp::BooleanVectorType::RealType>(
        new tlp::BooleanVectorType::RealType(std::vector<bool>(vb.begin(), vb.end())));
  }

  CHECK_QVARIANT(tlp::PointType::RealType);
  CHECK_QVARIANT(tlp::SizeType::RealType);
  CHECK_QVARIANT(tlp::StringType::RealType);
  CHECK_QVARIANT(tlp::ColorType::RealType);
  CHECK_QVARIANT(tlp::ColorVectorType::RealType);
  CHECK_QVARIANT(tlp::CoordVectorType::RealType);
  CHECK_QVARIANT(tlp::DoubleVectorType::RealType);
  CHECK_QVARIANT(tlp::IntegerVectorType::RealType);
  // same as tlp::CoordVectorType::RealType
  // CHECK_QVARIANT(tlp::SizeVectorType::RealType);
  CHECK_QVARIANT(tlp::StringVectorType::RealType);

  CHECK_QVARIANT(tlp::BooleanProperty *);
  CHECK_QVARIANT(tlp::DoubleProperty *);
  CHECK_QVARIANT(tlp::LayoutProperty *);
  CHECK_QVARIANT(tlp::StringProperty *);
  CHECK_QVARIANT(tlp::IntegerProperty *);
  CHECK_QVARIANT(tlp::SizeProperty *);
  CHECK_QVARIANT(tlp::ColorProperty *);
  CHECK_QVARIANT(tlp::BooleanVectorProperty *);
  CHECK_QVARIANT(tlp::DoubleVectorProperty *);
  CHECK_QVARIANT(tlp::CoordVectorProperty *);
  CHECK_QVARIANT(tlp::StringVectorProperty *);
  CHECK_QVARIANT(tlp::IntegerVectorProperty *);
  CHECK_QVARIANT(tlp::SizeVectorProperty *);
  CHECK_QVARIANT(tlp::ColorVectorProperty *);
  CHECK_QVARIANT(tlp::NumericProperty *);
  CHECK_QVARIANT(tlp::PropertyInterface *);

  CHECK_QVARIANT(ColorScale);

  CHECK_QVARIANT(StringCollection);

  CHECK_QVARIANT(QStringType::RealType);
  CHECK_QVARIANT(QStringListType::RealType);

  if (v.userType() == qMetaTypeId<FileDescriptor>()) {
    auto desc = v.value<FileDescriptor>();
    return new TypedData<std::string>(new std::string(QStringToTlpString(desc.absolutePath)));
  }

  if (v.userType() == qMetaTypeId<FontIconName>()) {
    return new TypedData<std::string>(
        new std::string(QStringToTlpString(v.value<FontIconName>().iconName)));
  }

  return nullptr;
}

#define CHECK_DATATYPE(TYPE)       \
  if (type == typeid(TYPE).name()) \
    return typedVariant<TYPE>(dm);

QVariant MetaTypes::dataTypeToQvariant(tlp::DataType *dm, const std::string &paramName) {
  std::string type = dm->getTypeName();

  // First, we set up some hack to provide custom types for string data whose name starts with
  // file:: or dir::
  QString name(paramName.c_str());

  if (type == typeid(std::string).name() &&
      (name.startsWith("file::") || name.startsWith("anyfile::") || name.startsWith("dir::"))) {
    FileDescriptor desc;
    desc.absolutePath = tlpStringToQString(*static_cast<std::string *>(dm->value));
    desc.type = name.startsWith("dir::") ? FileDescriptor::Directory : FileDescriptor::File;
    desc.mustExist = !name.startsWith("any");
    return QVariant::fromValue<FileDescriptor>(desc);
  }

  // ******

  CHECK_DATATYPE(tlp::GraphType::RealType);
  CHECK_DATATYPE(tlp::EdgeSetType::RealType);
  CHECK_DATATYPE(tlp::DoubleType::RealType);
  CHECK_DATATYPE(tlp::FloatType::RealType);
  CHECK_DATATYPE(tlp::IntegerType::RealType);
  CHECK_DATATYPE(tlp::UnsignedIntegerType::RealType);
  CHECK_DATATYPE(tlp::LongType::RealType);
  CHECK_DATATYPE(tlp::BooleanType::RealType);

  // CHECK_DATATYPE(tlp::BooleanVectorType::RealType);
  if (type == typeid(tlp::BooleanVectorType::RealType).name()) {
    tlp::BooleanVectorType::RealType result;

    if (dm) {
      result = *(static_cast<tlp::BooleanVectorType::RealType *>(dm->value));
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    return QVariant::fromValue<QVector<bool>>(QVector<bool>(result.begin(), result.end()));
#else
    return QVariant::fromValue<QVector<bool>>(QVector<bool>::fromStdVector(result));
#endif
  }

  CHECK_DATATYPE(tlp::PointType::RealType);
  CHECK_DATATYPE(tlp::SizeType::RealType);
  CHECK_DATATYPE(tlp::StringType::RealType);
  CHECK_DATATYPE(tlp::ColorType::RealType);
  CHECK_DATATYPE(tlp::ColorVectorType::RealType);
  CHECK_DATATYPE(tlp::CoordVectorType::RealType);
  CHECK_DATATYPE(tlp::DoubleVectorType::RealType);
  CHECK_DATATYPE(tlp::IntegerVectorType::RealType);
  // same as tlp::CoordVectorType::RealType
  // CHECK_DATATYPE(tlp::SizeVectorType::RealType);
  CHECK_DATATYPE(tlp::StringVectorType::RealType);

  CHECK_DATATYPE(tlp::BooleanProperty *);
  CHECK_DATATYPE(tlp::DoubleProperty *);
  CHECK_DATATYPE(tlp::LayoutProperty *);
  CHECK_DATATYPE(tlp::StringProperty *);
  CHECK_DATATYPE(tlp::IntegerProperty *);
  CHECK_DATATYPE(tlp::SizeProperty *);
  CHECK_DATATYPE(tlp::ColorProperty *);
  CHECK_DATATYPE(tlp::BooleanVectorProperty *);
  CHECK_DATATYPE(tlp::DoubleVectorProperty *);
  CHECK_DATATYPE(tlp::CoordVectorProperty *);
  CHECK_DATATYPE(tlp::StringVectorProperty *);
  CHECK_DATATYPE(tlp::IntegerVectorProperty *);
  CHECK_DATATYPE(tlp::SizeVectorProperty *);
  CHECK_DATATYPE(tlp::ColorProperty *);
  CHECK_DATATYPE(tlp::NumericProperty *);
  CHECK_DATATYPE(tlp::PropertyInterface *);

  CHECK_DATATYPE(tlp::ColorScale);

  CHECK_DATATYPE(tlp::StringCollection);

  // Qt built-in types
  CHECK_DATATYPE(QStringListType::RealType);
  CHECK_DATATYPE(QStringType::RealType);
  return QVariant();
}

void tlp::initQTypeSerializers() {
  DataSet::registerDataTypeSerializer<QStringListType::RealType>(
      KnownTypeSerializer<QStringListType>("qstringlist"));
  DataSet::registerDataTypeSerializer<QStringType::RealType>(
      KnownTypeSerializer<QStringType>("qstring"));
}
