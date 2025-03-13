TALIPOT_INSTALL_PYTHON_FILES(
  talipot/plugins ${CMAKE_CURRENT_SOURCE_DIR}/import
  ${CMAKE_CURRENT_SOURCE_DIR}/color ${CMAKE_CURRENT_SOURCE_DIR}/layout)

TALIPOT_INSTALL_PYTHON_FILES(
  talipot/plugins/general
  ${CMAKE_CURRENT_SOURCE_DIR}/general/SquarifiedTreeMapHelper.py)
TALIPOT_INSTALL_PYTHON_FILES(
  talipot/plugins/general ${CMAKE_CURRENT_SOURCE_DIR}/general/H3LayoutHelper.py)
