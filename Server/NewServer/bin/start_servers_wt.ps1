# Starts the debug servers as tabs of one Windows Terminal window (wt.exe).
# NewServer\bin is the working directory (servers load global_config.lua and globalconfig\ from it).
# Same start order as start_servers_d.bat; the guard server runs from _GUARD with the "cmd" argument.
# Usage: powershell -ExecutionPolicy Bypass -File start_servers_wt.ps1 [-DryRun]
param([switch]$DryRun)

$bin = $PSScriptRoot
# Prefer the portable Windows Terminal bundled at Server\terminal-1.25.2733.0, else the installed one.
$bundled = Join-Path $bin '..\..\terminal-1.25.2733.0\wt.exe'
$wtPath = if (Test-Path $bundled) { (Resolve-Path $bundled).Path } else { (Get-Command wt.exe -ErrorAction SilentlyContinue).Source }
if (-not $wtPath) { throw 'wt.exe not found in Server\terminal-1.25.2733.0 or on PATH; use start_servers_d.bat instead.' }
$wt = [pscustomobject]@{ Source = $wtPath }

$tabs = @("new-tab --title guardserver -d `"$bin\_GUARD`" cmd /k .\guardserver64_d.exe cmd")
$delay = 2
foreach ($name in 'dbserver', 'manageserver', 'datalogserver', 'gameserver', 'loginserver', 'balanceserver', 'gateserver') {
    $exe = "$name\${name}64_d.exe"
    if (-not (Test-Path "$bin\$exe")) { Write-Warning "Missing $exe"; continue }
    $delay += 2
    # The delay keeps the start order of start_servers_d.bat (each server waits for the ones before it).
    $tabs += "new-tab --title $name -d `"$bin`" cmd /k `"timeout /t $delay /nobreak >nul & $exe`""
}
$arguments = '-w cqyh-servers ' + ($tabs -join ' ; ')

if ($DryRun) { "wt $arguments"; return }
Start-Process -FilePath $wt.Source -ArgumentList $arguments
