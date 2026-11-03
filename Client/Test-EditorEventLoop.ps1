param([switch]$BuildOnly)

$ErrorActionPreference = 'Stop'
$engine = Join-Path $PSScriptRoot 'client/UE3'
$sourceRoot = Join-Path $engine 'Development/Src'
$wx = Join-Path $engine 'Development/Intermediate/wxWidgets-3.2.12'
$output = Join-Path $engine 'Development/Intermediate/EditorEventLoopTests'
New-Item -ItemType Directory -Force $output | Out-Null

# Compile the production methods and header against wxWidgets, with only the
# engine tick/globals stubbed. This never starts MMOGame or creates a visible window.
$launch = [IO.File]::ReadAllText((Join-Path $sourceRoot 'Launch/Src/LaunchMisc.cpp'))
function Get-MethodBody([string]$signature, [string]$text = $launch) {
    $start = $text.IndexOf($signature, [StringComparison]::Ordinal)
    if ($start -lt 0) { throw "Missing production method: $signature" }
    $brace = $text.IndexOf('{', $start)
    $depth = 1
    $end = $brace + 1
    while ($depth -gt 0 -and $end -lt $text.Length) {
        if ($text[$end] -eq '{') { $depth++ }
        if ($text[$end] -eq '}') { $depth-- }
        $end++
    }
    if ($depth -ne 0) { throw "Unbalanced production method: $signature" }
    return $text.Substring($start, $end - $start)
}
$methods = (Get-MethodBody 'int WxLaunchApp::OnRun()') + "`n" + (Get-MethodBody 'bool WxLaunchApp::ProcessIdle()')
$docking = [IO.File]::ReadAllText((Join-Path $sourceRoot 'UnrealEd/Src/Docking.cpp'))
$forwarder = Get-MethodBody 'void WxFloatingFrame::OnCommand( wxCommandEvent& In )' $docking
$harness = @'
#include <wx/wx.h>
#include <wx/evtloop.h>
#include <wx/thread.h>
#include <cstdio>
#include "LaunchApp.h"

static int GIsSlowTask = 0;
static int GIsRequestingExit = 0;
static int Ticks = 0;
static int Failures = 0;
static bool Reenter = false;
static void Check(bool passed, const char* name)
{
    std::printf("%s: %s\n", passed ? "PASS" : "FAIL", name);
    if (!passed) ++Failures;
}
bool WxLaunchApp::OnInit() { return true; }
int WxLaunchApp::OnExit() { return wxApp::OnExit(); }
void WxLaunchApp::ShutdownAfterError() {}
void WxLaunchApp::TickUnreal()
{
    ++Ticks;
    if (Reenter)
    {
        Reenter = false;
        const int before = Ticks;
        ProcessIdle();
        Check(Ticks == before, "recursive idle does not re-enter engine tick");
    }
}
class WxFloatingFrame : public wxFrame
{
public:
    wxWindow* ChildWindow;
    WxFloatingFrame() : wxFrame(NULL, wxID_ANY, "Hidden probe"), ChildWindow(new wxPanel(this)) {}
    void OnCommand(wxCommandEvent& In);
};
// PRODUCTION_FORWARDER
// PRODUCTION_METHODS

