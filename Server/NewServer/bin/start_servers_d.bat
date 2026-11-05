@echo off
rem Starts the debug servers with NewServer\bin as the working directory
rem (they load global_config.lua and globalconfig\ relative to it).
cd /d "%~dp0"
for %%S in (nameserver dbserver manageserver datalogserver gameserver loginserver balanceserver gateserver) do (
    if exist "%%S\%%S64_d.exe" (
        echo Starting %%S
        start "%%S" /D "%~dp0" "%~dp0%%S\%%S64_d.exe"
        timeout /t 2 /nobreak >nul
    ) else (
        echo Missing %%S\%%S64_d.exe
    )
)
