FIND_PATH(FRIBIDI_INCLUDE_DIR NAMES fribidi/fribidi.h)

FIND_LIBRARY(FRIBIDI_LIBRARY NAMES fribidi)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FriBidi REQUIRED_VARS FRIBIDI_LIBRARY
                                                        FRIBIDI_INCLUDE_DIR)