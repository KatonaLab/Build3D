call setenv.bat

rem windeployqt requires normalized paths

pushd %BUILD_DIR%\src\app\%BUILD_MODE%
set ABS_APP_PATH=%cd%\a3-dc.exe
popd

pushd %WORK_DIR%\src\app\qml\
set ABS_QML_PATH=%cd%
popd

windeployqt -verbose=1 "%ABS_APP_PATH%" -qmldir="%ABS_QML_PATH%" ^
	&& (echo "sucessfull windeployqt") || (echo "windeployqt failed" & pause & exit /b 3)

mkdir "%BUILD_DIR%\src\app\%BUILD_MODE%\%PYTHON_DIR_NAME%"
pushd "%BUILD_DIR%\src\app\%BUILD_MODE%\%PYTHON_DIR_NAME%"
set ABS_PYTHON_DST=%cd%
popd

mkdir "%BUILD_DIR%\src\app\%BUILD_MODE%\modules"
pushd "%BUILD_DIR%\src\app\%BUILD_MODE%\modules"
set ABS_MODULES_DST=%cd%
popd

copy "%VCREDIST_PATH%" "%BUILD_DIR%\src\app\%BUILD_MODE%\"
robocopy "%PYTHON_DIR_FULL_PATH%" "%ABS_PYTHON_DST%" /s /e /mt /nfl
robocopy "%SCRIPT_DIR%" "%ABS_MODULES_DST%" /s /e /mt /nfl
copy "%WORK_DIR%\a3-dc.bat" "%BUILD_DIR%\src\app\%BUILD_MODE%\"

echo %DEFINED_AT_COMPILATION_A3DC_BUILD_GIT_SHA%
echo "%DEFINED_AT_COMPILATION_A3DC_BUILD_GIT_SHA%-%DEFINED_AT_COMPILATION_A3DC_BUILD_DATE%"

if "%BUILD_ID%"=="" set BUILD_ID="manual-build"

iscc /Qp ^
	/DAPP_VERSION="%DEFINED_AT_COMPILATION_A3DC_BUILD_GIT_SHA%-%DEFINED_AT_COMPILATION_A3DC_BUILD_DATE%" ^
	/DBUILD_ID="%BUILD_ID%" ^
	/DBUILD_MODE="%BUILD_MODE%" ^
	win-install.iss && (echo "sucessfull deployment") || (echo "deployment failed" & pause & exit /b 4)

pause