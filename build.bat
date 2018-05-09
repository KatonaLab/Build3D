set BUILD_DIR="C:\Users\balint\projects\A3DC\build"
set QT_DIR="C:\Qt\5.10.1\msvc2017_64"
set QT_CREATOR_DIR="C:\Qt\Tools\QtCreator"
set VCREDIST_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.13.26020\vcredist_x64.exe"

set SRC_DIR="C:\Users\balint\projects\A3DC"
set PATH=%QT_DIR%\bin;%QT_CREATOR_DIR%\bin;%PATH%

set BUILD_MODE=debug

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
cd %SRC_DIR%

rem set QMAKEFLAGS=-d
rem -platform win32-msvc2015 -tp vs

cd %BUILD_DIR%
qmake.exe %SRC_DIR%\a3dc.pro -spec win32-msvc "CONFIG+=%BUILD_MODE%" "CONFIG+=qml_debug"  && jom.exe qmake_all
jom.exe

cd ..

windeployqt %BUILD_DIR%\src\app\%BUILD_MODE%\app.exe -qmldir=%BUILD_DIR%\..\src\app\qml\ -verbose=1
copy %VCREDIST_PATH% %BUILD_DIR%\src\app\%BUILD_MODE%\

iscc win-install.iss