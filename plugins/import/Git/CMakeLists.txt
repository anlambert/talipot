INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${TalipotCoreBuildInclude}
                    ${TalipotCoreInclude} ${LIBGIT2_INCLUDE_DIR})

TALIPOT_ADD_PLUGIN(
  NAME
  GitImport
  SRCS
  GitImport.cpp
  LINKS
  ${LibTalipotCoreName}
  ${LIBGIT2_LIBRARIES}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})

INSTALL(FILES git.png DESTINATION ${TalipotBitmapInstallDir})
