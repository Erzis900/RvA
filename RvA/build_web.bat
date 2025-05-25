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
