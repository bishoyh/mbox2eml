# TODO

Larger ideas beyond the current modernization pass. None are blocking; they are
recorded here so the low-hanging fruit stays visible.

## Correctness

- [ ] **Unescape `>From ` lines.** The mbox format escapes body lines that begin
      with `From ` by prefixing `>`. `mbox2eml` currently preserves the `>` in
      the output `.eml`. A strict reader should strip one leading `>` from
      `>+From ` runs when reconstructing each message. Add a fixture for this.
- [ ] **Handle mboxrd / mboxo variants explicitly.** Document which variant is
      assumed and, ideally, auto-detect.

## Features

- [ ] **Streaming mode for files larger than RAM.** Today the whole mbox is read
      into memory. A streaming parser that writes each message as it is found
      would lift the "input must fit in RAM" limit (see the memory note in the
      docs). This is the single biggest limitation.
- [ ] **Meaningful filenames.** Optionally name output files from the message
      `Date:` + `Subject:` or `Message-ID:` instead of a running counter, behind
      a flag so the default stays stable.
- [ ] **Progress reporting.** For multi-GB inputs, print a periodic progress
      line (messages found / bytes read) instead of one line per saved file.
- [ ] **`--quiet` / `--verbose` flags.** The per-file "Saved email_N.eml" log is
      noisy for large runs; make it opt-in.

## Build & distribution

- [ ] **CMake build alongside the Makefile** for consumers on CMake-only
      pipelines and easier MSVC builds.
- [ ] **Homebrew formula / prebuilt release binaries** documented in the README
      so users need not compile.
- [ ] **Man page** (`mbox2eml.1`) generated from the usage text.

## Testing

- [ ] **Windows CI test path.** The regression suite is skipped on Windows
      because it relies on POSIX `chmod`. Add a PowerShell-friendly smoke test.
- [ ] **Fuzz the boundary detector** with malformed `From ` lines.
