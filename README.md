# mbox2eml

<img src="docs/assets/icon.png" alt="mbox2eml" width="180" align="right">

Explode an mbox archive into individual `.eml` files. Fast, parallel, no dependencies beyond a C++23 compiler.

Full docs: **<https://twardoch.github.io/mbox2eml/>**

## The mbox problem

An mbox file is one giant text file. Every email in a folder — all of them — concatenated together, each starting with a line that begins `From `. Google Takeout gives you your Gmail this way. So does Thunderbird when you export a folder.

The format is portable but useless for anything else. Modern email clients, mail archivers, and search tools want individual `.eml` files — one message, one file, standard RFC 2822. `mbox2eml` does the conversion, and it does it in seconds.

## What it does

1. Reads the entire mbox file into memory.
2. Walks through it looking for `From <sender> <timestamp>` boundary lines.
3. Slices each detected message into its own string.
4. Writes them out as `email_1.eml`, `email_2.eml`, … in parallel across all CPU cores.

A 10 GB mbox with 500,000 messages takes seconds on modern hardware, not hours — provided you have the RAM to hold the input file.

## Build

Requires a C++23 compiler (GCC 13+, Clang 16+, MSVC 2022+) and `make`.

```bash
git clone https://github.com/twardoch/mbox2eml
cd mbox2eml
make
```

Useful targets: `make test` runs the regression suite, `make install` copies the binary to `/usr/local/bin` (override with `PREFIX=...`), `make clean` removes it. The resulting binary has no runtime dependencies.

## Usage

```bash
mbox2eml <mbox_file> <output_directory>
```

The output directory is created if it does not exist. Output filenames are `email_1.eml` through `email_N.eml`, in the order the messages appear in the mbox.

```bash
mbox2eml ~/Downloads/Takeout/Mail/All\ mail\ Including\ Spam\ and\ Trash.mbox ~/mail/eml/
# Extracted 47823 emails.
# Finished processing all emails.
```

Import the folder into any `.eml`-aware client (Apple Mail, Thunderbird, Outlook) or feed it to a mail archiver.

`mbox2eml` exits `0` when every message is written, and `1` on bad arguments, a missing input file, an un-writable output directory, or any failed write — so scripts can detect a partial extraction.

## How boundaries are detected

A line that merely starts with `From ` is not enough; email bodies contain such lines too. `mbox2eml` treats a line as a boundary only when it parses as a real mbox separator: `From`, a sender, a weekday, month, one-or-two-digit day, an `HH:MM:SS` time, and a four-digit year (an optional `+0000` timezone before the year is tolerated). Anything else stays in the message body. See [How it works](https://twardoch.github.io/mbox2eml/how-it-works.html) for the full rules.

## Memory note

`mbox2eml` reads the whole input into RAM before writing. That is what makes it fast, but the input file must fit in memory. For files larger than your available RAM, split the mbox first.

## License

MIT. Original work by Bishoy H.; modernized and maintained by Adam Twardoch.
