@echo off
REM Build Web Project Batch Script

SET BUILD_DIR=build_web

REM Create or clean the build directory
IF EXIST "%BUILD_DIR%" (
    echo Cleaning %BUILD_DIR%...
    rd /s /q "%BUILD_DIR%"
)
mkdir "%BUILD_DIR%"

REM Enter the build directory
pushd "%BUILD_DIR%"

REM Configure with emcmake (use CALL so control returns)
echo Running: emcmake cmake -S .. -B .
call emcmake cmake -S .. -B .
IF ERRORLEVEL 1 (
    echo Configuration failed.
    popd
    exit /b 1
)

REM Build with emmake (also use CALL)
echo Running: emmake cmake --build .
call emmake cmake --build .
IF ERRORLEVEL 1 (
    echo Build failed.
    popd
    exit /b 1
)

REM Return to original directory
popd

echo Build complete.

REM ——— Config ———
set "BUILD_DIR=build_web"
set "RELEASE_DIR=release"
set "ZIP_NAME=robots_vs_aliens.zip"

REM ——— Make sure the release folder exists ———
if not exist "%RELEASE_DIR%" mkdir "%RELEASE_DIR%"

REM ——— Copy & rename ———
copy "%BUILD_DIR%\robots_vs_aliens.data"     "%RELEASE_DIR%" >nul
copy "%BUILD_DIR%\robots_vs_aliens.html"     "%RELEASE_DIR%\index.html" >nul
copy "%BUILD_DIR%\robots_vs_aliens.js"       "%RELEASE_DIR%" >nul
copy "%BUILD_DIR%\robots_vs_aliens.wasm"     "%RELEASE_DIR%" >nul

REM ——— Remove any old zip ———
if exist "%RELEASE_DIR%\%ZIP_NAME%" del /f "%RELEASE_DIR%\%ZIP_NAME%"

REM ——— Zip it up with tar ———
tar -a -c -f "%RELEASE_DIR%\%ZIP_NAME%" -C "%RELEASE_DIR%" robots_vs_aliens.data index.html robots_vs_aliens.js robots_vs_aliens.wasm

if exist "%RELEASE_DIR%\%ZIP_NAME%" (
  echo Release package created at %RELEASE_DIR%\%ZIP_NAME%
) else (
  echo FAILED to create %ZIP_NAME%
)

endlocal