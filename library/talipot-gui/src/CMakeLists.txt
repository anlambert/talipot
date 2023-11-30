SET(SRCS
    InteractorConfigWidget.cpp
    ColorScaleConfigDialog.cpp
    CopyPropertyDialog.cpp
    CSVGraphImport.cpp
    CSVGraphMappingConfigurationWidget.cpp
    CSVImportConfigurationWidget.cpp
    CSVImportWizard.cpp
    CSVParserConfigurationWidget.cpp
    CSVParser.cpp
    DoubleStringsListSelectionWidget.cpp
    DownloadManager.cpp
    DragHandle.cpp
    GlCompositeHierarchyManager.cpp
    GlView.cpp
    GlWidget.cpp
    GlOffscreenRenderer.cpp
    GraphPropertiesSelectionWidget.cpp
    HeaderFrame.cpp
    ItemsListWidget.cpp
    MouseBoxZoomer.cpp
    MouseEdgeBendEditor.cpp
    MouseEdgeBuilder.cpp
    MouseInteractors.cpp
    MouseNodeBuilder.cpp
    MouseSelectionEditor.cpp
    MouseSelector.cpp
    MouseShowElementInfo.cpp
    NodeLinkDiagramView.cpp
    NodeLinkDiagramViewInteractor.cpp
    ShapeDialog.cpp
    PropertyCreationDialog.cpp
    QtGlSceneZoomAndPanAnimator.cpp
    QuaZIPFacade.cpp
    RenamePropertyDialog.cpp
    SimplePluginProgressWidget.cpp
    SimpleStringsListSelectionWidget.cpp
    RangeSlider.cpp
    StringsListSelectionWidget.cpp
    StringsListSelectionDialog.cpp
    TextureFileDialog.cpp
    TlpQtTools.cpp
    Project.cpp
    Settings.cpp
    ParameterListModel.cpp
    ItemDelegate.cpp
    MetaTypes.cpp
    ItemEditorCreators.cpp
    ColorButton.cpp
    Model.cpp
    ColorScaleButton.cpp
    View.cpp
    GlWidgetGraphicsItem.cpp
    ViewWidget.cpp
    InteractorComposite.cpp
    GLInteractor.cpp
    WorkspacePanel.cpp
    Interactor.cpp
    Workspace.cpp
    PlaceHolderWidget.cpp
    GraphHierarchiesModel.cpp
    Vec3fEditor.cpp
    GlOverviewGraphicsItem.cpp
    SceneConfigWidget.cpp
    QuickAccessBar.cpp
    FontDialog.cpp
    WorkspaceExposeWidget.cpp
    GraphTableItemDelegate.cpp
    StringEditor.cpp
    VectorEditor.cpp
    GlyphRenderer.cpp
    ClearableLineEdit.cpp
    CaptionItem.cpp
    CaptionGraphicsItem.cpp
    CaptionGraphicsSubItems.cpp
    GraphModel.cpp
    GraphElementModel.cpp
    SnapshotDialog.cpp
    Mimes.cpp
    SceneLayersModel.cpp
    SceneLayersConfigWidget.cpp
    TreeViewComboBox.cpp
    GlEntityItemModel.cpp
    PluginModel.cpp
    GraphNeedsSavingObserver.cpp
    FileDownloader.cpp
    ColorScalesManager.cpp
    ScientificDoubleSpinBox.cpp
    FontIconDialog.cpp
    FontIcon.cpp
    ViewActionsManager.cpp
    ViewToolTipAndUrlManager.cpp)

QTX_SET_INCLUDES_AND_DEFINITIONS()

ADD_DEFINITIONS(-DQT_DLL -DQT_THREAD_SUPPORT)

SET(PYTHON_PART_SRCS PythonVersionChecker.cpp)
SET(TLPGUI_PYTHON_COMPILE_DEFINITIONS TLP_PYTHON="${PYTHON_VERSION}")

IF(MSYS2_PYTHON)
  SET(TLPGUI_PYTHON_COMPILE_DEFINITIONS
      ${TLPGUI_PYTHON_COMPILE_DEFINITIONS} MSYS2_PYTHON=1
      PYTHON_HOME_PATH="${PYTHON_HOME_PATH}/../")
ENDIF(MSYS2_PYTHON)

SET_SOURCE_FILES_PROPERTIES(
  PythonVersionChecker.cpp PROPERTIES COMPILE_DEFINITIONS
                                      "${TLPGUI_PYTHON_COMPILE_DEFINITIONS}")

ADD_LIBRARY(talipot-gui-python STATIC ${PYTHON_PART_SRCS})

TARGET_LINK_LIBRARIES(talipot-gui-python ${QT_LIBRARIES})
SET_TARGET_PROPERTIES(talipot-gui-python PROPERTIES POSITION_INDEPENDENT_CODE
                                                    ON)
QT_WRAP_UI(
  UI_HEADERS
  ../designer/ColorScaleConfigDialog.ui
  ../designer/Vec3fEditor.ui
  ../designer/CopyPropertyDialog.ui
  ../designer/CSVGraphMappingConfigurationWidget.ui
  ../designer/CSVImportConfigurationWidget.ui
  ../designer/CSVImportWizard.ui
  ../designer/CSVParserConfigurationWidget.ui
  ../designer/CSVPropertyDialog.ui
  ../designer/DoubleStringsListSelectionWidget.ui
  ../designer/ElementInformationWidget.ui
  ../designer/GridOptionsWidget.ui
  ../designer/ShapeDialog.ui
  ../designer/PropertyCreationDialog.ui
  ../designer/QuickAccessBar.ui
  ../designer/SceneConfigWidget.ui
  ../designer/SceneLayersConfigWidget.ui
  ../designer/SimplePluginProgressWidget.ui
  ../designer/SimpleStringsListSelectionWidget.ui
  ../designer/SnapshotDialog.ui
  ../designer/StringsListSelectionDialog.ui
  ../designer/TextureFileDialog.ui
  ../designer/FontDialog.ui
  ../designer/FontIconDialog.ui
  ../designer/VectorEditor.ui
  ../designer/Workspace.ui
  ../designer/WorkspacePanel.ui
  ../designer/InteractorConfigWidget.ui
  ../designer/HeaderFrame.ui)

