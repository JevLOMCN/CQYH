# Building the server

One build system: CMake (3.25+, the copy bundled with Visual Studio 2022 is fine). C++23, x64.
CMake is the maintained build system. Open the Server folder or generate a solution; retained
legacy project metadata is historical, except for the Qt entry points described below.

## Visual Studio 2022

* **Open the folder** (`File > Open > Folder...` on `Server`). VS reads `CMakePresets.json`; pick the `vs2022` preset
  and a Debug/Release configuration, then `Build > Build All`. or
* **Generate a solution**: `cmake --preset vs2022` creates `build/vs2022/CQYHServer.sln`. Open that, build as usual.

Visual Studio 2026 has equivalent `vs2026`, `vs2026-debug` and `vs2026-release` presets.
Use CMake 4.2+ (including the copy bundled with VS 2026) for that generator.

## Command line

```
cmake --preset vs2022
cmake --build --preset vs2022-debug        (or vs2022-release)
ctest --preset vs2022-debug                (runs transport, Lua bindings and optional MySQL tests)
```

Server executables are written to `NewServer/bin/<server>/<server>64[_d].exe`, where the configs and scripts are, exactly as
the old projects did. Pass `-DCQYH_OUTPUT_TO_BIN=OFF` to keep them in the build directory instead.
Tool executables use `<build>/NewServer/tools/<tool>/<configuration>/` regardless of this option.

## Things worth knowing

* asio (standalone, header only) is downloaded on the first configure and pinned by hash (`cmake/Asio.cmake`). To
  build offline point `-DCQYH_ASIO_SOURCE_DIR=<asio checkout>` at an existing copy.
* sol2 v3.3.1 is vendored in `NewServer/3rd/sol2` alongside the other third-party libraries. The build uses these
  local headers directly; it does not download sol2. Lua 5.1 is compiled from the bundled `3rd/lua/src` sources
  with the server's toolchain, so the old Lua DLLs and their Visual C++ 2013 Debug runtime are unnecessary.
* zlib 1.3.2 is vendored in `NewServer/3rd/zlib` and compiled as the static `cqyh::zlib` library on Windows
  and Linux. No system zlib or prebuilt zlib library is needed. See its `README.md` for provenance and the
  separate legacy tool build. `ctest -R zlib --output-on-failure` runs the upstream API tests and verifies
  compatibility with compressed data produced by the former zlib 1.2.7.
* Server targets require C++23 or newer (default 23); older `CQYH_CXX_STANDARD` values are rejected.
  The shared MSBuild defaults preserve CMake's selected standard and select C++23 mode for standalone
  first-party projects. MSVC currently uses `/std:c++latest` for CMake's C++23 selection.
  CMake's server, test and all 15 maintained tool targets compile with `/permissive-`.
  Tools select `STRICT` in `cqyh_add_tool`.
* CMake's server and test MSVC targets enable STL exceptions consistently and use `/EHsc`.
  Baselib retains `/EHa` for its existing SEH wrappers. Precompiled headers no longer disable
  STL exceptions independently of their callers or the Asio transport.
* The source lists in the `CMakeLists.txt` files are explicit. A new `.cpp` has to be added there.
* `CQYH_MSVC_COMPILE_PROCESSES` limits compiler workers per MSBuild target (default 2). sol2 templates need
  substantial compiler memory; increase this only when the machine has enough memory for parallel targets too.
* `nameserver` remains opt-in (`-DCQYH_BUILD_NAMESERVER=ON`), since it was not in the old solution.
  It is validated in strict C++23 mode alongside the other servers.
* `NewServer/src/asimov` (Qt robot client), `bteditor` and `gameworld/script/script.vcxproj` (the Lua
  bytecode compiler step) remain outside the server CMake build. Legacy tools have explicit
  targets in `NewServer/tools/CMakeLists.txt` and use strict C++23. The duplicate
  `gmtool - 副本` is excluded from this migration.
* Source files use UTF-8. MSVC emits narrow string literals using GBK for compatibility with the legacy server.

## Baselib utility regression tests

`ctest -R baselib_utilities --output-on-failure` tests both the server and standalone tools' copies
of the directory and string utilities, including truncation, fixed-width source fields, non-ASCII
Windows paths, hidden entries and recursive directory operations.

Directory operations use `std::filesystem` with error codes. Nonrecursive creation reports an existing
directory as failure; recursive creation accepts it. Deletion is synchronous and reports actual
filesystem failure, replacing the old shell launch. Traversal selects files or directories correctly
and stops with failure if a callback fails.

String copy and append return the stored length and keep their bounded source reads. An append to a
destination without a terminator now terminates within its capacity. The legacy Windows `snprintf`
alias remains until its callers are migrated, since some depend on its truncation behavior.

## Protocol layout regression tests

On MSVC x64, `ctest -R protocol_layouts --output-on-failure` checks frozen size and alignment
contracts for 1,232 production packet and helper types. The baseline was measured with the
former C++17/permissive settings and compared independently with strict C++23 and STL exceptions
enabled. Update the snapshot alongside intentional protocol layout changes.

## File ownership

`NewServer/inc/baselib/file_owner.h` provides `cqyh::FilePtr` and `cqyh::OpenFile` for
`IFile`, plus `cqyh::CFilePtr` for C streams. `IFile::Close()` destroys its concrete
object; its base destructor is not virtual. The custom deleter preserves that contract.
Use `reset()` for early close and `release()` only when handing ownership to a legacy caller.
Existing raw `g_OpenFile` callers continue to close their files explicitly.

File creation now retains concrete ownership until initialization succeeds. Table and Lua
loaders own their temporary file handles and buffers with RAII, including failed loads and
successful recursive CRC scans. UTF-16 conversion frees the original byte allocation using
its correct type, and table buffers include space for parser lookahead.
Load failures retain their diagnostics; ordinary loose-file misses stay quiet for package fallback.
Directory removal logs the filesystem operation instead of the obsolete shell command.

