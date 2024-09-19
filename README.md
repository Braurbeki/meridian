# Meridian

Sequence review and conform tool. Opens a project, builds a timeline from it,
plays it back, exports cut lists. Not an editor — there is no trim model.

## Building

The supported toolchain is the container in `docker/`. It pins Qt 6, FFmpeg
and CMake so that a build is the same on everyone's machine.

```sh
docker build -t meridian-build -f docker/Dockerfile .
docker run --rm -it -v "$PWD":/work -w /work meridian-build bash

# inside the container:
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

Building on the host directly works too, if you have Qt 6.2+, FFmpeg
development headers and CMake 3.24+ on your path. There is no vendored
dependency tree and no package manager wrapper — CMake finds what is
installed. See `cmake/FindFFmpeg.cmake` if the FFmpeg probe fails.

VS Code users: `.devcontainer/` opens the same image.

## Running

```sh
./build/bin/meridian sample/RiverdaleDoc.mrp
```

The GUI needs a display. Inside the container without one, use the headless
entry points:

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
cmake/          build helpers and the FFmpeg locator
docker/         the supported toolchain
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

