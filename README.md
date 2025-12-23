Go Subset Analyzer in Pure C
A pure C lexical and syntax analyzer for a subset of the Go programming language. This project implements a complete language analysis pipeline including lexical analysis, syntax parsing, and symbol table management to identify and validate language components from Go source code.

Prerequisites
Note: You need to have make installed to build the project. If not installed, use:

bash
sudo apt install make
Project Structure
The program consists of the following modules:

ABB: Binary search tree adapted for this project's requirements

TS: Symbol table used by the lexical analyzer (uses ABB module)

alexico: Lexical analyzer code - tokenizes input source code

asyntax: Syntax analyzer code - parses token stream into abstract syntax tree

definitions.h: Header file containing useful project definitions

error: Error handling system for lexical and syntax errors

inputsys: Input buffer system used by the lexical analyzer

main: Initializes modules and orchestrates the analysis pipeline

Building
Navigate to the project directory:

bash
cd P1/
Build the analyzer using make:

bash
make
Usage
To analyze a Go subset source file:

bash
./programa filename
Where filename is the source code file to analyze.

The analyzer performs:

Lexical Analysis: Tokenizes the input source code

Syntax Analysis: Parses tokens and builds an abstract syntax tree (AST)

Symbol Table Management: Tracks identifiers and their scopes

Error Reporting: Identifies lexical and syntax errors

The program outputs analysis results including identified tokens, syntax structure, and any errors encountered.

Cleaning
To remove compiled files:

Navigate to the project directory:

bash
cd P1/
Clean the project:

bash
make clean
What This Program Does
This is a language analyzer, not a full compiler. It processes Go subset source code through:

Lexical Analysis: Breaks source code into tokens (keywords, identifiers, operators, literals)

Syntax Analysis: Verifies token sequences conform to grammar rules

AST Generation: Creates an abstract syntax tree representing program structure

Symbol Table: Maintains information about declared identifiers

The output is typically a representation of the analyzed code structure, error messages for invalid syntax, or a symbol table dump - not executable machine code.
