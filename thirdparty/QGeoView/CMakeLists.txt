set(CMAKE_AUTOMOC ON)

add_library(${QGeoViewLibrary} SHARED
    include/QGeoView/QGVGlobal.h
    include/QGeoView/QGVUtils.h
    include/QGeoView/QGVProjection.h
    include/QGeoView/QGVProjectionEPSG3857.h
    include/QGeoView/QGVCamera.h
    include/QGeoView/QGVMap.h
    include/QGeoView/QGVMapQGItem.h
    include/QGeoView/QGVMapQGView.h
    include/QGeoView/QGVMapRubberBand.h
    include/QGeoView/QGVItem.h
    include/QGeoView/QGVDrawItem.h
    include/QGeoView/QGVLayer.h
    include/QGeoView/QGVLayerTiles.h
    include/QGeoView/QGVLayerTilesOnline.h
    include/QGeoView/QGVLayerGoogle.h
    include/QGeoView/QGVLayerBing.h
    include/QGeoView/QGVLayerOSM.h
    include/QGeoView/QGVLayerBDGEx.h
    include/QGeoView/QGVWidget.h
    include/QGeoView/QGVWidgetCompass.h
    include/QGeoView/QGVWidgetScale.h
    include/QGeoView/QGVWidgetZoom.h
    include/QGeoView/QGVWidgetText.h
    include/QGeoView/Raster/QGVImage.h
    include/QGeoView/Raster/QGVIcon.h
    src/QGVUtils.cpp
    src/QGVGlobal.cpp
    src/QGVProjection.cpp
    src/QGVProjectionEPSG3857.cpp
    src/QGVCamera.cpp
    src/QGVMap.cpp
    src/QGVMapQGItem.cpp
    src/QGVMapQGView.cpp
    src/QGVMapRubberBand.cpp
    src/QGVItem.cpp
    src/QGVDrawItem.cpp
    src/QGVLayer.cpp
    src/QGVLayerTiles.cpp
    src/QGVLayerTilesOnline.cpp
    src/QGVLayerGoogle.cpp
    src/QGVLayerBing.cpp
    src/QGVLayerOSM.cpp
    src/QGVLayerBDGEx.cpp
    src/QGVWidget.cpp
    src/QGVWidgetCompass.cpp
    src/QGVWidgetScale.cpp
    src/QGVWidgetZoom.cpp
    src/QGVWidgetText.cpp
    src/Raster/QGVImage.cpp
    src/Raster/QGVIcon.cpp
)

target_include_directories(${QGeoViewLibrary}
    PUBLIC
    include
    PRIVATE
    include/QGeoView
)

target_compile_definitions(${QGeoViewLibrary}
    PRIVATE
    QGV_EXPORT
)

target_link_libraries(${QGeoViewLibrary}
    PRIVATE
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::Network
)

install(
    TARGETS ${QGeoViewLibrary}
    RUNTIME DESTINATION ${TalipotBinInstallDir}
    LIBRARY DESTINATION ${TalipotLibInstallDir}
    ARCHIVE DESTINATION ${TalipotLibInstallDir})
