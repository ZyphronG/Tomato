# Tomato
###### (please pronounce it british)

<br>

Hello! This is my first tool so please go easy on me.

This tool can convert Super Mario Galaxy .bmg / .tbl files to .xml and back to .bmg / .tbl! The .tbl will be sorted as well so no need to worry about that. It also converts flows from the .bmg to .xml and back to .bmg.

Please report bugs and other issues!

<br>

## Usage
To convert from .bmg / .tbl to .xml use:
```
Tomato -bmg2xml -bmg <BMGInputFile> -tbl <TBLInputFile> [-xml <XMLOutputFile>]
```
or to convert from .xml to .bmg / .tbl use:
```
Tomato -xml2bmg -xml <XMLInputFile> [-bmg <BMGOutputFile>] [-tbl <TBLOutputFile>] [-le (output little endian)]
```
Arguments in square brackets are optional. The order of arguments does not matter.

<br>

## Known Issues

Tomato does not support the japanese and korean language because of UTF-16 encoded text.

<br>

## Documentation

### Message Info

To-do: document Message Info.

### Message Tags

To-do: document Message Tags.

### Flow Tags

To-do: document Flow Tags.

<br>

Tomato also accepts numbers instead of the tag names it uses when converting from or to .bmg.
###### (take a look inside Tomato.cpp, MESGbmg1.cpp or MESGbmg1.h for tag names)
