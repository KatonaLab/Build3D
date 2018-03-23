set BUILD_DIR="C:\Users\balint\projects\A3DC\build"
set QT_DIR="C:\Qt\5.10.1\msvc2017_64"
set QT_CREATOR_DIR="C:\Qt\Tools/QtCreator"

set SRC_DIR="C:\Users\balint\projects\A3DC"
set PATH=%QT_DIR%\bin;%QT_CREATOR_DIR%\bin;%PATH%

cd %BUILD_DIR%

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
cd %SRC_DIR%

set QMAKEFLAGS=-d

rem -platform win32-msvc2015 -tp vs

qmake.exe %SRC_DIR%\a3dc.pro -spec win32-msvc "CONFIG+=debug" "CONFIG+=qml_debug" && jom.exe qmake_all
jom.exe

