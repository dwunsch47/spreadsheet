# Spreadsheet
Simple Excel-like spreadsheet with limited support of formulas

## Features
  - Simple binary and unary expressions support
  - ANTLR parser for formulas
  - In-formula cell reference support

## Design
Spreadsheet consists of cells, each having Excel-like position (ex. A2, C576) and support of either formula or numeric data. 
Each cell can be referenced by other cells by typing its global position (ex. 16 - B3).
It has three error states:
- `#REF!` - typed cell cannot be referenced
- `#VALUE!` - value cannot be computed
- `#DIV/0!` - division by zero error

More design schemes and examples are located in "design" folder

## Usage
1) Make sure you have appropriate Java SDK installed, since ANTLR is dependent on it.
2) Download antlr*.jar file from official ![ANTLR website](https://www.antlr.org/download.html) and put it in the same folder as `FindANTLR.cmake` file
3) Since this project was built and tested only with ANTLR version 4.12.0, you may have to edit `FindANTLR.cmake` file
4) Download ANTLR C++ Runtime from official website, make new folder `antlr4_runtime` in the same folder as `FindANTLR.cmake` file and extract downloaded runtime into new folder 
5) Create new folder `build`, go to it and start compilation
    ````
    cmake ..
    cmake --build .
    ````
