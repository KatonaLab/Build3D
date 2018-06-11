set BUILD_MODE=release

set BUILD_DIR="C:\Users\balint\projects\A3DC\build"
set QT_DIR="C:\Qt\5.10.1\msvc2017_64"
set QT_CREATOR_DIR="C:\Qt\Tools\QtCreator"
set PYTHON_DIR_NAME="python-3.6.5.amd64"
set PYTHON_DIR_FULL_PATH="C:\WinPython36\%PYTHON_DIR_NAME%"
set VCREDIST_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.13.26020\vcredist_x64.exe"

set SRC_DIR="C:\Users\balint\projects\A3DC"
set PATH=%QT_DIR%\bin;%QT_CREATOR_DIR%\bin;%PATH%
set SCRIPT_DIR="%SRC_DIR%\src\app\modules"

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
cd "%SRC_DIR%"

rem set QMAKEFLAGS=-d
rem -platform win32-msvc2015 -tp vs

cd %BUILD_DIR%
qmake.exe "%SRC_DIR%\a3dc.pro" -spec win32-msvc "CONFIG+=%BUILD_MODE%" "CONFIG+=qml_debug"  && jom.exe qmake_all
jom.exe

pause

cd ..

rem TODO: windeploy should put the files into a subdir and the program should use the dlls from there
rem mkdir %BUILD_DIR%\src\app\%BUILD_MODE%\qt
rem windeployqt %BUILD_DIR%\src\app\%BUILD_MODE%\app.exe -qmldir=%BUILD_DIR%\..\src\app\qml\  --plugindir %BUILD_DIR%\src\app\%BUILD_MODE%\qt --dir %BUILD_DIR%\src\app\%BUILD_MODE%\qt -verbose=1

windeployqt -verbose=1 "%BUILD_DIR%\src\app\%BUILD_MODE%\app.exe" -qmldir="%BUILD_DIR%\..\src\app\qml\"

copy "%VCREDIST_PATH%" "%BUILD_DIR%\src\app\%BUILD_MODE%\"
xcopy "%PYTHON_DIR_FULL_PATH%" "%BUILD_DIR%\src\app\%BUILD_MODE%\%PYTHON_DIR_NAME%" /s /e /y /i
xcopy "%SCRIPT_DIR%" "%BUILD_DIR%\src\app\%BUILD_MODE%\modules" /s /e /y /i
copy "%SRC_DIR%\app-starter.bat" "%BUILD_DIR%\src\app\%BUILD_MODE%\"

set A3DC_COMPILE_VERSION="%BUILD_MODE%"

pause

iscc win-install.iss

pause