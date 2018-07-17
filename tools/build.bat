call setenv.bat
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64

cd %BUILD_DIR%
qmake.exe "%WORK_DIR%\a3dc.pro" -spec win32-msvc "CONFIG+=%BUILD_MODE%" "CONFIG+=qml_debug" ^
	"DEFINED_AT_COMPILATION_A3DC_BUILD_DATE=%DEFINED_AT_COMPILATION_A3DC_BUILD_DATE%"^
	"DEFINED_AT_COMPILATION_A3DC_BUILD_GIT_SHA=%DEFINED_AT_COMPILATION_A3DC_BUILD_GIT_SHA%"^
	"DEFINED_AT_COMPILATION_A3DC_BUILD_MODE=%DEFINED_AT_COMPILATION_A3DC_BUILD_MODE%"^
	"DEFINED_AT_COMPILATION_A3DC_BUILD_PLATFORM=%DEFINED_AT_COMPILATION_A3DC_BUILD_PLATFORM%"^
	&& jom.exe qmake_all
jom.exe && (echo "sucessfull build") || (echo "build failed" & exit /b 1)
