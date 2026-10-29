# Building the server

One build system: CMake (3.25+, the copy bundled with Visual Studio 2022 is fine). C++23, x64.
The old makefiles and Visual Studio project files are gone; Visual Studio and the command line both use the
`CMakeLists.txt` files in this tree.

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

Executables are written to `NewServer/bin/<server>/<server>64[_d].exe`, where the configs and scripts are, exactly as
the old projects did. Pass `-DCQYH_OUTPUT_TO_BIN=OFF` to keep them in the build directory instead.

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
  CMake's server and test targets compile with `/permissive-`.
* CMake's server and test MSVC targets enable STL exceptions consistently and use `/EHsc`.
  Baselib retains `/EHa` for its existing SEH wrappers. Precompiled headers no longer disable
  STL exceptions independently of their callers or the Asio transport.
* The source lists in the `CMakeLists.txt` files are explicit. A new `.cpp` has to be added there.
* `CQYH_MSVC_COMPILE_PROCESSES` limits compiler workers per MSBuild target (default 2). sol2 templates need
  substantial compiler memory; increase this only when the machine has enough memory for parallel targets too.
* `nameserver` remains opt-in (`-DCQYH_BUILD_NAMESERVER=ON`), since it was not in the old solution.
  It is validated in strict C++23 mode alongside the other servers.
* `NewServer/src/asimov` (Qt robot client), `bteditor` and `gameworld/script/script.vcxproj` (the Lua
  bytecode compiler step) remain outside the server CMake build.
* Source files use UTF-8. MSVC emits narrow string literals using GBK for compatibility with the legacy server.

## Baselib utility regression tests

`ctest -R baselib_utilities --output-on-failure` tests the server's directory and string utilities, including truncation, fixed-width source fields, non-ASCII
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

`ctest -R baselib_resources --output-on-failure` exercises the server implementation,
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

## Networking

The client facing side (`gateserver`, `balanceserver`, `mgateserver`) uses the asio transport in
`NewServer/src/net/asio_net.cpp` (`netAsio`), which replaced the winsock IOCP and epoll implementations.
Server-to-server traffic still goes through `netIntranet` / `netSimple` (shared memory + a `select()` based reactor).
`NewServer/test/asio_net` is an integration test of the transport.