`ctest -R baselib_resources --output-on-failure` exercises both server and tool implementations,
including real loose/compressed/empty/truncated pak fixtures, stream lifetime, UTF-8/UTF-16
tables and Lua includes. Debug builds also check that repeated Lua loads retain no heap blocks.

## Shared utility contracts

`CSimpleTimer::FrameDuration` represents simulation frames at `GAME_FPS` (16 per second).
The integer timer API remains available; its counts, delay, late-trigger scheduling and
unsigned arithmetic retain their existing behavior. It does not use wall-clock milliseconds.

Calendar calculations use `std::chrono`, including the full legacy `int` year range through
a fallback outside `chrono::year`'s range. `get_month_day_count(year, month)` takes a zero-based
month. `localtime_ex()` follows the Windows system timezone; `get_time_str()` follows the CRT
timezone. Their borrowed results are stored per thread and overwritten by the next call on
that thread. `GetTickCountEx()` retains its platform-specific epoch: Windows uptime and Unix
epoch milliseconds. `get_time_zone()` retains its existing standard-offset convention.

`cqyh::ParseIntegerPrefix(std::string_view)` uses `std::from_chars` and returns
`std::expected<int32_t, std::errc>`. It accepts leading whitespace, signs and decimal prefixes,
and reports invalid input or overflow. `CTabFile` retains its legacy zero result for nonnumeric
fields and CRT overflow behavior, including the shipped level-table `MaxExp` sentinel. Table
column names may be passed as borrowed string views; the table continues to own its keys and
data. Header lookup and value-row/default-row lookup retain their separate indexing rules.

`ctest -R 'baselib_(timing|numbers|resources)' --output-on-failure` covers these contracts,
including timezone changes, concurrent time formatting, bounded names and missing defaults.

## Lua bindings

The server uses typed sol2 registrations in C++. Fusion registrations live under
`Common/fusion/src/script/bindings`, grouped into `config`, `data`, `definitions` and `support` folders.
Feature files such as `config/game_world_bindings.cpp` and `data/quest_bindings.cpp` keep each type's members
together. The `script/sol_*.cpp` entry points assemble these shared registrations; `bindings/registration.h`
declares their internal functions. `Common1/fusion` follows the same layout for its alternative protocol variant.
The other server and tool binding directories follow the same feature-based layout. Packet registrations are
under `bindings/external` and `bindings/internal`, named by direction and feature, for example
`external_client_to_game_movement_bindings.cpp` and `internal_manage_to_database_bindings.cpp`.
Enum-string registrations are grouped by their declaring headers under `bindings/definitions`.
Numbered binding source files are no longer used.
Edit these C++ registrations directly; there is no binding generator or package specification to regenerate.
Existing `LoadPackage("name.pkg")` strings remain module identifiers for scripts.
The networking objects are archived as `net`, `net_lua_external`, `net_lua_external_responses` and
`net_lua_internal` to stay below MSVC's 4 GB Debug archive limit, including in C++23 mode. The external
archive links the response archive transitively; CMake consumers link `cqyh::net` for the complete interface.

The shared policy in `NewServer/inc/baselib/sol_bindings.h` preserves zero-based C arrays, nested object references,
bounded string assignment and named objects passed through `CLuaScript::CallFunction`. `Cast(object, "CType")`
converts base pointers and packet pointers to registered script types. Existing scripts now use this function.
`ctest -R lua_bindings --output-on-failure` exercises these behaviors.

## MySQL

SQL statement parameter bytes, result rows, prepared handles and worker buffers now have
scoped owners. `cqyh::SqlResults` retains each byte array while exposing the existing
`std::list<void*>&` callback view; row addresses remain stable, and callbacks borrow them.
Partial rows are cleared before a retry. The completion flag distinguishes successful
retries from exhausted work, and statements recycle even if a callback fails or throws.
Fetch errors are retried rather than published as partial success. Synchronous calls finish
their own borrowed data before propagating exceptions from other completed callbacks.

The original `AsyncExecSqlStmt` / `SyncExecSqlStmt` interfaces continue to borrow user data.
For owned callback data, use `AsyncExecSqlStmtOwned` / `SyncExecSqlStmtOwned` with
`MakeOwnedSqlCallback(function, std::unique_ptr<T>)`. These retain data until completion,
rejection or shutdown. The supplied function borrows that data and must not delete it.
GM query callbacks use this interface. Shutdown joins workers before clearing either
queue; discarded queries release their captures without invoking callbacks.

`mysql_owner.h` provides scoped connection, statement and result handles. SQL managers
share a library lease, so stopping one manager leaves the others' client initialized.
Worker client-thread state outlives its statements and connection and is released before
the final library lease. NameServer result sets and SQL bootstrap files also use scoped
cleanup on guarded error exits.

`cqyh::MakeMysql()` returns an independent owned connection that retains its library lease.
Assign `cqyh::MysqlPtr{}` to release both the handle and the stateful deleter's lease.
`MysqlThreadContext` scopes client-thread initialization for database workers. Prepared
wrappers keep new handles local until initialization succeeds; their destructors and
idempotent `UnInit()` release them. Reconnect closes prepared handles before replacing
their connection. Worker shutdown waits before releasing queues or native resources.

Asynchronous friend/mail callbacks capture stable identifiers and look up live objects
on completion, avoiding borrowed pointers to pool entries that may have been deleted.
The remaining raw user-data contexts are synchronous borrows.

