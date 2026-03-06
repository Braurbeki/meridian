# Meridian architecture

Meridian is a sequence review and conform tool: it opens a project, builds a
timeline from it, plays it back, and exports cut lists. It is not an editor —
there is no trim model and no undo.

## Modules

Dependencies point downward. Nothing below depends on anything above it.

```
             app
              |
              ui                 Qt widgets. Renders only.
              |
    +---------+---------+
    |         |         |
 playback  plugins   compat/v1   Transport, exporters, legacy import
    |         |         |
    +---------+---------+
              |
           timeline               Sequence model + the builder
              |
              |
           project                Document model, .mrp reader/writer
              |
            core                  Media, metadata, time types
              |
            util                  Logging, strings, ids, diagnostics
```

### util
Logging with per-subsystem channels, string and path helpers, UUIDs, and the
diagnostic sink that carries non-fatal problems up to the Console panel.

### core
`MediaSource` (a file the project references), `MetadataDict` /
`MetadataValue` (weakly typed tag bags), rational time types, and the
libavformat-backed prober.

### project
The document model — clips, bins, and the *serialised* form of a sequence
(`TimelineSpec`). Reads and writes `.mrp`.

### timeline
The live sequence model, and `TimelineBuilder`, which turns a `TimelineSpec`
into a drawable `Timeline`. This is the only place editorial data becomes
display data.

### ui
Qt widgets. The timeline view is a pure renderer: every string it draws was
computed upstream.

## Debugging

Every module logs to a named channel. Raise one to trace level with:

```
MERIDIAN_LOG_CHANNELS=timeline,resolve ./meridian project.mrp
```

Channels: `project`, `media`, `timeline`, `playback`, `plugins`, `compat`,
`diag`. `*` enables everything.

`meridian --dump-labels <project>` builds the first sequence and prints its
segment captions without opening a window.
