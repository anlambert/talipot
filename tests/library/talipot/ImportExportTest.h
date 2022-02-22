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

#ifndef IMPORT_EXPORT_TEST_H
#define IMPORT_EXPORT_TEST_H

#include "CppUnitIncludes.h"

namespace tlp {
class Graph;
}

class ImportExportTest : public CppUnit::TestFixture {

public:
  ImportExportTest(const std::string &importAlgorithm, const std::string &exportAlgorithm);
  void setUp() override;
  void testgridImportExport();
  void testgridImportExportNonAsciiPath();
  void testSubGraphsImportExport();
  void testAttributes();
  void testNanInfValuesImportExport();
  void testMetaGraphImportExport();

protected:
  void updateIdProperty(tlp::Graph *graph) const;
  tlp::Graph *createSimpleGraph() const;
  void exportImportGraph(tlp::Graph *original,
                         const std::string &exportBaseFilename = "graph_export");
  void exportGraph(tlp::Graph *graph, const std::string &exportPluginName,
                   const std::string &filename);
  tlp::Graph *importGraph(const std::string &importPluginName, const std::string &filename);

  void testGraphsTopologiesAreEqual(tlp::Graph *first, tlp::Graph *second);
  void testGraphPropertiesAreEqual(tlp::Graph *first, tlp::Graph *second);
  void testGraphsAreEqual(tlp::Graph *first, tlp::Graph *second);
  void testGraphAttributesAreEqual(tlp::Graph *first, tlp::Graph *second);
  const std::string importAlgorithm;
  const std::string exportAlgorithm;
};

class TlpImportExportTest : public ImportExportTest {
  CPPUNIT_TEST_SUITE(TlpImportExportTest);
  CPPUNIT_TEST(testgridImportExport);
  CPPUNIT_TEST(testgridImportExportNonAsciiPath);
  CPPUNIT_TEST(testAttributes);
  CPPUNIT_TEST(testSubGraphsImportExport);
  CPPUNIT_TEST(testNanInfValuesImportExport);
  CPPUNIT_TEST(testMetaGraphImportExport);
  CPPUNIT_TEST_SUITE_END();

public:
  TlpImportExportTest();
};

class TlpBImportExportTest : public ImportExportTest {
  CPPUNIT_TEST_SUITE(TlpBImportExportTest);
  CPPUNIT_TEST(testgridImportExport);
  CPPUNIT_TEST(testgridImportExportNonAsciiPath);
  CPPUNIT_TEST(testAttributes);
  CPPUNIT_TEST(testSubGraphsImportExport);
  CPPUNIT_TEST(testNanInfValuesImportExport);
  CPPUNIT_TEST(testMetaGraphImportExport);
  CPPUNIT_TEST_SUITE_END();

public:
  TlpBImportExportTest();
};

class JsonImportExportTest : public ImportExportTest {
  CPPUNIT_TEST_SUITE(JsonImportExportTest);
  CPPUNIT_TEST(testgridImportExport);
  CPPUNIT_TEST(testgridImportExportNonAsciiPath);
  CPPUNIT_TEST(testAttributes);
  CPPUNIT_TEST(testSubGraphsImportExport);
  CPPUNIT_TEST(testNanInfValuesImportExport);
  CPPUNIT_TEST(testMetaGraphImportExport);
  CPPUNIT_TEST_SUITE_END();

public:
  JsonImportExportTest();
};

class TalipotSaveLoadGraphFunctionsTest : public ImportExportTest {
  CPPUNIT_TEST_SUITE(TalipotSaveLoadGraphFunctionsTest);
  CPPUNIT_TEST(testTalipotSaveLoadGraphFunctions);
  CPPUNIT_TEST(testSaveGraphNoSuchFile);
  CPPUNIT_TEST_SUITE_END();

public:
  TalipotSaveLoadGraphFunctionsTest();

  void setUp() override {}
  void testTalipotSaveLoadGraphFunctions();
  void testSaveGraphNoSuchFile();
};

#endif // IMPORT_EXPORT_TEST_H