`ctest -R baselib_sql_resources --output-on-failure` runs production manager/bootstrap
code with a link-time fake C API. It opens no database connection and checks callback
lifetimes, partial initialization, retries, shutdown queues and handle-close ordering.
It supplements the separately configured live integration test below.
`ctest -R mysql_handles --output-on-failure` checks all eleven role/log prepared wrappers,
owned connection moves, initialization failures and native close ordering without a server.

`CTabFile` owns its text and offset storage and cannot be copied. `UnInit()` releases both
buffers and resets metadata; reinitializing a live table preserves the existing data.
Malformed row widths and impossible dimension products are rejected before they can shift
later columns or read uninitialized offsets. Existing valid quoting, defaults and CRCs remain.

The CMake server build uses the Oracle MySQL **26.10.0 C API**, bundled in `NewServer/3rd/mysql-26.10.0`
and imported as `cqyh::mysql` SHARED in `cmake/ThirdParty.cmake`. Debug and Release use the same client library.
The Windows build copies `libmysql.dll`, `libssl-3-x64.dll` and `libcrypto-3-x64.dll` alongside each executable.
The Linux x64 shared client requires glibc 2.28+ and carries its OpenSSL libraries in `lib/linux64/private`.
See the dependency's `README.md` for upstream package URLs and checksums.
The old SDK remains in `NewServer/3rd` for the separate legacy tool projects.

Use the matching fresh-install schema and optional data in [Database/mysql-modern](../Database/mysql-modern/README.md).
The server now represents an unset `DeleteTime` as SQL NULL and converts it to zero in game data.
Existing databases need their zero deletion dates converted to NULL and those columns made nullable before
using this build. The archived SQL exports and `commonconfig/sql` bootstrap scripts target the old database;
they are not an upgrade path for populated databases. This change does not upgrade a local database service.

MySQL uses `caching_sha2_password` authentication; the modern client supports it without enabling the removed
`mysql_native_password` plugin. Rebuild the server executables before connecting to a modern database.

The Windows `mysql_roles` regression test runs the production role create/delete/restore writers under strict
SQL mode. It skips unless a test instance is explicitly selected. The account needs permission to create and
drop databases named `cqyh_mysql_test_*`; each run removes only its own database:

```powershell
$env:CQYH_TEST_MYSQL_PORT = "33360"
$env:CQYH_TEST_MYSQL_USER = "root"
# Optional: CQYH_TEST_MYSQL_HOST and CQYH_TEST_MYSQL_PASSWORD
ctest --preset vs2022-debug -R mysql_roles --output-on-failure
```

## Generator

`gen_msg` writes generated content to stdout and diagnostics to stderr. It returns
a nonzero exit code for invalid arguments, unreadable inputs or generation errors.
The `enum`, `msg` and `errcode` modes take no additional arguments. The `btnode`
and `btevent` modes accept an optional source header:

```powershell
gen_msg.exe msg > message_sizes.generated.cpp
gen_msg.exe btnode "path/to/bt_action.h" > nodes.generated.ini
```

CMake builds locate the default behavior-tree headers in the source checkout,
so these modes can run from another working directory. Supply an explicit header
when using the executable with a different checkout. Packet sizes come from the
registered C++ types; flexible packets retain the `-1` marker.

## Common1 validation

`Common1` is an alternative protocol variant. Its fusion library and Lua bindings
can be built independently, using its own definitions rather than the `Common`
headers used by the server build:

```powershell
cmake -S Server/Common1 -B Server/build/cxx23-common1 -A x64 -DCQYH_OUTPUT_TO_BIN=OFF
cmake --build Server/build/cxx23-common1 --config Debug
ctest --test-dir Server/build/cxx23-common1 -C Debug --output-on-failure
```

Repeat the build and tests with `Release` to validate that configuration. On MSVC
x64, the tests also freeze Common1's 799 packet/helper sizes and alignments against
its C++17 baseline. This build validates the existing Common1 library; the server
executables continue to use `Common`.

## Legacy file tools

With `CQYH_BUILD_TOOLS=ON`, `pak`, `update_string_map` and `gmtool_label_version`
use strict C++23 and standard-library storage and file ownership. Their offline
regressions run with `ctest -R '^tool_' --output-on-failure`. Build `test_tool_pak`
alongside those executables before running the tests.

`pak` retains the 32-bit version-1 archive structures, filename hashes, MD5 values,
compression settings and declaration order. Frozen archives cover named, unnamed
and uncompressed-metadata variants, including empty files and multiple chunks.
Listing, verification and extraction leave the input archive unchanged. Failed
commands now return nonzero; malformed records and extraction paths escaping the
destination are rejected. Input files are read into memory, with compression
scratch storage bounded by one chunk. The format still cannot represent files
larger than its 32-bit offsets and lengths.

The text tools retain their original no-argument paths and also accept explicit paths:

```powershell
gmtool_label_version.exe version_header.h pak-version.model pak.bat
update_string_map.exe error_def.h stringmap.txt stringmap_new.txt
```

The label generator preserves template bytes and line endings, inserting the
version after the first underscore. It fixes the original trailing NUL padding,
accepts zero version components and rejects missing or malformed definitions.
The string-map report remains UTF-16LE with a BOM and lists removed entries followed
by new entries in enum declaration order; existing translations are omitted.
Its first-token comment convention remains. UTF-8 input maps are also accepted.
BOM handling, duplicate final lines and empty comments are handled without fixed
buffers or unchecked reads. Invalid inputs are diagnosed before opening the output.

## Legacy log tools

`common_log [anticheat-log]` retains its original default input path and report.
Its reader uses standard streams and strings, accepts CRLF/LF and a final line
without a newline, and propagates callback failures. Timestamp validation rejects
malformed fields; timeout calculations also work across midnight. Reinitialization
clears all counters and pending role events.

