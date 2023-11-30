SET(SRCS
    src/main.cpp
    src/AboutDialog.cpp
    src/AlgorithmRunner.cpp
    src/AlgorithmRunnerItem.cpp
    src/ExpandableGroupBox.cpp
    src/ExportWizard.cpp
    src/GraphHierarchiesEditor.cpp
    src/ImportWizard.cpp
    src/PanelSelectionWizard.cpp
    src/PreferencesDialog.cpp
    src/PluginErrorReport.cpp
    src/PluginInformationListItem.cpp
    src/PluginsCenter.cpp
    src/SearchWidget.cpp
    src/SplashScreen.cpp
    src/TalipotLogger.cpp
    src/TalipotMainWindow.cpp
    src/ThemeUtils.cpp)

SET(ABOUT_COMPILE_DEFINITIONS
    SIP_VERSION="${SIP_VERSION_STR}" CPP_STANDARD="${CMAKE_CXX_STANDARD}"
    CPP_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}"
    CPP_COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}")

SET_SOURCE_FILES_PROPERTIES(
  src/AboutDialog.cpp PROPERTIES COMPILE_DEFINITIONS
                                 "${ABOUT_COMPILE_DEFINITIONS}")

QTX_SET_INCLUDES_AND_DEFINITIONS()
QTXWEB_SET_INCLUDES_AND_DEFINITIONS()

QT_ADD_RESOURCES(RCC_SRCS resources/TalipotApp.qrc OPTIONS -name TalipotApp)

QT_WRAP_CPP(
  MOC_SRCS
  include/AboutDialog.h
  include/AlgorithmRunner.h
  include/AlgorithmRunnerItem.h
  include/ExpandableGroupBox.h
  include/ExportWizard.h
  include/GraphHierarchiesEditor.h
  include/ImportWizard.h
  include/PanelSelectionWizard.h
  include/PluginInformationListItem.h
  include/PluginsCenter.h
  include/PreferencesDialog.h
  include/SearchWidget.h
  include/TalipotLogger.h
  include/TalipotMainWindow.h
  OPTIONS
  -b
  <talipot/PropertyInterface.h>)

QT_WRAP_UI(
  UI_SRCS
  designer/AboutDialog.ui
  designer/AlgorithmRunner.ui
  designer/AlgorithmRunnerItem.ui
  designer/GraphHierarchiesEditor.ui
  designer/ImportWizard.ui
  designer/ExportWizard.ui
  designer/PanelSelectionWizard.ui
  designer/PluginInformationListItem.ui
  designer/PluginsCenter.ui
  designer/PreferencesDialog.ui
  designer/SearchWidget.ui
  designer/TalipotLogger.ui
  designer/TalipotMainWindow.ui)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/utils/crash_handler include/
                    ${CMAKE_CURRENT_BINARY_DIR})

INCLUDE_DIRECTORIES(
  ${TalipotCoreBuildInclude}
  ${TalipotCoreInclude}
  ${TalipotOGLInclude}
  ${TalipotPythonInclude}
  ${TalipotGUIInclude}
  ${OPENGL_INCLUDE_DIR}
  ${OGDFInclude})

