# Inclusion header guard
IF(TalipotUseFile_included)
  RETURN()
ENDIF(TalipotUseFile_included)
SET(TalipotUseFile_included TRUE)

# ========================================================
# Build type detection
# ========================================================
IF(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
  SET(CMAKE_DEBUG_MODE TRUE)
ELSE(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
  SET(CMAKE_DEBUG_MODE FALSE)
ENDIF(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")

# ========================================================
# Check processor architecture
# ========================================================
STRING(COMPARE EQUAL "${CMAKE_SIZEOF_VOID_P}" "8" X86_64)

# ========================================================
# Consider *BSD as Linux
# ========================================================
STRING(COMPARE EQUAL "${CMAKE_SYSTEM_NAME}" "Linux" LINUX)
IF(NOT LINUX)
  STRING(FIND "${CMAKE_SYSTEM_NAME}" "BSD" BSD_POS)
  IF(BSD_POS GREATER -1)
    SET(LINUX TRUE)
    SET(BSD TRUE)
  ENDIF()
ENDIF(NOT LINUX)

IF(APPLE)
  EXECUTE_PROCESS(COMMAND sw_vers -productVersion OUTPUT_VARIABLE MACOS_VERSION)
  STRING(REPLACE "\n" "" MACOS_VERSION "${MACOS_VERSION}")
  STRING(REGEX MATCH "[0-9]+\\.[0-9]+" MACOS_VERSION "${MACOS_VERSION}")
ENDIF(APPLE)

# ========================================================
# Toolchains options and definitions
# ========================================================
MACRO(TALIPOT_SET_CXX_FLAGS flag)
  STRING(FIND "${CMAKE_CXX_FLAGS}" "${flag}" FLAG_POS)
  IF(${FLAG_POS} EQUAL -1)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
  ENDIF()
ENDMACRO(TALIPOT_SET_CXX_FLAGS)

MACRO(TALIPOT_SET_C_FLAGS flag)
  STRING(FIND "${CMAKE_C_FLAGS}" "${flag}" FLAG_POS)
  IF(${FLAG_POS} EQUAL -1)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}")
  ENDIF()
ENDMACRO(TALIPOT_SET_C_FLAGS)

MACRO(TALIPOT_SET_CACHE_VAR cache_var_name content concat force)
  GET_PROPERTY(
    HELP_STR
    CACHE ${cache_var_name}
    PROPERTY HELPSTRING)
  STRING(FIND "${${cache_var_name}}" "${content}" CNT_POS)
  IF(${CNT_POS} EQUAL -1)
    SET(VAR_VALUE "${content}")
    IF(${concat})
      SET(VAR_VALUE "${${cache_var_name}} ${content}")
    ENDIF(${concat})
    IF(${force})
      SET(${cache_var_name}
          "${VAR_VALUE}"
          CACHE STRING "${HELP_STR}" FORCE)
    ELSE(${force})
      SET(${cache_var_name}
          "${VAR_VALUE}"
          CACHE STRING "${HELP_STR}")
    ENDIF(${force})
  ENDIF(${CNT_POS} EQUAL -1)
ENDMACRO(
  TALIPOT_SET_CACHE_VAR
  cache_var_name
  content
  concat
  force)

MACRO(TALIPOT_SET_COMPILER_OPTIONS_AND_DEFINITIONS)

  # ========================================================
  # Operating system preprocessor macros
  # ========================================================
  IF(LINUX)
    ADD_DEFINITIONS("-D_LINUX")
  ENDIF(LINUX)
  IF(WIN32)
    ADD_DEFINITIONS("-D_WIN32")
    # ensure WIN32 is defined (as it is not the case when compiling with MinGW
    # and C++11 standard activated)
    ADD_DEFINITIONS("-DWIN32")
    # ensure math defines (e.g. M_PI) are available (as they have been dropped
    # from C++11 standard)
    ADD_DEFINITIONS("-D_USE_MATH_DEFINES")
  ENDIF(WIN32)
  IF(APPLE)
    ADD_DEFINITIONS("-D__APPLE__")
  ENDIF(APPLE)

  # ========================================================
  # AppImage build
  # ========================================================
  IF(TALIPOT_BUILD_FOR_APPIMAGE)
    ADD_DEFINITIONS("-DAPPIMAGE_BUILD")
  ENDIF(TALIPOT_BUILD_FOR_APPIMAGE)

  IF(X86_64)
    TALIPOT_SET_CXX_FLAGS("-DX86_64")
  ENDIF(X86_64)

  # When CMake policy CMP0025
  # (https://cmake.org/cmake/help/v3.0/policy/CMP0025.html) is set to NEW,
  # CMAKE_CXX_COMPILER_ID is equal to "AppleClang" when using clang compilers
  # bundled with XCode while it is equal to "Clang" when using upstream clang
  # compilers provided by MacPorts or Homebrew. So we need to handle both cases
  # to avoid build issues.
  STRING(FIND "${CMAKE_CXX_COMPILER_ID}" "Clang" CLANG_POS)
  STRING(COMPARE NOTEQUAL "${CLANG_POS}" "-1" CLANG)

  # Enable C++17 standard
  SET(CMAKE_CXX_STANDARD 20)
  SET(CMAKE_CXX_STANDARD_REQUIRED ON)
  SET(CMAKE_CXX_EXTENSIONS OFF)

  IF(NOT MSVC) # Visual Studio does not recognize these options
    TALIPOT_SET_CXX_FLAGS(
      "-Wall -Wextra -Wunused -Wno-long-long -Wold-style-cast")
    IF(NOT APPLE)
      TALIPOT_SET_CXX_FLAGS("-pedantic")
    ENDIF(NOT APPLE)

    IF(BSD)
      # That compiler flag is required on FreeBSD in order to get a backtrace
      # when Talipot crashes
      SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-omit-frame-pointer")
      IF(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8.0
         OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.8.0)
        # Those flags are required to compile Talipot with gcc >= 4.8 or clang
        # on FreeBSD 9.x and 10.x
        TALIPOT_SET_CXX_FLAGS("-D_GLIBCXX_USE_C99 -fno-omit-frame-pointer")
      ENDIF(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8.0
            OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.8.0)
      # Need to set rpath for the right libstdc++ to use
      IF(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.7.0
         AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9.0)
        SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath=/usr/local/lib/gcc48")
        SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,-rpath=/usr/local/lib/gcc48")
        SET(CMAKE_MODULE_LINKER_FLAGS "-Wl,-rpath=/usr/local/lib/gcc48")
      ENDIF(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.7.0
            AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9.0)
      IF(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8.0
         AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0.0)
        SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath=/usr/local/lib/gcc49")
        SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,-rpath=/usr/local/lib/gcc49")
        SET(CMAKE_MODULE_LINKER_FLAGS "-Wl,-rpath=/usr/local/lib/gcc49")
      ENDIF(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8.0
            AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0.0)
      IF(CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 5.0.0
         OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.0.0)
        SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath=/usr/local/lib/gcc5")
        SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,-rpath=/usr/local/lib/gcc5")
        SET(CMAKE_MODULE_LINKER_FLAGS "-Wl,-rpath=/usr/local/lib/gcc5")
      ENDIF(CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 5.0.0
            OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.0.0)
    ENDIF(BSD)
  ENDIF(NOT MSVC)

  IF(WIN32)
    IF(NOT MSVC) # visual studio does not recognize these options
      # Dynamic ling against libstdc++ on win32/MinGW The second test is for the
      # case where ccache is used (CMAKE_CXX_COMPILER_ARG1 contains the path to
      # the g++ compiler)
      IF(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ARG1}" MATCHES
                                     ".*[g][+][+].*")
        IF(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.0)
          SET(CMAKE_EXE_LINKER_FLAGS
              "${CMAKE_EXE_LINKER_FLAGS} -Wl,--subsystem,windows")
          # GCC 4.4 use double dashes and gcc 4.6 single dashes for this option
          IF(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.6)
            SET(CMAKE_EXE_LINKER_FLAGS
                "${CMAKE_EXE_LINKER_FLAGS} --shared-libgcc -Wl,--allow-multiple-definition"
            )
            SET(CMAKE_MODULE_LINKER_FLAGS
                "${CMAKE_MODULE_LINKER_FLAGS} --shared-libgcc  -Wl,--allow-multiple-definition"
            )
            SET(CMAKE_SHARED_LINKER_FLAGS
                "${CMAKE_SHARED_LINKER_FLAGS} --shared-libgcc  -Wl,--allow-multiple-definition"
            )
          ELSE()
            SET(CMAKE_EXE_LINKER_FLAGS
                "${CMAKE_EXE_LINKER_FLAGS} -shared-libgcc -Wl,--allow-multiple-definition"
            )
            SET(CMAKE_MODULE_LINKER_FLAGS
                "${CMAKE_MODULE_LINKER_FLAGS} -shared-libgcc  -Wl,--allow-multiple-definition"
            )
            SET(CMAKE_SHARED_LINKER_FLAGS
                "${CMAKE_SHARED_LINKER_FLAGS} -shared-libgcc  -Wl,--allow-multiple-definition"
            )
          ENDIF()
        ENDIF()
      ENDIF()
    ENDIF(NOT MSVC)

    IF(MSVC)
      IF(${CMAKE_GENERATOR} MATCHES "Visual Studio 9") # Visual studio 2008
                                                       # needs boost
        FIND_PACKAGE(BOOST REQUIRED)
        INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS}
                            ${Boost_INCLUDE_DIRS}/boost/tr1)
      ENDIF()

      # Tells VS to use multiple threads to compile
      TALIPOT_SET_CXX_FLAGS("/MP")
      # Makes VS define M_PI
      TALIPOT_SET_CXX_FLAGS("-D_USE_MATH_DEFINES")
      # Prevents VS to define min and max macros (name clash with std::min and
      # std::max)
      TALIPOT_SET_CXX_FLAGS("-DNOMINMAX")
      # Don't warn about the use of unsafe function
      TALIPOT_SET_CXX_FLAGS("-D_CRT_SECURE_NO_WARNINGS")
      # Disable some annoying compiler warnings
      TALIPOT_SET_CXX_FLAGS(
        "/wd4251 /wd4267 /wd4275 /wd4244 /wd4355 /wd4800 /wd4503 /wd4344 /wd4996"
      )

      SET(CMAKE_SHARED_LINKER_FLAGS
          "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
      SET(CMAKE_SHARED_LINKER_FLAGS
          "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBCMTD")

      SET(CMAKE_EXE_LINKER_FLAGS_RELEASE
          "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:windows /ENTRY:mainCRTStartup"
      )
      SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
          "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /SUBSYSTEM:windows /ENTRY:mainCRTStartup"
      )

      IF(X86_64)
        TALIPOT_SET_CXX_FLAGS("/bigobj")
        SET(CMAKE_SHARED_LINKER_FLAGS
            "${CMAKE_SHARED_LINKER_FLAGS}  /STACK:10000000 /MACHINE:X64")
        SET(CMAKE_EXE_LINKER_FLAGS
            "${CMAKE_EXE_LINKER_FLAGS}  /STACK:10000000 /MACHINE:X64")
        SET(CMAKE_MODULE_LINKER_FLAGS
            "${CMAKE_MODULE_LINKER_FLAGS}  /STACK:10000000 /MACHINE:X64")
      ELSE(X86_64)
        TALIPOT_SET_CXX_FLAGS("/arch:SSE2")
        SET(CMAKE_SHARED_LINKER_FLAGS
            "${CMAKE_SHARED_LINKER_FLAGS} /MACHINE:X86")
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MACHINE:X86")
        SET(CMAKE_MODULE_LINKER_FLAGS
            "${CMAKE_MODULE_LINKER_FLAGS} /MACHINE:X86")
      ENDIF(X86_64)

    ENDIF(MSVC)

    # Need to use response files with MSYS Makefiles and recent CMake version
    # (>= 3.7) bundled by MSYS2 otherwise OGDF library in thirdparty fails to
    # link
    IF("${CMAKE_GENERATOR}" MATCHES ".*MSYS.*")
      SET(CMAKE_NEED_RESPONSE
          TRUE
          CACHE BOOL "" FORCE)
    ENDIF("${CMAKE_GENERATOR}" MATCHES ".*MSYS.*")

  ENDIF(WIN32)

  # OpenMP (only available with clang starting the 3.7 version with libomp
  # installed)
  IF(NOT CLANG
     OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.7.0
     OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 3.7.0)
    FIND_PACKAGE(Threads)
    IF(CMAKE_DEBUG_MODE)
      OPTION(TALIPOT_ENABLE_MULTI_THREADING
             "Do you want to enable multithreaded code (debug mode)?" OFF)
    ELSE()
      OPTION(TALIPOT_ENABLE_MULTI_THREADING
             "Do you want to enable multithreaded code?" ON)
    ENDIF()
    IF(TALIPOT_ENABLE_MULTI_THREADING)
      # TALIPOT_CXX_THREADS can be set to force the use of the cxx threads
      # regardless the OpenMP availability
      IF(NOT TALIPOT_CXX_THREADS)
        IF(CLANG)
          EXECUTE_PROCESS(COMMAND ${CMAKE_CXX_COMPILER} --version
                          OUTPUT_VARIABLE CLANG_VERSION)
          STRING(FIND "${CLANG_VERSION}" "Apple" APPLE_POS)
          STRING(COMPARE EQUAL "${APPLE_POS}" "-1" LLVM_LIBOMP)
          # When using LLVM clang , some extra setup is required in order to
          # detect and use OpenMP through the libomp runtime
          IF(LLVM_LIBOMP)
            TALIPOT_SET_CACHE_VAR(OpenMP_C_FLAGS "-fopenmp=libomp" FALSE FALSE)
            TALIPOT_SET_CACHE_VAR(OpenMP_C_LIB_NAMES "libomp" FALSE FALSE)
            TALIPOT_SET_CACHE_VAR(OpenMP_CXX_FLAGS "-fopenmp=libomp" FALSE
                                  FALSE)
            TALIPOT_SET_CACHE_VAR(OpenMP_CXX_LIB_NAMES "libomp" FALSE FALSE)
            TALIPOT_SET_CACHE_VAR(OpenMP_libomp_LIBRARY "libomp" FALSE FALSE)
            GET_FILENAME_COMPONENT(LLVM_COMPILER_DIR "${CMAKE_C_COMPILER}"
                                   DIRECTORY)
            TALIPOT_SET_CACHE_VAR(CMAKE_C_FLAGS
                                  "-I${LLVM_COMPILER_DIR}/../include" TRUE TRUE)
            TALIPOT_SET_CACHE_VAR(CMAKE_CXX_FLAGS
                                  "-I${LLVM_COMPILER_DIR}/../include" TRUE TRUE)
            TALIPOT_SET_CACHE_VAR(CMAKE_EXE_LINKER_FLAGS
                                  "-L${LLVM_COMPILER_DIR}/../lib" TRUE TRUE)
            TALIPOT_SET_CACHE_VAR(CMAKE_SHARED_LINKER_FLAGS
                                  "-L${LLVM_COMPILER_DIR}/../lib" TRUE TRUE)
          ENDIF(LLVM_LIBOMP)
        ENDIF(CLANG)
        FIND_PACKAGE(OpenMP)
        IF(OPENMP_FOUND)
          SET(CMAKE_CXX_FLAGS_RELEASE
              "${CMAKE_CXX_FLAGS_RELEASE} ${OpenMP_CXX_FLAGS}")
          SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO
              "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${OpenMP_CXX_FLAGS}")
          SET(CMAKE_CXX_FLAGS_DEBUG
              "${CMAKE_CXX_FLAGS_DEBUG} ${OpenMP_CXX_FLAGS}")
          SET(OPENMP_CXX_FLAGS "${OpenMP_CXX_FLAGS}")
          IF(WIN32)
            IF(MSVC)
              SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /openmp")
              SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO
                  "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /openmp")
              SET(CMAKE_CXX_FLAGS_DEBUG
                  "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /openmp")
              SET(OPENMP_CXX_FLAGS "/openmp")
            ELSE()
              SET(CMAKE_CXX_STANDARD_LIBRARIES
                  "${CMAKE_CXX_STANDARD_LIBRARIES} -lgomp ${CMAKE_THREAD_LIBS_INIT}"
              )
              SET(OPENMP_LIBRARIES "-lgomp -lpthread")
            ENDIF()
          ENDIF()
        ELSE(OPENMP_FOUND)
          IF(WIN32)
            STRING(COMPARE NOTEQUAL "${OpenMP_C_FLAGS}" "" OMP_CFLAGS)
            IF(OMP_CFLAGS)
              # Force setting OpenMP flags on Windows platforms
              SET(CMAKE_CXX_FLAGS_RELEASE
                  "${CMAKE_CXX_FLAGS_RELEASE} ${OpenMP_C_FLAGS}")
              SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO
                  "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${OpenMP_C_FLAGS}")
              SET(OPENMP_CXX_FLAGS "${OpenMP_C_FLAGS}")
              IF(NOT MSVC)
                SET(CMAKE_SHARED_LINKER_FLAGS
                    "${CMAKE_SHARED_LINKER_FLAGS} ${OpenMP_C_FLAGS}")
                SET(OPENMP_LINKER_FLAGS "${OpenMP_C_FLAGS}")
                SET(CMAKE_CXX_STANDARD_LIBRARIES
                    "${CMAKE_CXX_STANDARD_LIBRARIES} -lgomp -lpthread")
                SET(OPENMP_LIBRARIES "-lgomp -lpthread")
              ENDIF(NOT MSVC)
              SET(OPENMP_FOUND TRUE)
            ELSE(OMP_CFLAGS)
              MESSAGE(
                "OpenMP not found: Multithreaded code will use C++11 threads")
            ENDIF(OMP_CFLAGS)
          ELSE(WIN32)
            MESSAGE(
              "OpenMP not found: Multithreaded code will use C++11 threads")
          ENDIF(WIN32)
        ENDIF(OPENMP_FOUND)
      ELSE(NOT TALIPOT_CXX_THREADS)
        MESSAGE("Multithreaded code will use C++11 threads")
      ENDIF(NOT TALIPOT_CXX_THREADS)
    ELSE(TALIPOT_ENABLE_MULTI_THREADING)
      MESSAGE("Multithreaded code is disabled")
      TALIPOT_SET_CXX_FLAGS("-DTLP_NO_THREADS")
    ENDIF(TALIPOT_ENABLE_MULTI_THREADING)
  ENDIF(
    NOT CLANG
    OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.7.0
    OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 3.7.0)

  IF(APPLE)
    ADD_DEFINITIONS(-DGL_SILENCE_DEPRECATION)
    # Suppress ranlib warnings about static library having no symbols
    SET(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
    SET(CMAKE_CXX_ARCHIVE_CREATE
        "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
    SET(CMAKE_C_ARCHIVE_FINISH
        "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
    SET(CMAKE_CXX_ARCHIVE_FINISH
        "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
  ENDIF(APPLE)

ENDMACRO(TALIPOT_SET_COMPILER_OPTIONS_AND_DEFINITIONS)

# for backward compatibility with Talipot < 5.1 for external projects
MACRO(SET_COMPILER_OPTIONS)
  TALIPOT_SET_COMPILER_OPTIONS_AND_DEFINITIONS()
ENDMACRO(SET_COMPILER_OPTIONS)

# Plugin server generation
FUNCTION(INSTALL)
  IF(TALIPOT_GENERATE_PLUGINSERVER)

    CMAKE_PARSE_ARGUMENTS(PLUGIN "" "DESTINATION;COMPONENT" "TARGETS" ${ARGN})
    IF(PLUGIN_UNPARSED_ARGUMENTS)
      CMAKE_PARSE_ARGUMENTS(PLUGIN "" "DESTINATION;COMPONENT" "FILES" ${ARGN})
      STRING(REPLACE ${TALIPOT_DIR} "" DEST ${PLUGIN_DESTINATION})
      SET(PLUGIN_DESTINATION
          "${CMAKE_BINARY_DIR}/pluginserver/${PLUGIN_COMPONENT}/${DEST}")

      IF(PLUGIN_UNPARSED_ARGUMENTS)
        CMAKE_PARSE_ARGUMENTS(PLUGIN "" "DESTINATION;COMPONENT" "DIRECTORY"
                              ${ARGN})
        STRING(REPLACE ${TALIPOT_DIR} "" DEST ${PLUGIN_DESTINATION})
        SET(PLUGIN_DESTINATION
            "${CMAKE_BINARY_DIR}/pluginserver/${PLUGIN_COMPONENT}/${DEST}")
        _INSTALL(DIRECTORY ${PLUGIN_DIRECTORY} DESTINATION
                 ${PLUGIN_DESTINATION})
      ENDIF()
      _INSTALL(FILES ${PLUGIN_FILES} DESTINATION ${PLUGIN_DESTINATION})
    ELSE()
      STRING(REPLACE ${TALIPOT_DIR} "" DEST ${PLUGIN_DESTINATION})
      SET(PLUGIN_DESTINATION
          "${CMAKE_BINARY_DIR}/pluginserver/${PLUGIN_COMPONENT}/${DEST}")
      FOREACH(TARGET ${PLUGIN_TARGETS})
        IF(MINGW)
          _INSTALL(TARGETS ${PLUGIN_TARGETS} RUNTIME DESTINATION
                   ${PLUGIN_DESTINATION})
        ELSE(MINGW)
          _INSTALL(TARGETS ${PLUGIN_TARGETS} DESTINATION ${PLUGIN_DESTINATION})
        ENDIF(MINGW)
      ENDFOREACH()
    ENDIF()

  ELSE(TALIPOT_GENERATE_PLUGINSERVER)
    _INSTALL(${ARGN})
  ENDIF()
ENDFUNCTION(INSTALL)

MACRO(TALIPOT_DISABLE_COMPILER_WARNINGS)
  IF(MSVC)
    STRING(REGEX REPLACE "/W[0-9]" "/W0" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    STRING(REGEX REPLACE "/W[0-9]" "/W0" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  ELSE(MSVC)
    TALIPOT_SET_CXX_FLAGS("-w")
    TALIPOT_SET_C_FLAGS("-w")
  ENDIF(MSVC)
ENDMACRO(TALIPOT_DISABLE_COMPILER_WARNINGS)

# for backward compatibility with Talipot < 5.1 for external projects
MACRO(DISABLE_COMPILER_WARNINGS)
  TALIPOT_DISABLE_COMPILER_WARNINGS()
ENDMACRO(DISABLE_COMPILER_WARNINGS)

# External libraries macros
IF(WIN32)

  IF(MINGW)
    # get paths to MINGW binaries, libraries and headers
    STRING(REPLACE "ar.exe" "" MINGW_BIN_PATH ${CMAKE_AR})
    SET(MINGW_LIB_PATH ${MINGW_BIN_PATH}/../lib)
    SET(MINGW_LIB64_PATH ${MINGW_BIN_PATH}/../lib64)
    SET(MINGW_INCLUDE_PATH ${MINGW_BIN_PATH}/../include)
  ENDIF(MINGW)

  MACRO(TALIPOT_FIND_EXTERNAL_LIB pattern result_var_name)
    UNSET(${result_var_name})
    UNSET(found_paths)
    FOREACH(win_path $ENV{CMAKE_LIBRARY_PATH} ${QT_BINARY_DIR}
                     ${MINGW_BIN_PATH} ${CMAKE_LIBRARY_PATH})
      STRING(REPLACE "\\" "/" cmake_path "${win_path}")
      FILE(GLOB match "${cmake_path}/${pattern}")
      IF(match)
        GET_FILENAME_COMPONENT(match_absolute "${match}" ABSOLUTE)
        SET(found_paths ${found_paths} ${match_absolute})
      ENDIF(match)
    ENDFOREACH()
    IF(found_paths)
      LIST(REMOVE_DUPLICATES found_paths)
      SET(${result_var_name} ${found_paths})
    ENDIF(found_paths)
  ENDMACRO(TALIPOT_FIND_EXTERNAL_LIB)

  MACRO(TALIPOT_GET_DLL_NAME_FROM_IMPORT_LIBRARY import_library dll_name)
    UNSET(${dll_name})
    IF(MINGW)
      EXECUTE_PROCESS(
        COMMAND ${MINGW_BIN_PATH}/dlltool.exe -I ${import_library}
        OUTPUT_VARIABLE DLL_FILENAME
        ERROR_VARIABLE DLLTOOL_ERROR)
      IF(DLLTOOL_ERROR)
        MESSAGE(
          "${import_library} is not a valid import library (likely a copy of the associated dll). \
Please provide a valid one in order to determine the dll the application depends to."
        )
      ELSE(DLLTOOL_ERROR)
        STRING(REPLACE "\n" "" ${dll_name} ${DLL_FILENAME})
      ENDIF(DLLTOOL_ERROR)
    ENDIF(MINGW)
    IF(MSVC)
      # Get path of MSVC compiler
      GET_FILENAME_COMPONENT(COMPILER_DIRECTORY ${CMAKE_CXX_COMPILER} DIRECTORY)
      # Get root path of Visual Studio
      IF(MSVC14)
        GET_FILENAME_COMPONENT(
          VS_DIR
          [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\14.0\\Setup\\VS;ProductDir]
          REALPATH)
      ELSEIF(MSVC12)
        GET_FILENAME_COMPONENT(
          VS_DIR
          [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\12.0\\Setup\\VS;ProductDir]
          REALPATH)
      ELSEIF(MSVC11)
        GET_FILENAME_COMPONENT(
          VS_DIR
          [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\11.0\\Setup\\VS;ProductDir]
          REALPATH)
      ELSEIF(MSVC10)
        GET_FILENAME_COMPONENT(
          VS_DIR
          [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\10.0\\Setup\\VS;ProductDir]
          REALPATH)
      ELSEIF(MSVC90)
        GET_FILENAME_COMPONENT(
          VS_DIR
          [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\9.0\\Setup\\VS;ProductDir]
          REALPATH)
      ENDIF()
      # Add temporarily some paths to the PATH environment variable in order to
      # locate some dlls needed to run lib.exe command (mspdb*.dll)
      SET(VS_IDE_DIR "${VS_DIR}/Common7/IDE")
      SET(VS_VC_BIN_DIR "${VS_DIR}/VC/bin")
      SET(PATH_BACKUP "$ENV{PATH}")
      SET(ENV{PATH} "${VS_IDE_DIR};${VS_VC_BIN_DIR};$ENV{PATH}")
      # Run the lib.exe command to list the content of the library file
      EXECUTE_PROCESS(
        COMMAND ${COMPILER_DIRECTORY}/lib.exe /list ${import_library}
        OUTPUT_VARIABLE LIBRARY_CONTENTS)
      # If the library is an import library, lib.exe outputs the associated dll
      # name instead of the object files for a static library
      STRING(REGEX MATCH "[^\r?\n]+\\.dll" ${dll_name} ${LIBRARY_CONTENTS})
      # Restore original PATH environment variable value
      SET(ENV{PATH} "${PATH_BACKUP}")
    ENDIF(MSVC)
  ENDMACRO(TALIPOT_GET_DLL_NAME_FROM_IMPORT_LIBRARY)

ENDIF(WIN32)

MACRO(TALIPOT_COPY_TARGET_LIBRARY_POST_BUILD target_name destination)
  STRING(MD5 DESTINATION_HASH "${destination}")
  SET(COPY_TARGET_NAME copy-${target_name}-${DESTINATION_HASH})

  IF(WIN32)
    ADD_CUSTOM_TARGET(
      ${COPY_TARGET_NAME} ALL
      COMMAND
        ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${target_name}>
        ${destination}/$<TARGET_FILE_NAME:${target_name}>
      DEPENDS ${target_name}
      VERBATIM)
  ELSE(WIN32)
    ADD_CUSTOM_TARGET(
      ${COPY_TARGET_NAME} ALL
      COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_SONAME_FILE:${target_name}>
        ${destination}/$<TARGET_SONAME_FILE_NAME:${target_name}>
      DEPENDS ${target_name}
      VERBATIM)
  ENDIF(WIN32)

  # optional parameters of the macro corresponds to targets that depend on the
  # above created custom target
  SET(DEPENDENCIES_TARGETS ${ARGN})

  FOREACH(DEPENDENCY_TARGET ${DEPENDENCIES_TARGETS})
    ADD_DEPENDENCIES(${DEPENDENCY_TARGET} ${COPY_TARGET_NAME})
  ENDFOREACH()
ENDMACRO(TALIPOT_COPY_TARGET_LIBRARY_POST_BUILD)

# internal cache variable to hold the names of the Talipot plugin targets
SET(TALIPOT_PLUGIN_TARGETS
    ""
    CACHE INTERNAL "")

# Talipot Plugin install macro (its purpose is to disable the installation of
# MinGW import libraries)
MACRO(TALIPOT_INSTALL_PLUGIN plugin_target destination)
  SET(COMPONENT_NAME ${plugin_target})
  STRING(REPLACE "-${TalipotVersion}" "" COMPONENT_NAME "${COMPONENT_NAME}")
  INSTALL(
    TARGETS ${plugin_target}
    RUNTIME DESTINATION ${destination}
    LIBRARY DESTINATION ${destination})

  # append the plugin target to the overall plugin targets list
  SET(TALIPOT_PLUGIN_TARGETS
      "${TALIPOT_PLUGIN_TARGETS};${plugin_target}"
      CACHE INTERNAL "")

  # When building a Python wheel, copy Talipot plugins in wheel build folder in
  # order to package them with the Talipot Python bindings
  IF(TALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET)
    SET(TALIPOT_PLUGIN_WHEEL_INSTALL_DIR
        "${TALIPOT_PYTHON_NATIVE_FOLDER}/plugins")

    TALIPOT_COPY_TARGET_LIBRARY_POST_BUILD(
      ${plugin_target} ${TALIPOT_PLUGIN_WHEEL_INSTALL_DIR} wheel)
  ENDIF(TALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET)
ENDMACRO(TALIPOT_INSTALL_PLUGIN)

# for backward compatibility with Talipot < 5.1 for external projects
MACRO(INSTALL_TALIPOT_PLUGIN)
  TALIPOT_INSTALL_PLUGIN(${ARGV})
ENDMACRO(INSTALL_TALIPOT_PLUGIN)

MACRO(TALIPOT_INSTALL_PYTHON_FILES install_suffix)
  FOREACH(PYTHON_FILE ${ARGN})
    IF(TARGET ${PYTHON_FILE})
      INSTALL(
        TARGETS ${PYTHON_FILE}
        RUNTIME DESTINATION ${TalipotPythonModulesInstallDir}/${install_suffix}
        LIBRARY DESTINATION ${TalipotPythonModulesInstallDir}/${install_suffix})
    ELSEIF(IS_DIRECTORY ${PYTHON_FILE})
      INSTALL(DIRECTORY ${PYTHON_FILE}
              DESTINATION ${TalipotPythonModulesInstallDir}/${install_suffix})
    ELSE()
      INSTALL(FILES ${PYTHON_FILE}
              DESTINATION ${TalipotPythonModulesInstallDir}/${install_suffix})
    ENDIF(TARGET ${PYTHON_FILE})
  ENDFOREACH()
ENDMACRO(TALIPOT_INSTALL_PYTHON_FILES)

# Convert a Windows path (C:/folder) to a Msys path (/C/folder)
MACRO(TALIPOT_WINDOWS_TO_MSYS_PATH WindowsPath ResultingPath)
  STRING(REGEX REPLACE "([a-zA-Z]):" "/\\1" ${ResultingPath} "${WindowsPath}")
ENDMACRO(TALIPOT_WINDOWS_TO_MSYS_PATH)

# Convert a Msys path (/c/folder) to a Windows path (c:/folder)
MACRO(TALIPOT_MSYS_TO_WINDOWS_PATH MsysPath ResultingPath)
  STRING(REGEX REPLACE "/([a-zA-Z])/" "\\1:/" ${ResultingPath} "${MsysPath}")
ENDMACRO(TALIPOT_MSYS_TO_WINDOWS_PATH)

INCLUDE(CMakeParseArguments)

MACRO(TALIPOT_ADD_PLUGIN)
  SET(TLP_VERSION ${TalipotVersion})
  IF("${TLP_VERSION}" STREQUAL "")
    SET(TLP_VERSION ${TALIPOT_VERSION})
  ENDIF("${TLP_VERSION}" STREQUAL "")
  CMAKE_PARSE_ARGUMENTS(PLUGIN "FIXUP_INSTALL" "NAME;INSTALL_DIR" "SRCS;LINKS"
                        ${ARGN})
  SET(PLUGIN_LIB_NAME ${PLUGIN_NAME}-talipot-${TLP_VERSION})
  ADD_LIBRARY(${PLUGIN_LIB_NAME} SHARED ${PLUGIN_SRCS})
  TARGET_LINK_LIBRARIES(${PLUGIN_LIB_NAME} ${PLUGIN_LINKS})
  TALIPOT_INSTALL_PLUGIN(${PLUGIN_LIB_NAME} ${PLUGIN_INSTALL_DIR})
  IF(WIN32 AND PLUGIN_FIXUP_INSTALL)
    SET(PLUGIN_DLL "${PLUGIN_LIB_NAME}.dll")
    IF(NOT MSVC)
      SET(PLUGIN_DLL "lib${PLUGIN_DLL}")
    ENDIF(NOT MSVC)
    # update the list of bundled libs to fixup
    SET_PROPERTY(
      GLOBAL APPEND
      PROPERTY FIXUP_BUNDLE_LIBS
               "${CMAKE_INSTALL_PREFIX}/${PLUGIN_INSTALL_DIR}/${PLUGIN_DLL}")
  ENDIF(WIN32 AND PLUGIN_FIXUP_INSTALL)
ENDMACRO(TALIPOT_ADD_PLUGIN)
