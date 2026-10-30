# Legend Eternal (CQYH) – Source Code

The 2016 Shanda Games *Legend Eternal* (传奇永恒 / CQYH) source code: the Unreal Engine 3 client and the server, split into `Client` and `Server`.

> **The client assets are not in this repository.** The game content and prebuilt binaries (about 64 GB) are too large for GitHub and are hosted separately. See [Client assets](#client-assets-required-to-build-and-run-the-client) below.

## Client assets (required to build and run the client)

This repository contains the client **source code only**. The game assets and prebuilt binaries the client needs are in a single archive:

**[`Client Assets.7z`](https://mirfiles.co.uk/resources/mir2/users/Jev/Legend%20Eternal/Organised/Client%20Assets.7z)**, hosted in the [Organised files folder](https://mirfiles.co.uk/resources/mir2/users/Jev/Legend%20Eternal/Organised/).

It contains the cooked game content (`.upk`, `.udk`, `.pak`, `.bik` videos, the `CookedPC` and `Content` folders), prebuilt third-party libraries (for example the Scaleform GFx `.lib` files), executables and DLLs, compiled UnrealScript packages (`.u`), the Mac binaries and the QA build tools.

**How to use it:**

1. Download `Client Assets.7z` and extract it with [7-Zip](https://www.7-zip.org/).
2. Extract it **into the `Client` folder of this repository**, so the folders merge: the archive uses the same relative paths as the repository (`client\UE3\MMOGame\CookedPC\...`, `client\UE3\Development\External\...` and so on).
3. There is nothing to configure: the extracted files are already ignored by `Client/.gitignore` (see the "Client assets" block at the end of that file), so git will not pick them up.

Without the assets you can read and edit the source, but you cannot build or run the client.

The same folder also hosts the other files from this release: packaged client builds, packaged server builds and tool installers. The setup and compile notes, database files and design documents are in this repository, in the [`Documentation`](Documentation) folder.

## Repository layout

`Client` and `Server` are independent: neither needs the other to compile. Both compile against the shared `Common` folder, so each one carries its own copy.

**`*GitIgnored*`** marks files and folders that come from [`Client Assets.7z`](#client-assets-required-to-build-and-run-the-client) (63.65 GB, 29,650 files). They sit in the normal places in the tree so everything builds, but `Client/.gitignore` stops git from committing them. `(partly GitIgnored)` means the folder itself is committed and only the prebuilt files inside it are ignored.

```text
Client/
├── client/                              The UE3-based game client
│   ├── UE3/
│   │   ├── Development/                 (partly GitIgnored)
│   │   │   ├── Src/                     C++ and UnrealScript source (UE3.sln, MMOGame.sln)
│   │   │   ├── External/                Bundled third-party libraries
│   │   │   │                            *GitIgnored*: prebuilt .lib/.dll files (Scaleform GFx, 4.5 GB)
│   │   │   ├── Tools/                   UnrealBuildTool and engine tools
│   │   │   ├── Flash/                   Flash / Scaleform UI source (*GitIgnored*: .swf/.fla)
│   │   │   └── Documentation/           *GitIgnored*: UnrealEngine3.chm (over 100 MB)
│   │   ├── Engine/                      Config, shaders, localization
│   │   │   └── Content/                 *GitIgnored*: engine content packages (.upk)
│   │   ├── MMOGame/                     Game config and localization
│   │   │   ├── CookedPC/                *GitIgnored*: cooked game packages (10.6 GB)
│   │   │   ├── Content/                 *GitIgnored*: game content (33.6 GB)
│   │   │   ├── Movies/                  *GitIgnored*: .bik videos (2.6 GB)
│   │   │   ├── Flash/                   *GitIgnored*: UI .swf/.fla files (1.5 GB)
│   │   │   └── Script*/                 *GitIgnored*: compiled UnrealScript packages (.u)
│   │   ├── Binaries/, BinariesRelease/  (partly GitIgnored) runtime support files
│   │   │   ├── Win32/, Win64/           *GitIgnored*: prebuilt .exe/.dll files
│   │   │   └── Mac/                     *GitIgnored*: Mac binaries
│   │   ├── Install/                     *GitIgnored*: redistributable installers (.exe/.cab)
│   │   └── Launcher*/, SDO/             Launcher and SDO login (*GitIgnored*: .exe/.dll)
│   ├── DevTools/                        DevLauncher, ReplayView
│   ├── PatchTools/                      FixClient, FileUpdate
│   ├── OtherTools/                      dump_analyze (crash dump analyser)
│   ├── PackageTool/                     Packaging scripts (*GitIgnored*: bundled .exe/.dll)
│   ├── ClientBuildServer/               Cook and packaging scripts (*GitIgnored*: bundled .exe/.dll)
│   └── SDOTest/, AODBenchmarkLaunch/    Small test and benchmark projects
├── Common/                              Shared protocol and data definitions (define, message, fusion)
├── _AutoBuild/                          Build-server scripts and packaging toolchain
│   ├── aod_pack/                        (partly GitIgnored) toolchain; bundled .exe/.dll are *GitIgnored*
│   └── edition/_CQ_S1/client_qa_build/  *GitIgnored*: UE3 dev/editor binaries (1.3 GB)
├── DevLauncher.exe                      *GitIgnored*: prebuilt tools
├── dump_analyze.exe                     *GitIgnored*
└── MsgBox.exe                           *GitIgnored*

Server/
├── CMakeLists.txt             Server build: CMake, C++23 (see Server/BUILD.md)
├── cmake/                     Shared CMake modules (compiler flags, third-party libs, asio)
├── NewServer/
│   ├── src/                   Server source (CMake): gameserver, gateserver, loginserver,
│   │                          dbserver, gmserver, manageserver, nameserver, balanceserver, ...
│   ├── bin/                   Runtime layout: commonconfig (game scripts and data tables),
│   │                          start/stop scripts, configs
│   ├── 3rd/                   Third-party libraries (MySQL, Lua, sol2, zlib, oasdk, ...)
│   ├── inc/, lib/             Shared headers and libraries
│   ├── tools/                 gmtool, maintaintool, guardserver, certserver and other tools (CMake, built from the same solution)
│   └── test/                  Test programs (asio_net: transport integration test)
├── Common/                    Shared protocol and data definitions (newest version)
└── Common1/                   Alternative "new protocol" variant (not used by the build)
```

### Things to know

- **Keep the folder layout.** Project files reach `Common` with paths such as `..\..\..\Common`, so `client` / `NewServer` and `Common` must stay side by side inside `Client` and `Server`.
- **Two `Common` copies.** `Client/Common` and `Server/Common` are separate; a change in one does not carry to the other.
- **Compiled server executables are not included.** Packaged server builds are in the [Organised folder](https://mirfiles.co.uk/resources/mir2/users/Jev/Legend%20Eternal/Organised/) under `Server Builds`.
- **Two things need `Client` and `Server` side by side.** The server tool `gen_msg` writes `ServerEnum.uc` and `MMOBT*.ini` into the client folder, and the client cook scripts in `ClientBuildServer` copy `NewServer\bin\commonconfig`. They do not work with the two apart.

### Dependencies

Visual Studio and the DirectX SDK come from Microsoft. Everything else is hosted in the [Organised files folder](https://mirfiles.co.uk/resources/mir2/users/Jev/Legend%20Eternal/Organised/):

| Needed for | Dependency | Download |
|---|---|---|
| Server build | Visual Studio 2022 (CMake 3.25+ is bundled) Windows SDK: 10.0 Platform Toolset: v143 | Microsoft (not hosted) |
| Client build | Visual Studio 2022 Windows SDK: 10.0 Platform Toolset: v143 (workloads: Desktop development with C++, .NET desktop development with the .NET Framework 4.8 targeting pack), DirectX SDK (June 2010) | Microsoft (not hosted) |
| Database / server C API | MySQL 26.10.0 (x64); the shared C API client is bundled in `Server/NewServer/3rd` | [Official MySQL downloads](https://dev.mysql.com/downloads/mysql/) and [server setup notes](Server/BUILD.md#mysql) |
| Database tool | Navicat for MySQL 12.1 (64-bit) | [`Navicat Install.7z`](https://mirfiles.co.uk/resources/mir2/users/Jev/Legend%20Eternal/Organised/Navicat%20Install.7z) |
| Client troubleshooting | DirectX repair tool (blank-screen and DirectX errors on old clients) | [`DirectX 4.2 Repair Tool.7z`](https://mirfiles.co.uk/resources/mir2/users/Jev/Legend%20Eternal/Organised/DirectX%204.2%20Repair%20Tool.7z) |
| Dev environment | Python 2.7.8, Perforce P4V, XAMPP 1.8.3 | [`Dev Tools Install.7z`](https://mirfiles.co.uk/resources/mir2/users/Jev/Legend%20Eternal/Organised/Dev%20Tools%20Install.7z) |
| Setup and compile notes | Build, database and runtime guides, SQL files, design documents | [`Documentation/`](Documentation) folder in this repository |

- **MySQL:** use the [modern schema and matching data](Database/mysql-modern/README.md) with the rebuilt server. The archived MySQL 5.1/5.5 installers and original database dumps belong to the historical release.
- **Navicat** is commercial software and needs a valid licence; the official version is on the [Navicat website](https://www.navicat.com/en/products#navicat). Any MySQL client works if you prefer another.
- **Legacy client problems:** if the DirectX SDK installer fails, uninstall the Microsoft Visual C++ redistributables (2005 to 2022), reinstall the SDK, then run the DirectX repair tool. The details are in the [`Documentation`](Documentation) folder.
- Packaged client builds, packaged server builds and the rest of the release are in the same folder.

### Building the client with Visual Studio 2022

Everything in `Client` opens and builds from Visual Studio 2022; Visual Studio 2010 is no longer required.

**Build order: engine first, then the client scripts.**

1. Open `UE3.sln` and build the engine (`MMOGame Win32` for a 32-bit client). This also produces `UnrealBuildTool` on the way and writes the game executable to `UE3\Binaries\Win32`.
2. Then open `MMOGame.sln` and build it to compile the UnrealScript (`.uc` to `.u` packages in `UE3\MMOGame\Script`). It cannot run before step 1 because it needs the game executable; if none is found it stops with a message saying so.
3. The cooked game content comes from [Client Assets](#client-assets-required-to-build-and-run-the-client), not from a build.

- **`UE3.sln`** (in `client\UE3\Development\Src`) builds the engine and game executables. Its projects use toolset v143 and Windows SDK 10.0 and hand the actual compile to `UnrealBuildTool`, which now finds Visual Studio 2022 itself (via `vswhere` and `vcvarsall.bat`) instead of requiring Visual Studio 2010. `UnrealBuildTool` is built from source automatically the first time it is needed (`Targets\Build.bat`), so there is no separate step for it. Build the target you need, for example `MMOGame Win32` (output: `UE3\Binaries\Win32\MMOGame-Win32-<Configuration>.exe`).
- **`MMOGame.sln`** is the UnrealScript project. The old `.ucproj` needed a Visual Studio 2010 extension, so it was replaced by `MMOGameScript.vcxproj`: it lists the `.uc` sources and its Build runs the UE3 script compiler (`MMOGame.exe make -auto -NOPAUSEONSUCCESS -full -DEMOMODE`) using the first `MMOGame.com` / `MMOGame.exe` it finds in `UE3\Binaries\Win32` or `Win64`. Visual Studio has no UnrealScript syntax highlighting; use an editor with an UnrealScript extension for that.
- The C# projects target .NET Framework 4.8, because Visual Studio 2022 does not ship the 3.5 and 4.0 targeting packs.
- The DirectX SDK (June 2010) is still required: `UnrealBuildTool` reads it through the `DXSDK_DIR` environment variable.
- Environment variables: set `UE3_VS_INSTALL_DIR` to pick a specific Visual Studio installation, or `UE3_USE_VS2010=1` to fall back to the original Visual Studio 2010 toolchain.
- **Status:** the engine and game (`MMOGame Win32`, Debug) compile and link with Visual Studio 2022 (v143), in about 1.5 minutes after the first build. The executable built here does not yet start with the Client Assets content; see "Known problems" under [Opening the client in the UE3 editor](#opening-the-client-in-the-ue3-editor-prebuilt-map-editor).
- **Build only `MMOGame Win32`.** In `UE3.sln`, *Build Solution* is set to build `MMOGame Win32` and what it needs. `UDKGame` (Win32/Win64/Mac), `UDKGameDLL`, `MMOGame Win64` and `MultiBuild` are still listed but not built: there is no Mac or UDK content here, and they only produce errors.
- **Do not commit a regenerated `MMOGameClasses.h`.** `Development\Src\MMOGame\Inc\MMOGameClasses.h` is generated by the script compiler. Running the script `make` (`MMOGame.sln`), or starting the game/editor while `MMOGame\Script\Manifest.txt` is missing (which triggers an automatic `make`), rewrites it, and the regenerated copy makes `UNPCTemplate::DisplayName` private, so `MMOFacade.cpp` stops compiling (`C2248`). If that happens restore it with `git checkout -- Client/client/UE3/Development/Src/MMOGame/Inc/MMOGameClasses.h`.
- **Why the script compiler keeps re-running.** `make` only writes `MMOGame\Script\Manifest.txt` when it finishes with *zero* warnings. This source compiles with 12 harmless ones (ten missing per-package `Globals.uci` files and two missing `Core.Progress` strings), so the manifest is never written and the next start compiles scripts again. The manifest is generated and is in `.gitignore`.
- **The Debug exe needs `MMOGame-Win32-Debug.exe.config`** next to it (the editor refuses to start without it: `MissingExeConfigFileError`). The prebuilt editor bundle includes one.
- The `MMOGame.exe` built from this source with the Release configuration also needs the thread-safe-statics switch off (`/Zc:threadSafeInit-`, already set in the build tools), otherwise start-up deadlocks in `FReimportManager::Instance()`.
- **Third-party libraries are Visual Studio 2010 builds**, so a few things are done at build/link time rather than by rebuilding them:
  - FBX import is left out of Visual Studio 2017+ builds (the bundled FBX SDK rejects newer compilers).
  - `Core\Src\ModernMSVCShims.cpp` and `UnrealEd\Src\WxImageShims.cpp` redirect a few runtime functions the old libraries import (`__iob_func`, `std::_Container_base12`, the `wxImage` move operations) to equivalents.
  - The linker rejects objects built with different `_MSC_VER` values, so the `/FAILIFMISMATCH:_MSC_VER=1600` marker inside some prebuilt `.lib` files (`recastd`, `squishd`, `FxSDKd`/`FxCGd`/`FxAnalysisd`, `nvTriStripD`, `NxTetraDebug`, `ConvexDecompositionDEBUG`) was changed to `1900`. These `.lib` files are part of **Client Assets**, so use the ones from the Client Assets download that carry this change.
  - Other configurations (Release/Shipping, Win64) may need the same treatment for their library variants; the first link error names the library.

### Opening the client in the UE3 editor (prebuilt map editor)

The editor that is known to work is the **prebuilt 2025 "Win32 - Modified and Compiled" map editor**, run against the full client tree. It is not an editor built from this source: that is what the release's `Map Editor Tool Description` describes. The steps below are the setup of a contributor who runs it successfully (`MMOGame.exe editor -log -NoGADWarning -dx9 -LANGUAGE=ENG`). The editor binaries are build output and are not in git.

1. Put [Client Assets](#client-assets-required-to-build-and-run-the-client) in place, and also the **retail client** (`Client 1.0.0.2 104129`): its `MMOGame\Config` data tables and `CookedPC` extras are missing from Client Assets.
2. Extract the **`Win32 - Modified and Compiled`** folder from the release's `Map Editor Tool Description`. It holds 43 files: `MMOGame.exe`, `MMOGame-Win32-Debug.exe`, the custom wxWidgets 2.8.9 DLLs and build leftovers. Overlay it onto `Client\client\UE3\Binaries\Win32` with `Client\Setup-PrebuiltEditor.ps1 -BundleDir "<path to that folder>"` (it copies only `.exe`, `.dll`, `.config` and backs up what it replaces).
3. Use the genuine `UE3ShaderCompileWorker.exe` (56 KB, 2015). The 274 KB 2020 file in Client Assets is a different program: it prints a `cmd` banner and exits. Pass it with `-ShaderWorker`.
4. In `MMOGame\Config\DefaultEditorUserSettings.ini` and `MMOEditorUserSettings.ini` set source control to `Disabled=True` and `AutoAddNewFiles=False`.
5. Install, or check for, the official runtimes: Visual C++ 2010 SP1 x86, DirectX End-User Runtimes (June 2010) and .NET Framework 4.x. A missing `mss32.dll` (Bink, delay-loaded) does not matter in editor mode.
6. Run `Client\Launch_Editor_DX9.bat` (use DX9 first; `Launch_Editor_DX11.bat` is the second stage). Open `Content\Maps\testgrid.udk` first, then `Content\Maps\_MMOMaps\Mir2\MirDg09\MirDg09.udk`.
7. **Do not save.** Do not use Save All, ESMAP/ESSCENE, any Build command, cooking or commandlets. Answer *No* to every "save modified packages" prompt. Consider blocking the exe's outbound network access in the firewall for the first launch: the binary talks to historical login/P4/web endpoints.

**Client source patch.** The release also says to replace `ScaleformEngine.cpp` in the client source and recompile. That patch is already applied here (`Development\Src\GFxUI\Src\ScaleformEngine.cpp`). It has not been rebuilt into a working editor.

**Known problems (not solved yet)**
- **The editor built from this source does not start.** `MMOGame-Win32-Debug.exe` built here reaches `FEngineLoop::PreInit` and then fails on `FX_Warrior_SE_Alondra.upk` (`Bad name index 8192/449`) and on the data tables (`StringMap.txt`, `CTabFile::Init`, UTF-16 BOM check). Use the prebuilt editor above until this is fixed.
- **`MMO*.ini` are generated files.** The engine rewrites them as `[IniVersion]`-only stubs (about 37 bytes) when it decides they are stale, and keeps the stubs if the start fails. After any failed start restore them (the complete copies are in the first import commit `f1411e9a`). The complete set a working editor needs includes `DrawSilhouettes` in `MMOSystemSettingsHD.ini`.
- When the prebuilt editor is run from a git working copy it still stopped in `FSystemSettings::LoadFromIni` with `Couldn't find system setting DrawSilhouettes in Ini section SystemSettingsEditor`, even with the contributor's `MMO*.ini` copied in and matching file timestamps. The contributor's own tree does not show this, so some difference between the two trees has not been found yet.
- Do not commit regenerated `*Classes.h` files (see the build notes above).
### Building the server with Visual Studio 2022

The servers **and the tools** build with CMake (x64, C++23) from one solution, **`Server\build\vs2022\CQYHServer.sln`**. Build everything from that solution. The old makefiles, the old per-project `.vcxproj` files and the per-tool `.sln` files (`gmtool.sln`, `guardserver.sln`, ...) are gone or stale and no longer work, because the libraries they referenced were removed.

1. In the Server folder run `cmake --preset vs2022` (the CMake bundled with Visual Studio 2022 is fine). This generates (or refreshes) `CQYHServer.sln`. The first configure downloads asio.
2. Open `CQYHServer.sln` in Visual Studio 2022 and build the Debug or Release configuration. (Or use `File > Open > Folder...` on `Server` and pick the `vs2022` preset.)
3. The executables are written to `NewServer\bin\<project>\` as `<name>64.exe` / `<name>64_d.exe`.

The solution groups the projects into folders: `Servers`, `Tools`, `Tests` and `Dependencies` (`Libraries` for baselib, fusion, net and gameworld; `ThirdParty` for Lua and zlib).

- The tools (`guardserver`, `exportdb`, `certserver`, `pak`, ...) are in the `Tools` folder. `gmtool` and `maintaintool` need Qt5: they are listed but excluded from the build until you configure with `-DCMAKE_PREFIX_PATH=<your Qt5 install>`. `encrypt` and `PosLog` need the MFC component of Visual Studio.
- `Server\build\vs2022` is committed so the solution can be opened straight away, but it contains absolute paths from the machine that generated it. After cloning to another location, delete `build\vs2022\CMakeCache.txt` and run `cmake --preset vs2022` again. Add a source file to the matching `CMakeLists.txt` and re-run that command; do not edit the generated `.sln` or `.vcxproj` files, because they are overwritten.

The client-facing network layer (gateserver, balanceserver, mgateserver) uses asio. `ctest --preset vs2022-debug` runs its integration test. Options, what is not covered (asimov, bteditor) and the GBK source-encoding note are in [`Server/BUILD.md`](Server/BUILD.md).

---
## 🔗 Quick Links

### Project Resources
- 🌐 [Legend Eternal Database](https://eternalmir.co.uk/) - by [DontReallyMind](https://www.lomcn.net/forum/members/dontreallymind.4351/)
- <img src="https://cdn.prod.website-files.com/6257adef93867e50d84d30e2/66e3d80db9971f10a9757c99_Symbol.svg" alt="Discord" width="20"/> [Discord](https://discord.gg/KahjsD7fhn)

### Official Resources

- 🌐 [Shanda Games - Legend Eternal Website](https://cq.web.sdo.com/)

### LOMCN Community

- 💬 [Development Section](https://www.lomcn.net/forum/forums/mir-3d-emulation.812/)
- 📚 [Tutorials](https://www.lomcn.net/forum/forums/mir-3d-tutorials.852/)
- 📦 [Releases](https://www.lomcn.net/forum/forums/mir-3d-releases.853/)
- 🐞 [Bug Reports](https://www.lomcn.net/forum/forums/mir-3d-bug-reports.813/)
- 🛠️ [Updates](https://www.lomcn.net/forum/forums/mir-3d-updates.814/)
- 📖 [Wiki](https://www.lomcn.net/wiki/index.php/Mir3DEmu/)

---

## 🙏 Special Thanks

- [DontReallyMind](https://www.lomcn.net/forum/members/dontreallymind.4351/)
- [CraZyEriK](https://www.lomcn.net/forum/members/crazyerik.9944/)
- [Armifer (ElAmO)](https://www.lomcn.net/forum/members/elamo.10165/)
- [Damian (CodePwr)](https://www.lomcn.net/forum/members/damian.1126/)
- [Jev](https://www.lomcn.net/forum/members/jev.29880/)
- [mir2pion](https://www.lomcn.net/forum/members/mir2pion.19657/)
- [Xander](https://www.lomcn.net/forum/members/xander.2473/)
- [Chriz](https://www.lomcn.net/forum/members/chriz.86/)

---

<h2 align="center">
  <img src="https://www.lomcn.net/forum/styles/uix/images/logo2024_100.png" width="32">
  Other Projects
</h2>

<table align="center" width="100%">

  <tr>
    <td width="48" align="center" valign="top">
      <a href="https://github.com/JevLOMCN/mir1/">
        <img src="https://raw.githubusercontent.com/JevLOMCN/mir4/main/Tools/icons/mir1.png" width="32" height="32">
      </a>
    </td>
    <td>
      <b>Mir 1</b> •
      <a href="https://github.com/JevLOMCN/mir1/">Project</a> •
      <a href="https://github.com/Suprcode/Carbon.Database">Database</a><br>
      Remake of ActozSoft's 1997 <i>The Legend Of Mir 1</i>
    </td>
  </tr>

  <tr>
    <td width="48" align="center" valign="top">
      <a href="https://github.com/Suprcode/Crystal">
        <img src="https://raw.githubusercontent.com/JevLOMCN/mir4/main/Tools/icons/mir2.png" width="32" height="32">
      </a>
    </td>
    <td>
      <b>Mir 2</b> •
      <a href="https://github.com/Suprcode/Crystal">Project</a> •
      <a href="https://github.com/Suprcode/Crystal.Database">Database</a> •
      <a href="https://github.com/Suprcode/Crystal.MapEditor">Map Editor</a><br>
      Remake of ActozSoft / Wemade Entertainment's 1999 <i>The Legend Of Mir 2</i>
    </td>
  </tr>

  <tr>
    <td width="48" align="center" valign="top">
      <a href="https://github.com/Suprcode/Zircon">
        <img src="https://raw.githubusercontent.com/JevLOMCN/mir4/main/Tools/icons/mir3.png" width="32" height="32">
      </a>
    </td>
    <td>
      <b>Mir 3</b> •
      <a href="https://github.com/Suprcode/Zircon">Project</a> •
      <a href="https://mirfiles.com/resources/mir3/zircon/Database.7z">Database</a> •
      <a href="https://www.lomcn.net/forum/threads/map-editor.109317/">Map Editor</a><br>
      Remake of Wemade Entertainment's 2003 <i>The Legend Of Mir 3</i>
    </td>
  </tr>

  <tr>
    <td width="48" align="center" valign="top">
      <a href="https://www.lomcn.net/forum/forums/woool-development-project-onyx.857/">
        <img src="https://raw.githubusercontent.com/JevLOMCN/mir4/main/Tools/icons/woool.png" width="32" height="32">
      </a>
    </td>
    <td>
      <b>WoOOL</b> •
      <a href="https://www.lomcn.net/forum/forums/woool-development-project-onyx.857/">Project</a> •
      <a href="https://www.lomcn.net/forum/threads/nmp-viewer-editor.114580">Map Editor</a><br>
      Remake of Shanda Games' (Shengqu Games) 2003 <i>The World Of Legend</i>
    </td>
  </tr>

  <tr>
    <td width="48" align="center" valign="top">
      <a href="https://github.com/mir-ethernity/mir-eternal">
        <img src="https://raw.githubusercontent.com/JevLOMCN/mir4/main/Tools/icons/mir3d.png" width="32" height="32">
      </a>
    </td>
    <td>
      <b>Mir 3D</b> •
      <a href="https://github.com/mir-ethernity/mir-eternal">Moon Spirit</a> •
      <a href="https://github.com/JevLOMCN/Eternal-Legend">Holy Cow</a> •
      <a href="https://github.com/JevLOMCN/CQYH">Official</a><br>
      Remake of Shanda Games' 2016 <i>Legend Eternal</i>
    </td>
  </tr>

  <tr>
    <td width="48" align="center" valign="top">
      <a href="https://github.com/JevLOMCN/mir4">
        <img src="https://raw.githubusercontent.com/JevLOMCN/mir4/main/Tools/icons/mir4.png" width="32" height="32">
      </a>
    </td>
    <td>
      <b>Mir 4</b> •
      <a href="https://github.com/JevLOMCN/mir4">Project</a><br>
      Remake of Wemade Entertainment's 2021 <i>Mir 4</i>
    </td>
  </tr>

  <tr>
    <td width="48" align="center" valign="top">
      <a href="https://mirfiles.co.uk/resources/mir2/users/Jev/Mir%205/Leaks/">
        <img src="https://raw.githubusercontent.com/JevLOMCN/mir4/main/Tools/icons/mir5.png" width="32" height="32">
      </a>
    </td>
    <td>
      <b>Mir 5</b> •
      <a href="https://github.com/JevLOMCN/mir5-private">Project</a> •
      <a href="https://mirfiles.co.uk/resources/mir2/users/Jev/Mir%205/Leaks/">Leaked Files</a> •
      <a href="https://mirfiles.co.uk/resources/mir2/users/Jev/Mir%205/Leaks/Mir5%20Alpha%202.mkv">Gameplay Video (24m)</a><br>
      Remake of Wemade Entertainment's unreleased <i>Mir 5</i>
    </td>
  </tr>

</table>