`analysis_log log-directory mode` uses mode `1` for error counts and `2` for network
flow rows. It reads `.log` files without recursion and retains the existing report
formats. Standard `std::jthread` workers own queued tasks and stream each input;
condition variables replace polling. Shutdown cancels pending work, requests active
work to stop, wakes result consumers and joins workers before releasing storage.
Partial startup and processing failures are reported instead of stranding tasks.
Log prefixes with and without a thread ID are supported. Both tools return nonzero
for input or processing failures and build in strict C++23 without server libraries.

`ctest -R '^tool_(common_log|analysis_log)$' --output-on-failure` checks frozen report
output, malformed records, state reset, queue ownership, allocation-failure rollback,
worker restart and shutdown. Build the tool executables plus `test_tool_common_log`
and `test_tool_analysis_log` first.

`RoleLogCheck dir1 date1 [dir2 date2]` reads the existing `Money`, `gmmoney` and
`RoleLogout_Basic` tables. Dates use `YYYY_MM_DD`, with the optional second date
later than the first. Standard maps/deques own records; the checker no longer
preallocates 5.3 million pool entries. Input order, the structured GM-money schema
and the same-second ambiguity/discard rules remain. Timestamps are checked before
conversion and retain the original local `mktime` convention (`tm_isdst=0`). Money
totals use checked 64-bit arithmetic. Missing files and malformed rows return a
failed exit instead of reporting a partial check as successful. A completed check
can still return success while reporting discrepancies in the money records.
Native dump handling remains outside the entry point's C++ lifetime guards.

Build `RoleLogCheck` and `test_tool_role_log`, then run `ctest -R '^tool_role_log$'
--output-on-failure`. The test compares 42 original reconciliation summaries and
58 detailed mismatch messages, then checks missing transaction sources, UTF-16LE,
time bounds, integer overflow, repeated use, percent signs in diagnostics and
allocation-failure cleanup.

`cash_accounting` retains its configured database defaults and appends the existing
`money_accounting` report. `--help` is safe to run without a database. Standard
containers own snapshots and index the next matching role in linear time, retaining
input order and the original inclusive query bounds. An owned SQL manager and
callbacks release workers and results on failure. Native SQL bindings widen role
IDs and timestamps to signed BIGINT to preserve their complete unsigned 32-bit
range. Checked 64-bit balance accumulation avoids signed overflow. Reports retain
their original text, byte-based name padding and Windows line endings; failures
return a failed exit. An interval is reported only after its complete query result
has been read. Previously appended intervals remain if a later interval fails.

Build `cash_accounting` and `test_tool_cash_accounting`, then run `ctest -R
'^tool_cash_accounting$' --output-on-failure`. Tests exercise the production SQL
manager against a client fake with no network/database access, compare original
report bytes, and check query bindings, ordering, retry rollback and shutdown.
No-argument production execution uses the existing configured database; automated
CLI checks exercise only help and invalid arguments. Use CMake for this target;
the archived Visual Studio 2008 project predates C++23.

The default `exportdb [database]` operation still writes `guild_stat.txt` and
`guild_member.txt`. Guild/member migration uses owned standard containers and
binary-mode streams, retaining LF output, row order and field truncation limits.
The version-5 guild parser checks lengths, CRC and enclosing block ranges before
reading structures or variable lists. Guild blobs are decompressed into a bounded
tool-owned buffer. Native ID/time bindings use signed BIGINT for the full unsigned
32-bit domain. Failed queries, malformed data and file errors return failure;
query/decoding failures leave the previous report intact. A write failure can
leave a partial file. Native initialization has lifetime guards, balanced timer
resolution requests and a failed exit when export/cleanup fails. `--help` is safe
without configuration or a database.

Build `exportdb` and `test_tool_exportdb`, then run `ctest -R '^tool_exportdb$'
--output-on-failure`. The fixtures compare original guild/member output and test
bounds, CRC, compressed data, native bindings, retries and missing configuration.
`ExportRoles` and `InsertRoles` remain inactive in the default command and require
an initialized gameworld. Their standard-library record pipeline owns report data,
checks compressed role envelopes/base offsets, uses consistent SQL string widths,
and streams reports beyond the old 16 MiB limit. Money/item totals retain 64 bits.
Text fields escape record delimiters; binary keys retain their native Windows
541-byte layout with initialized IDs, terminated names and all 128 variables.
Deleted-role repair rechecks offsets and recomputes CRC. Temporary Lua role globals
restore their previous value before the C++ role object is destroyed.

Insertion keeps at most 32 asynchronous requests outstanding, owns each callback's
completion state, drains on producer failure and detects exhausted SQL retries.
Earlier successful inserts can remain committed after a later failure; this helper
does not provide a transaction. It also rejects overflowing ID ranges and payloads
that exceed SQL capacities. The gameworld bridge retains `CRole::Save`'s native
fixed-buffer contract and checks its returned data before binding SQL parameters.

Build `test_tool_export_roles` and run `ctest -R '^tool_export_roles$'`. Tests use
the real SQL manager with a no-socket client fake, a real Lua VM, and independent
record producers/readers. They verify original-format bytes, fixed key offsets,
blob integrity/bounds, large output and asynchronous failure cleanup. They do not
load a configured gameworld or run real role creation/database insertion.

## Local Qt SDK

The GUI tools use Qt5 Core, Gui and Widgets. The local validation SDK is Qt 5.15.2
MSVC 2019 x64, installed under the ignored directory
`Server/build/dependencies/Qt/5.15.2/msvc2019_64`. It includes the base libraries,
build tools, platform plugins and SVG support. It was installed with `aqtinstall`:

```powershell
python -m pip install --target Server/build/dependencies/aqt-runtime aqtinstall==3.3.0
$env:PYTHONPATH = (Resolve-Path Server/build/dependencies/aqt-runtime).Path
python -m aqt install-qt windows desktop 5.15.2 win64_msvc2019_64 `
  --outputdir Server/build/dependencies/Qt --archives qtbase qtsvg
