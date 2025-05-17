@echo off
setlocal

:: Check if filename was passed as first argument
if "%~1"=="" (
    set /p "filename=Enter filename: "
) else (
    set "filename=%~1"
)

set /p "width=Enter Width for each frame: "
set /p "prefix=Enter a Prefix for each frame: "

python split_image.py "%filename%" "%prefix%" %width%
