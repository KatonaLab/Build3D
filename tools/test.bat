call setenv.bat

xcopy /s /e /y "%TEST_ASSETS%" "%BUILD_DIR%\src\test\%BUILD_MODE%\assets\"
xcopy "%PYTHON_DIR_FULL_PATH%" "%BUILD_DIR%\src\test\%BUILD_MODE%\%PYTHON_DIR_NAME%" /s /e /y /i

cd "%BUILD_DIR%\src\test\%BUILD_MODE%\"
set PATH=python-3.6.5.amd64;%PATH%
set PYTHONHOME=python-3.6.5.amd64

call "test.exe" && (echo "sucessfull test") || (echo "test failed" & exit /b 2)