cmake -S Server -B Server/build/cxx23-modernization -DCQYH_BUILD_QT_TOOLS=ON `
  -DCMAKE_PREFIX_PATH="D:/Programming/CPlusPlus/CQYH/Server/build/dependencies/Qt/5.15.2/msvc2019_64"
```

Use the SDK path on your machine when configuring another checkout. `qmake -query
QT_VERSION` verifies the installation. GM tool and maintenance tool use this SDK through
CMake AUTOMOC/AUTOUIC; no Qt Visual Studio extension or in-source generation is required.

## Standalone tool project entry points

All 17 `.vcxproj` files under `NewServer/tools` delegate compilation to CMake
through `cmake/ToolCMake.targets`. Their GUIDs, source/filter views, existing
comments and native resources are retained. Build, Rebuild, Clean, individual
C++/resource/custom-build actions, Link, Lib and MASM entry points use the same
configured CMake target. Legacy pre/post-build commands are outside the delegated
build pipeline; CMake owns dependencies, output copying and Qt generation.
The legacy IDE fast up-to-date shortcut is disabled so CMake checks its complete
dependency graph; CMake still decides which files actually need rebuilding.

Most projects select the corresponding tool name. The nested Encrypt project
selects `encrypt`, and `MaintainTool` selects `maintaintool`. The archived library
projects select the maintained Server implementations: `tools/baselib` builds
`baselib`, while `tools/net` builds `net_core` and its required split Lua archives.
They do not compile the archived library copies or restore the former monolithic
network archive. CMake-generated projects remain the complete dependency graph.

The 16 per-tool `.sln` files reference existing CMake-backed project entry points.
Deleted source-tree fusion, gameworld, baselib and net projects are no longer listed
as solution dependencies; CMake resolves those libraries from the selected build.
Certserver, pak, cash_accounting and update_string_map have modern `.vcxproj`
wrappers retaining their original project GUIDs; their `.vcproj` files remain
archival. The maintenance solution retains the valid guardserver project and
orders its build after maintenance so the two CMake invocations do not overlap.

Select **x64** with Debug or Release. The previously Win32-only common_log, PosLog
and version-label projects now expose x64 configurations too. Direct Win32 project
builds are rejected before compilation. Old solution configuration names are retained
as aliases to valid x64 project configurations, including Win32/Mixed Platforms names. Existing Final Debug and Final Release choices in the guard
and library projects map to CMake Debug and Release. Other supported CMake
configurations can be selected with `CQYHCMakeConfiguration`.

Configure this Server tree with a Visual Studio x64 generator first. The standalone
entry points use `Server/build` by default. For VS 2026 and the local Qt SDK:

```powershell
cmake -S Server -B Server/build -G "Visual Studio 18 2026" -A x64 -DCMAKE_PREFIX_PATH="D:/Programming/CPlusPlus/CQYH/Server/build/dependencies/Qt/5.15.2/msvc2019_64"
MSBuild Server/NewServer/tools/common_log/common_log.vcxproj /t:Build /p:Configuration=Release /p:Platform=x64
```

Use the Qt SDK path on your machine. With this default layout, opening a tool
solution and building requires no directory override. A different configured Server
build can still be selected with the `CQYHToolsBuildDirectory` MSBuild property.
A newly configured directory rebuilds its own object files and dependencies.

Tool executables, copied runtime DLLs, linker symbols and import outputs go under
`Server/build/NewServer/tools/<tool>/<configuration>/` with the default build directory.
For example:

```text
Server/build/NewServer/tools/gmtool/Debug/gmtool64_d.exe
Server/build/NewServer/tools/gmtool/Release/gmtool64.exe
Server/build/NewServer/tools/maintaintool/Debug/maintaintool64_d.exe
```

All tools share one CMake cache in `Server/build`. Shared libraries, object files
and Qt-generated files keep their CMake-managed locations. Projects in
`NewServer/src` retain their existing output paths; `CQYH_OUTPUT_TO_BIN` controls
server executable placement only.

`CQYHCMakeExecutable` can override the executable; otherwise the entry point uses
Visual Studio's bundled CMake, then `cmake.exe` on PATH. The default application
debugger command and working directory follow the tool's output folder.
Existing explicit debugger commands/working directories are retained; application
configurations are still needed to run the tools.

Application projects require `CQYH_BUILD_TOOLS=ON`; the two Qt projects additionally
require `CQYH_BUILD_QT_TOOLS=ON` and the configured Qt5 SDK. Encrypt and PosLog
require `CQYH_BUILD_MFC_TOOLS=ON` and MFC. Library entry points do not require the
tools option. Missing/foreign caches, a different source tree, missing targets,
disabled required options and unsupported platforms/configurations fail before
delegation. Clean affects only the selected generated target, including its own
executable/intermediates, rather than its shared dependencies or tracked headers.

GM/maintenance project source views retain the existing passive generated files;
CMake AUTOMOC/AUTOUIC generates active headers in the build directory. SDK paths
come from the CMake cache. `cmake/QtToolCMake.targets` remains a compatibility
adapter for older imports that set `CQYHQtToolTarget`.

All 17 project entry points were checked in Debug and Release. All 16 tool
solutions load and build in both configurations, and every solution project path,
GUID and configuration mapping is validated. Legacy solution aliases and the
maintenance/guard parallel graph build are checked as well. Additional checks cover target-only Clean/Rebuild (with shared-library
hashes preserved), direct compile/resource/library/assembly actions, legacy Final
configuration mappings, graph build and the Qt compatibility adapter. Nine
invalid platform/cache/option cases reject before compilation. Project GUIDs,
source items and existing project comments are preserved; no C++ implementation
or resource files changed for these entry-point and solution-loading updates.
Close and reopen an existing Visual Studio solution after updating it so cached
unavailable-project nodes are discarded; deleting user settings/caches is unnecessary.

