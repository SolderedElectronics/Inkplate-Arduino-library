@echo off
setlocal
rem make_font.bat - Windows counterpart to make_font.sh.
rem
rem Creates a throwaway virtualenv next to this script (nothing gets
rem installed into your system/global Python), installs the one dependency
rem (freetype-py) into it if it isn't already there, then runs make_font.py
rem with whatever arguments you passed. Safe to re-run - it reuses the venv
rem and skips reinstalling if freetype-py is already present.
rem
rem Usage (same arguments as make_font.py itself):
rem   make_font.bat C:\path\to\Font.ttf ae oe ue ss --size 18
rem   (accented characters work fine as arguments in a normal Windows
rem   terminal/PowerShell/cmd - just make sure the console is UTF-8, e.g.
rem   `chcp 65001` in cmd, if they show up garbled)

set "SCRIPT_DIR=%~dp0"
set "VENV_DIR=%SCRIPT_DIR%.venv"

where python >nul 2>nul
if errorlevel 1 (
    echo python not found on PATH - install Python 3 first. 1>&2
    exit /b 1
)

if not exist "%VENV_DIR%\Scripts\python.exe" (
    echo Setting up a local virtualenv at %VENV_DIR% ^(one-time^)...
    python -m venv "%VENV_DIR%"
    if errorlevel 1 exit /b 1
)

"%VENV_DIR%\Scripts\python.exe" -c "import freetype" >nul 2>nul
if errorlevel 1 (
    echo Installing freetype-py into the venv ^(one-time^)...
    "%VENV_DIR%\Scripts\pip.exe" install --quiet --upgrade pip freetype-py
    if errorlevel 1 exit /b 1
)

"%VENV_DIR%\Scripts\python.exe" "%SCRIPT_DIR%make_font.py" %*