TALIPOT_SET_CXX_FLAGS("${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

IF(WIN32)
  STRING(TIMESTAMP CURRENT_YEAR "%Y")

  INCLUDE(generate_product_version)
  GENERATE_PRODUCT_VERSION(
    VersionFiles
    NAME
    "Talipot"
    ICON
    ${PROJECT_SOURCE_DIR}/library/talipot-gui/resources/icons/talipot.ico
    VERSION_MAJOR
    ${TalipotMajorVersion}
    VERSION_MINOR
    ${TalipotMinorVersion}
    VERSION_PATCH
    ${TalipotReleaseVersion}
    COMPANY_NAME
    "The Talipot developers"
    COMPANY_COPYRIGHT
    "(C) Copyright 2019-${CURRENT_YEAR}"
    ORIGINAL_FILENAME
    "talipot.exe")

  SET(SRCS ${SRCS} ${VersionFiles})
ENDIF(WIN32)

ADD_EXECUTABLE(talipot ${SRCS} ${RCC_SRCS} ${MOC_SRCS} ${UI_SRCS})

TARGET_LINK_LIBRARIES(
  talipot
  crash_handler
  ${LibTalipotCoreName}
  ${LibTalipotOGLName}
  ${LibTalipotGUIName}
  ${LibTalipotPythonName}
  ${QT_LIBRARIES})

INSTALL(
  TARGETS talipot
  RUNTIME DESTINATION ${TalipotBinInstallDir}
  LIBRARY DESTINATION ${TalipotLibInstallDir}
  ARCHIVE DESTINATION ${TalipotLibInstallDir})

IF(WIN32)
  SET(TALIPOT_FIXUP_BUNDLE
      ON
      CACHE
        BOOL
        "Enable / disable the gathering and copy of Talipot DLL dependencies into the install bin folder"
  )

  # That simple install command does the magic of gathering all required dll
  # dependencies in order to run Talipot and copy them to the bin install folder
  # (thanks to the CMake module named BundleUtilities) In the case where a dll
  # dependency is not automatically found, you must add its path to the
  # CMAKE_LIBRARY_PATH variable.
  IF(TALIPOT_FIXUP_BUNDLE)
    # copy required Qt plugins
    INSTALL(DIRECTORY ${QT_PLUGINS_DIR}/imageformats
            DESTINATION ${TalipotBinInstallDir})
    INSTALL(DIRECTORY ${QT_PLUGINS_DIR}/platforms
            DESTINATION ${TalipotBinInstallDir})

    IF(EXISTS ${QT_PLUGINS_DIR}/styles)
      INSTALL(DIRECTORY ${QT_PLUGINS_DIR}/styles
              DESTINATION ${TalipotBinInstallDir})
    ENDIF(EXISTS ${QT_PLUGINS_DIR}/styles)

    # then schedule a call to FIXUP_BUNDLE at the end of the CMake install
    # process this process is configured here instead in the root CMakeLists.txt
    # as CMake inserts install commands before the ones defined in sub
    # directories and we need Talipot to be fully installed before running the
    # bundle fixing process
    SET(APP "\${CMAKE_INSTALL_PREFIX}/bin/talipot.exe")
    SET(LIB_DIRS ${QT_BINARY_DIR} ${PYTHON_HOME_PATH} ${CMAKE_LIBRARY_PATH})
    STRING(REPLACE "\\" "/" LIB_DIRS "${LIB_DIRS}")
    STRING(REPLACE ";" "\;" LIB_DIRS "${LIB_DIRS}")
    STRING(REPLACE "\\" "/" PATH_ESCAPED "$ENV{PATH}")
    STRING(REPLACE ";" "\;" PATH_ESCAPED "${PATH_ESCAPED}")
    GET_PROPERTY(FIXUP_BUNDLE_LIBS GLOBAL PROPERTY FIXUP_BUNDLE_LIBS)
    SET(FIXUP_BUNDLE_LIBS
        "${CMAKE_INSTALL_PREFIX}/bin/imageformats/qsvg.dll"
        "${CMAKE_INSTALL_PREFIX}/bin/platforms/qwindows.dll"
        "${FIXUP_BUNDLE_LIBS}")

    IF(MSVC)
      SET(FIXUP_BUNDLE_LIBS
          "${CMAKE_INSTALL_PREFIX}/bin/OGDF-talipot-${TalipotMajorVersion}_${TalipotMinorVersion}.dll"
          "${FIXUP_BUNDLE_LIBS}")
    ENDIF(MSVC)

    STRING(REPLACE "\\" "/" FIXUP_BUNDLE_LIBS "${FIXUP_BUNDLE_LIBS}")
    STRING(REPLACE ";" "\;" FIXUP_BUNDLE_LIBS "${FIXUP_BUNDLE_LIBS}")

    # CMake currently does not know how to locate dumpbin when compiling with
    # Visual Studio >= 2017, so we need to get its path and provide it to the
    # GetPrerequisites module in order for FIXUP_BUNDLE to work properly
    GET_FILENAME_COMPONENT(GP_CMD_PATH "${CMAKE_LINKER}" DIRECTORY)

    INSTALL(
      CODE "INCLUDE(BundleUtilities)
            SET(ENV{PATH} \"${PYTHON_HOME_PATH}\;${PATH_ESCAPED}\")
            SET(gp_cmd_paths \"${GP_CMD_PATH}\")
            FIXUP_BUNDLE(\"${APP}\" \"${FIXUP_BUNDLE_LIBS}\" \"${LIB_DIRS}\")")

    # When generating Talipot installers using MSYS2, we need to bundle OpenSSL
    # libraries in order for the Geographic view to work correctly. As Qt5 does
    # not link directly to OpenSSL for license issue and thus use a dynamic
    # loading approach, we are forced to use the following trick to copy the
    # needed dlls into the Talipot install folder.
    IF(MINGW AND EXISTS ${MINGW_BIN_PATH}/openssl.exe)
      INSTALL(
        CODE "
          INCLUDE(BundleUtilities)
          FILE(COPY \"${MINGW_BIN_PATH}/openssl.exe\"
               DESTINATION \"\${CMAKE_INSTALL_PREFIX}/bin/\")
          FIXUP_BUNDLE(\"\${CMAKE_INSTALL_PREFIX}/bin/openssl.exe\" \"\" \"${LIB_DIRS}\")
          FILE(REMOVE \"\${CMAKE_INSTALL_PREFIX}/bin/openssl.exe\")")
    ENDIF(MINGW AND EXISTS ${MINGW_BIN_PATH}/openssl.exe)
  ENDIF(TALIPOT_FIXUP_BUNDLE)
ENDIF(WIN32)

IF(LINUX)
  SET(TALIPOT_LINUX_DESKTOP_REGISTRATION
      ON
      CACHE
        BOOL
        "Register Talipot application and set file associations on Linux desktops ? [ON|OFF]"
  )

  IF(TALIPOT_LINUX_DESKTOP_REGISTRATION)
    CONFIGURE_FILE(
      resources/linux_desktop/Talipot.desktop.in
      ${CMAKE_CURRENT_BINARY_DIR}/Talipot-${TalipotVersion}.desktop)

    INSTALL(
      CODE "EXECUTE_PROCESS(
        COMMAND mkdir -p ${DESKTOP_CONFIG_INSTALL_PREFIX}/share/mime/packages
        COMMAND mkdir -p ${DESKTOP_CONFIG_INSTALL_PREFIX}/share/applications
        COMMAND mkdir -p ${DESKTOP_CONFIG_INSTALL_PREFIX}/share/pixmaps
        COMMAND mkdir -p ${DESKTOP_CONFIG_INSTALL_PREFIX}/share/icons)")

    INSTALL(
      FILES resources/linux_desktop/application-x-talipot-tlpx.xml
            resources/linux_desktop/application-x-talipot-tlp.xml
            resources/linux_desktop/application-x-talipot-tlpgz.xml
            resources/linux_desktop/application-x-talipot-tlpz.xml
            resources/linux_desktop/application-x-talipot-tlpb.xml
            resources/linux_desktop/application-x-talipot-tlpbgz.xml
            resources/linux_desktop/application-x-talipot-tlpbz.xml
      DESTINATION ${DESKTOP_CONFIG_INSTALL_PREFIX}/share/mime/packages)

    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/Talipot-${TalipotVersion}.desktop
            DESTINATION ${DESKTOP_CONFIG_INSTALL_PREFIX}/share/applications)

    INSTALL(
      FILES resources/icons/talipot-icon.png
      RENAME talipot.png
      DESTINATION ${DESKTOP_CONFIG_INSTALL_PREFIX}/share/pixmaps)

    INSTALL(
      FILES resources/icons/talipot-file-icon.png
      RENAME application-x-talipot.png
      DESTINATION ${DESKTOP_CONFIG_INSTALL_PREFIX}/share/pixmaps)

    INSTALL(
      FILES resources/icons/talipot-icon.png
      RENAME talipot.png
      DESTINATION ${DESKTOP_CONFIG_INSTALL_PREFIX}/share/icons)

    INSTALL(
      FILES resources/icons/talipot-file-icon.png
      RENAME application-x-talipot.png
      DESTINATION ${DESKTOP_CONFIG_INSTALL_PREFIX}/share/icons)

    INSTALL(
      CODE "
        EXECUTE_PROCESS(
          COMMAND update-desktop-database ${DESKTOP_CONFIG_INSTALL_PREFIX}/share/applications
          COMMAND update-mime-database ${DESKTOP_CONFIG_INSTALL_PREFIX}/share/mime
          OUTPUT_QUIET ERROR_QUIET)")
  ENDIF(TALIPOT_LINUX_DESKTOP_REGISTRATION)
ENDIF(LINUX)
