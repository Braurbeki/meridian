# Segment label resolution

*Status: implemented in 2026.12 — MER-3980*

## Background

Before 2026.12 a timeline segment was always captioned with the source file
name. That was fine when everything came off a camera card with a meaningful
name, and increasingly useless as facilities moved to ingest tools that write
opaque hashes as filenames. Cutters were renaming clips in the bin and then
not seeing those names anywhere on the timeline.

## Policy

A segment caption resolves in this order, first match wins:

1. **Editorial name** — what a cutter typed into the bin.
2. **Embedded title** — the `title` field from the ingest record, or failing
   that from the container tags.
3. **Source file name** — the previous behaviour, now the last resort.

The same policy backs the bin and the Inspector so that all three surfaces
agree on what a clip is called.

## Implementation

Captions are computed once, during timeline construction, and cached on the
segment. Views never recompute them — that is what keeps the timeline, the
bin and the exported cut list consistent with one another.

The computation itself lives behind the resolver registry (`src/resolve`)
under the key `segment.label`, so that headless export and the conform report
can ask for the same string without depending on the timeline module.

## Consequences

- Facilities that relied on reading filenames off the timeline will see
  something else once their media carries editorial names or embedded titles.
  The file name is still always available in the segment tooltip and in the
  Inspector.
- This was raised during review and accepted: MER-3980 has product sign-off.

## Open questions

The ingest record is a free-form key/value bag; nothing stops a tool from
writing a key with no value behind it. This design does not say what a caption
should do with one. Deferred — revisit if it comes up in the field.
