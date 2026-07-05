---
title: How it works
layout: default
nav_order: 3
---

# How it works

## The mbox format

The mbox format dates to Unix in the 1970s. Every message starts with a special `From ` separator line. Note the trailing space — that is what distinguishes the separator from the `From:` header inside a message:

```
From user@example.com Sat Jan  1 00:00:00 2000
From: Alice <user@example.com>
To: Bob <bob@example.com>
Subject: Hello

Body text here.

From another@example.com Sat Jan  1 00:01:00 2000
...
```

## Detecting a real boundary

A line that merely starts with `From ` is not enough — the body of an email can contain such a line ("From the desk of..."). `mbox2eml` only treats a line as a message boundary when it looks like a genuine mbox separator:

- It starts with `From ` (with the trailing space).
- It splits into the expected fields: `From`, a sender, day-of-week, month, day-of-month, time, year.
- The day-of-month is one or two digits.
- The time contains exactly two colons (`HH:MM:SS`).
- The year is exactly four digits. An optional timezone offset (`+0000`) right before the year is tolerated — some exporters inject one.

Any line that fails these checks stays in the body of the current message, so a stray `From ` line never splits an email in two. Lines that appear before the first valid separator (mbox preamble) are discarded.

These rules are exercised by the regression suite in `tests/regression.sh`, which covers preamble handling, false-positive `From ` body lines, missing input, write failures, and a multi-message round trip.

## Parallel writing

Boundary detection is inherently sequential — you have to scan the file top to bottom. Writing the slices out is not. Once the messages are sliced, `mbox2eml` divides them evenly across `std::thread::hardware_concurrency()` worker threads, each writing its own contiguous range of `.eml` files. A shared mutex guards only the log output; the file writes themselves need no coordination because each thread owns a distinct set of filenames.

Failed writes are tallied in an atomic counter. If any write fails, the program reports the count and exits non-zero.
