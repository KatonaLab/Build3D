set BUILD_MODE=release

set WORK_DIR=%~dp0\..\
set BUILD_DIR=%WORK_DIR%\build

set QT_DIR=C:\Qt\5.10.1\msvc2017_64
set QT_CREATOR_DIR=C:\Qt\Tools\QtCreator

set PYTHON_DIR_NAME=python-3.6.5.amd64
set PYTHON_DIR_FULL_PATH=C:\WinPython36\%PYTHON_DIR_NAME%

set CRASHPAD_DIR=C:\Users\balint\projects\tools\crashpad\crashpad

set VCREDIST_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.13.26020\vcredist_x64.exe

set PATH=%QT_DIR%\bin;%QT_CREATOR_DIR%\bin;%PATH%
set SCRIPT_DIR=%WORK_DIR%\src\app\modules

set TEST_ASSETS=C:\Users\balint\projects\a3dc-assets\test-assets

set GIT_CMD=git describe --dirty --always --tags

set DEFINED_AT_COMPILATION_A3DC_BUILD_DATE=%DATE:~10,4%.%DATE:~4,2%.%DATE:~7,2%.%TIME:~0,2%.%TIME:~3,2%
for /F "tokens=*" %%i in ('%GIT_CMD%') do set DEFINED_AT_COMPILATION_A3DC_BUILD_GIT_SHA=%%i
set DEFINED_AT_COMPILATION_A3DC_BUILD_MODE=%BUILD_MODE%
for /F "tokens=*" %%i in ('ver') do set DEFINED_AT_COMPILATION_A3DC_BUILD_PLATFORM=%%i
