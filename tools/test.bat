call setenv.bat

mkdir "%BUILD_DIR%\src\test\%BUILD_MODE%\assets\"
pushd "%BUILD_DIR%\src\test\%BUILD_MODE%\assets\"
set ABS_ASSET_DST=%cd%
popd

mkdir "%ABS_ASSET_DST%"
robocopy "%TEST_ASSETS%" "%ABS_ASSET_DST%" /s /e /mt /nfl
robocopy "%PYTHON_DIR_FULL_PATH%" "%BUILD_DIR%\src\test\%BUILD_MODE%\%PYTHON_DIR_NAME%" /s /e /mt /nfl

cd "%BUILD_DIR%\src\test\%BUILD_MODE%\"
set PATH=python-3.6.5.amd64;%PATH%
set PYTHONHOME=python-3.6.5.amd64

call "test.exe"
@REM && (echo "sucessfull test") || (echo "test failed" & pause & exit /b 2)

@REM pause