## Guardserver file handling

Incoming packets use a scoped standard-library owner and a value-initialized
`std::array` of dispatch callbacks. Header size and message IDs are checked before
dispatch; payload handlers validate the received span before decoding.

Log cleanup uses an owned `std::jthread` with cooperative cancellation and
`std::filesystem` traversal. Only valid `YYYY_MM_DD` directories under the configured
server's `log` folder are selected. The complete tree is checked for reparse points
(including Windows junctions) before deletion, and paths are checked again before
each removal. Nested ordinary directories are supported. A `_cleard_by_guard`
directory is created only after successful removal; locked files and other failures
are reported and remain eligible for retry. Shutdown and configuration reload join
the worker before releasing server state. Cancellation can leave a partially cleared
date directory without a completion marker.

Configuration loading restores the previous configuration on Lua/path errors.
Runtime paths use `std::filesystem` and are checked against the existing fixed-size
configuration ABI. Current-version and operation-result reads are bounded and do
not require write access. Operation results are claimed by rename before reading;
a Windows batch writer still holding its output handle leaves the result pending.
Completed result names must match exactly, ignoring ASCII case. Acknowledgements
retain the patch name and completed failures clear their pending operation state.

Build `guardserver`, `test_tool_guard_files` and `test_tool_guard_results`, then run
`ctest -C Debug -R '^tool_guard_(files|results)$' --output-on-failure` (or Release).
The filesystem tests use disposable workspace fixtures, including native Windows
junctions, locked files and cancellation. The result tests compile the real guard,
Lua loader, process registry and message/command code against in-memory sockets;
they do not initialize the native guard transport or perform live deployment.
Chunked uploads use an owned `std::ofstream`, exclusive C++23 `noreplace` temporary
files and atomic destination replacement after the final chunk closes successfully.
The receiver retains its connection, root and file/chunk metadata across packets;
out-of-order/mismatched records fail and disconnect discards incomplete files.
The maintenance tool's fixed-size wire record and final-chunk-only acknowledgement
remain unchanged. Configuration copies complete synchronously without a shell.
Rejected guard configuration reloads restore the prior runtime settings, installed
configuration and version-prefixed configuration copy. If a lock prevents rollback,
the previous file is retained in a sibling `.guard-upload-*/previous` backup.
Archive deployment and patch installation run in a separate owned `std::jthread`.
Generated batch files, shell copies, `xcopy` and shell directory removal are no longer
used. ZIP extraction still uses the bundled `7z.exe` (9.20), started directly with
owned process/thread/standard handles and an explicit inherited-handle list. Shutdown
requests cancellation and joins; cancellation terminates only the child created by
this operation. Diagnostics append to `deploy_version.log` / `hot_update_patch.log`.

The extracted package and required per-version configurations are staged and checked
before runtime retirement. The existing retirement list, package `_GUARD` prefix and
placeholder exclusions, unrelated-file overlay, current-version cache-prefix retention,
and current/result tag formats are preserved. Read-only configuration and overlay files
retain the old writable-copy behavior. IO uses extended-length Windows paths internally;
stored/configuration paths keep their ordinary spelling and normalize trailing separators.
Guard configuration reload remains on the main thread before the worker starts; the
worker captures its own paths and never accesses server configuration/network globals.
Uploads and overlapping deployment requests are refused while deployment is pending.
Completion/error acknowledgements and hot-patch reload commands run on the main loop.

Runtime replacement remains an in-place operation with no complete rollback: an IO
failure or cancellation after retirement can leave a partial runtime. Failures do not
publish a success result; obsolete-cache cleanup failures are logged after the runtime
copy. No live deployment is needed to run `test_tool_guard_deploy`: it uses disposable
workspace trees, a test-owned child executable and the bundled extractor to build/read
a real ZIP. Existing guard packet tests also exercise the real asynchronous ZIP/patch
path against in-memory sockets.

Guard receive dispatch validates fixed packet sizes and terminating zeros in variable
string tails against the actual received span. The packed protocol structures remain
unchanged. Notification and command-result text use owned standard strings; literal
percent signs stay data, and legacy display limits are retained. Error replies use
their fixed wire size, command replies stay within the transport maximum, and scoped
packet owners release failed sends. Private send boundaries normalize exceptions to
the existing failure return contract.

Registration checks type, slot, PID and timing before publishing a session. Singleton
server IDs must be one; array IDs must fit their actual slot counts. Maintenance-tool
credentials compare complete bounded values with the existing ASCII case-insensitive
rule, so a value with an extra suffix no longer matches its configured prefix.
Replacing a connection detaches the old peer before closing it. Failed retirement
leaves it detached rather than restoring a potentially expired borrowed pointer.
Disconnect notifications clear the old record before sending, so synchronous or delayed
callbacks cannot erase a replacement. The result fixture covers malformed bodies,
variable tails, response ownership, registration replacement and callback reentrancy
using memory sockets and metadata-only process records.

Guard startup uses an owned, quoted executable path and scoped SCM handles. The service
callback has the Windows SCM signature; status updates are serialized, and stop requests
survive initialization without a later transition back to running. Console and service
entry points preserve startup/main-loop failures through cleanup and return normal
process exit codes. Graceful socket draining has a five-second budget. Cleanup joins
workers, detaches session records while sockets are alive, then closes the transport
before clearing the message handler and packet allocator. Dump handling stops before
the logger. During draining the guard only pumps network completions. Console close
notifications wait up to four seconds for main-thread cleanup before returning to Windows.

