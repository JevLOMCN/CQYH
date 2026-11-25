"""Run the built UBT's real toolchain setup under UTF-8 and OEM console pages.

Requires Windows, the v143 C++ tools and SDK, and a Shipping UBT build.
Run: python -B -m unittest discover -s Client/tests -p test_toolchain_environment.py -v
"""

import json
import os
from pathlib import Path
import subprocess
import unittest


CLIENT = Path(__file__).resolve().parents[1]
UBT = CLIENT / "client/UE3/Development/Intermediate/UnrealBuildTool/Shipping/UnrealBuildTool.exe"


@unittest.skipUnless(os.name == "nt", "Visual Studio toolchain setup requires Windows")
class ToolchainEnvironmentTests(unittest.TestCase):
    def test_compiler_lookup_on_oem_and_utf8_console_pages(self):
        self.assertTrue(UBT.is_file(), "Build UnrealBuildTool's Shipping configuration first")
        powershell = Path(os.environ["SystemRoot"]) / "System32/WindowsPowerShell/v1.0/powershell.exe"
        for code_page in (850, 65001):
            for platform, architecture in (("Win32", "x86"), ("Win64", "x64")):
                with self.subTest(code_page=code_page, platform=platform):
                    script = r'''
$ErrorActionPreference = 'Stop'
$originalCodePage = [regex]::Match((& cmd.exe /d /c chcp | Out-String), '\d+').Value
try {
    & cmd.exe /d /c 'chcp CODE_PAGE >nul'
    # A compiler inherited from a developer prompt must not mask setup failure.
    $env:PATH = (($env:PATH -split ';') | Where-Object { $_ -notmatch '\\VC\\Tools\\MSVC\\' }) -join ';'
    $assembly = [Reflection.Assembly]::LoadFrom('UBT_PATH')
    $toolchain = $assembly.GetType('UnrealBuildTool.VCToolChain', $true)
    $platformType = $assembly.GetType('UnrealBuildTool.CPPTargetPlatform', $true)
    $flags = [Reflection.BindingFlags]'NonPublic,Static'
    $platform = [Enum]::Parse($platformType, 'TARGET_PLATFORM')
    $toolchain.GetMethod('InitializeEnvironmentVariables', $flags).Invoke($null, @($platform))
    $findTool = $toolchain.GetMethod('FindToolOnPath', $flags)
    $compiler = $findTool.Invoke($null, @('cl'))
    $linker = $findTool.Invoke($null, @('link'))
    $resourceCompiler = $findTool.Invoke($null, @('rc'))

    # Keep compiler/linker response-file encoding independent of batch encoding.
    $response = [IO.Path]::GetTempFileName()
    try {
        $lines = New-Object 'System.Collections.Generic.List[string]'
        $lines.Add('response-file encoding regression')
        $responseType = $assembly.GetType('UnrealBuildTool.ResponseFile', $true)
        $create = $responseType.GetMethod('Create', [type[]]@([string], $lines.GetType()))
        $null = $create.Invoke($null, @($response, $lines.PSObject.BaseObject))
        $bytes = [IO.File]::ReadAllBytes($response)
        if ($bytes.Length -lt 3 -or $bytes[0] -ne 239 -or $bytes[1] -ne 187 -or $bytes[2] -ne 191) {
            throw 'Compiler response files lost their UTF-8 BOM'
        }
    } finally {
        [IO.File]::Delete($response)
    }
    [pscustomobject]@{
        Compiler = $compiler
        Linker = $linker
        ResourceCompiler = $resourceCompiler
        Architecture = $env:VSCMD_ARG_TGT_ARCH
        Toolset = $env:VCToolsVersion
        SDK = $env:WindowsSdkDir
    } | ConvertTo-Json -Compress
} finally {
    if ($originalCodePage) { & cmd.exe /d /c "chcp $originalCodePage >nul" }
}
'''
                    script = script.replace("CODE_PAGE", str(code_page))
                    script = script.replace("UBT_PATH", str(UBT).replace("'", "''"))
                    script = script.replace("TARGET_PLATFORM", platform)
                    result = subprocess.run(
                        [str(powershell), "-NoProfile", "-Command", script],
                        capture_output=True, encoding="utf-8", errors="replace", timeout=45,
                    )
                    self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                    details = json.loads(result.stdout.strip())
                    self.assertEqual(details["Architecture"], architecture)
                    self.assertTrue(details["Toolset"].startswith("14.4"), details)
                    self.assertTrue(Path(details["SDK"]).is_dir(), details)
                    for name in ("Compiler", "Linker", "ResourceCompiler"):
                        self.assertTrue(Path(details[name]).is_file(), details)


if __name__ == "__main__":
    unittest.main()
