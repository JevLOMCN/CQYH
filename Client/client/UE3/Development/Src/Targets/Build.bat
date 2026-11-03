@ECHO off
REM %1 is the game name
REM %2 is the platform name
REM %3 is the configuration name
REM
REM Runs UnrealBuildTool (the project's C# build driver). The working directory is Development\Src.
REM
REM UnrealBuildTool is not part of the solution's build, and changes to its source (for example the Visual Studio
REM toolchain support) only take effect once it is rebuilt. So it is brought up to date here every time: an
REM incremental MSBuild of the Shipping configuration, which takes about a second when nothing has changed.
REM If MSBuild cannot be found, an already built copy is used instead.

SETLOCAL
REM The Windows editor uses the pinned wxWidgets 3.2 build by default.
REM Build-Client.ps1 prepares the libraries before invoking this script.
IF /I "%2"=="Win32" IF NOT DEFINED UE3_WX32_ROOT FOR %%I IN ("%~dp0..\..\Intermediate\wxWidgets-3.2.12") DO SET "UE3_WX32_ROOT=%%~fI"
IF /I "%2"=="Win64" IF NOT DEFINED UE3_WX32_ROOT FOR %%I IN ("%~dp0..\..\Intermediate\wxWidgets-3.2.12") DO SET "UE3_WX32_ROOT=%%~fI"
SET "UBT_DIR=..\Intermediate\UnrealBuildTool"
SET "UBT=%UBT_DIR%\Shipping\UnrealBuildTool.exe"

CALL :find_msbuild
IF NOT DEFINED MSB GOTO no_msbuild

"%MSB%" UnrealBuildTool\UnrealBuildTool.csproj /nologo /verbosity:quiet /property:Configuration=Shipping
IF ERRORLEVEL 1 (
	ECHO Building UnrealBuildTool failed.
	EXIT /B 999
)
IF NOT EXIST "%UBT%" (
	ECHO UnrealBuildTool.exe was not produced in %UBT_DIR%\Shipping.
	EXIT /B 999
)
GOTO run

:no_msbuild
ECHO MSBuild was not found; using an already built UnrealBuildTool if there is one.
SET "UBT="
FOR %%C IN (Shipping Release ShippingDebugConsole Debug) DO IF NOT DEFINED UBT IF EXIST "%UBT_DIR%\%%C\UnrealBuildTool.exe" SET "UBT=%UBT_DIR%\%%C\UnrealBuildTool.exe"
IF NOT DEFINED UBT (
	ECHO UnrealBuildTool.exe not found in %UBT_DIR%. Build the UnrealBuildTool project in UE3.sln ^(configuration Shipping^) and try again.
	EXIT /B 999
)

:run
"%UBT%" %*
EXIT /B %ERRORLEVEL%

REM ---------------------------------------------------------------------------------------------
REM Sets MSB to the MSBuild.exe of Visual Studio 2022 (or, failing that, the newest Visual Studio).
:find_msbuild
SET "MSB="
IF DEFINED VSINSTALLDIR IF EXIST "%VSINSTALLDIR%MSBuild\Current\Bin\MSBuild.exe" SET "MSB=%VSINSTALLDIR%MSBuild\Current\Bin\MSBuild.exe"
IF DEFINED MSB GOTO :EOF
SET "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
IF NOT EXIST "%VSWHERE%" GOTO :EOF
SET "MSBTMP=%TEMP%\ue3_msbuild_path_%RANDOM%.txt"
"%VSWHERE%" -products * -version "[17.0,18.0)" -requires Microsoft.Component.MSBuild -find MSBuild\Current\Bin\MSBuild.exe > "%MSBTMP%" 2>nul
SET /P MSB=< "%MSBTMP%"
IF DEFINED MSB GOTO cleanup_msbtmp
"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\Current\Bin\MSBuild.exe > "%MSBTMP%" 2>nul
SET /P MSB=< "%MSBTMP%"
:cleanup_msbtmp
IF EXIST "%MSBTMP%" ERASE "%MSBTMP%" >nul 2>&1
GOTO :EOF
