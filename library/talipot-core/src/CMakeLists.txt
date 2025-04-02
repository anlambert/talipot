SET(talipot_LIB_SRCS
    AcyclicTest.cpp
    BiconnectedTest.cpp
    BooleanProperty.cpp
    BoundingBox.cpp
    Color.cpp
    ColorProperty.cpp
    ColorScale.cpp
    ConnectedTest.cpp
    ConnectedTestListener.cpp
    ConvexHull.cpp
    DataSet.cpp
    Delaunay.cpp
    Dijkstra.cpp
    DoubleProperty.cpp
    DrawingTools.cpp
    FaceIterator.cpp
    FontAwesome.cpp
    GraphAbstract.cpp
    Graph.cpp
    GraphDecorator.cpp
    GraphImpl.cpp
    GraphMeasure.cpp
    GraphProperty.cpp
    GraphStorage.cpp
    GraphTools.cpp
    GraphUpdatesRecorder.cpp
    GraphView.cpp
    IconicFont.cpp
    IdManager.cpp
    ImportModule.cpp
    IntegerProperty.cpp
    LayoutProperty.cpp
    MapIterator.cpp
    MaterialDesignIcons.cpp
    NumericProperty.cpp
    Observable.cpp
    Ordering.cpp
    OuterPlanarTest.cpp
    ParametricCurves.cpp
    ParallelTools.cpp
    PlanarConMap.cpp
    PlanarityTest.cpp
    PlanarityTestEmbed.cpp
    PlanarityTestImpl.cpp
    PlanarityTestObstr.cpp
    PlanarityTestTools.cpp
    Plane.cpp
    Plugin.cpp
    PluginLibraryLoader.cpp
    PluginLoaderTxt.cpp
    PropertyAlgorithm.cpp
    PropertyInterface.cpp
    PropertyManager.cpp
    PropertyTypes.cpp
    SimplePluginProgress.cpp
    SimpleTest.cpp
    SizeProperty.cpp
    StlFunctions.cpp
    StringCollection.cpp
    StringProperty.cpp
    TLPExport.cpp
    TLPImport.cpp
    TlpTools.cpp
    TreeTest.cpp
    TriconnectedTest.cpp
    WithParameter.cpp
    YajlFacade.cpp
    PluginsManager.cpp
    talipotconf.cpp
    Exception.cpp
    PluginProgress.cpp
    ViewSettings.cpp
    TlpJsonExport.cpp
    TlpJsonImport.cpp
    TLPBExport.cpp
    TLPBImport.cpp
    Vector.cpp
    Matrix.cpp
    MutableContainer.cpp
    PropertyProxy.cpp)

# disable GCC var tracking on these file as it eats too much compile time and
# warnings are emitted otherwise
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(FontAwesome.cpp MaterialDesignIcons.cpp
                              COMPILE_FLAGS -fno-var-tracking)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

IF(HAVE_REENTRANT_QHULL)
  ADD_DEFINITIONS("-DHAVE_REENTRANT_QHULL")
ENDIF(HAVE_REENTRANT_QHULL)

INCLUDE_DIRECTORIES(
  ${TalipotCoreBuildInclude}
  ${TalipotCoreInclude}
  ${PROJECT_SOURCE_DIR}
  ${ZLIB_INCLUDE_DIR}
  ${YAJL_INCLUDE_DIRS}
  ${QHULL_INCLUDE_DIRS}
  ${GZStreamInclude}
  ${Zstd_INCLUDE_DIRS}
  ${ZstdStreamInclude})

ADD_CUSTOM_COMMAND(
  OUTPUT
    ${CMAKE_CURRENT_SOURCE_DIR}/FontAwesomeData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MaterialDesignIconsData.cpp
    ${CMAKE_SOURCE_DIR}/library/talipot-core/include/talipot/FontAwesomeConstants.h
    ${CMAKE_SOURCE_DIR}/library/talipot-core/include/talipot/MaterialDesignIconsConstants.h
    ${CMAKE_SOURCE_DIR}/library/talipot-python/bindings/talipot-core/FontAwesome.sip
    ${CMAKE_SOURCE_DIR}/library/talipot-python/bindings/talipot-core/MaterialDesignIcons.sip
  COMMAND
    ${PYTHON_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/utils/scripts/fonticons/generate_font_icons_data.py
    ${CMAKE_SOURCE_DIR}
  DEPENDS
    ${CMAKE_SOURCE_DIR}/utils/scripts/fonticons/generate_font_icons_data.py
    ${CMAKE_SOURCE_DIR}/utils/scripts/fonticons/fa-icons.json
    ${CMAKE_SOURCE_DIR}/utils/scripts/fonticons/mdi-icons.json
    ${CMAKE_SOURCE_DIR}/header.txt
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET_SOURCE_FILES_PROPERTIES(
  FontAwesome.cpp MaterialDesignIcons.cpp
  PROPERTIES
    OBJECT_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/FontAwesomeData.cpp;${CMAKE_CURRENT_SOURCE_DIR}/MaterialDesignIconsData.cpp"
)

ADD_LIBRARY(${LibTalipotCoreName} SHARED ${talipot_LIB_SRCS})

TARGET_LINK_LIBRARIES(
  ${LibTalipotCoreName}
  ${GZStreamLibrary}
  ${YAJL_LIBRARIES}
  ${QHULL_LIBRARIES}
  ${Zstd_LIBRARIES}
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT})

IF(WIN32)
  IF(MSVC)
    TARGET_LINK_LIBRARIES(${LibTalipotCoreName} Dbghelp)
    IF(${CMAKE_GENERATOR} MATCHES "Visual Studio 9"
       OR ${CMAKE_GENERATOR} MATCHES "NMake Makefiles")
      SET_TARGET_PROPERTIES(${LibTalipotCoreName}
                            PROPERTIES LINK_FLAGS " /NODEFAULTLIB:LIBCMT.lib ")
    ENDIF()
  ENDIF()
ENDIF(WIN32)

INSTALL(
  TARGETS ${LibTalipotCoreName}
  RUNTIME DESTINATION ${TalipotBinInstallDir}
  LIBRARY DESTINATION ${TalipotLibInstallDir}
  ARCHIVE DESTINATION ${TalipotLibInstallDir})
