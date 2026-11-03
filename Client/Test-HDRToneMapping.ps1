param([string]$ShaderDirectory)
$ErrorActionPreference = 'Stop'
$engine = Join-Path $PSScriptRoot 'client/UE3'
if (!$ShaderDirectory) { $ShaderDirectory = Join-Path $engine 'Engine/Shaders' }
$ShaderDirectory = (Resolve-Path -LiteralPath $ShaderDirectory).Path
$output = Join-Path $engine 'Development/Intermediate/HDRToneMappingTests'
New-Item -ItemType Directory -Force $output | Out-Null
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$vs = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (!$vs) { throw 'Install Visual Studio C++ tools first.' }
$vcvars = Join-Path $vs 'VC/Auxiliary/Build/vcvars64.bat'
$directX = $env:DXSDK_DIR
if (!$directX) { $directX = Join-Path ${env:ProgramFiles(x86)} 'Microsoft DirectX SDK (June 2010)' }
if (!(Test-Path -LiteralPath (Join-Path $directX 'Include/d3dx9.h'))) { throw 'Install the June 2010 DirectX SDK first.' }
$source = Join-Path $PSScriptRoot 'tests/HDRToneMapping.cpp'
$executable = Join-Path $output 'HDRToneMappingTests.exe'
$batch = Join-Path $output 'Build.cmd'
$buildLog = Join-Path $output 'Build.log'
$commands = @"
@echo off
call "$vcvars" >nul
if errorlevel 1 exit /b %errorlevel%
cl /nologo /std:c++17 /O2 /fp:precise /EHsc /I "$directX/Include" "$source" /Fo"$output/HDRToneMapping.obj" /Fe"$executable" /link /LIBPATH:"$directX/Lib/x64" d3d9.lib d3dx9.lib user32.lib
exit /b %errorlevel%
"@
[IO.File]::WriteAllText($batch,$commands,[Text.Encoding]::ASCII)
& cmd.exe /d /c "`"$batch`" > `"$buildLog`" 2>&1"
if ($LASTEXITCODE -ne 0) { Get-Content -LiteralPath $buildLog -Tail 20; throw 'HDR shader test build failed.' }
# Compile the production HLSL functions and execute them on a hidden DX9 device.
# This does not start MMOGame, show a window, present frames or modify game settings.
& $executable $ShaderDirectory | Tee-Object -FilePath (Join-Path $output 'Results.log')
if ($LASTEXITCODE -ne 0) { throw 'HDR GPU shader checks failed. See Development/Intermediate/HDRToneMappingTests/Results.log.' }
