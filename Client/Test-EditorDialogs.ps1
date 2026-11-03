param(
    [ValidateSet('Win32', 'Win64')]
    [string[]]$Platform = @('Win64'),
    [ValidateSet('Debug', 'Release')]
    [string[]]$Configuration = @('Release'),
    [ValidateSet('NullRHI', 'DX9')]
    [string]$Renderer = 'NullRHI',
    [int]$TimeoutSeconds = 120
)

$ErrorActionPreference = 'Stop'
$engine = Join-Path $PSScriptRoot 'client/UE3'
$output = Join-Path $engine 'Development/Intermediate/EditorDialogTests'
New-Item -ItemType Directory -Force $output | Out-Null
$results = @()
foreach ($architecture in $Platform) {
    foreach ($build in $Configuration) {
        $label = "$architecture-$build-$Renderer"
        $name = if ($build -eq 'Debug') { "MMOGame-$architecture-Debug.exe" } else { 'MMOGame.exe' }
        $directory = Join-Path $engine "Binaries/$architecture"
        $log = Join-Path $output "$label.log"
        if (Test-Path -LiteralPath $log) { Move-Item -LiteralPath $log -Destination "$log.previous" -Force }
        $start = New-Object Diagnostics.ProcessStartInfo
        $start.FileName = Join-Path $directory $name
        $start.WorkingDirectory = $directory
        $start.UseShellExecute = $false
        $start.WindowStyle = [Diagnostics.ProcessWindowStyle]::Minimized
        # Deliberately omit -unattended: it bypasses the dialogs under test.
        $renderArgument = if ($Renderer -eq 'NullRHI') { '-nullrhi' } else { '-dx9' }
        $start.Arguments = "editor $renderArgument -NoSplash -NoGADWarning -LANGUAGE=INT -nowrite -VerifyEditorDialogs -FORCELOGFLUSH ABSLOG=`"$log`""
        Write-Host "Testing editor dialogs $label"
        $process = [Diagnostics.Process]::Start($start)
        try {
            $completed = $process.WaitForExit($TimeoutSeconds * 1000)
        } finally {
            if (!$process.HasExited) { $process.Kill(); $process.WaitForExit() }
        }
        $text = if (Test-Path -LiteralPath $log) { [IO.File]::ReadAllText($log) } else { '' }
        $passed = $completed -and $process.ExitCode -eq 0 -and
            $text -match 'VerifyEditorDialogs: Load Errors native OK and close messages passed' -and
            $text -match 'VerifyEditorDialogs: SUCCESS' -and
            $text -notmatch 'Critical:|Assertion failed:|No localization:'
        $results += [pscustomobject]@{ Platform=$architecture; Configuration=$build; Renderer=$Renderer; Passed=$passed; ExitCode=$process.ExitCode; Log=$log }
        $results | ConvertTo-Json | Set-Content (Join-Path $output 'results.json')
        Write-Host "$label : $passed"
        if (!$passed) { Get-Content -LiteralPath $log -Tail 15 -ErrorAction SilentlyContinue | Out-Host }
    }
}
if ($results.Passed -contains $false) { throw "Editor dialog verification failed. See $output." }
