param(
    [int]$TimeoutSeconds = 1800,
    [string[]]$Map,
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug',
    [ValidateRange(1, 64)]
    [int]$LocalProcessorCount = 4,
    [string]$EditorExecutable,
    [ValidatePattern('^[A-Za-z0-9_]+$')]
    [string[]]$LightingLevel
)

$ErrorActionPreference = 'Stop'
if ($LightingLevel -and (!$Map -or $Map.Count -ne 1)) {
    throw 'LightingLevel requires exactly one persistent map in -Map.'
}
$engine = Join-Path $PSScriptRoot 'client/UE3'
if (Get-Process -Name 'MMOGame*' -ErrorAction SilentlyContinue | Where-Object { !$_.HasExited }) {
    throw 'Save your work and close the editor/client before rebuilding map data.'
}
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
# A persistent-map build also saves its loaded streaming levels. Preserve all
# sibling map files, as well as the shared display/login maps listed above.
$contentRoot = [IO.Path]::GetFullPath((Join-Path $engine 'MMOGame/Content'))
foreach ($selectedMap in $maps) {
    $selectedPath = [IO.Path]::GetFullPath((Join-Path $contentRoot $selectedMap))
    if (!$selectedPath.StartsWith($contentRoot + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Map must be inside MMOGame/Content: $selectedMap"
    }
    if (!(Test-Path -LiteralPath $selectedPath -PathType Leaf)) { throw "Map not found: $selectedPath" }
    $backupMaps += @(Get-ChildItem -LiteralPath (Split-Path $selectedPath) -Filter '*.udk' -File -Recurse | ForEach-Object {
        $_.FullName.Substring($contentRoot.Length + 1)
    })
}
$backupMaps = @($backupMaps | ForEach-Object {
    [IO.Path]::GetFullPath((Join-Path $contentRoot $_)).Substring($contentRoot.Length + 1)
} | Select-Object -Unique)
$executableName = if ($Configuration -eq 'Release') { 'MMOGame.exe' } else { 'MMOGame-Win64-Debug.exe' }
$executable = Join-Path $engine "Binaries/Win64/$executableName"
if ($EditorExecutable) { $executable = (Resolve-Path -LiteralPath $EditorExecutable).Path }
if (!(Test-Path -LiteralPath $executable)) { throw "Build Win64 $Configuration with Build-Client.ps1 first." }
$lightmassPath = Join-Path $engine 'Binaries/Win64/UnrealLightmass.exe'
if (!(Test-Path -LiteralPath $lightmassPath)) { throw 'Build UnrealLightmass.sln in Release|Win64 first.' }
$lightmassBytes = [IO.File]::ReadAllBytes($lightmassPath)
$peOffset = if ($lightmassBytes.Length -ge 64) { [BitConverter]::ToInt32($lightmassBytes, 60) } else { -1 }
if ($peOffset -lt 0 -or $peOffset -gt ($lightmassBytes.Length - 6) -or
    [BitConverter]::ToUInt32($lightmassBytes, $peOffset) -ne 0x4550 -or
    [BitConverter]::ToUInt16($lightmassBytes, $peOffset + 4) -ne 0x8664) {
    throw 'Win64/UnrealLightmass.exe is not an x64 executable. Build UnrealLightmass.sln in Release|Win64 first; a 32-bit worker can exhaust its 4 GB address space on large maps.'
}
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
    $developer.SettableDeveloperOptions.LocalJobsDefaultProcessorCount = [string]$LocalProcessorCount
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
    if ($LightingLevel) { $start.Arguments += " -LightingLevels=" + ($LightingLevel -join '+') }
    Write-Host "Rebuilding $map (log: $log)"
    $process = [Diagnostics.Process]::Start($start)
    $process.PriorityClass = [Diagnostics.ProcessPriorityClass]::BelowNormal
    if (!$process.WaitForExit($TimeoutSeconds * 1000)) {
        try { $process.Kill() } catch { Write-Warning "Could not terminate timed-out editor: $_" }
        $null = $process.WaitForExit(5000)
        throw "Map build timed out. Backups: $run"
    }
    if ($process.ExitCode -ne 0 -or !(Test-Path $log) -or
        [IO.File]::ReadAllText($log) -notmatch 'RebuildMapData: SUCCESS') {
        throw "Map build failed. Review $log. Original maps are backed up under $run."
    }
    $reloadLog = Join-Path $run (([IO.Path]::GetFileNameWithoutExtension($map)) + '-Reload.log')
    $start.Arguments = "editor `"$path`" -VerifyMapLightingAfterLoad -TraceLightingInvalidation -dx9 -NoSplash -NoGADWarning -unattended -nowrite -FORCELOGFLUSH ABSLOG=`"$reloadLog`""
    Write-Host "Verifying rendered reload of $map (log: $reloadLog)"
    $reload = [Diagnostics.Process]::Start($start)
    if (!$reload.WaitForExit(120000)) {
        try { $reload.Kill() } catch { Write-Warning "Could not terminate reload verification: $_" }
        $null = $reload.WaitForExit(5000)
        throw "Rendered reload timed out. Review $reloadLog."
    }
    if ($reload.ExitCode -ne 0 -or !(Test-Path -LiteralPath $reloadLog) -or
        [IO.File]::ReadAllText($reloadLog) -notmatch 'VerifyMapLightingAfterLoad: SUCCESS') {
        throw "Saved map failed rendered lighting verification. Review $reloadLog."
    }
  }
} finally {
    if (!$worker.HasExited) {
        try { $worker.Kill() } catch { Write-Warning "Could not terminate the owned Swarm agent: $_" }
        $null = $worker.WaitForExit(5000)
    }
}
Write-Host "Map lighting and paths rebuilt. Backups and logs: $run"
