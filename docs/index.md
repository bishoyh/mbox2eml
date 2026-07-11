---
title: Home
layout: default
nav_order: 1
---

# mbox2eml

Explode an mbox archive into individual `.eml` files. Fast, parallel, no dependencies beyond a C++23 compiler.

![mbox2eml — one giant envelope cut into a fan of individual envelopes](assets/icon.png){: style="max-width:320px" }

## The mbox problem

An mbox file is one giant text file: every message in a folder concatenated together, each one starting with a line that begins `From `. Google Takeout hands you your Gmail this way. So does Thunderbird when you export a folder.

Portable, yes. Useful, no. Modern mail clients, archivers, and search tools want individual `.eml` files — one message, one file, standard RFC 2822. `mbox2eml` does that split, and it does it in seconds.

## What it does

1. Reads the entire mbox file into memory.
2. Walks it looking for `From <sender> <timestamp>` boundary lines.
3. Slices each detected message into its own string.
4. Writes them out as `email_1.eml`, `email_2.eml`, … in parallel across every CPU core.

A 10 GB mbox with half a million messages finishes in seconds on modern hardware, provided you have the RAM to hold the input file.

## Quick start

```bash
git clone https://github.com/twardoch/mbox2eml
cd mbox2eml
make
./mbox2eml ~/Downloads/Takeout/Mail/*.mbox ~/mail/eml/
```

See [Usage](usage.md) for the full command reference and [How it works](how-it-works.md) for the boundary-detection rules.

## License

MIT. Original work by Bishoy H.; modernized and maintained by Adam Twardoch.
