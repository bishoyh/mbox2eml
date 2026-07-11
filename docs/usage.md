---
title: Usage
layout: default
nav_order: 2
---

# Usage

```bash
mbox2eml <mbox_file> <output_directory>
```

`mbox2eml` takes exactly two arguments: the mbox file to read and the directory to write into. The output directory is created if it does not exist. If it exists but is a file, the program refuses to run rather than clobber it.

## Example

```bash
mbox2eml "~/Downloads/Takeout/Mail/All mail Including Spam and Trash.mbox" ~/mail/eml/
# Extracted 47823 emails.
# Saved email_1.eml
# ...
# Finished processing all emails.
```

Output filenames are `email_1.eml` through `email_N.eml`, numbered in the order the messages appear in the mbox. Import the folder into any `.eml`-aware client (Apple Mail, Thunderbird, Outlook) or feed it to a mail archiver.

## Exit codes

| Code | Meaning |
|------|---------|
| `0`  | All messages extracted and written successfully. |
| `1`  | Bad arguments, missing input file, un-writable output directory, or one or more `.eml` files failed to write. |

Every failed write is logged to stderr and counted; the run ends with a non-zero exit code so scripts and CI can detect a partial extraction.

## Build from source

Requires a C++23 compiler (GCC 13+, Clang 16+, MSVC 2022+) and `make`.

```bash
make          # build ./mbox2eml
make test     # build, then run the regression suite
make install  # install to /usr/local/bin (override with PREFIX=...)
make clean    # remove the binary
```

The resulting binary has no runtime dependencies. On Linux it is statically linked against libstdc++ so it runs on any distribution; on macOS it links dynamically against the system libstdc++.

## Memory note

`mbox2eml` reads the whole input file into RAM before writing anything out. That is what makes it fast, but it means the input file must fit in memory. A 10 GB mbox needs roughly 10 GB of free RAM. For files larger than your available memory, split the mbox first with a streaming tool.
