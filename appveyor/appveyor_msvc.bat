rem Matrix-driven Appveyor CI script for Talipot using Visual Studio compiler

rem Check which type of Talipot build to perform based on the job number
rem   - odd number = core build
rem   - even number = complete build
rem We build Talipot in two phases (core build then complete build)
rem to avoid AppVeyor build timeouts. Object files generated during
rem the core build will be made available for the complete build
rem thanks to the use of clcache.
set /a TALIPOT_BUILD_CORE_ONLY = %APPVEYOR_JOB_NUMBER% %% 2
echo TALIPOT_BUILD_CORE_ONLY=%TALIPOT_BUILD_CORE_ONLY%

rem save start time
set starttime=%time%

rem let's compile clcache in order to speedup incremental builds
cd C:/
set PATH=C:/Python39-x64;C:/Python39-x64/Scripts;%PATH%
pip install clcache
set CLCACHE_MSBUILD_CONF=/p:TrackFileAccess=false /p:CLToolExe=clcache.exe^
  /p:CLToolPath=C:\Python39-x64\Scripts

rem create a directory to store some Talipot dependencies
cd C:/ && md talipot_dependencies
cd C:/talipot_dependencies && md bin
cd C:/talipot_dependencies && md include
cd C:/talipot_dependencies && md lib

cd C:\Tools\vcpkg
git pull
call bootstrap-vcpkg.bat

rem install talipot core build dependencies
vcpkg install --triplet x64-windows zlib zstd qhull yajl cppunit

rem get, compile and install graphviz
cd C:/talipot_dependencies
git clone https://gitlab.com/graphviz/graphviz.git
cd graphviz
git checkout 2.46.0
git submodule update --init
md build && cd build
cmake -G "%CMAKE_VS_GENERATOR%"^
  -A "%MSVC_ARCH%"^
  -DCMAKE_INSTALL_PREFIX="C:/talipot_dependencies"^
  -DCMAKE_PROGRAM_PATH="C:/msys64/usr/bin"^
  -DVERSION="2.46.0"^
  -DDATE="2021-01-18"^
  ..
if %errorlevel% neq 0 exit /b %errorlevel%
msbuild INSTALL.vcxproj /clp:ErrorsOnly /p:Configuration=Release %CLCACHE_MSBUILD_CONF%
if %errorlevel% neq 0 exit /b %errorlevel%

if "%TALIPOT_BUILD_CORE_ONLY%" == "0" (
  goto install_complete_talipot_build_dependencies
) else {
  goto talipot_build
}


:install_complete_talipot_build_dependencies

rem install talipot complete build dependencies
vcpkg install --recurse --triplet x64-windows freetype glew fontconfig fribidi

rem get, compile and install quazip
cd C:/talipot_dependencies
curl -LO https://github.com/stachenov/quazip/archive/v1.1.zip
7z x v1.1.zip
cd quazip-1.1
md build && cd build
cmake -G "%CMAKE_VS_GENERATOR%"^
  -A "%MSVC_ARCH%"^
  -DCMAKE_INSTALL_PREFIX="C:/talipot_dependencies"^
  -DCMAKE_PREFIX_PATH="%QT5_DIR%"^
  -DCMAKE_INCLUDE_PATH="C:/Tools/vcpkg/installed/x64-windows/include"^
  -DCMAKE_LIBRARY_PATH="C:/Tools/vcpkg/installed/x64-windows/lib" ..
if %errorlevel% neq 0 exit /b %errorlevel%
msbuild INSTALL.vcxproj /clp:ErrorsOnly /p:Configuration=Release %CLCACHE_MSBUILD_CONF%
if %errorlevel% neq 0 exit /b %errorlevel%
goto talipot_build


:talipot_build

rem compute time elapsed since script started
set endtime=%time%
for /f "tokens=1-4 delims=:.," %%a in ("%starttime%") do (
   set /a "start=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100")
)
for /f "tokens=1-4 delims=:.," %%a in ("%endtime%") do (
   set /a "end=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)
set /a elapsed=end-start
set /a hh=elapsed/(60*60*100), rest=elapsed%%(60*60*100), mm=rest/(60*100),^
  rest%%=60*100, ss=rest/100, cc=rest%%100

rem if more than 30 minutes elapsed, it means vcpkg dependencies got recompiled
rem and that talipot build will likely fail due to the 1 hour timeout on appveyor
rem so exit without error in order to put those dependencies in cache and avoid
rem recompiling them on next build
if %mm% gtr 30 exit /b 0

rem we are good to go, let's compile and install Talipot now
cd %APPVEYOR_BUILD_FOLDER%
md build && cd build
set INCLUDE_PATH=C:/Tools/vcpkg/installed/x64-windows/include;^
C:/talipot_dependencies/include;^
C:/talipot_dependencies/include/graphviz
set LIBRARY_PATH=C:/Tools/vcpkg/installed/x64-windows/bin;^
C:/Tools/vcpkg/installed/x64-windows/lib;^
C:/talipot_dependencies/lib;^
C:/talipot_dependencies/bin
cmake -G "%CMAKE_VS_GENERATOR%"^
  -A "%MSVC_ARCH%"^
  -DCMAKE_INSTALL_PREFIX="C:/talipot"^
  -DCMAKE_INCLUDE_PATH="%INCLUDE_PATH%"^
  -DCMAKE_LIBRARY_PATH="%LIBRARY_PATH%"^
  -DCMAKE_PREFIX_PATH="%QT5_DIR%"^
  -DPYTHON_EXECUTABLE="%PYTHON_EXECUTABLE%"^
  -DTALIPOT_BUILD_CORE_ONLY=%TALIPOT_BUILD_CORE_ONLY%^
  -DTALIPOT_BUILD_DOC=OFF^
  -DTALIPOT_BUILD_TESTS=ON ..
if %errorlevel% neq 0 exit /b %errorlevel%
msbuild INSTALL.vcxproj /verbosity:minimal /m /p:Configuration=Release %CLCACHE_MSBUILD_CONF%
if %errorlevel% neq 0 exit /b %errorlevel%
rem finally run Talipot tests
ctest --force-new-ctest-process --output-on-failure --build-config "Release"
if %errorlevel% neq 0 exit /b %errorlevel%

if "%TALIPOT_BUILD_CORE_ONLY%" == "0" (
  msbuild bundlers/win/bundle.vcxproj /m /p:Configuration=Release %CLCACHE_MSBUILD_CONF%
)
