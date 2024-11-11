# mbox2eml

A simple tool to convert mbox files from Google Takeout to a folder of eml files.

## Features

- Extracts emails from an mbox file into a folder
- Utilizes multithreading for efficient processing

## Requirements

- C++ compiler supporting C++23
- Make

## Building

To build the tool, run the following commands:

```sh
make
```
## Usage

To convert an mbox file to individual eml files, use the following command:

```sh

./mbox2eml <mbox_file> <output_directory>

```

## Example

./mbox2eml myemails.mbox output_emails

This will extract emails from myemails.mbox and save them into the output_emails directory.




