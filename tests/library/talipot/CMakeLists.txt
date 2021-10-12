ADD_LIBRARY(testPlugin SHARED TestPlugin.cpp)
SET_TARGET_PROPERTIES(testPlugin PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(testPlugin ${LibTalipotCoreName})

ADD_LIBRARY(testPlugin2 SHARED TestPlugin2.cpp)
SET_TARGET_PROPERTIES(testPlugin2 PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(testPlugin2 ${LibTalipotCoreName})

ADD_CUSTOM_TARGET(
  copyTestData ALL
  ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/DATA
  ${CMAKE_CURRENT_BINARY_DIR}/DATA
  VERBATIM)

# disable MSVC warning 'potential divide by 0' for DoublePropertyTest
IF(MSVC)
  TALIPOT_SET_CXX_FLAGS("/wd4723")
ENDIF(MSVC)

UNIT_TEST(ImportExportTest ImportExportTest.cpp talipotlibtest.cpp)
UNIT_TEST(ExistEdgeTest ExistEdgeTest.cpp talipotlibtest.cpp)
UNIT_TEST(ExtendedClusterOperationTest ExtendedClusterOperationTest.cpp
          talipotlibtest.cpp)
UNIT_TEST(VectorTest VectorTest.cpp talipotlibtest.cpp)
UNIT_TEST(MatrixTest MatrixTest.cpp talipotlibtest.cpp)
UNIT_TEST(TestAlgorithmTest TestAlgorithmTest.cpp talipotlibtest.cpp)
UNIT_TEST(BooleanPropertyTest BooleanPropertyTest.cpp talipotlibtest.cpp)
UNIT_TEST(GraphPropertyTest GraphPropertyTest.cpp talipotlibtest.cpp)
UNIT_TEST(SuperGraphTest SuperGraphTest.cpp talipotlibtest.cpp)
UNIT_TEST(TlpImportExportTest TlpImportExportTest.cpp talipotlibtest.cpp)
UNIT_TEST(IdManagerTest IdManagerTest.cpp talipotlibtest.cpp)
UNIT_TEST(DataSetTest DataSetTest.cpp talipotlibtest.cpp)
UNIT_TEST(ObservablePropertyTest ObservablePropertyTest.cpp talipotlibtest.cpp)
UNIT_TEST(MutableContainerTest MutableContainerTest.cpp talipotlibtest.cpp)
UNIT_TEST(ObservableGraphTest ObservableGraphTest.cpp talipotlibtest.cpp)
UNIT_TEST(PushPopTest PushPopTest.cpp talipotlibtest.cpp)
UNIT_TEST(IntegerPropertyMinMaxSubgraphTest
          IntegerPropertyMinMaxSubgraphTest.cpp talipotlibtest.cpp)
UNIT_TEST(IntegerPropertyMinMaxUpdateTest IntegerPropertyMinMaxUpdateTest.cpp
          talipotlibtest.cpp)
UNIT_TEST(PropertiesMinMaxAfterAddNodeTest PropertiesMinMaxAfterAddNodeTest.cpp
          talipotlibtest.cpp)
UNIT_TEST(IsMetaEdgeTest IsMetaEdgeTest.cpp talipotlibtest.cpp)
UNIT_TEST(StringPropertyTest StringPropertyTest.cpp talipotlibtest.cpp)
UNIT_TEST(DoublePropertyTest DoublePropertyTest.cpp talipotlibtest.cpp)
UNIT_TEST(PlanarityTestTest PlanarityTestTest.cpp talipotlibtest.cpp)
UNIT_TEST(WithParameterTest WithParameterTest.cpp talipotlibtest.cpp)
UNIT_TEST(FaceIteratorTest FaceIteratorTest.cpp talipotlibtest.cpp)
UNIT_TEST(PluginsTest PluginsTest.cpp talipotlibtest.cpp)
UNIT_TEST(IteratorTest IteratorTest.cpp talipotlibtest.cpp)
UNIT_TEST(ParallelToolsTest ParallelToolsTest.cpp talipotlibtest.cpp)
UNIT_TEST(TlpToolsTest TlpToolsTest.cpp talipotlibtest.cpp)
UNIT_TEST(GraphTraversalTest GraphTraversalTest.cpp talipotlibtest.cpp)
UNIT_TEST(PropertyUtilsTest PropertyUtilsTest.cpp talipotlibtest.cpp)

SET_TESTS_PROPERTIES(PluginsTest PROPERTIES DEPENDS copyTestData)
