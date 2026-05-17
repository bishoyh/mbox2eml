# mbox2eml

Explode an mbox archive into individual `.eml` files. Fast, parallel, no dependencies beyond a C++23 compiler.

## The mbox problem

An mbox file is one giant text file. Every email in your inbox — all of them — concatenated together, separated by a line that starts with `From `. Google Takeout gives you your Gmail this way. So does Thunderbird when you export a folder.

The format is portable but useless for anything else. Modern email clients, mail archivers, and search tools want individual `.eml` files — one message, one file, standard RFC 2822 format. `mbox2eml` does the conversion.

## What it does

1. Reads the entire mbox file into memory.
2. Walks through it looking for `From <sender> <timestamp>` boundary lines.
3. Slices each detected message into its own string.
4. Writes them out as `email_1.eml`, `email_2.eml`, … in parallel across all CPU cores.

A 10 GB mbox with 500,000 messages takes seconds on modern hardware, not hours.

## Build

Requires a C++23 compiler (GCC 13+, Clang 16+, MSVC 2022+) and `make`.

```bash
git clone https://github.com/twardoch/mbox2eml
cd mbox2eml
make
```

The resulting `mbox2eml` binary has no runtime dependencies.

## Usage

```bash
mbox2eml <mbox_file> <output_directory>
```

The output directory is created if it does not exist.

## Example

```bash
mbox2eml ~/Downloads/Takeout/Mail/All\ mail\ Including\ Spam\ and\ Trash.mbox ~/mail/eml/
# Extracted 47823 emails.
# Finished processing all emails.
```

Output filenames are `email_1.eml` through `email_N.eml` in the order the messages appear in the mbox. Import the folder into any `.eml`-aware client (Apple Mail, Thunderbird, Outlook) or feed it to a mail archiver.

## mbox format explained

The mbox format dates back to Unix in the 1970s. Every message starts with a special "From " separator line (note the trailing space — that distinguishes it from the email header `From:`):

```
From user@example.com Sat Jan  1 00:00:00 2000
From: Alice <user@example.com>
To: Bob <bob@example.com>
Subject: Hello

Body text here.

From another@example.com Sat Jan  1 00:01:00 2000
...
```

`mbox2eml` detects these boundaries by checking for lines that start with `From ` followed by a sender address and a date with a recognisable year. Each detected boundary starts a new `.eml` file.

## .eml format

An `.eml` file is a single RFC 2822 email message: headers, a blank line, then the body. Most email clients open `.eml` files directly. They are plain text (or MIME-encoded), human-readable, and trivially parseable by any email library.

## License

MIT