class ProbeTimer : public wxTimer
{
public:
    void Notify() override
    {
        Stop();
        Check(Ticks > 0, "normal event loop ticks engine");
        int before = Ticks;
        wxEventLoopBase::GetActive()->Yield(true);
        Check(Ticks == before, "yield from UI callback does not tick engine");

        before = Ticks;
        wxMutexGuiLeave();
        wxTheApp->ProcessIdle();
        wxMutexGuiLeaveOrEnter();
        Check(Ticks == before, "engine does not run without GUI ownership");

        before = Ticks;
        GIsSlowTask = 1;
        wxTheApp->ProcessIdle();
        GIsSlowTask = 0;
        Check(Ticks == before, "slow task does not tick engine");

        before = Ticks;
        {
            wxEventLoop nested;
            wxEventLoopActivator activate(&nested);
            wxTheApp->ProcessIdle();
        }
        Check(Ticks == before, "nested modal loop does not tick engine");

        {
            WxFloatingFrame frame;
            int forwards = 0;
            frame.Bind(wxEVT_MENU, [&](wxCommandEvent& event) {
                if (++forwards < 4) frame.OnCommand(event);
            });
            wxCommandEvent event(wxEVT_MENU, wxID_HIGHEST + 1);
            frame.ProcessWindowEvent(event);
            wxTheApp->ProcessPendingEvents();
            Check(forwards == 1, "unhandled forwarded menu command does not cycle");
        }
        Reenter = true;
        wxTheApp->ProcessIdle();
        Check(!Reenter, "normal idle resumes engine tick");
        wxTheApp->ExitMainLoop();
    }
};
class ProbeApp : public WxLaunchApp
{
public:
    bool OnInit() override
    {
        SetExitOnFrameDelete(false);
        delete wxLog::SetActiveTarget(new wxLogStderr());
        const int before = Ticks;
        ProcessIdle();
        Check(Ticks == before, "initialization does not tick engine");
        return true;
    }
    int OnRun() override
    {
        ProbeTimer timer;
        timer.Start(50, true);
        return WxLaunchApp::OnRun();
    }
};
wxIMPLEMENT_APP_NO_MAIN(ProbeApp);
int main(int argc, char** argv)
{
    const int result = wxEntry(argc, argv);
    std::printf("Event-loop checks: %d failure(s)\n", Failures);
    return result ? result : (Failures ? 1 : 0);
}
'@
$harness = $harness.Replace('// PRODUCTION_METHODS', $methods).Replace('// PRODUCTION_FORWARDER', $forwarder)
[IO.File]::WriteAllText((Join-Path $output 'EventLoopProbe.cpp'), $harness, [Text.UTF8Encoding]::new($false))
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$vs = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath | Select-Object -First 1
if (!$vs) { throw 'Visual Studio C++ tools were not found.' }
$compile = @"
@echo off
call "$vs\VC\Auxiliary\Build\vcvars64.bat" > nul
if errorlevel 1 exit /b 1
cl /nologo /MD /EHsc /DUE3_WX32=1 /DWXUSINGDLL /DUNICODE /D_UNICODE /I"$wx\include" /I"$wx\lib\vc_x64_dll\mswu" /I"$sourceRoot\Launch\Inc" "$output\EventLoopProbe.cpp" /Fo"$output\EventLoopProbe.obj" /Fe"$output\EventLoopProbe.exe" /link /LIBPATH:"$wx\lib\vc_x64_dll" wxmsw32u_core.lib wxbase32u.lib /SUBSYSTEM:CONSOLE
"@
[IO.File]::WriteAllText((Join-Path $output 'Build.cmd'), $compile, [Text.Encoding]::ASCII)
& cmd /d /c (Join-Path $output 'Build.cmd') *> (Join-Path $output 'Build.log')
if ($LASTEXITCODE -ne 0) { Get-Content (Join-Path $output 'Build.log') -Tail 20; throw 'Event-loop probe compilation failed.' }
if ($BuildOnly) { return }

$savedPath = $env:PATH
try {
    $env:PATH = (Join-Path $engine 'Binaries/Win64') + ';' + $env:PATH
    $start = New-Object Diagnostics.ProcessStartInfo
    $start.FileName = Join-Path $output 'EventLoopProbe.exe'
    $start.UseShellExecute = $false
    $start.CreateNoWindow = $true
    $start.RedirectStandardOutput = $true
    $start.RedirectStandardError = $true
    $process = [Diagnostics.Process]::Start($start)
    try {
        if (!$process.WaitForExit(10000)) { throw 'Event-loop probe timed out.' }
        $result = $process.StandardOutput.ReadToEnd() + $process.StandardError.ReadToEnd()
        [IO.File]::WriteAllText((Join-Path $output 'Results.log'), $result)
        Write-Output $result
        if ($process.ExitCode -ne 0) { throw "Event-loop checks failed: $($process.ExitCode)" }
    } finally {
        if (!$process.HasExited) { $process.Kill(); $process.WaitForExit() }
    }
} finally {
    $env:PATH = $savedPath
}
