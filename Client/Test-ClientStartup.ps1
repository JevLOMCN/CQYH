param(
    [ValidateSet('Win32', 'Win64')]
    [string[]]$Platform = @('Win32', 'Win64'),
    [ValidateSet('Debug', 'Release')]
    [string[]]$Configuration = @('Debug', 'Release'),
    [int]$TimeoutSeconds = 120,
    [int]$StableSeconds = 10,
    [string]$ExecFile,
    [switch]$Visible,
    [string]$ExtraArguments = ''
)

$ErrorActionPreference = 'Stop'
$engine = Join-Path $PSScriptRoot 'client/UE3'
$resultsDirectory = Join-Path $engine 'Development/Intermediate/StartupTests'
New-Item -ItemType Directory -Force $resultsDirectory | Out-Null
$results = @()
$resultsFile = Join-Path $resultsDirectory 'results.json'
$savedResults = if (Test-Path $resultsFile) { @(Get-Content $resultsFile -Raw | ConvertFrom-Json) } else { @() }
foreach ($architecture in $Platform) {
    foreach ($build in $Configuration) {
        $label = "$architecture-$build"
        $directory = Join-Path $engine "Binaries/$architecture"
        $executable = if ($build -eq 'Debug') { "MMOGame-$architecture-Debug.exe" } else { 'MMOGame.exe' }
        $log = Join-Path $resultsDirectory "$label.log"
        if (Test-Path $log) { Move-Item -LiteralPath $log -Destination "$log.previous" -Force }
        $start = New-Object Diagnostics.ProcessStartInfo
        $start.FileName = Join-Path $directory $executable
        $start.WorkingDirectory = $directory
        $start.UseShellExecute = $false
        $start.WindowStyle = if ($Visible) { [Diagnostics.ProcessWindowStyle]::Normal } else { [Diagnostics.ProcessWindowStyle]::Minimized }
        $start.Arguments = "-dx9 -windowed -ResX=1280 -ResY=720 -NoSplash -NoGADWarning -unattended -FORCELOGFLUSH ABSLOG=`"$log`""
        if ($ExtraArguments) { $start.Arguments += " $ExtraArguments" }
        if ($ExecFile) {
            # UE3 prepends its Binaries directory to exec filenames.
            $binaryUri = [Uri]::new((Join-Path $engine 'Binaries') + [IO.Path]::DirectorySeparatorChar)
            $execUri = [Uri]::new((Resolve-Path $ExecFile).Path)
            $relativeExec = [Uri]::UnescapeDataString($binaryUri.MakeRelativeUri($execUri).ToString()).Replace('/', '\')
            $start.Arguments += " EXEC=`"$relativeExec`""
        }
        Write-Host "Testing $label"
        $process = [Diagnostics.Process]::Start($start)
        $timer = [Diagnostics.Stopwatch]::StartNew()
        $readySince = $null
        $passed = $false
        $title = ''
        $reason = 'Startup timed out'
        $shutdownClean = $false
        try {
            while ($timer.Elapsed.TotalSeconds -lt $TimeoutSeconds) {
                if ($process.WaitForExit(1000)) { $reason = "Exited during startup ($($process.ExitCode))"; break }
                $text = ''
                if (Test-Path $log) {
                    $stream = [IO.File]::Open($log, [IO.FileMode]::Open, [IO.FileAccess]::Read, [IO.FileShare]::ReadWrite)
                    $reader = New-Object IO.StreamReader($stream)
                    try { $text = $reader.ReadToEnd() } finally { $reader.Dispose() }
                }
                if ($text -match 'Critical:|=== Critical error:|Assertion failed:') { $reason = 'Fatal startup error in log'; break }
                $process.Refresh()
                if ($text -match 'Initializing Engine Completed' -and $text -match 'finished loading, Login_n_MS' -and $process.MainWindowHandle -ne 0 -and $process.Responding) {
                    if ($null -eq $readySince) { $readySince = $timer.Elapsed.TotalSeconds }
                    if ($timer.Elapsed.TotalSeconds - $readySince -ge $StableSeconds) {
                        $title = $process.MainWindowTitle
                        $passed = $true
                        $reason = 'Login level loaded; window remained responsive'
                        break
                    }
                } else { $readySince = $null }
            }
        } finally {
            if (!$process.HasExited) {
                if (!$passed -or !$process.CloseMainWindow() -or !$process.WaitForExit(60000)) {
                    $process.Kill()
                    $process.WaitForExit()
                    if ($passed) { $reason += '; cleanup required process termination' }
                }
            }
            $shutdownClean = $process.ExitCode -eq 0
        }
        $results += [pscustomobject]@{ Platform=$architecture; Configuration=$build; Passed=$passed; ShutdownClean=$shutdownClean; Reason=$reason; WindowTitle=$title; ExitCode=$process.ExitCode; Seconds=[Math]::Round($timer.Elapsed.TotalSeconds,1); Log=$log }
        $savedResults = @($savedResults | Where-Object { $_.Platform -ne $architecture -or $_.Configuration -ne $build }) + @($results[-1])
        $savedResults | ConvertTo-Json | Set-Content $resultsFile
        Write-Host "$label : $passed - $reason"
        if (!$passed -and (Test-Path $log)) { Get-Content $log -Tail 12 | Out-Host }
    }
}
if ($results.Passed -contains $false) { throw 'One or more client startup tests failed. See Development/Intermediate/StartupTests/results.json.' }
