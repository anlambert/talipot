@echo off
set NSIS_PATH=%1
set TALIPOT_DIR=%2
set DEST_DIR=%3
set PYTHON_STDLIB_DIR=%4
set PYTHON_VERSION=%5
set SRC_DIR=%cd%

echo 'Wiping destination directory'
if EXIST "%DEST_DIR%\\files" (
  del /s /f /q "%DEST_DIR%\\files\\*.*" >nul 2>&1
  for /f %%f in ('dir /ad /b "%DEST_DIR%\\files"') do rd /s /q "%DEST_DIR%\\files\\%%f"
) else ( mkdir "%DEST_DIR%\\files" )

echo 'Copying Talipot files'
xcopy "%TALIPOT_DIR%" "%DEST_DIR%\\files" /E /Q

echo 'Copying Python standard library'
if EXIST "%PYTHON_STDLIB_DIR%\\lib-dynload" (
  set PYTHON_STD_LIB_DEST_DIR="%DEST_DIR%\\files\\lib\\python%PYTHON_VERSION%"
) else (
  set PYTHON_STD_LIB_DEST_DIR="%DEST_DIR%\\files\\lib"
)

mkdir "%PYTHON_STD_LIB_DEST_DIR%" >nul 2>&1
xcopy "%PYTHON_STDLIB_DIR%" "%PYTHON_STD_LIB_DEST_DIR%" /E /Q

set PYTHON_DLLS_LIB_DEST_DIR="%DEST_DIR%\\files\\DLLs"
mkdir "%PYTHON_DLLS_LIB_DEST_DIR%"
xcopy "%PYTHON_STDLIB_DIR%\\..\\DLLs" "%PYTHON_DLLS_LIB_DEST_DIR%" /E /Q 2>&1
set DLLS_FILES=
for /f "delims=" %%a in ('dir /b "%PYTHON_DLLS_LIB_DEST_DIR%"') do set DLLS_FILES="%%a"
IF {%DLLS_FILES%}=={} (
  rmdir "%PYTHON_DLLS_LIB_DEST_DIR%"
)

if EXIST "%PYTHON_STD_LIB_DEST_DIR%\\site-packages" (
  del /s /f /q "%PYTHON_STD_LIB_DEST_DIR%\\site-packages\\*.*" >nul 2>&1
  for /f %%f in ('dir /ad /b "%PYTHON_STD_LIB_DEST_DIR%\\site-packages"') do rd /s /q "%PYTHON_STD_LIB_DEST_DIR%\\site-packages\\%%f"
)
if EXIST "%PYTHON_STD_LIB_DEST_DIR%\\test" (
  del /s /f /q "%PYTHON_STD_LIB_DEST_DIR%\\test\\*.*" >nul 2>&1
  for /f %%f in ('dir /ad /b "%PYTHON_STD_LIB_DEST_DIR%\\test"') do rd /s /q "%PYTHON_STD_LIB_DEST_DIR%\\test\\%%f"
)
for /d /r "%PYTHON_STD_LIB_DEST_DIR%" %%d in (__pycache__) do @if exist "%%d" rd /s /q "%%d"

echo 'Removing not needed dlls copied by fixup_bundle'
del /Q /F /S "%DEST_DIR%\\files\\bin\\opengl*.dll" >nul 2>&1

echo 'Removing non dll files from lib directory'
del /Q /F /S "%DEST_DIR%\\files\\lib\\*.dll.a" >nul 2>&1

echo 'Copying NSIS script and licence'
copy "%SRC_DIR%\\Talipot.nsi" "%DEST_DIR%\\"
copy "%SRC_DIR%\\LICENSE" "%DEST_DIR%\\"

echo 'Running NSIS installer generator'
cd "%DEST_DIR%"
set PATH=%NSIS_PATH:"=%;%PATH%
if "%OUT_FILE%" == "" (
  makensis /V4 Talipot.nsi
) else (
  makensis /V4 /DOUT_FILE=%OUT_FILE% Talipot.nsi
)

echo 'Cleaning up files'
del /s /f /q "%DEST_DIR%\\files\\*.*" >nul 2>&1
for /f %%f in ('dir /ad /b "%DEST_DIR%\\files"') do rd /s /q "%DEST_DIR%\\files\\%%f"

echo 'NSIS installer generator completed !'
