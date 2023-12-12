SET(LIB_SRCS GraphvizUtils.cpp GraphvizLayout.cpp)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${TalipotCoreBuildInclude}
                    ${TalipotCoreInclude} ${Graphviz_INCLUDE_DIRECTORIES})

IF(MSVC)
  ADD_COMPILE_DEFINITIONS(GVDLL)
ENDIF(MSVC)

ADD_LIBRARY(graphviz_utils STATIC ${LIB_SRCS})
TARGET_LINK_LIBRARIES(graphviz_utils ${Graphviz_LIBRARIES}
                      ${LibTalipotCoreName})
SET_TARGET_PROPERTIES(graphviz_utils PROPERTIES POSITION_INDEPENDENT_CODE ON)
