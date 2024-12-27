IF(WIN32)
  TALIPOT_SET_CXX_FLAGS("-DDLL_TALIPOT_PYTHON")
ENDIF(WIN32)

IF(CMAKE_COMPILER_IS_GNUCXX OR CLANG)
  # silent warning about deprecated but required PyEval_ReleaseLock call
  TALIPOT_SET_CXX_FLAGS("-Wno-deprecated-declarations")
ENDIF(CMAKE_COMPILER_IS_GNUCXX OR CLANG)

INCLUDE_DIRECTORIES(BEFORE ../bindings/talipot-core/build)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIR} ${PYTHON_INCLUDE_PATH}
                    ${OPENGL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(BEFORE ${TalipotCoreBuildInclude})
INCLUDE_DIRECTORIES(
  ${TalipotCoreInclude} ${TalipotOGLInclude} ${TalipotGUIInclude}
  ${TalipotPythonInclude} ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR})

QTX_SET_INCLUDES_AND_DEFINITIONS()

SET(${LibTalipotPythonName}_LIB_SRCS
    ConsoleUtilsModule.cpp
    PythonCodeEditor.cpp
    PythonCodeHighlighter.cpp
    ParenMatcherHighlighter.cpp
    PythonInterpreter.cpp
    PythonShellWidget.cpp
    TalipotUtilsModule.cpp
    APIDataBase.cpp
    AutoCompletionDataBase.cpp
    ConsoleHandlers.h
    PythonEditorsTabWidget.cpp
    PythonCppTypesConverter.cpp
    PythonIDE.cpp
    PythonPluginCreationDialog.cpp
    PythonREPLWidget.cpp
    PythonVersionChecker.cpp)

SET(TLP_PYTHON_COMPILE_DEFINITIONS TLP_PYTHON="${PYTHON_VERSION}")

IF(MSYS2_PYTHON)
  SET(TLP_PYTHON_COMPILE_DEFINITIONS
      ${TLP_PYTHON_COMPILE_DEFINITIONS} MSYS2_PYTHON=1
      PYTHON_HOME_PATH="${PYTHON_HOME_PATH}/../")
ENDIF(MSYS2_PYTHON)

SET_SOURCE_FILES_PROPERTIES(
  PythonVersionChecker.cpp PROPERTIES COMPILE_DEFINITIONS
                                      "${TLP_PYTHON_COMPILE_DEFINITIONS}")

SET(${LibTalipotPythonName}_UI
    ../designer/FindReplaceDialog.ui ../designer/PythonPluginCreationDialog.ui
    ../designer/PythonIDE.ui ../designer/PythonREPLWidget.ui)

SET(${LibTalipotPythonName}_MOC
    ConsoleHandlers.h
    ../include/talipot/PythonShellWidget.h
    ../include/talipot/PythonCodeEditor.h
    ../include/talipot/PythonEditorsTabWidget.h
    ../include/talipot/PythonInterpreter.h
    ../include/talipot/PythonIDE.h
    ../include/talipot/PythonPluginCreationDialog.h
    ../include/talipot/PythonREPLWidget.h)

QT_WRAP_UI(${LibTalipotPythonName}_UI_SRCS ${${LibTalipotPythonName}_UI})
QT_WRAP_CPP(${LibTalipotPythonName}_MOC_SRCS ${${LibTalipotPythonName}_MOC})

ADD_LIBRARY(
  ${LibTalipotPythonName} SHARED
  ${${LibTalipotPythonName}_LIB_SRCS} ${${LibTalipotPythonName}_MOC_SRCS}
  ${${LibTalipotPythonName}_UI_SRCS})

ADD_DEPENDENCIES(${LibTalipotPythonName} ${TalipotPythonModuleName})

TARGET_LINK_LIBRARIES(${LibTalipotPythonName} ${LibTalipotCoreName})
TARGET_LINK_LIBRARIES(${LibTalipotPythonName} ${LibTalipotGUIName})
TARGET_LINK_LIBRARIES(${LibTalipotPythonName} ${QT_LIBRARIES})
TARGET_LINK_LIBRARIES(${LibTalipotPythonName} ${PYTHON_LIBRARY})

INSTALL(
  TARGETS ${LibTalipotPythonName}
  RUNTIME DESTINATION ${TalipotBinInstallDir}
  LIBRARY DESTINATION ${TalipotLibInstallDir}
  ARCHIVE DESTINATION ${TalipotLibInstallDir})
