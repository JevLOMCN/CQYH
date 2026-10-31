@echo off
rem Generates Server\build\vs2022\CQYHServer.sln with the CMake bundled in Visual Studio 2022
rem (or any cmake on PATH), then opens it. Server\build is git-ignored: it holds absolute paths
rem from this machine, so every developer generates it locally.
rem
rem Usage:  GenerateSolution.bat [path to Qt5 SDK] [--no-open]
rem   e.g.  GenerateSolution.bat C:/Qt/5.15.2/msvc2019_64
rem The Qt path is only needed for gmtool / maintaintool (Qt 5.15.2, MSVC 2019 64-bit). It is
rem remembered in the CMake cache, so later runs can leave it out.
setlocal EnableExtensions

set "QT_PREFIX="
set "OPEN_SLN=1"
:parse
if "%~1"=="" goto parsed
if /I "%~1"=="--no-open" (set "OPEN_SLN=0") else (set "QT_PREFIX=%~1")
shift
goto parse
:parsed

set "CMAKE_EXE="
where cmake >nul 2>&1 && set "CMAKE_EXE=cmake"
if not defined CMAKE_EXE (
    set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    if exist "%VSWHERE%" (
        for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.CMake.Project -property installationPath`) do (
            if exist "%%I\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" set "CMAKE_EXE=%%I\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
        )
    )
)
if not defined CMAKE_EXE (
    echo cmake not found. Install Visual Studio 2022 with "C++ CMake tools for Windows", or add cmake to PATH.
    exit /b 1
)

pushd "%~dp0"
if defined QT_PREFIX (
    "%CMAKE_EXE%" --preset vs2022 "-DCMAKE_PREFIX_PATH=%QT_PREFIX%"
) else (
    "%CMAKE_EXE%" --preset vs2022
)
set "RESULT=%ERRORLEVEL%"
popd
if not "%RESULT%"=="0" (
    echo.
    echo CMake failed. If it complains about Qt5, pass your Qt SDK path:  GenerateSolution.bat C:/Qt/5.15.2/msvc2019_64
    exit /b %RESULT%
)

echo.
echo Generated %~dp0build\vs2022\CQYHServer.sln
if "%OPEN_SLN%"=="1" start "" "%~dp0build\vs2022\CQYHServer.sln"
exit /b 0
