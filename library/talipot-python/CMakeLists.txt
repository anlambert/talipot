# VS2013 and beyond implement C99 math functions.  Of interest to Python is the
# function round(). Python conditionally provides its own implementation of
# round() based on whether or not HAVE_ROUND is defined, but in no case is
# HAVE_ROUND ever defined. This leads to a huge spew of warnings when compiling
# with VS2013, and presumably it also leads to undefined behavior. So define
# HAVE_ROUND for that compiler.
IF(MSVC12)
  TALIPOT_SET_CXX_FLAGS("-DHAVE_ROUND")
ENDIF(MSVC12)

ADD_SUBDIRECTORY(modules)
ADD_SUBDIRECTORY(bindings)
ADD_SUBDIRECTORY(plugins)

# libtalipot-python is not needed when not building talipot gui
IF(NOT TALIPOT_BUILD_CORE_ONLY)
  ADD_SUBDIRECTORY(include)
  ADD_SUBDIRECTORY(src)
  INSTALL(DIRECTORY api DESTINATION ${TalipotShareInstallDir})
ENDIF(NOT TALIPOT_BUILD_CORE_ONLY)
