SET(LIB_SRCS CrashHandler.cpp StackWalker.cpp)

IF(NOT MSVC AND NOT BSD)
  IF(LINUX OR (MINGW AND NOT CLANG))
    IF(MINGW)
      # ensure correct bfd header file is selected with MSYS2
      SET(CMAKE_INCLUDE_PATH ${MINGW_INCLUDE_PATH}/binutils
                             ${CMAKE_INCLUDE_PATH})

      # ensure correct binutils libraries are selected with MSYS2
      SET(CMAKE_LIBRARY_PATH ${MINGW_LIB_PATH}/binutils ${CMAKE_LIBRARY_PATH})

      # ensure correct iberty header file is selected with MSYS2
      SET(CMAKE_INCLUDE_PATH ${MINGW_INCLUDE_PATH}/libiberty
                             ${CMAKE_INCLUDE_PATH})
    ENDIF(MINGW)

    FIND_PACKAGE(Bfd)
    FIND_PACKAGE(Iberty)

    IF(BFD_FOUND)
      SET(LIB_SRCS ${LIB_SRCS} BfdWrapper.cpp)
      ADD_COMPILE_DEFINITIONS(HAVE_BFD)

      # workaround for latest versions of GNU binutils
      ADD_COMPILE_DEFINITIONS(PACKAGE PACKAGE_VERSION)
      INCLUDE_DIRECTORIES(BEFORE ${BFD_INCLUDE_DIR})

    ELSE(BFD_FOUND)
      MESSAGE(
        "Install GNU binutils development package to get a detailed stack trace when Talipot crashes."
      )
    ENDIF(BFD_FOUND)
  ENDIF(LINUX OR (MINGW AND NOT CLANG))
ENDIF(NOT MSVC AND NOT BSD)

INCLUDE_DIRECTORIES(BEFORE ${TalipotCoreBuildInclude} ${TalipotCoreInclude})

ADD_LIBRARY(crash_handler STATIC ${LIB_SRCS})

TARGET_LINK_LIBRARIES(crash_handler ${CMAKE_DL_LIBS})

IF(BFD_FOUND)
  TARGET_LINK_LIBRARIES(crash_handler ${BFD_LIBRARY} ${ZLIB_LIBRARY}
                        ${Zstd_LIBRARIES})

  # some build environments requires the need to link against the intl library
  IF(INTL_LIBRARY)
    TARGET_LINK_LIBRARIES(crash_handler ${INTL_LIBRARY})
  ENDIF(INTL_LIBRARY)

  # some build environments requires the need to link against the sframe library
  IF(SFRAME_LIBRARY)
    TARGET_LINK_LIBRARIES(crash_handler ${SFRAME_LIBRARY})
  ENDIF(SFRAME_LIBRARY)
ENDIF(BFD_FOUND)

IF(IBERTY_LIBRARY)
  TARGET_LINK_LIBRARIES(crash_handler ${IBERTY_LIBRARY})
ENDIF(IBERTY_LIBRARY)

IF(WIN32 AND NOT MSVC)
  TARGET_LINK_LIBRARIES(crash_handler imagehlp)
ELSEIF(MSVC)
  TARGET_LINK_LIBRARIES(crash_handler dbghelp)
ENDIF(WIN32 AND NOT MSVC)