The console editor owns its text and rejects complete overlong lines; extended keys and
backspace cannot turn a truncated suffix into a command. `test_tool_guard_runtime`
compiles the actual entry point against in-memory SCM, transport, process and dump
boundaries. It covers startup failures, exceptions, concurrent service controls, graceful
and timed-out draining, service path quoting, and cleanup order without registering a
Windows service. `test_tool_guard_console` checks the editor independently; packet tests
verify that shutdown rejects business messages while keeping receive ownership valid.
Build these targets with `guardserver`, then run `ctest -C Debug -R '^tool_guard_'`
with `--output-on-failure`, and repeat with Release.

Certserver uses an owned compressed certificate vector and scoped
packet ownership, checks zlib errors and the complete packet size, frees unexpected
receives, and retains the allocator while transport shutdown callbacks finish.
`test_tool_cert_packets` compares the reply bytes with legacy zlib compression and
round-trips the entire math table. It also exercises allocation/send failures,
shutdown, callback cleanup and incompressible input through memory sockets. The
certificate key, wire structure and compression algorithm are unchanged. Its CMake
target uses strict C++23.

Certserver service status is serialized and stop requests are atomic. RUNNING is
reported only after the listener opens; startup, main-loop and cleanup failures
reach both the final SCM status and process exit code. Scoped ownership releases
the acceptor, transport, handler, allocator and logger in that order, including
partial initialization and exceptional failure paths. The handler's shutdown gate
rejects new replies while retaining receive ownership through transport teardown.
Installation uses an owned, growing Unicode executable path with quotes and scoped
SCM handles. The service identity, automatic LocalSystem startup, port 8111 and
transport capacities remain unchanged. No working-directory change was introduced.

`test_tool_cert_runtime` compiles the actual entry point and handler against private
in-memory SCM/network/allocator boundaries. It tests startup failures, callbacks
during teardown, cleanup failure propagation, concurrent service controls and
install/remove argument handling without registering a service or opening a port.
`test_tool_cert_math` compares every serialized math-table byte with a retained
legacy generator. The standard math/array implementation deliberately preserves
the original PI constant and rounding. Run `ctest -C Debug -R '^tool_cert_'` with
`--output-on-failure`, and repeat for Release. CMake-generated Visual Studio
projects are the maintained C++23 entry point; the old VS2008 `.vcproj` is retained
as legacy metadata with its source list updated.

`account_logout` now uses standard-library streams, paths, containers, formatting
and checked hexadecimal parsing without server-library dependencies. It reads
`db_file/session.txt`, `db_file/user.txt`, `db_file/logout.txt` and
`dl_file/AccountLogin.txt` beneath the input directory. The default directory stays
`../account_logout`; `--input directory` selects another location and `--help`
prints usage. Windows arguments use the Unicode entry point. Input files need only
read access and username bytes retain their existing encoding, including GBK.

Valid report text and ordering, the first duplicate-name match, and the zero-ID
sentinel behavior remain unchanged. The obsolete pool teardown warning disappears.
Success now exits zero and failures exit one. Full unsigned 32-bit session IDs are
preserved, malformed/overflowing IDs and mismatched exports report their location,
and names longer than the legacy 63-byte limit fail instead of truncating. UTF-8
BOMs and empty exports are handled. Standard indexes remove the 65,535-record pool
limit and quadratic searches. `test_tool_account_logout` covers analysis and actual
CLI behavior on disposable files, including read-only/Unicode paths and 70,000 rows.
The maintained standalone project also selects strict C++23 with exceptions.

## Maintenance tool

The Qt application owns its runtime and window in scope. Sessions own process vectors,
statistics and upload state; packet, file and callback owners remain alive through
native sends and shutdown. Transfers validate source and chunk metadata and acknowledge
the final chunk only, preserving the guard protocol. Disconnect cancels incomplete work.
Logging copies borrowed text into a bounded queue and delivers plain text on the UI thread.

Area/version/patch discovery uses native filesystem paths and staged configuration/model
updates. Malformed or unreadable inputs preserve the previous view. Exports use checked
standard streams and a staging file. Statistics use checked arithmetic and PID identities;
ring histories retain chronological order across slot reuse. Credentials and commands are
owned, byte limits are checked before serialization, and failed login remains editable.
Filesystem URLs use Qt's native URL API. Framework-owned widgets remain parent-owned.

Build `maintaintool` and its `test_tool_maintain_*` targets, then run
`ctest -C Debug -R '^tool_maintain_' --output-on-failure` and repeat for Release.
Fixtures cover actual offscreen Qt controls, model/configuration publication, exports,
statistics, uploads, callback reentry and startup/shutdown using private native boundaries.

## GM tool

GM tool uses standard containers, strings, spans, checked parsing/formatting, streams,
filesystem paths and scoped owners throughout its configuration, role/item data,
command workflows, logging, file transforms and runtime. Qt continues to own parented
widgets; native transport, SCM and Windows APIs remain explicit platform boundaries.
Configuration is built as an owned candidate before publication. Role edits retain
wire tags, lengths and the existing 32 KiB limit; decoders check enclosing spans before
reading packed fields. Initialized message builders preserve packet layouts and encoding.

Commands use typed shared owners retained through sends, replies and modal UI callbacks.
Reply ledgers identify each issued peer and consume each expected reply once. Partial
broadcasts wait for accepted peers only and are never automatically retried. Role/name
continuations retain their original peer rather than adopting a later selection. Reports
use owned rows, checked streams and native paths; incomplete batches preserve prior output.
Unused private command implementations were removed after checking their consumers.

