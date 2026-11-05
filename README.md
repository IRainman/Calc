# Calc
[![Windows MSBuild](https://github.com/irainman/calc/actions/workflows/windows-msbuild.yml/badge.svg)](https://github.com/irainman/calc/actions/workflows/windows-msbuild.yml) [![CMake](https://github.com/irainman/calc/actions/workflows/cmake.yml/badge.svg)](https://github.com/irainman/calc/actions/workflows/cmake.yml) [![Release](https://github.com/irainman/calc/actions/workflows/release.yml/badge.svg)](https://github.com/irainman/calc/actions/workflows/release.yml) [![CodeFactor](https://www.codefactor.io/repository/github/irainman/calc/badge)](https://www.codefactor.io/repository/github/irainman/calc)

Plese give a star to this repo! 

The home of Calc is here:

http://studio2000.xyz/txt/calc.shtml

## Supported configurations

| Platform | Compiler / linker | Architecture | Build configurations |
|---|---|---|---|
| Windows | MSVC `cl.exe` / `link.exe`, v145 | x64 | Debug, Release |
| Windows | MSVC `cl.exe` / `link.exe`, v145 | Win32 | Debug, Release |
| Linux | GCC | x86-64-v2 | Debug, Release |
| Linux | Clang + LLD | x86-64-v2 | Debug, Release |
| Linux | GCC | 32-bit Prescott / SSE3 | Debug, Release |
| Linux | Clang + LLD | 32-bit Prescott / SSE3 | Debug, Release |

CMake 4.3.0 or newer is required. `CMakePresets.json` uses preset schema
version 11, introduced in CMake 4.3. Windows presets use the `Visual Studio 18
2026` generator and the `v145` MSVC toolset. The CMake project explicitly
rejects clang-cl and MinGW for Windows builds.

Linux presets use Ninja Multi-Config, so Debug and Release use one configure
directory per compiler/architecture preset. Build presets set `jobs` to `0`,
which requests the build tool's default parallelism without imposing a fixed
job count.

## Dependencies

The repository expects the following local sibling layout because
`Calc/pch.hpp` includes the dependency files using repository-relative paths:

```text
<parent>/
+-- Calc/
¦   +-- CMakeLists.txt
¦   L-- Calc/
+-- fast_float/include/fast_float/fast_float.h
+-- fmt/include/fmt/compile.h
L-- zmij/zmij.cc
```

Use the project forks in those sibling directories. No separately built
third-party libraries are required; the headers/source are consumed through
`pch.hpp`.

## Windows — MSVC

Requirements:

- CMake 4.3+
- Visual Studio 18 2026 with MSVC v145
- Windows SDK and Resource Compiler
- Win32/x86 build tools when using the Win32 presets

Run from a Visual Studio Developer PowerShell or Developer Command Prompt.

```powershell
# x64
cmake --preset windows-msvc-x64
cmake --build --preset windows-msvc-x64-debug
cmake --build --preset windows-msvc-x64-release

# Win32 / x86
cmake --preset windows-msvc-win32
cmake --build --preset windows-msvc-win32-debug
cmake --build --preset windows-msvc-win32-release
```

The Visual Studio generator selects Microsoft's compiler and linker. The
`CMakeLists.txt` checks `CMAKE_CXX_COMPILER_ID` to prevent a clang-cl build.
`Calc/Calc.rc` is compiled as a Windows resource and the executable uses the
Windows GUI subsystem.

## Linux — GCC

Requirements: CMake 4.3+, GCC with C++23 support, and Ninja.

```sh
cmake --preset linux-gcc-x64
cmake --build --preset linux-gcc-x64-debug
cmake --build --preset linux-gcc-x64-release
```

## Linux — Clang

Requirements: CMake 4.3+, Clang with C++23 support, LLD, and Ninja. LLD is
selected explicitly so Clang LTO does not depend on the LLVMgold plugin for
GNU ld.

```sh
cmake --preset linux-clang-x64
cmake --build --preset linux-clang-x64-debug
cmake --build --preset linux-clang-x64-release
```

## Linux — 32-bit x86

The x86 presets add `-m32` to compilation and executable linking. They require
a complete 32-bit development environment (C++ standard library, startup
objects and system libraries). The target baseline is Prescott / SSE3.

```sh
cmake --preset linux-gcc-x86
cmake --build --preset linux-gcc-x86-debug
cmake --build --preset linux-gcc-x86-release

cmake --preset linux-clang-x86
cmake --build --preset linux-clang-x86-debug
cmake --build --preset linux-clang-x86-release
```

## Build configuration

### Common

- C++23 is required; compiler extensions are disabled.
- Only Debug and Release are generated.
- `Calc/pch.hpp` is the target precompiled header.
- The current `.vcxproj` C++ source list is retained.
- `Calc/pch.hpp` contains relative dependency includes; the dependency
  repositories must remain siblings of the Calc repository.
- Debug defines `_DEBUG`; Release defines `NDEBUG`.

### Windows / MSVC

| Setting | Debug | Release |
|---|---|---|
| Optimization | `/Od` | `/O2`, `/Oi` |
| Debug information | `/ZI` | `/Zi` |
| Runtime checks | `/RTC1` | — |
| Whole-program optimization | — | `/GL` + `/LTCG` |
| Link optimization | incremental | `/OPT:REF`, `/OPT:ICF`, `/RELEASE` |

The additional MSVC compile options preserve the project's intended settings:
`/Wall`, `/MP`, `/FS`, `/std:c++latest`, `/Zc:__cplusplus`,
`/Zc:preprocessor`, `/Zc:wchar_t-`, `/GR-`, `/EHsc`, `/fp:precise`, `/Gy`,
`/Gw`, `/GA`, `/Qpar`, `/GT`, and Release assembler listings (`/FAs`). Win32
uses `/arch:SSE2` and Release `/Oy`; x64 uses its architectural SSE2 baseline.
The x64 calling convention is defined by the platform ABI, so no x86-only
`/Gr` option is passed. The runtime is the dynamic CRT (`/MDd` for Debug and
`/MD` for Release).

### Linux / GCC and Clang

| Setting | Debug | Release |
|---|---|---|
| Optimization | `-O0` | `-O3` |
| Debug information | `-g3` | — |
| Frame pointers | retained | omitted |
| LTO | — | `-flto` |
| Section collection | — | function/data sections + `--gc-sections` |

The x86-64 baseline is `-march=x86-64-v2` (SSE3, SSSE3, SSE4.1, SSE4.2). The
32-bit baseline is `-march=prescott` (SSE3). RTTI is disabled on both platforms,
matching the existing Visual Studio project. Clang Release additionally uses
LLD's safe identical-code folding.

## Hardening / mitigation flags

Per the requested project build policy, the selected compiler and executable
hardening options are disabled in both Debug and Release.

Windows compiler options include `/GS-`, `/sdl-`, `/guard:cf-`, and
`/guard:ehcont-`. Link options disable ASLR, DEP compatibility, CET compatibility,
CFG metadata, UAC manifest data, and (architecture-dependent) High Entropy VA
or SafeSEH. Spectre instrumentation is not enabled; MSVC leaves it off by
default, so no Spectre switch is passed.

Linux compiler options disable stack protector, stack-clash/stack-check
instrumentation, CET control-flow and shadow-stack instrumentation, PIE, and
`_FORTIFY_SOURCE`.
The linker disables PIE and RELRO, requests an executable stack, and selects
lazy binding.

These are binary build settings. They cannot override protections imposed
independently by the operating system, kernel, loader, or execution environment.

## Output layout

Each configure preset has its own build directory. Executables are placed under
the selected configuration directory, for example:

```text
out/build/windows-msvc-x64/Debug/Calc.exe
out/build/windows-msvc-x64/Release/Calc.exe
out/build/linux-gcc-x64/Debug/Calc
out/build/linux-gcc-x64/Release/Calc
```

## Current platform behavior

The Windows target builds the existing Win32 GUI and its resource script. The
Linux target builds the current console REPL in `gui.cpp`; a Linux GUI has not
been implemented in the repository yet.

## Validation status

The files were checked for JSON syntax, preset names/configuration references,
source-file names against the current `Calc.vcxproj`, and compiler/linker flag
spelling. Linux GCC and Clang x64 fixture builds were used to exercise CMake
generation, PCH handling, LTO, and the entry-point bridge. Those fixture builds
are not a complete build of Calc's real sources.

The environment used to regenerate these files has CMake 3.31.6, not CMake
4.3+, and has no Windows SDK/MSVC installation. Consequently, the schema-v11
presets were checked against the CMake 4.3 documentation but were not executed
with a CMake 4.3 binary here; the actual Windows build and RC compilation must
still be run on a Windows machine with the stated toolchain. The 32-bit Linux
presets require multilib libraries that are not installed in this environment.

## Preparing a release

Start from a clean working tree on the branch that should receive the release
commit. The scripts read the local system date and generate a four-component
version in `YYYY.MM.DD.N` format. The fourth component is a per-day release
counter: it starts at `0` when the date changes and increments by one for each
additional release on the same date. The counter is based on the current
`FILEVERSION` in `Calc/Calc.rc` and must remain within the Windows resource
version limit of `0..65535`.

Run the script without a version argument:

```powershell
release.cmd
```

```sh
bash release.sh
```

An optional quoted argument overrides the default commit message:

```powershell
release.cmd "Prepare the Calc release"
```

```sh
bash release.sh "Prepare the Calc release"
```

Both scripts update the four-component version in `Calc/Calc.rc` and the root
`CMakeLists.txt`, then commit and push the current branch and annotated
`vYYYY.MM.DD.0` tag. They only stage those two version files. Pushing the tag
starts the cross-platform release workflow. It publishes Windows x86/x64
executable and PDB archives, Linux GCC/Clang x86-64-v2 executable archives, and
`Calc-source.zip`, which contains Calc, its three source dependencies, and
`SOURCE-VERSIONS.txt` with the exact exported Git revisions.

The Linux script requires Python 3; both scripts require Git and a configured
`origin` remote.

## TODO

The project is currently in the moving stage from an old local SVN repo to GitHub.
Done: Correct native Windows builds awailable in release page.

After the first step, my plan is to add a GUI for Linux and do additional steps to make the application cross-platform.

This is only my small pet project, that I additionally use for improving quality and performance in widely used libraries.

## SAST Tools

[PVS-Studio](https://pvs-studio.com/en/pvs-studio/?utm_source=website&utm_medium=github&utm_campaign=open_source) is a static analyzer for C, C++, C#, and Java code.
