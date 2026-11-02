param(
    [int]$TimeoutSeconds = 1800,
    [string[]]$Map
)

$ErrorActionPreference = 'Stop'
$engine = Join-Path $PSScriptRoot 'client/UE3'
$run = Join-Path $engine ('Development/Intermediate/MapRebuild/' + (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Force $run | Out-Null
# Build shared levels first, then the login scene, then its persistent map.
$maps = @(
    'Maps/_MMOCommon/HalfShowLevel.udk',
    'Maps/_MMOCommon/PawnShowLevel_Handbook.udk',
    'Maps/_MMOMaps/Mir2/login_new/Login_n_MS.udk',
    'Maps/_MMOMaps/Mir2/login_new/Login_n.udk'
)
$backupMaps = @($maps + $Map | Where-Object { $_ } | Select-Object -Unique)
if ($Map) { $maps = $Map }
$executable = Join-Path $engine 'Binaries/Win64/MMOGame-Win64-Debug.exe'
if (!(Test-Path -LiteralPath $executable)) { throw 'Build Win64 Debug with Build-Client.ps1 first.' }
# Swarm must be closed when changing these options. Never terminate a user's
# existing worker: it may be processing another map.
if (Get-Process -Name SwarmAgent -ErrorAction SilentlyContinue) { throw 'Close SwarmAgent before starting this local rebuild.' }
$optionsPath = Join-Path $engine 'Binaries/SwarmAgent.Options.xml'
if (Test-Path -LiteralPath $optionsPath) {
    Copy-Item -LiteralPath $optionsPath -Destination (Join-Path $run 'SwarmAgent.Options.xml')
    [xml]$options = Get-Content -LiteralPath $optionsPath -Raw
} else {
    [xml]$options = '<SettableOptions />'
}
foreach ($setting in @(@('EnableStandaloneMode', 'true'), @('CoordinatorRemotingHost', 'localhost'), @('AllowedRemoteAgentNames', ''), @('CacheFolder', (Join-Path $engine 'Development/Intermediate/SwarmCache')))) {
    $node = $options.DocumentElement.SelectSingleNode($setting[0])
    if (!$node) { $node = $options.CreateElement($setting[0]); $null = $options.DocumentElement.AppendChild($node) }
    $node.InnerText = $setting[1]
}
if (Test-Path -LiteralPath $optionsPath) { (Get-Item -LiteralPath $optionsPath).IsReadOnly = $false }
$options.Save($optionsPath)
$developerPath = Join-Path $engine 'Binaries/SwarmAgent.DeveloperOptions.xml'
if (Test-Path -LiteralPath $developerPath) {
    Copy-Item -LiteralPath $developerPath -Destination (Join-Path $run 'SwarmAgent.DeveloperOptions.xml')
    [xml]$developer = Get-Content -LiteralPath $developerPath -Raw
    $developer.SettableDeveloperOptions.LocalJobsDefaultProcessorCount = [string][Environment]::ProcessorCount
    (Get-Item -LiteralPath $developerPath).IsReadOnly = $false
    $developer.Save($developerPath)
}
foreach ($map in $backupMaps) {
    $path = Join-Path $engine "MMOGame/Content/$map"
    $backup = Join-Path $run "OriginalMaps/$map"
    New-Item -ItemType Directory -Force (Split-Path $backup) | Out-Null
    Copy-Item -LiteralPath $path -Destination $backup
    (Get-Item -LiteralPath $path).IsReadOnly = $false
}
$workerStart = [Diagnostics.ProcessStartInfo]::new()
$workerStart.FileName = Join-Path $engine 'Binaries/SwarmAgent.exe'
$workerStart.WorkingDirectory = Join-Path $engine 'Binaries'
$workerStart.UseShellExecute = $false
$workerStart.WindowStyle = [Diagnostics.ProcessWindowStyle]::Hidden
# The helper owns the agent for the whole batch. An agent owned by one editor
# can exit while the next editor is connecting to it.
$worker = [Diagnostics.Process]::Start($workerStart)
try {
  foreach ($map in $maps) {
    $path = Join-Path $engine "MMOGame/Content/$map"
    $log = Join-Path $run (([IO.Path]::GetFileNameWithoutExtension($map)) + '.log')
    $start = [Diagnostics.ProcessStartInfo]::new()
    $start.FileName = $executable
    $start.WorkingDirectory = Split-Path $executable
    $start.UseShellExecute = $false
    $start.Arguments = "editor `"$path`" -RebuildMapData -dx9 -NoSplash -NoGADWarning -unattended -FORCELOGFLUSH ABSLOG=`"$log`""
    Write-Host "Rebuilding $map (log: $log)"
    $process = [Diagnostics.Process]::Start($start)
    if (!$process.WaitForExit($TimeoutSeconds * 1000)) {
        $process.Kill()
        $process.WaitForExit()
        throw "Map build timed out. Backups: $run"
    }
    if ($process.ExitCode -ne 0 -or !(Test-Path $log) -or
        [IO.File]::ReadAllText($log) -notmatch 'RebuildMapData: SUCCESS') {
        throw "Map build failed. Review $log. Original maps are backed up under $run."
    }
  }
} finally {
    if (!$worker.HasExited) { $worker.Kill(); $worker.WaitForExit() }
}
Write-Host "Map lighting and paths rebuilt. Backups and logs: $run"
