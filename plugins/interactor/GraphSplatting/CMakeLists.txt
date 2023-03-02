INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TalipotCoreBuildInclude}
  ${TalipotCoreInclude}
  ${TalipotOGLInclude}
  ${TalipotGUIInclude}
  ${OPENGL_INCLUDE_DIR})

SET(LIB_SRCS GraphSplattingInteractor.cpp
             GraphSplattingInteractorConfigWidget.cpp ColorScaleWidget.cpp)

QT_WRAP_UI(LIB_UI_HEADERS GraphSplattingInteractorConfigWidget.ui)

QT_WRAP_CPP(LIB_MOC_SRCS GraphSplattingInteractorConfigWidget.h
            GraphSplattingInteractor.h)

TALIPOT_ADD_PLUGIN(
  NAME
  GraphSplatting
  SRCS
  ${LIB_SRCS}
  ${LIB_UI_HEADERS}
  ${LIB_MOC_SRCS}
  LINKS
  ${LibTalipotCoreName}
  ${LibTalipotOGLName}
  ${LibTalipotGUIName}
  ${OPENGL_gl_LIBRARY}
  INSTALL_DIR
  ${TalipotInteractorPluginsInstallDir})
