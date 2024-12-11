INCLUDE_DIRECTORIES(
  ${TalipotCoreBuildInclude}
  ${TalipotCoreInclude}
  ${TalipotOGLInclude}
  ${TalipotGUIInclude}
  ${TalipotUIGUIInclude}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${QGeoViewInclude})

QTX_SET_INCLUDES_AND_DEFINITIONS()

SET(LIB_SRCS
    GeographicView.cpp
    GeographicViewConfigWidget.cpp
    GeolocationConfigWidget.cpp
    ProgressWidgetGraphicsProxy.cpp
    GeographicViewInteractors.cpp
    AddressSelectionDialog.cpp
    GeographicViewGraphicsView.cpp
    GeographicViewShowElementInfo.cpp
    NominatimGeocoder.cpp
    QGVLayerEsri.cpp
    QGVLayerGeoPortail.cpp
    QGVCustomTilesLayer.cpp)

QT_WRAP_UI(
  LIB_UI_HEADERS GeolocationConfigWidget.ui GeographicViewConfigWidget.ui
  ProgressWidget.ui AddressSelectionDialog.ui)

QT_ADD_RESOURCES(LIB_RCC_SRCS GeographicViewResource.qrc)

QT_WRAP_CPP(
  LIB_MOC_SRCS
  GeographicView.h
  GeographicViewGraphicsView.h
  ProgressWidgetGraphicsProxy.h
  GeographicViewConfigWidget.h
  GeographicViewShowElementInfo.h
  GeolocationConfigWidget.h)

TALIPOT_ADD_PLUGIN(
  NAME
  GeographicView
  SRCS
  ${LIB_SRCS}
  ${LIB_UI_HEADERS}
  ${LIB_MOC_SRCS}
  ${LIB_RCC_SRCS}
  LINKS
  ${LibTalipotCoreName}
  ${LibTalipotOGLName}
  ${LibTalipotGUIName}
  ${OPENGL_gl_LIBRARY}
  ${QT_LIBRARIES}
  ${QGeoViewLibrary}
  INSTALL_DIR
  ${TalipotViewPluginsInstallDir}
  FIXUP_INSTALL)
