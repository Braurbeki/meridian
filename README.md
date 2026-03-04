# Meridian

Sequence review and conform tool. Opens a project, builds a timeline from it,
plays it back, exports cut lists. Not an editor — there is no trim model.

## Requirements

- **Qt 6.2 or newer** (Widgets)
- **CMake 3.24 or newer**
- A C++17 compiler — MSVC 2019+, Clang 12+, or GCC 9+

FFmpeg is **optional**. If the development headers are on the system, CMake
picks them up and the media prober reads duration, streams and container tags.
Without them the prober only establishes that a file is where the project says
it is, and everything else behaves identically. There is no vendored
dependency tree and no package manager wrapper — CMake finds what is
installed.

## Building

### Windows

Install Qt with the [official installer](https://www.qt.io/download-qt-installer)
(pick the `msvc2022_64` component) and Visual Studio 2022 with the C++
workload. Then, from a Developer PowerShell:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.8.0/msvc2022_64"
cmake --build build --config Debug
.\build\bin\Debug\meridian.exe sample\RiverdaleDoc.mrp
```

If the executable cannot find the Qt DLLs, run
`C:\Qt\6.8.0\msvc2022_64\bin\windeployqt.exe build\bin\Debug\meridian.exe`
once, or put that `bin` directory on `PATH`.

### macOS

```sh
brew install qt          # brew install ffmpeg as well, if you want probing
cmake -S . -B build -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
cmake --build build -j
./build/bin/meridian sample/RiverdaleDoc.mrp
```

### Linux

```sh
sudo apt install build-essential cmake qt6-base-dev   # + libavformat-dev, optional
cmake -S . -B build
cmake --build build -j
./build/bin/meridian sample/RiverdaleDoc.mrp
```

### Tests

```sh
ctest --test-dir build --output-on-failure
```

## Running without a display

The GUI needs a window server. These do not:

```sh
./build/bin/meridian --dump-labels sample/RiverdaleDoc.mrp
./build/bin/meridian --help
```

## Debugging

Every subsystem logs to a named channel, off by default below `info`:

```sh
MERIDIAN_LOG_CHANNELS=timeline,project ./build/bin/meridian --dump-labels sample/RiverdaleDoc.mrp
MERIDIAN_LOG_CHANNELS='*' ./build/bin/meridian sample/RiverdaleDoc.mrp
```

Channels: `project`, `media`, `timeline`, `playback`, `plugins`, `compat`,
`diag`.

## Layout

```
cmake/          build helpers and the optional FFmpeg locator
docs/           architecture notes, design records, support tickets
sample/         a small project with media, for manual testing
src/util/       logging, strings, ids, diagnostics
src/core/       media sources, metadata, time types, the prober
src/project/    document model and .mrp reader/writer
src/timeline/   sequence model and the timeline builder
src/playback/   transport clock, frame cache, playback engine
src/compat/v1/  importer for pre-2024 documents
src/plugins/    optional features, including exporters
src/ui/         Qt widgets
src/app/        entry point and command line
tests/          unit tests (no external test dependency)
```

See `docs/architecture.md` for how the modules fit together.