QT_WRAP_CPP(
  MOC_SRCS
  ../include/talipot/CaptionGraphicsItem.h
  ../include/talipot/CaptionGraphicsSubItems.h
  ../include/talipot/CaptionItem.h
  ../include/talipot/ColorButton.h
  ../include/talipot/ColorScaleButton.h
  ../include/talipot/ColorScaleConfigDialog.h
  ../include/talipot/Vec3fEditor.h
  ../include/talipot/CopyPropertyDialog.h
  ../include/talipot/CSVGraphMappingConfigurationWidget.h
  ../include/talipot/CSVImportConfigurationWidget.h
  ../include/talipot/CSVImportWizard.h
  ../include/talipot/CSVParserConfigurationWidget.h
  ../include/talipot/DoubleStringsListSelectionWidget.h
  ../include/talipot/DownloadManager.h
  ../include/talipot/DragHandle.h
  ../include/talipot/FileDownloader.h
  ../include/talipot/GLInteractor.h
  ../include/talipot/GlView.h
  ../include/talipot/GlWidget.h
  ../include/talipot/GlWidgetGraphicsItem.h
  ../include/talipot/GlOverviewGraphicsItem.h
  ../include/talipot/GlEntityItemModel.h
  ../include/talipot/GraphHierarchiesModel.h
  ../include/talipot/GraphNeedsSavingObserver.h
  ../include/talipot/GraphTableItemDelegate.h
  ../include/talipot/HeaderFrame.h
  ../include/talipot/Interactor.h
  ../include/talipot/InteractorComposite.h
  ../include/talipot/ItemsListWidget.h
  ../include/talipot/NodeLinkDiagramView.h
  ../include/talipot/PlaceHolderWidget.h
  ../include/talipot/PropertyCreationDialog.h
  ../include/talipot/QtGlSceneZoomAndPanAnimator.h
  ../include/talipot/QuickAccessBar.h
  ../include/talipot/SceneConfigWidget.h
  ../include/talipot/SceneLayersModel.h
  ../include/talipot/SceneLayersConfigWidget.h
  ../include/talipot/SimplePluginProgressWidget.h
  ../include/talipot/SimpleStringsListSelectionWidget.h
  ../include/talipot/SnapshotDialog.h
  ../include/talipot/RangeSlider.h
  ../include/talipot/MouseShowElementInfo.h
  ../include/talipot/StringsListSelectionDialog.h
  ../include/talipot/TextureFileDialog.h
  ../include/talipot/TreeViewComboBox.h
  ../include/talipot/FontDialog.h
  ../include/talipot/ItemDelegate.h
  ../include/talipot/Mimes.h
  ../include/talipot/Model.h
  ../include/talipot/Project.h
  ../include/talipot/Settings.h
  ../include/talipot/VectorEditor.h
  ../include/talipot/View.h
  ../include/talipot/ViewWidget.h
  ../include/talipot/WorkspacePanel.h
  ../include/talipot/Workspace.h
  ../include/talipot/WorkspaceExposeWidget.h
  ../include/talipot/FontIconDialog.h
  ../include/talipot/ShapeDialog.h
  ../include/talipot/ViewActionsManager.h
  ../include/talipot/ViewToolTipAndUrlManager.h
  OPTIONS
  -b
  <talipot/Graph.h>
  -b
  <talipot/Interactor.h>
  -b
  <talipot/View.h>
  -b
  <talipot/WorkspacePanel.h>)

QT_ADD_RESOURCES(RCC_SRCS ../resources/TalipotGUIResource.qrc)

INCLUDE_DIRECTORIES(
  ${TalipotGUIBuildInclude}
  ${TalipotCoreBuildInclude}
  ${TalipotCoreInclude}
  ${TalipotOGLInclude}
  ${TalipotGUIInclude}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${ZLIB_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${QUAZIP_INCLUDE_DIRS})

ADD_LIBRARY(${LibTalipotGUIName} SHARED ${SRCS} ${MOC_SRCS} ${RCC_SRCS}
                                        ${UI_HEADERS})

TARGET_LINK_LIBRARIES(
  ${LibTalipotGUIName} ${QT_LIBRARIES} ${LibTalipotCoreName}
  ${LibTalipotOGLName} ${QUAZIP_LIBRARIES})

TARGET_LINK_LIBRARIES(${LibTalipotGUIName} talipot-gui-python)

INSTALL(
  TARGETS ${LibTalipotGUIName}
  RUNTIME DESTINATION ${TalipotBinInstallDir}
  LIBRARY DESTINATION ${TalipotLibInstallDir}
  ARCHIVE DESTINATION ${TalipotLibInstallDir})

INSTALL(FILES ../resources/icons/talipot.ico
        DESTINATION ${TalipotBitmapInstallDir})

INSTALL(FILES ../resources/icons/talipot-file-icon.ico
        DESTINATION ${TalipotBitmapInstallDir})
