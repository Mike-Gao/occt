@echo off
setlocal

if exist "%~dp0cmake_gen_custom.bat" call "%~dp0cmake_gen_custom.bat"

if "%VS%"=="" set VS=14
if "%VSDATA%"=="" set VSDATA=2015
if "%VSPLATFORM%"=="" set VSPLATFORM=Win64
if "%BUILD_DIR%"=="" set BUILD_DIR=build
if "%OCCT3RDPARTY%"=="" set "OCCT3RDPARTY=%~dp0..\3rdparty"
if "%INSTALL_DIR%"=="" set "INSTALL_DIR=%~dp0install"

set BUILD_DOC=OFF
set USE_FPE_SIGNAL=ON
set USE_OCCT_DEBUG=OFF
set USE_FREEIMAGE=OFF
set USE_TBB=ON
set USE_VTK=OFF
set USE_PCH=OFF

if not "%VSPLATFORM%"=="" set "arch_compile=Visual Studio %VS% %VSDATA% %VSPLATFORM%"
if     "%VSPLATFORM%"=="" set "arch_compile=Visual Studio %VS% %VSDATA%"

set "INSTALL_DIR=%INSTALL_DIR:\=/%"
set "OCCT3RDPARTY=%OCCT3RDPARTY:\=/%"

if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%
 
cmake -G "%arch_compile%" -D3RDPARTY_DIR:STRING=%OCCT3RDPARTY% ^
  -DBUILD_DOC_Overview:BOOL=%BUILD_DOC% -DINSTALL_DIR:STRING=%INSTALL_DIR% ^
  -DBUILD_WITH_DEBUG:BOOL=%USE_OCCT_DEBUG% -DBUILD_ENABLE_FPE_SIGNAL_HANDLER:BOOL=%USE_FPE_SIGNAL% ^
  -DUSE_FREEIMAGE:BOOL=%USE_FREEIMAGE% ^
  -DUSE_TBB:BOOL=%USE_TBB% -DUSE_VTK:BOOL=%USE_VTK% -DBUILD_USE_PCH:BOOL=%USE_PCH% ^
  ..

endlocal