param(
    [ValidateSet('Win32', 'Win64')]
    [string[]]$Platform = @('Win32', 'Win64'),
    [ValidateSet('Debug', 'Release')]
    [string[]]$Configuration = @('Debug', 'Release')
)

$ErrorActionPreference = 'Stop'
$engine = Join-Path $PSScriptRoot 'client/UE3'
& (Join-Path $PSScriptRoot 'Build-WxWidgets32.ps1') -Platform $Platform -Configuration $Configuration

# The editor links against these versioned DLLs; preserve any existing 2.8
# files used by older binaries and deploy only the requested 3.2 runtimes.
foreach ($architecture in $Platform) {
    $libPath = if ($architecture -eq 'Win64') { 'Development/Intermediate/wxWidgets-3.2.12/lib/vc_x64_dll' } else { 'Development/Intermediate/wxWidgets-3.2.12/lib/vc_dll' }
    $lib = Join-Path $engine $libPath
    $runtime = Join-Path $engine "Binaries/$architecture"
    foreach ($build in $Configuration) {
        $version = if ($build -eq 'Debug') { '3212ud_' } else { '3212u_' }
        $dlls = @(Get-ChildItem -LiteralPath $lib -Filter '*.dll' -File | Where-Object { $_.Name.StartsWith('wx') -and $_.Name.Contains($version) })
        if (!$dlls.Count) { throw "No wxWidgets DLLs found for $architecture $build in $lib" }
        $dlls | Copy-Item -Destination $runtime -Force
    }
}
