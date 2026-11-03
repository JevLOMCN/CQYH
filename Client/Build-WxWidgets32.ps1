param(
    [ValidateSet('Win32', 'Win64')]
    [string[]]$Platform = @('Win32', 'Win64'),
    [ValidateSet('Debug', 'Release')]
    [string[]]$Configuration = @('Debug', 'Release')
)

$ErrorActionPreference = 'Stop'
$engine = Join-Path $PSScriptRoot 'client/UE3'
$root = Join-Path $engine 'Development/Intermediate/wxWidgets-3.2.12'
$expectedCommit = '3b7c7fc839093ad28146bd17555af2565c792c96'
if (!(Test-Path -LiteralPath (Join-Path $root '.git'))) {
    if (Test-Path -LiteralPath $root) { throw "Not a wxWidgets Git checkout: $root" }
    git clone --depth 1 --branch v3.2.12 https://github.com/wxWidgets/wxWidgets.git $root
    if ($LASTEXITCODE -ne 0) { throw 'wxWidgets download failed.' }
}
$commit = git -C $root rev-parse HEAD
if ($LASTEXITCODE -ne 0 -or $commit -ne $expectedCommit) { throw "Unexpected wxWidgets revision at $root" }
git -C $root submodule update --init --recursive --depth 1
if ($LASTEXITCODE -ne 0) { throw 'wxWidgets dependency download failed.' }

# Preserve UE3's native-window and window-procedure exception callbacks.
$patch = Join-Path $PSScriptRoot 'wxWidgets32-UE3.patch'
& git -C $root apply --reverse --check $patch 2>$null
if ($LASTEXITCODE -ne 0) {
    & git -C $root apply --check $patch
    if ($LASTEXITCODE -ne 0) { throw 'wxWidgets UE3 integration patch cannot be applied cleanly.' }
    & git -C $root apply $patch
    if ($LASTEXITCODE -ne 0) { throw 'wxWidgets UE3 integration patch failed.' }
}

# UE3 relies on APIs that wxWidgets keeps behind this compatibility switch.
# Both the library and every consumer must use the same setting.
$setupFiles = @((Join-Path $root 'include/wx/msw/setup.h'))
$setupFiles += @(Get-ChildItem -LiteralPath (Join-Path $root 'lib') -Filter setup.h -Recurse -File -ErrorAction SilentlyContinue | ForEach-Object FullName)
foreach ($file in $setupFiles) {
    $text = [IO.File]::ReadAllText($file)
    $original = $text
    if ($text -notmatch '#define WXWIN_COMPATIBILITY_2_8 [01]') { throw "Compatibility setting missing in $file" }
    $text = $text -replace '#define WXWIN_COMPATIBILITY_2_8 0', '#define WXWIN_COMPATIBILITY_2_8 1'
    # UE3's FString has both ANSI and wide constructors; the extra implicit
    # ANSI conversion makes otherwise valid Unicode calls ambiguous.
    $text = $text -replace '#define wxUSE_UNSAFE_WXSTRING_CONV 1', '#define wxUSE_UNSAFE_WXSTRING_CONV 0'
    if ($text -ne $original) { [IO.File]::WriteAllText($file, $text, [Text.UTF8Encoding]::new($false)) }
}
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$msbuild = & $vswhere -latest -products '*' -requires Microsoft.Component.MSBuild -find 'MSBuild/Current/Bin/MSBuild.exe' | Select-Object -First 1
if (!$msbuild) { throw 'Visual Studio MSBuild was not found.' }
foreach ($architecture in $Platform) {
    foreach ($build in $Configuration) {
        $msbuildPlatform = if ($architecture -eq 'Win64') { 'x64' } else { 'Win32' }
        $log = Join-Path $root "wx-build-$architecture-$build.log"
        Write-Host "Building wxWidgets 3.2.12 $architecture $build (log: $log)"
        & $msbuild (Join-Path $root 'build/msw/wx_vc17.sln') /nologo '/t:base;net;core;adv;media;html;qa;xml;xrc;aui;richtext' "/p:Configuration=DLL $build" "/p:Platform=$msbuildPlatform" /p:PlatformToolset=v143 /p:WindowsTargetPlatformVersion=10.0 /m:4 /v:minimal *> $log
        if ($LASTEXITCODE -ne 0) { Get-Content -LiteralPath $log -Tail 20 | Out-Host; throw "wxWidgets build failed: $log" }
    }
}
