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
