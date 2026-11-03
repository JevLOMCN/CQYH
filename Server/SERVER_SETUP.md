# Running the CQYH servers on Windows

This guide describes a **single-machine, Windows x64** setup using the maintained CMake build and the included guard process. Paths below are relative to the repository root unless otherwise stated. It is a setup guide based on the checked-in scripts and source; a complete live game session has not been verified as part of writing it.

## 1. Install the prerequisites

1. Install **Visual Studio 2022 or 2026**, with the **Desktop development with C++** workload, the x64 MSVC toolset and a Windows SDK. The server uses C++23. [Visual Studio downloads](https://visualstudio.microsoft.com/downloads/).
2. Install **CMake 3.25 or later**. The `vs2026` preset requires CMake **4.2 or later**. Visual Studio may already include CMake. [CMake downloads](https://cmake.org/download/).
3. Install and start **MySQL Community Server**, together with the `mysql` command-line client. The supplied modern database export has been tested against MySQL 9.7.1 and 26.10.0; that is not a guarantee of full game-session compatibility. [MySQL Community Server](https://dev.mysql.com/downloads/mysql/).
4. On a separate machine used only to run a **Release** build, install the [Microsoft Visual C++ x64 Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist). Debug builds require the matching Visual Studio debug runtime; use Release for deployment.

The MySQL **client library** is bundled in `Server/NewServer/3rd/mysql-26.10.0`; it is not the MySQL database service. CMake copies its runtime DLLs next to the built servers. Lua, sol2 and zlib are bundled. Asio is downloaded at the first CMake configure; for an offline build, supply `-DCQYH_ASIO_SOURCE_DIR=<path-to-asio-source>`.

Qt is only needed for the optional Qt management tools. The commands below disable those tools but keep the guard server enabled. Other optional tools, including MFC-based tools, have their own dependencies; the targeted build below does not request them.

## 2. Build the servers and guard

Open PowerShell in `Server` and choose the preset matching the installed Visual Studio generator. For example, with Visual Studio 2026:

```powershell
Set-Location 'D:\Programming\CPlusPlus\CQYH\Server'
cmake --preset vs2026 -DCQYH_BUILD_TOOLS=ON -DCQYH_BUILD_QT_TOOLS=OFF
cmake --build --preset vs2026-release --target guardserver manageserver dbserver balanceserver loginserver gateserver gameserver datalogserver
```

For Visual Studio 2022, replace `vs2026` and `vs2026-release` with `vs2022` and `vs2022-release`. The Release executables are placed under `Server/NewServer/bin/<service>/`, including `_GUARD/guardserver64.exe`. Check that the build completed successfully and those executables and their copied DLLs exist. See [BUILD.md](BUILD.md) for build details and test presets.

The normal server build also provides optional GM and mobile services, but the target list above covers the services configured for the local guard. Add `gmserver`, `mobileserver` or `mgateserver` to the build and configure their counts only if you intend to use them. `nameserver` is an opt-in target and is not in the normal guard startup list.

## 3. Prepare MySQL

Start the MySQL service and confirm the `mysql` client is on your PATH. The default `globalconfig/dbconfig.lua` points to `127.0.0.1:3306`, database `cqyh_name`; configure the server's actual account and password there. Check the main, log and replica connection fields, plus the database settings in `globalconfig/dlconfig.lua` if datalogserver is enabled. Area configuration can override base settings: inspect `globalconfig/area_config.lua` and files it loads as well.

For a **fresh local database**, import the matching modern schema and then its data, once. **The schema contains DROP TABLE statements**; do not run it against an existing database you need to keep. From `Database/mysql-modern`, using Windows Command Prompt:

```bat
mysql -h 127.0.0.1 -P 3306 -u root -p --default-character-set=utf8mb4 < cqyh_name_mysql26_10_schema_only.sql
mysql -h 127.0.0.1 -P 3306 -u root -p --default-character-set=utf8mb4 cqyh_name < cqyh_name_mysql26_10_data_only.sql
```

Enter the MySQL password at each prompt. Stop if either command reports an error. The data import expects empty tables from the matching schema. See [Database/mysql-modern/README.md](../Database/mysql-modern/README.md), including its notes on existing databases and sample game accounts. An existing database requires a separate migration; these files are not an in-place upgrade procedure.

## 4. Review configuration

The services are launched with `Server/NewServer/bin` as their working directory. They load `global_config.lua`, which includes files under `globalconfig/`. In particular:

| File | Purpose |
| --- | --- |
| `globalconfig/dbconfig.lua` | MySQL connection fields for database and log/replica connections. |
| `globalconfig/area_config.lua` | Area-specific overrides applied after the base configuration. |
| `globalconfig/msconfig.lua`, `lsconfig.lua`, `bsconfig.lua`, `gtconfig.lua`, `gsconfig.lua` | Management, login, balance, gateway and game service settings. |
| `_GUARD/guard_config.lua` | Guard entry point; loads the following two guard files. |
| `_GUARD/guardconfig/guardconfig.lua` and `guard_area_config.lua` | Guard listen address, service counts, paths and management credentials. The area values override the base guard configuration. |

Check IP addresses, ports, local service counts and all relevant database credentials for your environment. The supplied guard configuration contains example management credentials and binds to `0.0.0.0`; replace the credentials and restrict network access before exposing it. The base guard port is 8810, login port is 8201, and balance external port is 8701; verify overrides and client configuration rather than assuming these are the effective values everywhere.

**Missing loose file in this checkout:** `global_config.lua` includes `globalconfig/additional_spawn_npc.lua`, but that file was not found as a loose file when this guide was written. The loader may also find files in a package; check the runtime log. If the include cannot be opened, the loader logs an error and may continue, and the effect on game data is unknown. Restore the intended file from the appropriate deployment or remove the include only after establishing that the custom spawn configuration is unnecessary. An empty placeholder would not restore its behaviour.

Keep the existing `commonconfig`, Lua scripts, data tables and runtime directories alongside the executables. External authentication, billing and anti-cheat integrations may require additional configuration or services; their complete local setup has not been verified here.

## 5. Start the services

1. Start MySQL and verify the configured database is reachable.
2. Open PowerShell at the server runtime root and run the supplied Release launcher:

   ```powershell
   Set-Location 'D:\Programming\CPlusPlus\CQYH\Server\NewServer\bin'
   .\start_server.bat
   ```

   The batch file changes into `_GUARD` and starts `guardserver64.exe cmd start -ar`. `-a` starts all services selected by the guard configuration; `-r` selects Release executables. Debug builds use `_d` executables and `start_server_d.bat` instead. The scripts create console windows and return before service readiness is established.

3. Check the guard and service consoles and logs for successful initialization, database connections and service registration. The shared logging code writes dated files under `log/` relative to the process working directory. Confirm the processes remain running. A process starting is not proof the whole game is ready; test login and a game session with the matching client.

The guard launches configured processes in this order: **manageserver → dbserver → balanceserver → loginserver → gateserver → gameserver → gmserver → datalogserver → mobileserver → mgateserver**. Services with a configured count of zero are skipped. The supplied guard area file sets datalogserver, GM and mobile counts to zero. The launcher does not wait for a readiness check between process launches, so investigate connection failures and retry after dependencies are ready.

The older Linux `start_server.sh` starts services in a different order and does not use the Windows guard. This guide does not claim that script is a verified deployment procedure.

## 6. Troubleshooting and shutdown

- **Executable missing:** rebuild the correct Release or Debug target and confirm its path under `bin/<service>/`; build `guardserver` separately from the regular server targets.
- **DLL missing:** rebuild to run the CMake copy steps; verify the MySQL client and OpenSSL DLLs sit next to the relevant executable and install the matching x64 VC++ runtime.
- **MySQL connection failure:** check that the MySQL service is running, the user has access to `cqyh_name`, and the effective host, port, username and password match both base and area configuration. Do not paste passwords into logs or screenshots.
- **Lua include error:** check the runtime working directory and restore the missing `additional_spawn_npc.lua` described above.
- **Port conflict or unreachable client:** compare the effective service, guard and client addresses and ports; check local firewall rules and whether another process is using the port.
- **A process exits after launch:** inspect its own console and logs, not just the guard window. Authentication, game data, third-party services, or an invalid config can prevent readiness.

Do not use `stop_server_d.bat` as a graceful shutdown procedure: it invokes `taskkill /F`. The exact safe shutdown and save sequence for a live game has not been validated in this checkout. For a disposable local test, stop clients first and verify saved data after stopping the server processes.

## Verification limits

The build documentation records successful Windows x64 builds and isolated MySQL import/role-writer tests. The database test notes explicitly say a full game session was not tested. This guide was checked against the current configuration and source, but MySQL installation, schema import, server launch, client login and shutdown were **not** executed while writing it.
