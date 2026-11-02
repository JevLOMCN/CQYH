param(
    [ValidateSet('Win32', 'Win64')]
    [string[]]$Platform = @('Win32', 'Win64'),
    [ValidateSet('Debug', 'Release')]
    [string[]]$Configuration = @('Debug', 'Release'),
    [switch]$SkipScripts
)

$ErrorActionPreference = 'Stop'
$source = Join-Path $PSScriptRoot 'client/UE3/Development/Src'
$logDirectory = Join-Path $PSScriptRoot 'client/UE3/Development/Intermediate/BuildLogs'
New-Item -ItemType Directory -Force $logDirectory | Out-Null
& (Join-Path $PSScriptRoot 'Build-WxWidgets.ps1') -Platform $Platform -Configuration $Configuration
for ($pass = 0; $pass -lt 2; $pass++) {
    $failed = @()
    Push-Location $source
    try {
        foreach ($targetPlatform in $Platform) {
            foreach ($targetConfiguration in $Configuration) {
                $label = "$targetPlatform-$targetConfiguration"
                $log = Join-Path $logDirectory "$label.log"
                Write-Host "Building $label (log: $log)"
                & cmd.exe /d /c "Targets\Build.bat MMOGame $targetPlatform $targetConfiguration -NoXGE > `"$log`" 2>&1"
                if ($LASTEXITCODE -ne 0) {
                    $failed += $label
                    Get-Content $log -Tail 20 | Out-Host
                } else {
                    Write-Host "$label succeeded"
                }
            }
        }
    } finally {
        Pop-Location
    }
    if ($failed.Count) {
        throw "Client builds failed: $($failed -join ', '). See $logDirectory."
    }
    if ($SkipScripts) { break }

    # Use an executable from this build matrix, then regenerate its matching
    # script packages. Old prebuilt packages can have incompatible native layouts.
    $compilerConfiguration = if ($Configuration -contains 'Release') { 'Release' } else { 'Debug' }
    $compilerName = if ($compilerConfiguration -eq 'Release') { 'MMOGame.exe' } else { "MMOGame-$($Platform[0])-Debug.exe" }
    $compilerDirectory = Join-Path $PSScriptRoot "client/UE3/Binaries/$($Platform[0])"
    $scriptLog = Join-Path $logDirectory 'Scripts.log'
    $start = New-Object Diagnostics.ProcessStartInfo
    $start.FileName = Join-Path $compilerDirectory $compilerName
    $start.WorkingDirectory = $compilerDirectory
    $start.UseShellExecute = $false
    $start.Arguments = "make -auto -unattended -AllowScriptWarnings -NoPause -NoGADWarning ABSLOG=`"$scriptLog`""
    Write-Host "Compiling matching script packages (log: $scriptLog)"
    $compiler = [Diagnostics.Process]::Start($start)
    $compiler.WaitForExit()
    if ($compiler.ExitCode -eq 0) { break }
    if ($pass -eq 0 -and (Get-Content $scriptLog -Raw) -match 'NativeRebuildRequired') {
        Write-Host 'Native declarations changed; rebuilding the selected configurations before retrying scripts.'
        continue
    }
    Get-Content $scriptLog -Tail 20 | Out-Host
    throw "Client script compilation failed. See $scriptLog."
}
