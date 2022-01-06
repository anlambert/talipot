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

#ifndef TALIPOT_META_TYPES_H
#define TALIPOT_META_TYPES_H

#include <QVector>
#include <QStringList>
#include <QVariant>

#include <talipot/DataSet.h>
#include <talipot/Graph.h>
#include <talipot/Color.h>
#include <talipot/Coord.h>
#include <talipot/Size.h>
#include <talipot/BooleanProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/ColorProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/StringProperty.h>
#include <talipot/LayoutProperty.h>
#include <talipot/GraphProperty.h>
#include <talipot/IntegerProperty.h>
#include <talipot/ColorScale.h>
#include <talipot/GlGraphStaticData.h>
#include <talipot/GlLabel.h>
#include <talipot/Font.h>
#include <talipot/StringCollection.h>
#include <talipot/ViewSettings.h>

// Helper class for filesystem types handling
struct FileDescriptor {
  enum FileType { File, Directory };

  FileDescriptor() = default;
  FileDescriptor(const QString &absolutePath, FileType fileType, bool existing = true)
      : absolutePath(absolutePath), type(fileType), mustExist(existing) {}
  FileDescriptor(const FileDescriptor &d) {
    absolutePath = d.absolutePath;
    type = d.type;
    mustExist = d.mustExist;
  }
  FileDescriptor &operator=(const FileDescriptor &) = default;
  QString absolutePath;
  FileType type;
  // indicate if the file or dir must exist
  bool mustExist;
  QString fileFilterPattern;
};

struct TextureFile {
  QString texturePath;
  TextureFile() = default;
  TextureFile(const QString &texturePath) : texturePath(texturePath) {}
};

struct FontIconName {

  FontIconName() = default;

  FontIconName(const QString &iconName) : iconName(iconName) {}

  QString iconName;
};

// Declare talipot type compatible with QVariant
Q_DECLARE_METATYPE(tlp::DataSet)

Q_DECLARE_METATYPE(tlp::DoubleProperty *)
Q_DECLARE_METATYPE(tlp::DoubleVectorProperty *)
Q_DECLARE_METATYPE(std::vector<double>)

Q_DECLARE_METATYPE(tlp::Color)
Q_DECLARE_METATYPE(tlp::ColorProperty *)
Q_DECLARE_METATYPE(tlp::ColorVectorProperty *)
Q_DECLARE_METATYPE(std::vector<tlp::Color>)

Q_DECLARE_METATYPE(tlp::Size)
Q_DECLARE_METATYPE(tlp::SizeProperty *)
Q_DECLARE_METATYPE(tlp::SizeVectorProperty *)
Q_DECLARE_METATYPE(std::vector<tlp::Size>)

Q_DECLARE_METATYPE(std::string)
Q_DECLARE_METATYPE(tlp::StringProperty *)
Q_DECLARE_METATYPE(tlp::StringVectorProperty *)
Q_DECLARE_METATYPE(std::vector<std::string>)

Q_DECLARE_METATYPE(tlp::Coord)
Q_DECLARE_METATYPE(tlp::LayoutProperty *)
Q_DECLARE_METATYPE(tlp::CoordVectorProperty *)
Q_DECLARE_METATYPE(std::vector<tlp::Coord>)

Q_DECLARE_METATYPE(tlp::Graph *)
Q_DECLARE_METATYPE(tlp::GraphProperty *)

Q_DECLARE_METATYPE(tlp::IntegerProperty *)
Q_DECLARE_METATYPE(tlp::IntegerVectorProperty *)
Q_DECLARE_METATYPE(std::vector<int>)

Q_DECLARE_METATYPE(tlp::BooleanProperty *)
Q_DECLARE_METATYPE(tlp::BooleanVectorProperty *)
// Q_DECLARE_METATYPE(std::vector<bool>)
Q_DECLARE_METATYPE(QVector<bool>)

Q_DECLARE_METATYPE(tlp::node)
Q_DECLARE_METATYPE(tlp::edge)
Q_DECLARE_METATYPE(std::set<tlp::edge>)

Q_DECLARE_METATYPE(tlp::NumericProperty *)

Q_DECLARE_METATYPE(tlp::PropertyInterface *)

Q_DECLARE_METATYPE(tlp::ColorScale)

Q_DECLARE_METATYPE(tlp::StringCollection)

Q_DECLARE_METATYPE(TextureFile)
Q_DECLARE_METATYPE(FileDescriptor)
Q_DECLARE_METATYPE(FontIconName)

Q_DECLARE_METATYPE(tlp::NodeShape::NodeShapes)
Q_DECLARE_METATYPE(tlp::EdgeShape::EdgeShapes)
Q_DECLARE_METATYPE(tlp::EdgeExtremityShape::EdgeExtremityShapes)

Q_DECLARE_METATYPE(tlp::LabelPosition::LabelPositions)
Q_DECLARE_METATYPE(tlp::Font)

Q_DECLARE_METATYPE(QString)
Q_DECLARE_METATYPE(QStringList)

namespace tlp {
class TLP_QT_SCOPE MetaTypes {
  MetaTypes() = default;

  template <typename T>
  static QVariant typedVariant(tlp::DataType *dm) {
    T result;

    if (dm) {
      result = *(static_cast<T *>(dm->value));
    }

    return QVariant::fromValue<T>(result);
  }

public:
  static tlp::DataType *qVariantToDataType(const QVariant &);
  static QVariant dataTypeToQvariant(tlp::DataType *, const std::string &paramName);
};

class TLP_QT_SCOPE QStringListType : public TypeInterface<QStringList> {
public:
  static void write(std::ostream &, const RealType &);
  static bool read(std::istream &, RealType &);
  FORWARD_STRING_METHODS(QStringListType)
};

class TLP_QT_SCOPE QStringType : public TypeInterface<QString> {
public:
  static void write(std::ostream &, const RealType &);
  static bool read(std::istream &, RealType &);
  static std::string toString(const RealType &);
  static bool fromString(RealType &, const std::string &);
};

extern void initQTypeSerializers();
}

#endif // TALIPOT_META_TYPES_H
