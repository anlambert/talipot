ADD_SUBDIRECTORY(bitmaps)

# Override the CMAKE_RC_COMPILE_OBJECT variable to fix an issue with the windres
# compiler coming with binutils 2.32 on MSYS2
IF(WIN32 AND NOT MSVC)
  SET(CMAKE_RC_COMPILE_OBJECT
      "<CMAKE_RC_COMPILER> -O coff -o <OBJECT> <SOURCE>")
ENDIF(WIN32 AND NOT MSVC)

ADD_SUBDIRECTORY(talipot)
