param(
    [int]$ProcessId = 0,
    [ValidateRange(1, 120)]
    [int]$Seconds = 5,
    [switch]$StartupCapture
)

$ErrorActionPreference = 'Stop'
$identity = [Security.Principal.WindowsIdentity]::GetCurrent()
$principal = New-Object Security.Principal.WindowsPrincipal($identity)
if (!$principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    throw 'Run this capture from an administrator PowerShell. It does not launch or stop the editor.'
}
if ($StartupCapture -and $ProcessId) { throw 'Use either -StartupCapture or -ProcessId.' }
if (!$StartupCapture -and !$ProcessId) {
    $editors = @(Get-Process MMOGame* -ErrorAction SilentlyContinue)
    if ($editors.Count -ne 1) { throw 'Specify -ProcessId for the already-running editor.' }
    $ProcessId = $editors[0].Id
}
if (!$StartupCapture) {
    $editor = Get-Process -Id $ProcessId
    if ($editor.ProcessName -notlike 'MMOGame*') { throw 'The selected process is not MMOGame.' }
}
$xperfCommand = Get-Command xperf.exe -ErrorAction SilentlyContinue
$xperf = if ($xperfCommand) { $xperfCommand.Source } else {
    Join-Path ${env:ProgramFiles(x86)} 'Windows Kits\10\Windows Performance Toolkit\xperf.exe'
}
if (!(Test-Path -LiteralPath $xperf)) { throw 'Windows Performance Toolkit (xperf.exe) is not installed.' }
$output = Join-Path $PSScriptRoot ('client/UE3/Development/Intermediate/EditorStall/' + (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Force $output | Out-Null
if (!$StartupCapture) { [pscustomobject]@{
    ProcessId = $ProcessId
    Started = $editor.StartTime
    Captured = Get-Date
    Threads = @($editor.Threads | ForEach-Object {
        [pscustomobject]@{ Id=$_.Id; State=[string]$_.ThreadState; CPU=$_.TotalProcessorTime.TotalMilliseconds }
    })
} | ConvertTo-Json -Depth 4 | Set-Content (Join-Path $output 'Process.json') }
[pscustomobject]@{ Started=Get-Date; Seconds=$Seconds; StartupCapture=[bool]$StartupCapture } |
    ConvertTo-Json | Set-Content (Join-Path $output 'Capture.json')

# Context-switch stacks are captured by Windows as threads run and block.
# No debugger attachment, thread suspension, editor input, or editor launch.
# If another kernel recording already owns this session, fail and leave it alone.
& $xperf -on PROC_THREAD+LOADER+CSWITCH -stackwalk CSwitch -buffersize 64 -minbuffers 64 -maxbuffers 128 -f (Join-Path $output 'raw.etl')
if ($LASTEXITCODE -ne 0) { throw 'Recording did not start. No existing recording was stopped.' }
try {
    Write-Host "Recording for $Seconds seconds. The trace will save automatically."
    if ($StartupCapture) { Write-Host 'You can now open the editor normally. Do not run it as administrator.' }
    Start-Sleep -Seconds $Seconds
} finally {
    & $xperf -d (Join-Path $output 'Stall.etl')
    if ($LASTEXITCODE -ne 0) { throw 'Saving the trace failed; check xperf recording status.' }
}
Write-Output "Trace saved to $output"
