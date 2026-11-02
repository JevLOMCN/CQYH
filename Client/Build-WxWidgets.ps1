param(
    [ValidateSet('Win32', 'Win64')]
    [string[]]$Platform = @('Win32', 'Win64'),
    [ValidateSet('Debug', 'Release')]
    [string[]]$Configuration = @('Debug', 'Release')
)

$ErrorActionPreference = 'Stop'
$engine = Join-Path $PSScriptRoot 'client/UE3'
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$msbuild = & $vswhere -latest -products '*' -requires Microsoft.Component.MSBuild -find 'MSBuild/Current/Bin/MSBuild.exe' | Select-Object -First 1
if (!$msbuild) { throw 'Visual Studio MSBuild was not found.' }
$intermediate = Join-Path $engine 'Development/Intermediate/WxWidgets'
New-Item -ItemType Directory -Force $intermediate | Out-Null
$solution = Join-Path $engine 'Development/External/wxWidgets/build/msw/wx_dll.sln'

foreach ($architecture in $Platform) {
  foreach ($build in $Configuration) {
    $msbuildPlatform = if ($architecture -eq 'Win64') { 'x64' } else { 'Win32' }
    $runtime = Join-Path $engine "Binaries/$architecture"
    $stampName = if ($build -eq 'Debug') { "$architecture-v143.stamp" } else { "$architecture-Release-v143.stamp" }
    $stamp = Join-Path $intermediate $stampName
    if (!(Test-Path $stamp)) {
        # Assets include old-format, sometimes read-only symbol files. Preserve
        # those and the original DLLs before generating modern replacements.
        $backup = Join-Path $intermediate "OriginalRuntime/$architecture/$build"
        New-Item -ItemType Directory -Force $backup | Out-Null
        $runtimeFiles = Get-ChildItem $runtime -File -Filter 'wxmsw28u*' | Where-Object { ($_.Name -like 'wxmsw28ud*') -eq ($build -eq 'Debug') }
        foreach ($file in $runtimeFiles) {
            Copy-Item -LiteralPath $file.FullName -Destination $backup -Force
            $file.IsReadOnly = $false
            if ($file.Extension -eq '.pdb') { Remove-Item -LiteralPath $file.FullName }
        }
    }
    Get-ChildItem (Join-Path $engine 'Development/External/wxWidgets/lib/vc_dll') -Recurse -File |
        ForEach-Object { $_.IsReadOnly = $false }
    $log = Join-Path $intermediate "$architecture-$build.log"
    Write-Host "Building wxWidgets $build for $architecture (log: $log)"
    & $msbuild $solution /nologo '/t:base;net;xml;core;adv;html;richtext;aui;xrc;qa' "/p:Configuration=DLL Unicode $build" "/p:Platform=$msbuildPlatform" /p:PlatformToolset=v143 /p:WindowsTargetPlatformVersion=10.0 /m:4 /v:minimal *> $log
    if ($LASTEXITCODE -ne 0) {
        Get-Content $log -Tail 20 | Out-Host
        throw "wxWidgets build failed for $architecture."
    }
    Set-Content -LiteralPath $stamp -Value 'v143'
  }
}
