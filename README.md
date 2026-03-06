# Meridian

Sequence review and conform tool. Opens a project, builds a timeline from it,
plays it back, exports cut lists. Not an editor — there is no trim model.

## Building

You need CMake 3.24+, a C++17 compiler, and:

```sh
pipx install conan
pipx install aqtinstall
conan profile detect
```

(`pip install --user conan aqtinstall` also works where your Python allows
it; many distributions and Homebrew now refuse it under PEP 668.)

Qt comes from the official binaries — `aqtinstall` is a scripted downloader
for them, no Qt Account needed — and FFmpeg comes from Conan.

Install Qt once — pick the line for your platform:

```sh
aqt install-qt windows desktop 6.8.0 win64_msvc2022_64 -O ~/Qt
aqt install-qt linux   desktop 6.8.0 linux_gcc_64      -O ~/Qt
aqt install-qt mac     desktop 6.8.0 clang_64          -O ~/Qt
```

Then, from the repository root:

```sh
conan install . --build=missing
cmake --preset conan-release -DCMAKE_PREFIX_PATH=~/Qt/6.8.0/msvc2022_64   # Windows
cmake --preset conan-release -DCMAKE_PREFIX_PATH=~/Qt/6.8.0/gcc_64        # Linux
cmake --preset conan-release -DCMAKE_PREFIX_PATH=~/Qt/6.8.0/macos         # macOS
cmake --build --preset conan-release
ctest --preset conan-release
```

Qt from a system package manager works just as well — point
`CMAKE_PREFIX_PATH` at it instead (`$(brew --prefix qt)`, or drop the flag
entirely on Linux with `qt6-base-dev` installed).

> If Conan rejects your compiler version as unknown, upgrade Conan or lower
> `compiler.version` in `~/.conan2/profiles/default`. It only affects how
> Conan hashes binary compatibility for the C libraries it fetches.

## Running

```sh
./build/Release/bin/meridian sample/RiverdaleDoc.mrp
```

Without a display:

```sh
./build/Release/bin/meridian --dump-labels sample/RiverdaleDoc.mrp
./build/Release/bin/meridian --list-resolvers
./build/Release/bin/meridian --help
```

## Debugging

Every subsystem logs to a named channel, off by default below `info`:

```sh
MERIDIAN_LOG_CHANNELS=timeline,resolve ./build/Release/bin/meridian --dump-labels sample/RiverdaleDoc.mrp
MERIDIAN_LOG_CHANNELS='*' ./build/Release/bin/meridian sample/RiverdaleDoc.mrp
```

Channels: `project`, `media`, `timeline`, `resolve`, `playback`, `plugins`, `compat`, `diag`.

## Layout

```
cmake/          build helpers
docs/           architecture notes, design records, support tickets
sample/         a small project with media, for manual testing
src/util/       logging, strings, ids, diagnostics
src/core/       media sources, metadata, time types, the prober
src/project/    document model and .mrp reader/writer
src/resolve/    named field resolvers, registered by key
src/timeline/   sequence model and the timeline builder
src/playback/   transport clock, decoder, frame cache, playback engine
src/compat/v1/  importer for pre-2024 documents
src/plugins/    optional features, including exporters
src/ui/         Qt widgets
src/app/        entry point and command line
tests/          unit tests (no external test dependency)
```

See `docs/architecture.md` for how the modules fit together.
