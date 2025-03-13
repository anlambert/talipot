INCLUDE_DIRECTORIES(${TalipotCoreBuildInclude} ${TalipotCoreInclude}
                    ${TalipotOGDFInclude} ${OGDFInclude})

IF(WIN32)
  TALIPOT_SET_CXX_FLAGS("-DDLL_TALIPOT_OGDF")
ENDIF(WIN32)

DISABLE_COMPILER_WARNINGS()

SET(TalipotOGDF_SRCS src/TalipotToOGDF.cpp src/OGDFLayoutPluginBase.cpp
                     src/OGDFUtils.cpp)

ADD_LIBRARY(${LibTalipotOGDFName} SHARED ${TalipotOGDF_SRCS})

TARGET_LINK_LIBRARIES(${LibTalipotOGDFName} ${LibTalipotCoreName}
                      ${OGDFLibrary})

INSTALL(
  TARGETS ${LibTalipotOGDFName}
  RUNTIME DESTINATION ${TalipotBinInstallDir}
  LIBRARY DESTINATION ${TalipotLibInstallDir}
  ARCHIVE DESTINATION ${TalipotLibInstallDir})

INSTALL(FILES include/talipot/OGDFLayoutPluginBase.h
              include/talipot/TalipotToOGDF.h include/talipot/OGDFUtils.h
        DESTINATION ${TalipotIncludeInstallDir}/talipot/)