Sessions have a distinct owned identity for every connection, including reconnects that
reuse a native socket address. Sends snapshot it before allocator callbacks. Replies,
login attempts, role editors, UI confirmations and cached exchange dialogs retain that
identity and recheck it after callbacks. Old work cannot act on a replacement binding.
Role editors also retain cache-entry identity so an old editor cannot modify or erase a
replacement role. Native callbacks expose only a socket pointer: the wire protocol has
no request ID, so indistinguishable late replies to a later same-kind request cannot be
identified without a protocol change. Runtime-generation and connection checks do not
add such an identifier to the wire.

Login requests have a finite timeout and connection watcher; validation uses UTF-8 byte
limits before serialization. User actions capture owned values before confirmations and
block reentrant submissions. The runtime unwinds partial startup in reverse order and
keeps transport dependencies alive through shutdown callbacks. Logging uses typed
`std::format`, owned plain text and explicit legacy decoding; percent signs stay data.

Exchange tables and history own bounded snapshots. Rendering clips visible points and
uses checked timestamp/price arithmetic. Polls use independent monotonic deadlines;
connection or selection changes stop remaining requests. Order inputs retain integer
hundredths and full-width IDs. Scoped Qt dialogs and parent-owned timers replace manual
modal lifetimes and polling. Role-file readers stage complete results before publication.

Build `gmtool` and the `test_tool_gm_*` targets, then run
`ctest -C Debug -R '^tool_gm_' --output-on-failure` and repeat for Release.
The 24 fixtures include actual offscreen Qt dialogs, command/reply routing, wire bytes,
malformed data, configuration, files, rendering, allocation/send failures, reconnects,
reentry, runtime teardown and the production entry point's bounded idle path. Native
boundaries are private fakes except for that idle check; no live GM operations are issued.

## Encrypt

Encrypt uses owned standard buffers, streams and native filesystem paths. It supports
files beyond the old 1 MiB buffer, checks the format's 32-bit limits and retains the exact
little-endian length, padding and chained-XOR bytes. Encryption writes a decimal CRC32
filename in the current directory, hashing the basename up to its first dot in the original
Windows ACP encoding. Unrepresentable basenames fail explicitly; Unicode parent and
decryption paths remain native. Decryption replaces its selected file.

Exclusive C++23 `ios::noreplace` staging files are closed and checked before publication.
Windows replacement uses a small native boundary; read, transform, write and publication
errors preserve existing output in the tested failure cases. Scoped shell-manager and
operation owners protect MFC callbacks, reentry and window recreation. Build `encrypt`,
`test_tool_encrypt_files`, `test_tool_encrypt_dialog` and `test_tool_gm_file_transform`,
then run `ctest -C Debug -R '^tool_(encrypt_|gm_file_transform)' --output-on-failure`.
Repeat for Release. Tests compare original bytes and exercise actual hidden MFC controls.

## Position log viewer

PosLog parses into an owned map/vector model before replacing the view. `from_chars` and
chrono validate full unsigned scene IDs, dates/times and 64-bit coordinates. CR, LF, CRLF
and a final unterminated record are accepted; oversized rows and embedded NUL fail.
Hour-only grouping across dates and the original integer projection are preserved.
Out-of-scene positions are skipped before arithmetic; pixels are clipped before indexing.

The 512/1024 bitmap output retains the exact 54-byte little-endian header, bottom-up rows
and 24-bit BGR pixels. Standard buffers and exclusive staged publication replace raw pixel
allocation and C streams. Typed row keys preserve complete scene IDs. Cancelled/failed
loads preserve the view, and saves capture positions/size before the file picker, use
native Unicode paths and append `.bmp` only when missing. The parent-owned canvas reads
weak model state; native window identity guards destruction/recreation callbacks.

Build `PosLog`, `test_tool_poslog` and `test_tool_poslog_dialog`, then run
`ctest -C Debug -R '^tool_poslog' --output-on-failure` and repeat for Release.
Fixtures compare bitmap bytes and exercise the actual hidden MFC resource, controls,
notifications and canvas, with file choices/error presentation captured.

## Tools validation and boundaries

All 15 maintained executables use strict C++23 with consistent exception and source/wire
encoding settings. Enable `CQYH_BUILD_TOOLS`; additionally enable `CQYH_BUILD_QT_TOOLS`
and configure Qt5 for the two Qt tools. Windows MFC is needed for Encrypt and PosLog.
Build all maintained tools and `test_tool_*` targets before running
`ctest --test-dir <build> -C Debug -R '^tool_' --output-on-failure`, then repeat for Release.
The generated CMake solution includes the fixture targets. Shared baselib, frozen protocol
layout and zlib compatibility regressions supplement the tool suite.

The completed migration was validated with all 15 tool builds in Debug and Release,
56 tool regressions plus 20 shared utility/protocol/zlib regressions in each configuration,
and a final refresh of the 34 affected GM/guard Debug fixtures. The four affected GM
retained-connection fixtures also pass AddressSanitizer. The six existing GM GUI golden
images match in both configurations. Compiler command logs confirm the standard,
conformance, exceptions and source/wire encoding settings for every maintained target.

GUI tests use Qt offscreen or hidden actual MFC controls with private files. Service,
process, transport and database fixtures use private boundaries; production deployment,
service registration, real database changes and live GM actions are not acceptance-test
requirements and were not performed. Targeted AddressSanitizer runs instrument first-party
code; Qt/MFC/native support libraries remain uninstrumented and leak detection is disabled.
Legacy wire/file limits, report formats and native deployment/transaction limitations
are described above. The duplicate `gmtool - 副本` remains outside the maintained targets.

## Networking

The client facing side (`gateserver`, `balanceserver`, `mgateserver`) uses the asio transport in
`NewServer/src/net/asio_net.cpp` (`netAsio`), which replaced the winsock IOCP and epoll implementations.
Server-to-server traffic still goes through `netIntranet` / `netSimple` (shared memory + a `select()` based reactor).
`NewServer/test/asio_net` is an integration test of the transport.

