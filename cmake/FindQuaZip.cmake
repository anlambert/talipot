# QUAZIP_FOUND - QuaZip library was found
#
# QUAZIP_INCLUDE_DIR - Path to QuaZip include dir
#
# QUAZIP_INCLUDE_DIRS - Path to QuaZip and zlib include dir (combined from
# QUAZIP_INCLUDE_DIR + ZLIB_INCLUDE_DIR)
#
# QUAZIP_LIBRARIES - List of QuaZip libraries
#
# QUAZIP_ZLIB_INCLUDE_DIR - The include dir of zlib headers

# Unset related CMake variables in order to change the lib version without
# having to delete the current CMake cache
UNSET(QUAZIP_FOUND CACHE)
UNSET(QUAZIP_LIBRARIES CACHE)
UNSET(QUAZIP_INCLUDE_DIR CACHE)
UNSET(QUAZIP_INCLUDE_DIRS CACHE)
UNSET(QUAZIP_ZLIB_INCLUDE_DIR CACHE)

IF(WIN32)
  FIND_LIBRARY(
    QUAZIP_LIBRARIES
    NAMES libquazip${QT_MAJOR_VERSION} quazip${QT_MAJOR_VERSION}
          quazip1-qt${QT_MAJOR_VERSION})
  FIND_PATH(
    QUAZIP_INCLUDE_DIR
    NAMES quazip.h
    PATH_SUFFIXES
      include/quazip${QT_MAJOR_VERSION}
      quazip${QT_MAJOR_VERSION}
      QuaZip-Qt${QT_MAJOR_VERSION}-1.0/quazip
      QuaZip-Qt${QT_MAJOR_VERSION}-1.1/quazip
      QuaZip-Qt${QT_MAJOR_VERSION}-1.2/quazip
      QuaZip-Qt${QT_MAJOR_VERSION}-1.3/quazip
      QuaZip-Qt${QT_MAJOR_VERSION}-1.4/quazip)
  FIND_PATH(QUAZIP_ZLIB_INCLUDE_DIR NAMES zlib.h)
ELSE(WIN32)

  # special case when using Qt on unix
  SET(QUAZIP_LIBRARY_NAMES
      quazip${QT_MAJOR_VERSION} quazip-qt${QT_MAJOR_VERSION}
      quazip1-qt${QT_MAJOR_VERSION})
  IF(APPLE)
    # needed by homebrew on MacOS
    SET(QUAZIP_LIBRARY_NAMES ${QUAZIP_LIBRARY_NAMES} quazip)
  ENDIF(APPLE)
  FIND_LIBRARY(
    QUAZIP_LIBRARIES
    NAMES ${QUAZIP_LIBRARY_NAMES}
    HINTS /usr/lib /usr/lib64 /usr/local/lib /opt/local/lib)

  # special case when using Qt on unix
  SET(QUAZIP_PATH_SUFFIXES
      quazip${QT_MAJOR_VERSION}
      quazip
      QuaZip-Qt${QT_MAJOR_VERSION}-1.0/quazip
      QuaZip-Qt${QT_MAJOR_VERSION}-1.1/quazip
      QuaZip-Qt${QT_MAJOR_VERSION}-1.2/quazip
      QuaZip-Qt${QT_MAJOR_VERSION}-1.3/quazip
      QuaZip-Qt${QT_MAJOR_VERSION}-1.4/quazip)
  FIND_PATH(
    QUAZIP_INCLUDE_DIR quazip.h
    HINTS /usr/include /usr/local/include /usr/local/include/quazip
    PATH_SUFFIXES ${QUAZIP_PATH_SUFFIXES})
  FIND_PATH(QUAZIP_ZLIB_INCLUDE_DIR zlib.h HINTS /usr/include
                                                 /usr/local/include)
ENDIF(WIN32)

INCLUDE(FindPackageHandleStandardArgs)
SET(QUAZIP_INCLUDE_DIRS ${QUAZIP_INCLUDE_DIR} ${QUAZIP_ZLIB_INCLUDE_DIR})
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
  QuaZip DEFAULT_MSG QUAZIP_LIBRARIES QUAZIP_INCLUDE_DIR
  QUAZIP_ZLIB_INCLUDE_DIR QUAZIP_INCLUDE_DIRS)
