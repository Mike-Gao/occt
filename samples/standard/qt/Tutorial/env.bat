@echo off

if exist "%~dp0custom.bat" (
  call "%~dp0custom.bat" %1 %2 %3
)

rem call "%~dp0..\..\..\..\env.bat" %1 %2 %3
call "C:\Work\install-CR31570\env.bat" %1 %2 %3


if ["%ARCH%"] == ["32"] set VCARCH=x86
if ["%ARCH%"] == ["64"] set VCARCH=amd64

if /I ["%1"] == ["vc141"] set "VCVER=vc141"
set "BIN_DIR=win%ARCH%\%VCVER%\bind"
set "LIB_DIR=win%ARCH%\%VCVER%\libd"

if ["%CASDEB%"] == [""] (
  set "BIN_DIR=win%ARCH%\%VCVER%\bin"
  set "LIB_DIR=win%ARCH%\%VCVER%\lib"
)

set "PATH=%~dp0%BIN_DIR%;%PATH%"

if not "%QTDIR%" == "" (
  set "RES_DIR=%~dp0win%ARCH%\%VCVER%\res"

  set "CSF_ResourcesDefaults=!RES_DIR!"
  set "CSF_TutorialResourcesDefaults=!RES_DIR!"
  set "CSF_IEResourcesDefaults=!RES_DIR!"

  set "PATH=%QTDIR%/bin;%PATH%"
  set "QT_QPA_PLATFORM_PLUGIN_PATH=%QTDIR%\plugins\platforms"
)
set "CSF_SampleSources=%~dp0..\..\samples_src"