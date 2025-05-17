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

:: Default cleanup to "n"
set "cleanup=n"

:: Ask user if folder exists
if exist frames_output (
    echo frames_output folder already exists.
    set /p "cleanup=Do you want to delete it and start fresh? (y/n): "
)

python split_image.py "%filename%" "%prefix%" %width% %cleanup%

:: Open the folder after script ends
explorer frames_output

pause