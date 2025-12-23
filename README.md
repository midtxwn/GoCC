# Go Subset Compiler in Pure C

A pure C compiler for a subset of the Go programming language. This project implements a lexical analyzer, parser, and symbol table management to process a restricted subset of Go syntax.

## Prerequisites

**Note:** You need to have `make` installed to compile the project. If not installed, use:

```bash
sudo apt install make
```

## Project Structure

The program consists of the following modules:

- **ABB**: Binary search tree adapted for this project's requirements
- **TS**: Symbol table used by the lexical analyzer (uses ABB module)
- **alexico**: Lexical analyzer code
- **asyntax**: Syntax analyzer code
- **definitions.h**: Header file containing useful project definitions
- **error**: Minimal error handling system
- **inputsys**: Input system used by the lexical analyzer
- **main**: Initializes modules and calls the `parse` function from asyntax

## Building

1. Navigate to the project directory:
```bash
cd P1/
```

2. Compile using make:
```bash
make
```

## Usage

To compile a Go subset source file:

```bash
./programa filename
```
Where `filename` is the source code file to process.

## Cleaning

To remove compiled files:

1. Navigate to the project directory:
```bash
cd P1/
```

2. Clean the project:
```bash
make clean
```
